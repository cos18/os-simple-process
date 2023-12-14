#include "paging.hpp"

ChildProcess::ChildProcess(void) {
	this->state = STATE_NEW;
	this->cpu_dur = rand() % (CPU_MAX_DUR - CPU_MIN_DUR + 1) + CPU_MIN_DUR;
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

void ChildProcess::setParentMsgId(int idx, int cpu_id, int io_id, int log_id) {
	this->child_recv_id = msgget((key_t)(MSG_ID_CHILDS + idx), IPC_CREAT|0666);
	this->parent_cpu_send_id = cpu_id;
	this->parent_io_send_id = io_id;
	this->parent_log_send_id = log_id;
}

void ChildProcess::setState(e_state state) {
	this->state = state;
}

void ChildProcess::setPageTable(unsigned short start_idx) {
	this->pt = PageTable(start_idx);
}

void ChildProcess::startProcess(void) {
	this->pid = fork();

	if (this->pid == 0)
		this->watch();
		
}

void ChildProcess::watch(void) {
	msg_load		msg;

	while (1) {
		if (msgrcv(this->child_recv_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, 0) != -1) {
			switch (msg.type) {
				case TYPE_RUN_CPU_PROCESS:
					this->runCPUBurst();
					break;
				case TYPE_RUN_IO_PROCESS:
					this->runIOBurst();
					break;
				case TYPE_CHILD_READY:
					this->state = STATE_READY;
					break;
				case TYPE_CHILD_WAITING:
					this->state = STATE_WAITING;
					break;
				case TYPE_TERMINATE_CHILD:
					this->state = STATE_TERMINATED;
					break;
				case TYPE_CHILD_INFO:
					this->sendParentInfo();
					break;
				case TYPE_KILL_PROCESS:
					msgctl(this->child_recv_id, IPC_RMID, 0);
					exit(0);
					break;
				default:
					break;
			}
		}
	}
}

void ChildProcess::runCPUBurst(void) {
	msg_load	msg;

	this->state = STATE_RUNNING;
	if (this->cpu_dur == this->cpu_dur_left) {
		srand(time(NULL));
		if (rand() % 100 < IO_PROBABILITY_PERCENTILE) {
			this->is_io = true;
			this->io_start_time = rand() % (this->cpu_dur - 1) + 1;
			this->io_dur = rand() % IO_MAX_DUR + 1;
			this->io_dur_left = this->io_dur;
		}
	}
	this->cpu_dur_left--;
	if (this->is_io && ((this->cpu_dur - this->cpu_dur_left) == this->io_start_time)) {
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
}

void ChildProcess::runIOBurst(void) {
	msg_load	msg;

	this->io_dur_left--;
	if (this->io_dur_left == 0) {
		msg.mtype = 1;
		msg.send_pid = getpid();
		msg.type = TYPE_CHILD_IO_END;
		msgsnd(this->parent_io_send_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);
	}
}

void ChildProcess::update(int log_msg_id) {
	msg_load		msg;
	msg_child_info	msg_child;
	int				size;

	msg.mtype = 1;
	msg.send_pid = 0;
	msg.type = TYPE_CHILD_INFO;
	
	msgsnd(this->child_recv_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);
	do {
		size = msgrcv(log_msg_id, &msg_child, sizeof(msg_child) - sizeof(msg_child.mtype), 1, 0);
	} while (size < 0);
	this->cpu_dur_left = msg_child.cpu_dur_left;
	this->is_io = msg_child.is_io;
	this->io_start_time = msg_child.io_start_time;
	this->io_dur = msg_child.io_dur;
	this->io_dur_left = msg_child.io_dur_left;
	this->state = msg_child.state;
}

void ChildProcess::sendParentInfo(void) {
	msg_child_info	msg_child;

	msg_child.mtype = 1;
	msg_child.send_pid = this->pid;
	msg_child.cpu_dur_left = this->cpu_dur_left;
	msg_child.is_io = this->is_io;
	msg_child.io_start_time = this->io_start_time;
	msg_child.io_dur = this->io_dur;
	msg_child.io_dur_left = this->io_dur_left;
	msg_child.state = this->state;
	msgsnd(this->parent_log_send_id, &msg_child, sizeof(msg_child) - sizeof(msg_child.mtype), 0);
}

ostream &operator<<(ostream &ost, ChildProcess &pos) {
	ost << "pid " << pos.pid << " / state ";
	switch (pos.state) {
		case STATE_NEW:
			ost << "NEW";
			break;
		case STATE_READY:
			ost << "READY";
			break;
		case STATE_RUNNING:
			ost << "RUNNING";
			break;
		case STATE_WAITING:
			ost << "WAITING";
			break;
		case STATE_TERMINATED:
			ost << "TERMINATED";
			break;
	}
	ost << " / cpu_dur " << pos.cpu_dur << " / cpu_dur_left ";
	ost << ((pos.state == STATE_RUNNING) ? pos.cpu_dur_left + 1 : pos.cpu_dur_left);
	if (pos.is_io)
		ost << " / io_dur " << pos.io_dur << " / io_dur_left " << pos.io_dur_left;
	return ost;
}
