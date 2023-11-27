#include "scheduler.hpp"

int main(int argc, char **argv) {
	try {
		ParentProcess parent(argc, argv);
		parent.run();
	} catch (const ParentProcess::ParamException &e) {
		cerr << e.what() << endl << endl;
		print_info();
	}
}