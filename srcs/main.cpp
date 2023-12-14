#include "paging.hpp"

ParentProcess parent;

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

	parent.run();
}