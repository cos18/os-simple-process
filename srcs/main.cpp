#include "scheduler.hpp"

ParentProcess parent;

void alarmListener(void) {
	parent.listener();
}

int main(int argc, char **argv) {
	struct itimerval it_val;
	srand(time(NULL));
	signal(SIGALRM, (void (*)(int))alarmListener);

	try {
		parent.init(argc, argv);
	} catch (const ParentProcess::ParamException &e) {
		cerr << e.what() << endl << endl;
		printInfo();
		exit(1);
	}

	it_val.it_value.tv_sec = 1;
	it_val.it_value.tv_usec = 0;   
	it_val.it_interval = it_val.it_value;
	it_val.it_interval.tv_sec = 1;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	while (1) {}
}