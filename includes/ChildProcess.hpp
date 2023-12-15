#pragma once

#include "paging.hpp"

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
