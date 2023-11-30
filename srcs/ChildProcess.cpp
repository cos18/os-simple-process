#include "scheduler.hpp"

ChildProcess::ChildProcess(void) {
	this->state = STATE_NEW;
	this->cpu_dur = 2;
	this->cpu_dur_left = this->cpu_dur;

	this->is_io = false;
	this->io_start_time = 1;
	this->io_dur = 1;
	this->io_dur_left = 1;
}

const int& ChildProcess::getChildMsgId(void) {
	return this->child_recv_id;
}

const e_state& ChildProcess::getState(void) {
	return this->state;
}

void ChildProcess::setParentMsgId(int idx, int &cpu_id, int &io_id) {
	this->child_recv_id = msgget((key_t)(MSG_ID_CHILDS + idx), IPC_CREAT|0666);
	this->parent_cpu_send_id = cpu_id;
	this->parent_io_send_id = io_id;
}

void ChildProcess::setState(e_state state) {
	this->state = state;
}

void ChildProcess::startProcess(void) {
	this->pid = fork();

	if (this->pid == 0)
		this->watch();
}

void ChildProcess::watch(void) {
	msgbuf msg;
	msg.mtype = 1;
	while (1) {
		if (msgrcv(this->child_recv_id, &msg, sizeof(msg), 1, 0) != -1) {
			switch (msg.type) {
				case TYPE_RUN_CPU_PROCESS:
					if (this->cpu_dur == this->cpu_dur_left) {
						// setting io
					}
					this->cpu_dur_left--;
					if (this->cpu_dur_left == 0) {
						msg.mtype = 1;
						msg.send_pid = getpid();
						msg.type = TYPE_CHILD_END;
						msgsnd(this->parent_cpu_send_id, &msg, sizeof(msg), 0);
						exit(0);
					}
				default:
					break;
			}
		}
	}
}

ostream &operator<<(ostream &ost, ChildProcess &pos) {
	ost << "[ChildProcess] pid " << pos.pid << " / cpu_dur " << pos.cpu_dur << " / child_recv_id " << pos.child_recv_id;
	return ost;
}
