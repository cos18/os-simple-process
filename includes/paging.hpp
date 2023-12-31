#pragma once

#define PROCESS_NUM 10
#define TIME_UNIT 10

#define SHM_KEY 1234
#define PAGE_SIZE 4
#define BACKING_STORE_PAGE_SIZE 512
#define PHYSICAL_MEMORY_PAGE_SIZE 256
#define PAGE_TABLE_SIZE 128
#define MEMORY_ACCESS_REQUEST_SIZE 10
#define SIMULATE_TIME_TICK 10000

#include <stdlib.h>
#include <exception>
#include <time.h>
#include <unistd.h>
#include <sys/msg.h>
#include <queue>
#include <sys/time.h>
#include <sys/wait.h>
#include <csignal>
#include <cstring>
#include <fstream>
#include <thread>
#include <chrono>
#include <iostream>
#include <climits>
#include <random>
#include <set>
#include <sys/shm.h>
#include <sys/stat.h>
#include <format>

using namespace std;

#include "PhysicalMemory.hpp"
#include "PageTable.hpp"
#include "list.hpp"
#include "ChildProcess.hpp"
#include "ParentProcess.hpp"

enum msg_id {
	MSG_ID_CPU,
	MSG_ID_LOG,
	MSG_ID_CHILDS
};

void printInfo(void);
void checkBackingStorePath(void);
void resetBackingStore(void);
void checkBackingStore(unsigned short page_idx);
void insertMemory(unsigned short* memory, unsigned short page_idx);
void writeStoreData(unsigned short* memory, unsigned short page_idx);
unsigned short* mmu(unsigned short* memory, unsigned short page_idx);
