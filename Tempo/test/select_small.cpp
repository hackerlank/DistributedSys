#include <time.h>
#include <Tempo/tempo.hpp>

int main()
{
        Tempo::job_generator gen(0.01, 5, 80, 0.7, 1, 1, 1, 1, 1);
        gen.seed(time(NULL));
        Tempo::job j1 = gen();

        Tempo::job_generator gen1(0.1, 6, 80, 0.2, 1, 1, 1, 1, 1);
        gen1.seed(time(NULL) + 1);
        Tempo::job j2 = gen1();
        Tempo::job j3 = gen1();

	std::list<Tempo::pool> pools;
	pools.push_back(Tempo::pool("analyst", 200, 200, 1, 10, 10, Tempo::pool::SCHED_FAIR));
	pools.push_back(Tempo::pool("modeling", 2, 200, 1, 10, 10, Tempo::pool::SCHED_FAIR));

	pools.begin()->add_job(j1);
	pools.rbegin()->add_job(j2);
	pools.rbegin()->add_job(j3);

	Tempo::selector sel(pools.begin(), pools.end());

	Tempo::task_desc::ref *task;
	double t = 0;
	while (sel.has_map()) {
		ULIB_DEBUG("map min ctime=%f, @%f, popped=%lu, seen=%lu",
			   sel.map_min_ctime(), t, sel.maps_popped(), sel.maps_seen());
		sel.dump_seen_task_tree();
		task = sel.pop_map(t++);
		if (task == NULL)
			ULIB_DEBUG("No task chosen");
		else
			printf("%s\n", task->gettask()->to_str().c_str());
	}

	ULIB_DEBUG("Selected all maps ..., popped=%lu, seen=%lu", sel.maps_popped(), sel.maps_seen());

	while (sel.has_reduce()) {
		ULIB_DEBUG("reduce min ctime=%f, @%f, popped=%lu, seen=%lu",
			   sel.reduce_min_ctime(), t, sel.reduces_popped(), sel.reduces_seen());
		sel.dump_seen_task_tree();
		task = sel.pop_reduce(t++);
		if (task == NULL)
			ULIB_DEBUG("No task chosen");
		else
			printf("%s\n", task->gettask()->to_str().c_str());
	}

	sel.dump_seen_task_tree();

        return 0;
}
