#pragma once

#define SHM_KEY 1234
#define PAGE_SIZE 4
#define BACKING_STORE_PAGE_SIZE 512
#define PHYSICAL_MEMORY_PAGE_SIZE 256
#define PAGE_TABLE_SIZE 128
#define MEMORY_ACCESS_REQUEST_SIZE 10
#define SIMULATE_TIME_TICK 100

#include <climits>
#include <random>
#include <set>
#include <sys/shm.h>
#include <sys/stat.h>
#include "PageTable.hpp"
#include "PhysicalMemory.hpp"
#include "scheduler.hpp"

void checkBackingStore(void);
void resetBackingStore(void);
