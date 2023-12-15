#include "paging.hpp"

PhysicalMemory::PhysicalMemory(void) {
	this->shmid = shmget(SHM_KEY, PHYSICAL_MEMORY_PAGE_SIZE * PAGE_SIZE * sizeof(unsigned short), IPC_CREAT | 0666);
	this->memory = (unsigned short*)shmat(this->shmid, NULL, 0);
	this->free_page_list_head = createFreePageList(PHYSICAL_MEMORY_PAGE_SIZE);

	this->idx_to_page_list[0] = this->free_page_list_head;
	this->backing_store_idx[0] = USHRT_MAX;
	for (int i = 1; i < PHYSICAL_MEMORY_PAGE_SIZE; i++) {
		this->idx_to_page_list[i] = this->idx_to_page_list[i - 1]->next;
		this->backing_store_idx[i] = USHRT_MAX;
	}
}

PhysicalMemory::~PhysicalMemory(void) {
	shmdt(this->memory);
	shmctl(this->shmid, IPC_RMID, NULL);
	freeList(this->free_page_list_head);
}

unsigned short PhysicalMemory::validPage(unsigned short page_idx, PageTable *pt) {
	free_page_list* target = findListNode(this->free_page_list_head, page_idx);
	unsigned short*	addr = this->memory;
	
	if (target) {
		addr += (target->page_number * PAGE_SIZE);
	} else {
		target = this->free_page_list_head;
		if (backing_store_idx[target->page_number] != USHRT_MAX) resetListNode(target);
		addr += (target->page_number * PAGE_SIZE);
		checkBackingStore(page_idx);
		insertMemory(addr, target->page_number);
		backing_store_idx[target->page_number] = page_idx;
	}
	subscribeTableToNode(target, pt);
	pushListNodeToLast(target);
	return target->page_number;
}