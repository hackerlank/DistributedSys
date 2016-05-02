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

#ifndef _COLOSSAL_HELPER_H
#define _COLOSSAL_HELPER_H

#include "task.hpp"
#include "pool.hpp"
#include "common.hpp"
#include "job_tracker.hpp"

namespace Tempo {

struct ut_pt {
	double time;

	ut_pt() { }

	ut_pt(double t) : time(t) { }

	bool operator> (const ut_pt &other) const
	{
		double a = time < 0? -time: time;
		double b = other.time < 0? -other.time: other.time;
		return a > b;
	}
};

DEFINE_HEAP(ut, ut_pt, std::greater<ut_pt>());

// Compute cluster utilization
double compute_utilization(
	const job_tracker::pool_container_type &pools,
	task::task_type type, int nslots);

// Compute cluster utilization of the pool
double compute_utilization(const pool &p, task::task_type type, int nslots);

// Show the progress of job processing
void show_progress(double map, double reduce);

void print_pool_settings(const job_tracker::pool_container_type &pools);

// Import from workload generated by the parser, where each line is in the format:
// POOL"\t"JOB:PRIORITY"\t"TASK"\t"<MAP|REDUCE>"\t"CTIME"\t"STIME"\t"FTIME
int import_workload(const char *file, job_tracker::pool_container_type *pools);

// Import from workload generated by the parser, where each line is in the format:
// POOL"\t"JOB:PRIORITY"\t"TASK"\t"<MAP|REDUCE>"\t"CTIME"\t"PTIME
int import_workload1(const char *file, job_tracker::pool_container_type *pools);

int export_schedule(const char *file, const job_tracker::pool_container_type &pools);

}

#endif
