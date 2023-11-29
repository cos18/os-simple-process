#pragma once

#define PSTATE_NEW			0
#define PSTATE_READY		1
#define PSTATE_RUNNING		2
#define PSTATE_WAITING		3
#define PSTATE_TERMINATED	4

class ChildProcess {
	private:
		int		pid;
		int		child_send_id;
		int		child_recv_id;

		int		cpu_dur; // determined at the time of child process creation
		int		cpu_dur_left;

		bool	is_io; // When child process enters running state
		int		io_start_time;
		int		io_dur;
		int		io_dur_left;


	public:
		unsigned char	state;

		ChildProcess();

		void run();

		friend ostream & operator<<(ostream &ost, ChildProcess &pos);
};
