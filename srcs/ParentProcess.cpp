#include "scheduler.hpp"

ParentProcess::ParentProcess(int argc, char **argv) {
    if (argc != 3) throw ParentProcess::ParamException();

    this->time_quantum = atoi(argv[1]);
    this->time_log = atoi(argv[2]);
    if (this->time_quantum <= 0 || this->time_log <= 0)
        throw ParentProcess::ParamException();
}

void ParentProcess::run(void) {
    cout << "RUN";
}

const char *ParentProcess::ParamException::what() const throw() {
	return "ParamException: Wrong Parameter!";
}