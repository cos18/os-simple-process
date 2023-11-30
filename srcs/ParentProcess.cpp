#include "scheduler.hpp"

ParentProcess::ParentProcess() {}

void ParentProcess::init(int argc, char **argv) {
	if (argc != 3) throw ParentProcess::ParamException();

	this->time_quantum = atoi(argv[1]);
	this->time_log = atoi(argv[2]);
	if (this->time_quantum <= 0 || this->time_log <= 0)
		throw ParentProcess::ParamException();
	this->gtimer = 0;

	this->cpu_msg_id = msgget((key_t)rand(), IPC_CREAT|0666);
	this->curr_cpu_burst = NULL;

	this->io_msg_id = msgget((key_t)rand(), IPC_CREAT|0666);
	this->curr_io_burst = NULL;

	this->plist = new ChildProcess[10];

	for (int idx = 0; idx < 10; idx++) {
		this->plist[idx].setParentMsgId(this->cpu_msg_id, this->io_msg_id);
		this->ready_queue.push(this->plist + idx);
		this->plist[idx].setState(STATE_READY);
	}
}

ParentProcess::~ParentProcess(void) {
	delete[] this->plist;
}

void ParentProcess::listener(void) {
	// running ready queue
	this->ready_queue.pop();
	// running io queue

	// logging

	// ALL PROCESS CLEAR
	if (this->curr_cpu_burst == NULL && this->curr_io_burst == NULL
		&& this->ready_queue.empty() && this->io_queue.empty()) {
		printf("END!!\n");
		exit(0);
	}
	this->gtimer++;
}

const char *ParentProcess::ParamException::what() const throw() {
	return "ParamException: Wrong Parameter!";
}