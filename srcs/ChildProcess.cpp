#include "scheduler.hpp"

ChildProcess::ChildProcess(void) {
	this->child_send_id = msgget((key_t)rand(), IPC_CREAT|0666);
	this->child_recv_id = msgget((key_t)rand(), IPC_CREAT|0666);
	this->state = PSTATE_NEW;
	this->cpu_dur = rand() % 10 + 6;
	this->cpu_dur_left = this->cpu_dur;

	this->pid = fork();

	if (this->pid == 0) {
		// RUN IN PROCESS
		exit(0);
	}
}

ostream &operator<<(ostream &ost, ChildProcess &pos) {
	ost << "[ChildProcess] pid " << pos.pid << " / cpu_dur " << pos.cpu_dur;
	return ost;
}
