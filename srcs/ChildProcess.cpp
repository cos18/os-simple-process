#include "scheduler.hpp"

ChildProcess::ChildProcess(void) {
	this->child_recv_id = msgget((key_t)rand(), IPC_CREAT|0666);
	this->state = STATE_NEW;
	this->cpu_dur = rand() % 10 + 6;
	this->cpu_dur_left = this->cpu_dur;

	printf("test\n");

	this->pid = fork();

	if (this->pid == 0) {
		// RUN IN PROCESS
		exit(0);
	}
}

const int& ChildProcess::getChildMsgId(void) {
	return this->child_recv_id;
}

const e_state& ChildProcess::getState(void) {
	return this->state;
}

void ChildProcess::setParentMsgId(int &cpu_id, int &io_id) {
	this->parent_cpu_send_id = cpu_id;
	this->parent_io_send_id = io_id;
}

void ChildProcess::setState(e_state state) {
	this->state = state;
}

ostream &operator<<(ostream &ost, ChildProcess &pos) {
	ost << "[ChildProcess] pid " << pos.pid << " / cpu_dur " << pos.cpu_dur;
	return ost;
}
