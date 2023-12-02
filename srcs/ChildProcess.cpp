#include "scheduler.hpp"

ChildProcess::ChildProcess(void) {
	this->state = STATE_NEW;
	this->cpu_dur = rand() % 5 + 6;
	// this->cpu_dur = 2;
	this->cpu_dur_left = this->cpu_dur;

	this->is_io = false;
	this->io_start_time = -1;
	this->io_dur = -1;
	this->io_dur_left = -1;
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
	msg_load msg;
	msg.mtype = 1;
	while (1) {
		if (msgrcv(this->child_recv_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, 0) != -1) {
			switch (msg.type) {
				case TYPE_RUN_CPU_PROCESS:
					if (this->cpu_dur == this->cpu_dur_left) {
						srand(time(NULL));
						if (rand() % 10 < 5) {
							this->is_io = true;
							this->io_start_time = rand() % (this->cpu_dur - 1) + 1;
							this->io_dur = rand() % 3 + 1;
							this->io_dur_left = this->io_dur;
						}
					}
					this->cpu_dur_left--;
					if (this->is_io && (this->cpu_dur - this->cpu_dur_left) == this->io_start_time) {
						msg.mtype = 1;
						msg.send_pid = getpid();
						msg.type = TYPE_CHILD_IO_INTERRUPT;
						msgsnd(this->parent_cpu_send_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);
					}
					if (this->cpu_dur_left == 0) {
						msg.mtype = 1;
						msg.send_pid = getpid();
						msg.type = TYPE_CHILD_END;
						msgsnd(this->parent_cpu_send_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);
					}
					break;
				case TYPE_RUN_IO_PROCESS:
					this->io_dur_left--;
					if (this->io_dur_left == 0) {
						msg.mtype = 1;
						msg.send_pid = getpid();
						msg.type = TYPE_CHILD_IO_END;
						msgsnd(this->parent_io_send_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);
					}
					break;
				case TYPE_CLEAR_PROCESS:
					exit(0);
				default:
					break;
			}
		}
	}
}

ostream &operator<<(ostream &ost, ChildProcess &pos) {
	ost << "[ChildProcess] pid " << pos.pid << " / cpu_dur " << pos.cpu_dur;
	return ost;
}
