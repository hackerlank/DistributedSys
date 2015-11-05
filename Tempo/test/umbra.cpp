#include <math.h>
#include <stdio.h>
#include <ulib/math_rng_normal.h>
#include <Tempo/umbra.hpp>

using namespace Tempo;

normal_rng rng;

struct problem : public umbra::problem {
	void proj(gsl_vector *x)
	{
		printf("(");
		for (unsigned int i = 0; i < x->size; ++i)
			printf("%lf%c", gsl_vector_get(x, i),
			       i == x->size - 1? ')': ',');
		putchar('\n');
	}

	void eval(const gsl_vector *x, gsl_vector *y)
	{
		double sum = 0;
		for (unsigned int i = 0; i < x->size; ++i) {
			double d = gsl_vector_get(x, i) - 4;
			sum += d * d;
		}
		gsl_vector_set(y, 0, exp((sum + sqrt(7)*normal_rng_next(&rng))/100));
		sum = 0;
		for (unsigned int i = 0; i < x->size; ++i) {
			double d = gsl_vector_get(x, i) - 7;
			sum += d * d;
		}
		gsl_vector_set(y, 1, exp((sum + sqrt(11)*normal_rng_next(&rng))/100));
		sum = 0;
		for (unsigned int i = 0; i < x->size; ++i) {
			double d = gsl_vector_get(x, i) - 3;
			sum += d * d;
		}
		gsl_vector_set(y, 2, exp((sum + sqrt(3)*normal_rng_next(&rng))/100));
	}
};

int main()
{
	umbra solver;
	problem prb;

	normal_rng_init(&rng);

	gsl_vector *x0   = gsl_vector_alloc(5);
	gsl_matrix *xval = gsl_matrix_alloc(100, 5);
	gsl_matrix *fval = gsl_matrix_alloc(3, 100);

	for (unsigned int i = 0; i < x0->size; ++i)
		gsl_vector_set(x0, i, normal_rng_next(&rng));

	if (solver(&prb, x0, 100, 1, 1.0, 0.2, 0.001, 5.0, xval, fval,
			    umbra::OPT_LINEAR, true)) {
		fprintf(stderr, "optimization failed\n");
		return -1;
	}

	printf("Best obj by OPT_LINEAR: %lf %lf %lf\n",
	       gsl_matrix_get(fval, 0, 99),
	       gsl_matrix_get(fval, 1, 99),
	       gsl_matrix_get(fval, 2, 99));

	getchar();

	if (solver(&prb, x0, 100, 1, 1.0, 0.2, 0.001, 5.0, xval, fval,
		     umbra::OPT_LOG, true)) {
		fprintf(stderr, "optimization failed\n");
		return -1;
	}

	printf("Best obj by OPT_LOG: %lf %lf %lf\n",
	       gsl_matrix_get(fval, 0, 99),
	       gsl_matrix_get(fval, 1, 99),
	       gsl_matrix_get(fval, 2, 99));

	gsl_vector_free(x0);
	gsl_matrix_free(xval);
	gsl_matrix_free(fval);

	return 0;
}
