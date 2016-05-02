#include <time.h>
#include <Tempo/tempo.hpp>

int main()
{
	Tempo::job j1;
        Tempo::job j2;

	Tempo::task t1;
	t1.id = 1;
	t1.ctime = 0;
	t1.ptime = 3;
	t1.stime = -1;
	t1.ftime = -1;
	t1.type = Tempo::task::TASK_TYPE_MAP;


	Tempo::task t2;
	t2.id = 2;
	t2.ctime = 0;
	t2.ptime = 3;
	t2.stime = -1;
	t2.ftime = -1;
	t2.type = Tempo::task::TASK_TYPE_MAP;

	Tempo::task t3;
	t3.id = 3;
	t3.ctime = 1;
	t3.ptime = 2;
	t3.stime = -1;
	t3.ftime = -1;
	t3.type = Tempo::task::TASK_TYPE_MAP;

	Tempo::task t4;
	t4.id = 4;
	t4.ctime = 1;
	t4.ptime = 2;
	t4.stime = -1;
	t4.ftime = -1;
	t4.type = Tempo::task::TASK_TYPE_MAP;

	j1.id = 1;
	j1.fs_ctx_map.uid = 1;
	j1.tasks[Tempo::task::TASK_TYPE_MAP].push_back(t1);
	j1.tasks[Tempo::task::TASK_TYPE_MAP].push_back(t2);

	j2.id = 2;
	j2.fs_ctx_map.uid = 2;
	j2.tasks[Tempo::task::TASK_TYPE_MAP].push_back(t3);
	j2.tasks[Tempo::task::TASK_TYPE_MAP].push_back(t4);

	Tempo::job_tracker jt(2, 0);

	Tempo::pool &mod  = jt.add_pool("modeling", 1, 1, 1, 1, 0, Tempo::pool::SCHED_FAIR);
	Tempo::pool &prod = jt.add_pool("prod",     1, 1, 1, 1, 0, Tempo::pool::SCHED_FAIR);

	mod.add_job(j1);
	prod.add_job(j2);

	jt.process();

	printf("------------ WORKLOAD ------------\n");
	printf("%s\n", mod.to_str().c_str());
	printf("%s\n", prod.to_str().c_str());
	printf("----------------------------------\n");

        return 0;
}
