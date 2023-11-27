#pragma once

#include "scheduler.hpp"

class ParentProcess {
	private:
		int time_quantum;
		int time_log;
	
	public:
		ParentProcess(int argc, char **argv);
		void run(void);

		class ParamException: public std::exception {
			public:
				virtual const char *what() const throw();
		};
};
