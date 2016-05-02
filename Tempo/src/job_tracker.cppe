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

#include "job_tracker.hpp"

namespace Tempo
{

job_tracker::job_tracker(int nmaps, int nreduces, double now)
{
	_eng = new engine(nmaps, nreduces, now);
}

job_tracker::~job_tracker()
{
	delete _eng;
}

bool job_tracker::set_metrics(const char * met, int met_win)
{
	return _eng->set_metrics(met, met_win);
}

pool & job_tracker::add_pool(const std::string &ns, double mto, double fto,
			     double weight, int minmap, int minred,
			     pool::sched_mode sched)
{
	return _eng->add_pool(ns, mto, fto, weight, minmap, minred, sched);
}

void job_tracker::process()
{
	_eng->process();
}

void job_tracker::reset_time(double now)
{
	_eng->time_now = now;
}

void job_tracker::scale_minshares()
{
	_eng->scale_minshares();
}

size_t job_tracker::running_maps() const
{
	return _eng->running_maps->size();
}

size_t job_tracker::running_reduces() const
{
	return _eng->running_reduces->size();
}

const job_tracker::pool_container_type &job_tracker::getpools() const
{
	return _eng->getpools();
}

job_tracker::pool_container_type &job_tracker::getpools()
{
	return _eng->getpools();
}

}
