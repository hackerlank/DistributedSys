/* Tempo
 * Copyright (c) 2015 Zilong Tan (eric.zltan@gmail.com)
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

#include <ctime>
#include <cmath>
#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <algorithm>
#include <gsl/gsl_blas.h>
#include <libconfig.h++>
#include <ulib/hash_open.h>
#include <Tempo/Tempo.hpp>

using namespace std;
using namespace ulib;
using namespace Tempo;
using namespace libconfig;

const char *CONFIG_FILE = "./conf/opt.conf";

class problem : public umbra::problem {
public:
    typedef vector<double> gen_param_t;

    void proj(gsl_vector *x)
    {
	size_t npools = _jt->getpools().size();

	for (size_t i = 0; i < npools; ++i) {
	    size_t base = i * 5;
	    if (gsl_vector_get(x, 0 + base) < 0)
		gsl_vector_set(x, 0 + base, -1.0); // -1 disables preemptions
	    if (gsl_vector_get(x, 1 + base) < 0)
		gsl_vector_set(x, 1 + base, -1.0); // -1 disables preemptions
	    if (gsl_vector_get(x, 2 + base) < 0.5)
		gsl_vector_set(x, 2 + base, 0.5);  // weights must be >= 0.5
	    double val = gsl_vector_get(x, 3 + base);
	    // roundup to the nearest nonnegative integer
	    gsl_vector_set(x, 3 + base, (long)(max(val, 0.0) + 0.5));
	    val = gsl_vector_get(x, 4 + base);
	    // roundup to the nearest nonnegative integer
	    gsl_vector_set(x, 4 + base, (long)(max(val, 0.0) + 0.5));
	}
    }

    void eval(const gsl_vector *x, gsl_vector *y)
    {
	apply_params(x);
	_jt->reset_time();
	_jt->process();
	compute_objs(y);
    }

    problem(const string &conf)
    {
	try {
	    _conf.readFile(conf.c_str());
	} catch (const FileIOException &e) {
	    cerr << "I/O error while reading " << conf << endl;
	    exit(EXIT_FAILURE);
	} catch(const ParseException &pex) {
	    cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
		 << " - " << pex.getError() << std::endl;
	    exit(EXIT_FAILURE);
	}

	try {
	    _nmap       = _conf.lookup("cluster.total_maps");
	    _nreduce    = _conf.lookup("cluster.total_reduces");
	    _workload   = (const char *)_conf.lookup("optimizer.workload");
	    _traj_file  = (const char *)_conf.lookup("optimizer.trajectory");
	    _niter      = _conf.lookup("optimizer.number_of_iterations");
	    _bs         = _conf.lookup("optimizer.batch_size");
	    _deg        = _conf.lookup("optimizer.exploration_degree");
	    _alpha      = _conf.lookup("optimizer.learning_rate");
	    _lambda     = _conf.lookup("optimizer.regularization");
	    _tau        = _conf.lookup("optimizer.bandwidth");
	} catch (const SettingNotFoundException &e) {
	    cerr << "Missing a setting in configuration file" << endl;
	    exit(EXIT_FAILURE);
	}

	cerr << "Initialize the job tracker ...\t";
	_jt = new job_tracker(_nmap, _nreduce);
	init_job_tracker();

	_fp_traj = NULL;
    }

    ~problem()
    {
	delete _jt;
	if (_fp_traj)
	    fclose(_fp_traj);
    }

    int    get_niter() const { return _niter; }
    int    get_bs() const { return _bs; }
    double get_deg() const { return _deg; }
    double get_alpha() const { return _alpha; }
    double get_lambda() const { return _lambda; }
    double get_bandwidth() const { return _tau; }
    bool   get_strict() const { return _strict; }
    size_t var_count() const { return _jt->getpools().size() * 5; }
    size_t obj_count() const { return _jt->getpools().size(); }

    void get_config(gsl_vector *x) const
    {
	const job_tracker::pool_container_type &pools = _jt->getpools();
	typename job_tracker::pool_container_type::const_iterator it = pools.begin();
	for (size_t i = 0; it != pools.end(); ++i, ++it) {
	    size_t base = i * 5;
	    gsl_vector_set(x, base + 0, it->ms_timeout);
	    gsl_vector_set(x, base + 1, it->hf_timeout);
	    gsl_vector_set(x, base + 2, it->fs_ctx_map.weight);
	    gsl_vector_set(x, base + 3, it->fs_ctx_map.minshare);
	    gsl_vector_set(x, base + 4, it->fs_ctx_reduce.minshare);
	}
    }


private:
    void init_job_tracker()
    {
	// setup pools
	const Setting &pools = _conf.lookup("pools");
	int npools = pools.getLength();

	for (int i = 0; i < npools; ++i) {
	    const Setting &pool = pools[i];
	    string name;
	    string sched_mode;
	    double min_share_timeout;
	    double fair_share_timeout;
	    double weight;
	    string uds;
	    double slack = 0.0;
	    int    map_min_share;
	    int    reduce_min_share;
	    gen_param_t param;
	    param.resize(9);
	    if (!(pool.lookupValue("name", name) &&
		  pool.lookupValue("sched_mode", sched_mode) &&
		  pool.lookupValue("min_share_timeout", min_share_timeout) &&
		  pool.lookupValue("fair_share_timeout", fair_share_timeout) &&
		  pool.lookupValue("weight", weight) &&
		  pool.lookupValue("uds", uds) &&
		  pool.lookupValue("map_min_share", map_min_share) &&
		  pool.lookupValue("reduce_min_share", reduce_min_share))) {
		cerr << "Missing pool settings for pool " << i << endl;
		exit(EXIT_FAILURE);
	    }
	    pool::sched_mode sched;
	    if (sched_mode == "fair")
		sched = pool::SCHED_FAIR;
	    else if (sched_mode == "fcfs" || sched_mode == "fifo")
		sched = pool::SCHED_FCFS;
	    else {
		ULIB_WARNING("invalid scheduling mode:%s for pool %s",
			     sched_mode.c_str(), name.c_str());
		exit(EXIT_FAILURE);
	    }

	    if (!strcasecmp(uds.c_str(), "ignore"))
		slack = -1;
	    else if (!strcasecmp(uds.c_str(), "deadline")) {
		if (!pool.lookupValue("slack", slack) || slack < 0) {
		    cerr << "Illegal slack specified for pool " << i << endl;
		    exit(EXIT_FAILURE);
		}
	    } else if (!strcasecmp(uds.c_str(), "latency"))
		slack = -2;
	    else if (!strcasecmp(uds.c_str(), "map_util"))
		slack = -3;
	    else if (!strcasecmp(uds.c_str(), "red_util"))
		slack = -4;
	    else {
		cerr << "UDS " << uds << " is not predefined" << endl;
		exit(EXIT_FAILURE);
	    }
	    _slack.push_back(slack);
	    _jt->add_pool(name, min_share_timeout, fair_share_timeout,
			  weight, map_min_share, reduce_min_share, sched);
	}
	_jt->scale_minshares();
	cerr << "Loaded " << npools << " pools" << endl;
	if (import_workload1(_workload.c_str(), &_jt->getpools())) {
	    cerr << "Unable to load workload" << endl;
	    exit(EXIT_FAILURE);
	}
	cerr << "Loaded workload" << endl;
    }

    void apply_params(const gsl_vector *x)
    {
	job_tracker::pool_container_type &pools = _jt->getpools();
	if (x->size != 5 * pools.size()) {
	    cerr << "Number of decision variables is incorrect: " << x->size << endl;
	    exit(EXIT_FAILURE);
	}

	size_t i = 0;
	for (typename job_tracker::pool_container_type::iterator it = pools.begin();
	     it != pools.end(); ++it, ++i) {
	    size_t base = i * 5;
	    it->reinit(
		       it->name,
		       gsl_vector_get(x, base + 0),
		       gsl_vector_get(x, base + 1),
		       gsl_vector_get(x, base + 2),
		       gsl_vector_get(x, base + 3),
		       gsl_vector_get(x, base + 4),
		       it->sched);
	}
	_jt->scale_minshares();
    }

    void compute_objs(gsl_vector *y)
    {
	if (_fp_traj == NULL && (_fp_traj = fopen(_traj_file.c_str(), "w")) == NULL) {
	    cerr << "Cannot open trajectory file " << _traj_file << endl;
	    exit(EXIT_FAILURE);
	}

	job_tracker::pool_container_type &pools = _jt->getpools();
	size_t npool = pools.size();
	size_t i = 0;
	for (typename job_tracker::pool_container_type::const_iterator it = pools.begin();
	     it != pools.end(); ++it, ++i) {
	    fprintf(_fp_traj, "%lf %lf %lf %lf %lf%c",
		    it->ms_timeout,
		    it->hf_timeout,
		    it->fs_ctx_map.weight,
		    it->fs_ctx_map.minshare,
		    it->fs_ctx_reduce.minshare,
		    i == npool - 1? '\t': ' ');
	}

	i = 0;
	for (typename job_tracker::pool_container_type::const_iterator it = pools.begin();
	     it != pools.end(); ++it, ++i) {
	    double jd = 0.0;
	    if (_slack[i] < 0) {
		if (fabs(_slack[i] + 1) < 0.1) // ignore UDS
		    jd = 0;
		else if (fabs(_slack[i] + 2) < 0.1) { // latency UDS
		    for (size_t j = 0; j < it->jobs.size(); ++j)
			jd += it->jobs[j].ftime - it->jobs[j].ctime;
		    if (it->jobs.size())
			jd /= it->jobs.size();
		    else
			jd = 0.0;
		} else if (fabs(_slack[i] + 3) < 0.1) // map util UDS
		    jd = -compute_utilization(*it, task::TASK_TYPE_MAP, _nmap);
		else if (fabs(_slack[i] + 4) < 0.1) // reduce util UDS
		    jd = -compute_utilization(*it, task::TASK_TYPE_REDUCE, _nreduce);
		else {
		    cerr << "Unknown slack value:" << _slack[i] << endl;
		    exit(EXIT_FAILURE);
		}
	    } else {  // use the number of deadline violations
		for (size_t j = 0; j < it->jobs.size(); ++j) {
		    if (double_equal(_job_ftime[it->jobs[j].id], 0))
			_job_ftime[it->jobs[j].id] = it->jobs[j].ftime;
		    else {
			if (_slack[i] < 1.0) {
			    // slack is in percentage,
			    // slack computed based on job processing time
			    jd += it->jobs[j].ftime > _job_ftime[it->jobs[j].id] +
				_slack[i] * (_job_ftime[it->jobs[j].id] - it->jobs[j].ctime);
			} else
			    jd += it->jobs[j].ftime > _job_ftime[it->jobs[j].id] + _slack[i];
		    }
		}
	    }
	    gsl_vector_set(y, i, jd);
	    fprintf(_fp_traj, "%lf%c", jd,
		    i == npool - 1? '\n': ' ');
	}

	fflush(_fp_traj);
    }

    Config _conf;
    job_tracker * _jt;
    FILE *_fp_traj;

    int _nmap;
    int _nreduce;
    string _workload;
    string _traj_file;

    int _niter;
    int _bs;
    double _deg;
    double _alpha;
    double _lambda;
    double _tau;
    bool   _strict;
    vector<double> _slack;
    open_hash_map<uint64_t,double> _job_ftime;
};

int main()
{
    problem inst(CONFIG_FILE);
    umbra solver;

    srand(time(NULL));

    int nvar, nobj;

    nvar = inst.var_count();
    nobj = inst.obj_count();

    cerr << nvar << " variables and " << nobj << " objective functions" << endl;
    int niter     = inst.get_niter();
    int bs        = inst.get_bs();
    double deg    = inst.get_deg();
    double alpha  = inst.get_alpha();
    double lambda = inst.get_lambda();
    double tau    = inst.get_bandwidth();
    bool strict   = inst.get_strict();

    cerr << "Running " << niter << " iterations," << endl
         << "    Batch size  : " << bs << endl
         << "    Exploration : " << deg << endl
         << "    Alpha       : " << alpha << endl
         << "    Lambda      : " << lambda << endl
         << "    Bandwidth   : " << tau << endl
         << "    Strict      : " << strict << endl;

    gsl_vector *x0 = gsl_vector_alloc(nvar);
    if (x0 == NULL) {
	fprintf(stderr, "failed to alloc x0\n");
	exit(EXIT_FAILURE);
    }
    gsl_matrix *fval = gsl_matrix_alloc(nobj, niter);
    if (fval == NULL) {
	fprintf(stderr, "failed tp alloc fval matrix\n");
	exit(EXIT_FAILURE);
    }
    gsl_matrix *xval = gsl_matrix_alloc(niter, nvar);
    if (xval == NULL) {
	fprintf(stderr, "failed tp alloc xval matrix\n");
	exit(EXIT_FAILURE);
    }

    inst.get_config(x0);
    gsl_vector *ub = gsl_vector_alloc(nobj);
    inst.proj(x0);
    inst.eval(x0, ub);
    
    solver(&inst, x0, niter, bs, deg, alpha, lambda, tau, xval, fval);
    //solver(&inst, x0, niter, bs, deg, alpha, lambda, tau, ub, xval, fval);

    cerr << "Optimization completed, trajectory has been saved" << endl;

    gsl_vector_free(ub);
    gsl_vector_free(x0);
    gsl_matrix_free(xval);
    gsl_matrix_free(fval);

    return 0;
}
