#include "paging.hpp"

ChildProcess::ChildProcess(void) {
	this->state = STATE_NEW;
	this->operation_cnt = 0;
	this->is_request_pending = false;
}

const int& ChildProcess::getPID(void) {
	return this->pid;
}

const int& ChildProcess::getChildMsgId(void) {
	return this->child_recv_id;
}

const e_state& ChildProcess::getState(void) {
	return this->state;
}

void ChildProcess::setParentMsgId(int idx, int cpu_id, int log_id) {
	this->child_recv_id = msgget((key_t)(MSG_ID_CHILDS + idx), IPC_CREAT|0666);
	this->parent_cpu_send_id = cpu_id;
	this->parent_log_send_id = log_id;
}

void ChildProcess::setState(e_state state) {
	this->state = state;
}

void ChildProcess::setLogicalMemoryStartIdx(unsigned short start_idx) {
	this->logical_memory_start_idx = start_idx;
}

void ChildProcess::startProcess(void) {
	this->pid = fork();

	if (this->pid == 0) {
		this->gen = mt19937(this->rd());
		this->nd_page = normal_distribution<>(
			this->logical_memory_start_idx + PAGE_TABLE_SIZE / 2.0,
			PAGE_TABLE_SIZE / 8.0
		);
		this->watch();
	} else {
		this->pt = PageTable(this->logical_memory_start_idx);
	}
}

void ChildProcess::watch(void) {
	msg_load		msg;

	while (1) {
		if (msgrcv(this->child_recv_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, 0) != -1) {
			switch (msg.type) {
				case TYPE_RUN_CPU_PROCESS:
					this->runCPUBurst();
					break;
				case TYPE_PAGE_FAULT:
					this->state = STATE_WAITING;
					this->is_request_pending = true;
					break;
				case TYPE_PAGE_HIT:
					this->runOperation(msg);
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
	msg_load		msg;
	set<unsigned short>	page_idx_set;

	this->state = STATE_RUNNING;

	if (!(this->is_request_pending)) {
		while (page_idx_set.size() < 10) {
			double number = round(this->nd_page(this->gen));
			if (number < this->logical_memory_start_idx || number >= this->logical_memory_start_idx + PAGE_TABLE_SIZE)
				continue;
			page_idx_set.insert(number);
		}
		srand(time(NULL));
		auto it = page_idx_set.begin();
		for (int idx = 0; idx < 10; idx++) {
			this->curr_request_va[idx].page_number = *it;
			this->curr_request_va[idx].page_offset = rand() % PAGE_SIZE;
			it++;
		}
	}

	msg.mtype = 1;
	msg.send_pid = getpid();
	for (int idx = 0; idx < 10; idx++)
		msg.page_idx[idx] = this->curr_request_va[idx].page_number;
	msg.type = TYPE_PAGE_REQUEST;
	msgsnd(this->parent_cpu_send_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0);
}

void ChildProcess::runOperation(msg_load &msg) {
	this->operation_cnt++;
	this->is_request_pending = false;
	(void)msg;
	// TODO: Run Operation
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
	this->operation_cnt = msg_child.operation_cnt;
	this->state = msg_child.state;
}

void ChildProcess::sendParentInfo(void) {
	msg_child_info	msg_child;

	msg_child.mtype = 1;
	msg_child.send_pid = this->pid;
	msg_child.operation_cnt = this->operation_cnt;
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
	ost << " / Logical Memory Start idx " << pos.logical_memory_start_idx;
	ost << " / operation_cnt " << pos.operation_cnt;
	return ost;
}
