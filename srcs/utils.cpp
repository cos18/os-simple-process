#include "scheduler.hpp"

void printInfo(void) {
	cout << "Usage : ./scheduler (time quantum) (logging time tick)\n\n";
	cout << "Example" << endl;
	cout << "./scheduler 3 1" << endl;
	cout << "=> Scheduler will run with 3 seconds time quantum ";
	cout << "and write log with 1 second interval" << endl;
}