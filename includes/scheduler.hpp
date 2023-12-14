#pragma once

#include <stdlib.h>
#include <exception>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <queue>
#include <sys/time.h>
#include <csignal>
#include <fstream>
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;

#include "ChildProcess.hpp"
#include "ParentProcess.hpp"

#define PROCESS_NUM 10
#define CPU_MIN_DUR 6
#define CPU_MAX_DUR 20
#define IO_PROBABILITY_PERCENTILE 50
#define IO_MAX_DUR 5

enum msg_type {
	TYPE_RUN_CPU_PROCESS,
	TYPE_CHILD_READY,
	TYPE_CHILD_WAITING,
	TYPE_CHILD_END,
	TYPE_CHILD_INFO,
	TYPE_TERMINATE_CHILD,
	TYPE_KILL_PROCESS,
	STATE_REQUEST,
	STATE_RESPONSE_PAGE_FAULT,
	STATE_RESPONSE_PAGE_HIT
};

enum msg_id {
	MSG_ID_CPU,
	MSG_ID_LOG,
	MSG_ID_CHILDS
};
	
typedef struct {
	long			mtype;
	int				send_pid;
	unsigned short	page_idx[MEMORY_ACCESS_REQUEST_SIZE];
	char*			page_ptr[MEMORY_ACCESS_REQUEST_SIZE];
	msg_type		type;
} msg_load;

void printInfo(void);
