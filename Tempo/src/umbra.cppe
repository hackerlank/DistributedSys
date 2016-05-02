/* Tempo
 * Copyright (c) Zilong Tan, Shivnath Babu {ztan,shivnath}@cs.duke.edu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, subject to the conditions listed
 * in the Tempo LICENSE file. These conditions include: you must preserve this
 * copyright notice, and you cannot mention the copyright holders in
 * advertising related to the Software without their permission.  The Software
 * is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This notice is a
 * summary of the Tempo LICENSE file; the license in that file is legally
 * binding.
 */

#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_permutation.h>
#include <ulib/util_log.h>
#include "umbra.hpp"

namespace Tempo {

void umbra::perturb(gsl_vector *x, const gsl_vector *ref, double deg, int r)
{
	for (unsigned int i = 0; i < x->size; ++i) {
		double s = sqrt(fabs(gsl_vector_get(ref, i))) * normal_rng_next(&_rng);
		// Alternative: Pure Gaussian perturbation
		// (void)ref;
		// double s = normal_rng_next(&_rng);
		gsl_vector_set(x, i, deg*(1.0+s)/pow(r,1.0/3.0));
	}
}

double umbra::utility(
	const gsl_vector *ynew,
	const gsl_vector *yold,
	bool  strict)
{
	const gsl_vector *v = ynew;

	if (strict) {
		for (unsigned int i = 0; i < ynew->size; ++i) {
			if (gsl_vector_get(ynew, i) > gsl_vector_get(yold, i)) {
				v = yold;
				break;
			}
		}
	}

	double u = 0;
	for (unsigned int i = 0; i < v->size; ++i)
		u += gsl_vector_get(v, i);

	return u;
}

double umbra::kernel(const gsl_vector * x, double tau)
{
	double d = gsl_blas_dnrm2(x);
	tau *= tau;

	return exp(-d*d/2.0/tau);
}

int umbra::update(const gsl_matrix *batch, const gsl_vector *y,
		  gsl_vector *delta, double lambda, double tau)
{
	int ret = -1;
	int signum;
	gsl_matrix *wm = NULL;
	gsl_matrix *im = NULL;
	gsl_matrix *pm = NULL;
	gsl_matrix *km = NULL;
	gsl_matrix *tm = NULL;
	gsl_permutation *perm = NULL;
	gsl_vector_view dv;

	wm = gsl_matrix_alloc(batch->size2, batch->size2);
	if (wm == NULL) {
		ULIB_FATAL("failed to alloc weight matrix");
		goto done;
	}
	im = gsl_matrix_alloc(batch->size2, batch->size2);
	if (im == NULL) {
		ULIB_FATAL("failed to alloc inverted weight matrix");
		goto done;
	}
	pm = gsl_matrix_alloc(batch->size2, batch->size1);
	if (pm == NULL) {
		ULIB_FATAL("failed to alloc product matrix");
		goto done;
	}
	km = gsl_matrix_calloc(batch->size1, batch->size1);
	if (km == NULL) {
		ULIB_FATAL("failed to alloc kernel matrix");
		goto done;
	}
	tm = gsl_matrix_alloc(batch->size2, batch->size1);
	if (tm == NULL) {
		ULIB_FATAL("failed to alloc temp matrix");
		goto done;
	}
	perm = gsl_permutation_alloc(batch->size2);
	if (perm == NULL) {
		ULIB_FATAL("failed to alloc the permutation");
		goto done;
	}

	dv = gsl_matrix_diagonal(km);
	for (size_t i = 0; i < km->size1; ++i) {
		gsl_vector_const_view row = gsl_matrix_const_row(batch, i);
		gsl_vector_set(&dv.vector, i, kernel(&row.vector, tau));
	}

	dv = gsl_matrix_diagonal(wm);
	if (gsl_blas_dgemm(CblasTrans, CblasNoTrans, 1.0, batch, km, 0.0, tm) ||
	    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, tm, batch, 0.0, wm) ||
	    gsl_vector_add_constant(&dv.vector, lambda) ||
	    gsl_linalg_LU_decomp(wm, perm, &signum) ||
	    gsl_linalg_LU_invert(wm, perm, im) ||
	    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0, im, batch, 0.0, tm) ||
	    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, tm, km, 0.0, pm) ||
	    gsl_blas_dgemv(CblasNoTrans, 1.0, pm, y, 0.0, delta)) {
		ULIB_FATAL("delta computation error");
		goto done;
	}

	ret = 0;
