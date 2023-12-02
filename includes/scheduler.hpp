#pragma once

#include <stdlib.h>
#include <exception>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <queue>
#include <sys/time.h>
#include <csignal>
#include <iostream>
using namespace std;

#include "ChildProcess.hpp"
#include "ParentProcess.hpp"

enum msg_type {
	TYPE_RUN_CPU_PROCESS,
	TYPE_RUN_IO_PROCESS,
	TYPE_CHILD_IO_INTERRUPT,
	TYPE_CHILD_IO_END,
	TYPE_CHILD_END,
	TYPE_CLEAR_PROCESS
};

enum msg_id {
	MSG_ID_PARENT_CPU,
	MSG_ID_PARENT_IO,
	MSG_ID_CHILDS
};
	
typedef struct {
	long		mtype;
	int			send_pid;
	msg_type	type;
} msg_load;

void printInfo(void);
