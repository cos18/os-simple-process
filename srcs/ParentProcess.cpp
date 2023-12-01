#include "scheduler.hpp"

ParentProcess::~ParentProcess(void) {
	if (getpid() == this->pid) {
		msgctl(this->cpu_msg_id, IPC_RMID, 0);
		msgctl(this->io_msg_id, IPC_RMID, 0);
		for (int i = 0; i < 10; i++) {
			msgctl(this->plist[i].getChildMsgId(), IPC_RMID, 0);
		}
		delete[] this->plist;
	}
}

void ParentProcess::init(int argc, char **argv) {
	if (argc != 3) throw ParentProcess::ParamException();

	this->time_quantum = atoi(argv[1]);
	this->time_log = atoi(argv[2]);
	if (this->time_quantum <= 0 || this->time_log <= 0)
		throw ParentProcess::ParamException();

	this->gtimer = 0;
	this->pid = getpid();

	this->cpu_msg_id = msgget((key_t)(MSG_ID_PARENT_CPU), IPC_CREAT|0666);
	this->curr_cpu_burst = NULL;

	this->io_msg_id = msgget((key_t)(MSG_ID_PARENT_IO), IPC_CREAT|0666);
	this->curr_io_burst = NULL;

	this->plist = new ChildProcess[10];

	for (int idx = 0; idx < 10; idx++) {
		this->plist[idx].setParentMsgId(idx, this->cpu_msg_id, this->io_msg_id);
		this->ready_queue.push(this->plist + idx);
		this->plist[idx].setState(STATE_READY);
		this->plist[idx].startProcess();
	}
}

void ParentProcess::run(void) {
	struct itimerval it_val;

	this->listener();

	it_val.it_value.tv_sec = 1;
	it_val.it_value.tv_usec = 0;   
	it_val.it_interval = it_val.it_value;
	it_val.it_interval.tv_sec = 1;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	while (1) {}
}

void ParentProcess::listener(void) {
	// running ready queue
	this->manageCPU();

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

void ParentProcess::manageCPU(void) {
	msg_load msg;

	ssize_t msg_size = msgrcv(this->cpu_msg_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, IPC_NOWAIT);
	if (msg_size > 0 || (this->curr_cpu_burst && (this->curr_cpu_quantum == this->time_quantum))) {
		if (msg_size <= 0)
			this->ready_queue.push(this->curr_cpu_burst);
		else {
			msg.mtype = 1;
			msg.send_pid = 0;
			msg.type = TYPE_CLEAR_PROCESS;
			msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		}
		this->curr_cpu_burst = NULL;
		this->curr_cpu_quantum = 0;
	}

	if (!this->ready_queue.empty() && this->curr_cpu_burst == NULL) {
		this->curr_cpu_burst = this->ready_queue.front();
		this->ready_queue.pop();
	}

	if (this->curr_cpu_burst) {
		msg.mtype = 1;
		msg.send_pid = 0;
		msg.type = TYPE_RUN_CPU_PROCESS;
		msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		cout << "RUNNING " << *(this->curr_cpu_burst) << endl;
		this->curr_cpu_quantum++;
	}
}

const char *ParentProcess::ParamException::what() const throw() {
	return "ParamException: Wrong Parameter!";
}