done:
	gsl_matrix_free(wm);
	gsl_matrix_free(im);
	gsl_matrix_free(pm);
	gsl_matrix_free(km);
	gsl_matrix_free(tm);
	gsl_permutation_free(perm);
	return ret;
}

int umbra::operator()(
	umbra::problem *task,
	gsl_vector *x0,
	int niter,
	int bs,            // mini-batch size
	double deg,        // degree of exploration
	double alpha,      // step size
	double lambda,     // regularization factor
	double tau,        // bandwidth
	gsl_matrix *xval,
	gsl_matrix *fval,
	method_t method,
	bool strict)
{
	int ret = -1;
	gsl_vector_view v, f, s, t, z;

	v = gsl_matrix_row(xval, 0);
	gsl_vector_memcpy(&v.vector, x0);
	f = gsl_matrix_column(fval, 0);
	task->eval(x0, &f.vector);

	gsl_matrix *batch = gsl_matrix_alloc(bs, x0->size);
	if (batch == NULL) {
		ULIB_FATAL("failed to alloc a batch matrix: %d x %zu", bs, x0->size);
		return -1;
	}

	gsl_vector *y = gsl_vector_alloc(bs);
	if (y == NULL) {
		ULIB_FATAL("failed to alloc target vector: %d", bs);
		gsl_matrix_free(batch);
		return -1;
	}

	for (int i = 1, r = 1; i < niter; ++i) {
		t = gsl_matrix_row(xval, i - 1);
		z = gsl_matrix_row(xval, i);
		s = gsl_matrix_column(fval, i);
		f = gsl_matrix_column(fval, 0);
		task->eval(x0, &f.vector);
		bool changed = false;
		for (int j = 0; j < bs; ++j) {
			v = gsl_matrix_row(batch, j);
			perturb(&v.vector, &t.vector, deg, r);
			gsl_vector_memcpy(&z.vector, &t.vector);
			gsl_vector_add(&z.vector, &v.vector);
			task->proj(&z.vector);
			task->eval(&z.vector, &s.vector);
			double u1 = utility(&s.vector, &f.vector, strict);
			gsl_vector_memcpy(&z.vector, &t.vector);
			gsl_vector_sub(&z.vector, &v.vector);
			task->proj(&z.vector);
			task->eval(&z.vector, &s.vector);
			double u2 = utility(&s.vector, &f.vector, strict);
			gsl_vector_set(y, j, method == OPT_LOG? log(u1/u2): u1-u2);
			changed = changed || fabs(u1 - u2) > PRECISION;
		}
		if (changed) {
			// compute the new x
			if (update(batch, y, &z.vector, lambda, tau)) {
				ULIB_FATAL("failed to update the decision variables");
				goto done;
			}
			gsl_vector_scale(
				&z.vector, -alpha/(1.0+alpha*lambda*(r++)) *
				(method == OPT_LOG? utility(&f.vector, &f.vector, strict): 1.0));
			gsl_vector_add(&z.vector, &t.vector);
			task->proj(&z.vector);
		} else
			gsl_vector_memcpy(&z.vector, &t.vector);
		// must re-evaluate no matter if xval(i) == xval(i-1)
		task->eval(&z.vector, &s.vector);
	}

	ret = 0;

done:
	gsl_matrix_free(batch);
	gsl_vector_free(y);

	return ret;
}

}
