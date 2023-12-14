#include "scheduler.hpp"

void printInfo(void) {
	cout << "Usage : ./paging (time quantum) (logging time tick)\n\n";
	cout << "Example" << endl;
	cout << "./paging 3 1" << endl;
	cout << "=> Scheduler will run with 3 unit time quantum and write log with 1 unit interval";
}