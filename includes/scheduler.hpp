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

enum msg_id {
	MSG_ID_CPU,
	MSG_ID_LOG,
	MSG_ID_CHILDS
};

void printInfo(void);
