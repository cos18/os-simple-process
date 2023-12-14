#pragma once

#include "paging.hpp"

class PhysicalMemory {
	private:
		int				shmid;
		unsigned short	last_used[PHYSICAL_MEMORY_PAGE_SIZE];

	public:
		char*			memory; // memory size : PHYSICAL_MEMORY_PAGE_SIZE * PAGE_SIZE
		PhysicalMemory(void);
		~PhysicalMemory(void);
};
