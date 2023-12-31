#include "paging.hpp"

ParentProcess::~ParentProcess(void) {
	if (getpid() == this->pid) {
		msg_load msg;

		for (int i = 0; i < PROCESS_NUM; i++) {
			msg.mtype = 1;
			msg.send_pid = 0;
			msg.type = TYPE_KILL_PROCESS;
			msgsnd(this->plist[i].getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		}
		for (int i = 0; i < PROCESS_NUM; i++) {
			waitpid(this->plist[i].getPID(), NULL, 0);
		}
		msgctl(this->cpu_msg_id, IPC_RMID, 0);
		msgctl(this->log_msg_id, IPC_RMID, 0);
		delete[] this->plist;
		this->log_file_stream.close();
	}
}

void ParentProcess::init(int argc, char **argv) {
	if (argc != 2) throw ParentProcess::ParamException();

	this->time_quantum = atoi(argv[1]);
	if (this->time_quantum <= 0)
		throw ParentProcess::ParamException();

	this->gtimer = 0;
	this->pid = getpid();

	this->cpu_msg_id = msgget((key_t)(MSG_ID_CPU), IPC_CREAT|0666);
	this->curr_cpu_burst = NULL;

	this->plist = new ChildProcess[10];

	this->log_file_stream.open("paging_dump.txt", ios_base::trunc);
	this->log_msg_id = msgget((key_t)(MSG_ID_LOG), IPC_CREAT|0666);

	int nd_max = BACKING_STORE_PAGE_SIZE - PAGE_TABLE_SIZE, start_idx;
	random_device rd;
	mt19937 gen(rd());
	normal_distribution<double> d(nd_max / 2.0, nd_max / 8.0);

	for (int idx = 0; idx < PROCESS_NUM; idx++) {
		this->plist[idx].setParentMsgId(idx, this->cpu_msg_id, this->log_msg_id);
		this->ready_queue.push(this->plist + idx);
		this->plist[idx].setState(STATE_READY);
		do {
			start_idx = std::round(d(gen));
		} while (start_idx < 0 || start_idx >= nd_max);
		this->plist[idx].setLogicalMemoryStartIdx(start_idx);
		this->plist[idx].startProcess();
	}
}

void ParentProcess::run(void) {
	struct itimerval it_val;

	this->listener();

	it_val.it_value.tv_sec = TIME_UNIT / 1000;
	it_val.it_value.tv_usec = (TIME_UNIT * 1000) % 1000000;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	while (1) {}
}

void ParentProcess::listener(void) {
	cout << "Parent are running at " << this->gtimer << " tick (" << this->gtimer * TIME_UNIT << "ms)\n";

	this->log_file_stream << "[" << this->gtimer << "] LOG START" << endl;
	this->cleanup();
	this->manageCPU();

	if (this->gtimer == SIMULATE_TIME_TICK) {
		cout << "END!!\n";
		this->pm.storeAllMemory();
		exit(0);
	}

	this->log_file_stream << "[" << this->gtimer << "] LOG END" << endl << endl;
	this->gtimer++;
}

void ParentProcess::cleanup(void) {
	msg_load msg;
	bool is_valid = true;

	msg.type = TYPE_CHILD_READY;
	ssize_t msg_size = msgrcv(this->cpu_msg_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, IPC_NOWAIT);
	if (msg_size > 0 && (msg.type == TYPE_PAGE_REQUEST)) {
		this->log_file_stream << "[" << this->gtimer << "] Receive PageTable request at " << msg.send_pid << endl;
		this->log_file_stream << "PageTable request: ";
		for (int idx = 0; idx < MEMORY_ACCESS_REQUEST_SIZE; idx++) {
			this->log_file_stream << msg.page_idx[idx] << " ";
		}
		this->log_file_stream << endl;
		is_valid = this->curr_cpu_burst->pt.checkPageValid(msg, this->pm, this->log_file_stream);
		msg.mtype = 1;
		msg.send_pid = 0;
		msg.type = is_valid ? TYPE_PAGE_HIT : TYPE_PAGE_FAULT;
		msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		if (!is_valid) {
			this->ready_queue.push(this->curr_cpu_burst);
			this->curr_cpu_burst = NULL;
			this->curr_cpu_quantum = 0;
		}
	}
	if (this->curr_cpu_quantum == this->time_quantum) {
		this->ready_queue.push(this->curr_cpu_burst);
		msg.mtype = 1;
		msg.send_pid = 0;
		msg.type = TYPE_CHILD_READY;
		msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		this->curr_cpu_burst = NULL;
		this->curr_cpu_quantum = 0;
	}
}

void ParentProcess::manageCPU(void) {
	msg_load msg;

	if (!this->ready_queue.empty() && this->curr_cpu_burst == NULL) {
		this->curr_cpu_burst = this->ready_queue.front();
		this->ready_queue.pop();
	}

	msg.mtype = 1;
	msg.send_pid = 0;
	msg.type = TYPE_RUN_CPU_PROCESS;
	msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
	this->curr_cpu_quantum++;
}

const char *ParentProcess::ParamException::what() const throw() {
	return "ParamException: Wrong Parameter!";
}