#pragma once

enum e_state {
	STATE_NEW,
	STATE_READY,
	STATE_RUNNING,
	STATE_WAITING,
	STATE_TERMINATED
};

class ChildProcess {
	private:
		int				pid;
		int				parent_cpu_send_id;
		int				parent_io_send_id;
		int				child_recv_id;

		int				cpu_dur; // determined at the time of child process creation
		int				cpu_dur_left;

		bool			is_io; // When child process enters running state
		int				io_start_time;
		int				io_dur;
		int				io_dur_left;

	public:
		e_state	state;

		ChildProcess();

		const int&		getChildMsgId(void);
		const e_state&	getState(void);
		void			setParentMsgId(int idx, int &cpu_id, int &io_id);
		void			setState(e_state state);

		void			startProcess(void);
		void			watch(void);

		friend ostream&	operator<<(ostream &ost, ChildProcess &pos);
};
