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
	long				mtype;
	int					send_pid;
	unsigned short		cpu_dur_left;
	bool				is_io;
	unsigned short		io_start_time;
	unsigned short		io_dur;
	unsigned short		io_dur_left;
	e_state				state;
} msg_child_info;

class ChildProcess {
	private:
		int				pid;
		int				parent_cpu_send_id;
		int				parent_io_send_id;
		int				parent_log_send_id;
		int				child_recv_id;

		unsigned short	cpu_dur;
		unsigned short	cpu_dur_left;

		bool			is_io;
		unsigned short	io_start_time;
		unsigned short	io_dur;
		unsigned short	io_dur_left;

		PageTable		pt;

	public:
		e_state	state;

		ChildProcess();

		const int&		getChildMsgId(void);
		const e_state&	getState(void);
		void			setParentMsgId(int idx, int cpu_id, int io_id, int log_id);
		void			setState(e_state state);
		void			setPageTable(unsigned short start_idx);

		void			startProcess(void);
		void			watch(void);
		void			runCPUBurst(void);
		void			runIOBurst(void);
		void			sendParentInfo(void);
		void			update(int log_msg_id);

		friend ostream&	operator<<(ostream &ost, ChildProcess &pos);
};
