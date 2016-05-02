#include <time.h>
#include <Tempo/tempo.hpp>

int main()
{
        Tempo::job_generator gen1(0.01, 3, 1, 0.6, 0, 0.1, 0.1, 0, 0);
        gen1.seed(time(NULL));

	Tempo::job j1 = gen1();

	Tempo::job_tracker jt(1, 1);

	Tempo::pool &mod  = jt.add_pool("modeling", 10, 10, 1, 1, 1, Tempo::pool::SCHED_FAIR);

	mod.add_job(j1);

	jt.process();

	printf("------------ WORKLOAD ------------\n");
	printf("%s\n", mod.to_str().c_str());
	printf("----------------------------------\n");

        return 0;
}
