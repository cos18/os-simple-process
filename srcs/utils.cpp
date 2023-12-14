#include "paging.hpp"

void printInfo(void) {
	cout << "Usage : ./paging (time quantum) (logging time tick)\n\n";
	cout << "Example" << endl;
	cout << "./paging 3 1" << endl;
	cout << "=> paging simulator will run with 3 unit time quantum and write log with 1 unit interval" << endl << endl;
	cout << "OR just run './paging reset' to reset backing store" << endl;
}

void checkBackingStore(void) {
	struct stat st;
	if (stat("backing_store", &st) == -1) {
		mkdir("backing_store", 0777);
	}
}

void resetBackingStore(void) {
	system("cd backing_store && rm -rf `ls | grep -v .gitignore`");
}
