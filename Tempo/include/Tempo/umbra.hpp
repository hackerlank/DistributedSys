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

#ifndef _COLOSSAL_UMBRA_H
#define _COLOSSAL_UMBRA_H

#include <gsl/gsl_matrix.h>
#include <ulib/math_rng_normal.h>

namespace Tempo {

class umbra {
public:
	// Provides two optimization methods,
	// 1. The OPT_LOG is appropriate if the value of the objective
	// functions are positive, and subject to large estimation
	// variance.
	// 2. The OPT_LOG is a general method, and should be used when
	// OPT_LOG is not applicable.
	enum method_t {
		OPT_LINEAR,
		OPT_LOG
	};

	// The precision of the objective functions.
	const static double PRECISION = 1e-32;

        // The umbra optimization task interface
	struct problem {
		// Map x to the nearest feasible point
		virtual void proj(gsl_vector *x) = 0;
		// Estimate the value of the objective functions
		virtual void eval(const gsl_vector *x, gsl_vector *y) = 0;
	};

        // Returns 0 on success, and [xval fval'] will hold the
        // results of each iteration
        int operator() (
		problem *task,
		gsl_vector *x0,
		int niter,
		int bs,            // mini-batch size
		double deg,        // degree of exploration
		double alpha,      // step size
		double lambda,     // regularization factor
		double tau,        // bandwidth
		gsl_matrix *xval,
		gsl_matrix *fval,
		method_t method = OPT_LINEAR,
		bool strict = false);

	umbra() { normal_rng_init(&_rng); }

private:
	void perturb(gsl_vector *x, const gsl_vector *ref, double deg, int r);
	static double kernel(const gsl_vector * x, double tau);
	static double utility(const gsl_vector *ynew, const gsl_vector *yold, bool strict);
	static int    update(const gsl_matrix *batch, const gsl_vector *y,
			     gsl_vector *delta, double lambda, double tau);

	normal_rng _rng;
};

}

#endif
