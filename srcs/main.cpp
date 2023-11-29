#include "scheduler.hpp"

ParentProcess parent;

#define INTERVAL 500

void alarmListener(void) {
	parent.listener();
}

int main(int argc, char **argv) {
	srand(time(NULL));
	signal(SIGALRM, (void (*)(int))alarmListener);

	try {
		parent.init(argc, argv);
	} catch (const ParentProcess::ParamException &e) {
		cerr << e.what() << endl << endl;
		printInfo();
		exit(1);
	}


	struct itimerval it_val;  /* for setting itimer */

	it_val.it_value.tv_sec =     INTERVAL/1000;
	it_val.it_value.tv_usec =    (INTERVAL*1000) % 1000000;   
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
	perror("error calling setitimer()");
	exit(1);
	}
	// parent.run();
	while (1)  {

	}
}