#pragma once

#define SHM_KEY 1234
#define PAGE_SIZE 4
#define BACKING_STORE_PAGE_SIZE 512
#define PHYSICAL_MEMORY_PAGE_SIZE 256
#define PAGE_TABLE_SIZE 128

#include <climits>
#include <random>
#include <sys/shm.h>
#include "PageTable.hpp"
#include "PhysicalMemory.hpp"
#include "scheduler.hpp"
