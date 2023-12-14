#pragma once

#include "paging.hpp"

class ParentProcess {
	private:
		unsigned short			time_quantum;
		unsigned short			time_log;
		unsigned short			time_unit;
		ChildProcess			*plist;

		int						cpu_msg_id;
		queue<ChildProcess*>	ready_queue;
		ChildProcess			*curr_cpu_burst;
		unsigned short			curr_cpu_quantum;

		//int					mem_msg_id;
		PhysicalMemory			pm;

		int						log_msg_id;
		ofstream				log_file_stream;

		unsigned short			gtimer;
		int						pid;
	
	public:
		~ParentProcess(void);

		void init(int argc, char **argv);
		void run(void);
		void listener(void);
		void cleanup(void);
		void manageCPU(void);
		void writeLog(void);

		class ParamException: public std::exception {
			public:
				virtual const char *what() const throw();
		};
};
