#include "paging.hpp"

PhysicalMemory::PhysicalMemory(void) {
	this->shmid = shmget(SHM_KEY, PHYSICAL_MEMORY_PAGE_SIZE * PAGE_SIZE, IPC_CREAT | 0666);
	this->memory = (char*)shmat(this->shmid, NULL, 0);
	for (int idx = 0; idx < PHYSICAL_MEMORY_PAGE_SIZE; idx++) {
		this->last_used[idx] = 0;
	}
}

PhysicalMemory::~PhysicalMemory(void) {
	shmdt(this->memory);
	shmctl(this->shmid, IPC_RMID, NULL);
}