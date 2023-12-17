#include "paging.hpp"

PhysicalMemory::PhysicalMemory(void) {
	this->shmid = shmget(SHM_KEY, PHYSICAL_MEMORY_PAGE_SIZE * PAGE_SIZE * sizeof(unsigned short), IPC_CREAT | 0666);
	this->memory = (unsigned short*)shmat(this->shmid, NULL, 0);
	this->free_page_list_head = createFreePageList();
}

PhysicalMemory::~PhysicalMemory(void) {
	free_page_list* tmp = this->free_page_list_head;
	while (tmp) {
		if (tmp->backing_store_idx != USHRT_MAX)
			writeStoreData(mmu(this->memory, tmp->physical_page_number), tmp->backing_store_idx);
		tmp = tmp->next;
	}
	shmdt(this->memory);
	shmctl(this->shmid, IPC_RMID, NULL);
	freeList(this->free_page_list_head);
}

// TODO
unsigned short PhysicalMemory::validPage(unsigned short page_idx, PageTable *pt) {
	free_page_list* target = findListNode(this->free_page_list_head, page_idx);
	unsigned short*	addr = target ? mmu(this->memory, target->physical_page_number) : NULL;
	
	if (!target) {
		target = this->free_page_list_head;
		addr = mmu(this->memory, target->physical_page_number);
		if (target->backing_store_idx != USHRT_MAX) {
			writeStoreData(addr, target->backing_store_idx);
			resetListNode(target);
		}
		checkBackingStore(page_idx);
		insertMemory(addr, page_idx);
	}
	target->backing_store_idx = page_idx;
	subscribeTableToNode(target, pt);

	if (target == this->free_page_list_head) this->free_page_list_head = target->next;

	if (target->next) {
		free_page_list* last = target;
		while (last->next != NULL) last = last->next;
		last->next = target;
		if (target->prev) target->prev->next = target->next;
		target->next->prev = target->prev;
		target->prev = last;
		target->next = NULL;
	}

	return target->physical_page_number;
}