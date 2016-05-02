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

#include <cstdio>
#include <cstring>
#include <ulib/hash_func.h>
#include "job.hpp"

namespace Tempo
{

std::string job::to_str(const char *prefix) const
{
        char buf[1024];

        snprintf(buf, sizeof(buf), "%016lx,%lf", id, ctime);
        std::string s = buf;
        for (std::vector<task>::const_iterator it = tasks[task::TASK_TYPE_MAP].begin();
             it != tasks[task::TASK_TYPE_MAP].end(); ++it)
                s += "\n" + std::string(prefix) + "\t" + it->to_str();
        for (std::vector<task>::const_iterator it = tasks[task::TASK_TYPE_REDUCE].begin();
             it != tasks[task::TASK_TYPE_REDUCE].end(); ++it)
                s += "\n" + std::string(prefix) + "\t" + it->to_str();

        return s;
}

uint64_t job::id_from_str(const char *str)
{
	return hash_fast64(str, strlen(str), 0xfeedbeefdeedbeefull);
}

}
