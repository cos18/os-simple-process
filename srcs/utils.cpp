#include "scheduler.hpp"

void printInfo(void) {
	cout << "Usage : ./scheduler (time quantum) (logging time tick) (time unit - optional)\n\n";
	cout << "Example" << endl;
	cout << "./scheduler 3 1" << endl;
	cout << "=> Scheduler will run with 3 unit time quantum and write log with 1 unit interval";
	cout << "when time unit is not provided, default time unit is 1000ms (1 second)" << endl << endl;
	cout << "Example" << endl;
	cout << "./scheduler 3 1 10" << endl;
	cout << "=> Scheduler will run with 3 unit time quantum, write log with 1 unit interval";
	cout << "and time unit is 10ms." << endl;
	cout << "* Minimum time unit is 10ms" << endl;
}