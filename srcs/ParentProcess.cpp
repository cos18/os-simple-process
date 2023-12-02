#include "scheduler.hpp"

ParentProcess::~ParentProcess(void) {
	if (getpid() == this->pid) {
		msg_load msg;

		for (int i = 0; i < PROCESS_NUM; i++) {
			msg.mtype = 1;
			msg.send_pid = 0;
			msg.type = TYPE_KILL_PROCESS;
			msgsnd(this->plist[i].getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		}
		this_thread::sleep_for(chrono::milliseconds(100));
		msgctl(this->cpu_msg_id, IPC_RMID, 0);
		msgctl(this->io_msg_id, IPC_RMID, 0);
		msgctl(this->log_msg_id, IPC_RMID, 0);
		delete[] this->plist;
		this->log_file_stream.close();
	}
}

void ParentProcess::init(int argc, char **argv) {
	if (argc < 3 || argc > 4) throw ParentProcess::ParamException();

	this->time_quantum = atoi(argv[1]);
	this->time_log = atoi(argv[2]);
	if (this->time_quantum <= 0 || this->time_log <= 0)
		throw ParentProcess::ParamException();
	this->time_unit = 1000;
	if (argc == 4)
		this->time_unit = atoi(argv[3]);
	if (this->time_unit < 100)
		throw ParentProcess::ParamException();

	this->gtimer = 0;
	this->pid = getpid();

	this->cpu_msg_id = msgget((key_t)(MSG_ID_CPU), IPC_CREAT|0666);
	this->curr_cpu_burst = NULL;

	this->io_msg_id = msgget((key_t)(MSG_ID_IO), IPC_CREAT|0666);
	this->curr_io_burst = NULL;

	this->plist = new ChildProcess[10];

	this->log_file_stream.open("schedule_dump.txt", ios_base::trunc);
	this->log_msg_id = msgget((key_t)(MSG_ID_LOG), IPC_CREAT|0666);

	for (int idx = 0; idx < PROCESS_NUM; idx++) {
		this->plist[idx].setParentMsgId(idx, this->cpu_msg_id, this->io_msg_id, this->log_msg_id);
		this->ready_queue.push(this->plist + idx);
		this->plist[idx].setState(STATE_READY);
		this->plist[idx].startProcess();
	}
}

void ParentProcess::run(void) {
	struct itimerval it_val;

	this->listener();

	it_val.it_value.tv_sec = this->time_unit / 1000;
	it_val.it_value.tv_usec = (this->time_unit * 1000) % 1000000;
	it_val.it_interval = it_val.it_value;
	if (setitimer(ITIMER_REAL, &it_val, NULL) == -1) {
		perror("error calling setitimer()");
		exit(1);
	}
	while (1) {}
}

void ParentProcess::listener(void) {
	cout << "Parent are running at " << this->gtimer << " tick (" << this->gtimer * this->time_unit << "ms)\n";

	this->cleanup();
	this->manageCPU();
	this->manageIO();

	if (this->curr_cpu_burst == NULL && this->curr_io_burst == NULL
		&& this->ready_queue.empty() && this->io_queue.empty()) {
		cout << "END!!\n";
		exit(0);
	}

	if (this->gtimer % this->time_log == 0)
		this->writeLog();

	this->gtimer++;
}

void ParentProcess::cleanup(void) {
	msg_load msg;

	msg.type = TYPE_CHILD_READY;
	ssize_t msg_size = msgrcv(this->cpu_msg_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, IPC_NOWAIT);
	if (msg_size > 0 || (this->curr_cpu_burst && (this->curr_cpu_quantum == this->time_quantum))) {
		switch (msg.type) {
			case TYPE_CHILD_END:
				msg.mtype = 1;
				msg.send_pid = 0;
				msg.type = TYPE_TERMINATE_CHILD;
				msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
				break;
			case TYPE_CHILD_IO_INTERRUPT:
				this->io_queue.push(this->curr_cpu_burst);
				msg.mtype = 1;
				msg.send_pid = 0;
				msg.type = TYPE_CHILD_WAITING;
				msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
				break;
			default:
				this->ready_queue.push(this->curr_cpu_burst);
				msg.mtype = 1;
				msg.send_pid = 0;
				msg.type = TYPE_CHILD_READY;
				msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		}
		this->curr_cpu_burst = NULL;
		this->curr_cpu_quantum = 0;
	}

	if (msgrcv(this->io_msg_id, &msg, sizeof(msg) - sizeof(msg.mtype), 1, IPC_NOWAIT) > 0) {
		this->ready_queue.push(this->curr_io_burst);
		msg.mtype = 1;
		msg.send_pid = 0;
		msg.type = TYPE_CHILD_READY;
		msgsnd(this->curr_io_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		this->curr_io_burst = NULL;
	}
}

void ParentProcess::manageCPU(void) {
	msg_load msg;

	if (!this->ready_queue.empty() && this->curr_cpu_burst == NULL) {
		this->curr_cpu_burst = this->ready_queue.front();
		this->ready_queue.pop();
	}

	if (this->curr_cpu_burst) {
		msg.mtype = 1;
		msg.send_pid = 0;
		msg.type = TYPE_RUN_CPU_PROCESS;
		msgsnd(this->curr_cpu_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
		this->curr_cpu_quantum++;
	}
}

void ParentProcess::manageIO(void) {
	msg_load msg;

	if (!this->io_queue.empty() && this->curr_io_burst == NULL) {
		this->curr_io_burst = this->io_queue.front();
		this->io_queue.pop();
	}

	if (this->curr_io_burst) {
		msg.mtype = 1;
		msg.send_pid = 0;
		msg.type = TYPE_RUN_IO_PROCESS;
		msgsnd(this->curr_io_burst->getChildMsgId(), &msg, sizeof(msg) - sizeof(msg.mtype), 0);
	}
}

void ParentProcess::writeLog(void) {
	for (int i = 0; i < PROCESS_NUM; i++)
		this->plist[i].update(this->log_msg_id);

	this->log_file_stream << "[" << this->gtimer << "] LOG START" << endl;
	if (this->curr_cpu_burst)
		this->log_file_stream << "[" << this->gtimer << "] CURRENT RUNNING PROCESS : " << *(this->curr_cpu_burst) << endl;
	else
		this->log_file_stream << "[" << this->gtimer << "] CURRENT RUNNING PROCESS : NONE" << endl;

	if (this->ready_queue.empty())
		this->log_file_stream << "[" << this->gtimer << "] READY QUEUE IS EMPTY" << endl;
	else {
		this->log_file_stream << "[" << this->gtimer << "] READY QUEUE LIST" << endl;
		ChildProcess *start = this->ready_queue.front();
		do {
			this->log_file_stream << *(this->ready_queue.front()) << endl;
			this->ready_queue.push(this->ready_queue.front());
			this->ready_queue.pop();
		} while (start != this->ready_queue.front());
	}

	if (this->curr_io_burst)
		this->log_file_stream << "[" << this->gtimer << "] CURRENT IO PROCESS : " << *(this->curr_io_burst) << endl;
	else
		this->log_file_stream << "[" << this->gtimer << "] CURRENT IO PROCESS : NONE" << endl;

	if (this->io_queue.empty())
		this->log_file_stream << "[" << this->gtimer << "] IO QUEUE IS EMPTY" << endl;
	else {
		ChildProcess *start = this->io_queue.front();
		do {
			this->log_file_stream << *(this->io_queue.front()) << endl;
			this->io_queue.push(this->io_queue.front());
			this->io_queue.pop();
		} while (start != this->io_queue.front());
	}

	this->log_file_stream << "[" << this->gtimer << "] CURRENT ALL PROCESS" << endl;
	for (int i = 0; i < PROCESS_NUM; i++)
		this->log_file_stream << this->plist[i] << endl;
	this->log_file_stream << endl;
}

const char *ParentProcess::ParamException::what() const throw() {
	return "ParamException: Wrong Parameter!";
}