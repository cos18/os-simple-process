#pragma once

#include "scheduler.hpp"

class ParentProcess {
	private:
		int						time_quantum;
		int						time_log;
		int						time_unit;
		ChildProcess			*plist;

		int						cpu_msg_id;
		queue<ChildProcess*>	ready_queue;
		ChildProcess			*curr_cpu_burst;
		int						curr_cpu_quantum;

		int						io_msg_id;
		queue<ChildProcess*>	io_queue;
		ChildProcess			*curr_io_burst;

		int						gtimer;
		int						pid;
	
	public:
		~ParentProcess(void);

		void init(int argc, char **argv);
		void run(void);
		void listener(void);
		void cleanup(void);
		void manageCPU(void);
		void manageIO(void);

		class ParamException: public std::exception {
			public:
				virtual const char *what() const throw();
		};
};
