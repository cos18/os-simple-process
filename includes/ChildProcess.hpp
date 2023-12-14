#pragma once

#include "paging.hpp"

enum msg_type {
	TYPE_RUN_CPU_PROCESS,
	TYPE_PAGE_REQUEST,
	TYPE_PAGE_FAULT,
	TYPE_PAGE_HIT,
	TYPE_CHILD_READY,
	TYPE_CHILD_WAITING,
	TYPE_CHILD_END,
	TYPE_CHILD_INFO,
	TYPE_TERMINATE_CHILD,
	TYPE_KILL_PROCESS
};

typedef struct {
	long			mtype;
	int				send_pid;
	unsigned short	page_idx[MEMORY_ACCESS_REQUEST_SIZE];
	char*			page_ptr[MEMORY_ACCESS_REQUEST_SIZE];
	msg_type		type;
} msg_load;

enum e_state {
	STATE_NEW,
	STATE_READY,
	STATE_RUNNING,
	STATE_WAITING,
	STATE_TERMINATED
};

typedef struct {
	long						mtype;
	int							send_pid;
	unsigned short				operation_cnt;
	e_state						state;
} msg_child_info;

class ChildProcess {
	private:
		int						pid;
		int						parent_cpu_send_id;
		int						parent_log_send_id;
		int						child_recv_id;

		unsigned short			operation_cnt;

		unsigned short			logical_memory_start_idx;
		va						curr_request_va[10];
		bool					is_request_pending;

		random_device			rd;
		mt19937					gen;
		normal_distribution<>	nd_page;

	public:
		e_state					state;
		PageTable				pt;

		ChildProcess();

		const int&				getPID(void);
		const int&				getChildMsgId(void);
		const e_state&			getState(void);
		void					setParentMsgId(int idx, int cpu_id, int log_id);
		void					setState(e_state state);
		void					setLogicalMemoryStartIdx(unsigned short start_idx);

		void					startProcess(void);
		void					watch(void);
		void					runCPUBurst(void);
		void					runOperation(msg_load &msg);
		void					sendParentInfo(void);
		void					update(int log_msg_id);

		friend ostream&			operator<<(ostream &ost, ChildProcess &pos);
};
