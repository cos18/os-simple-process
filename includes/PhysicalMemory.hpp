#pragma once

#include "paging.hpp"

class PageTable;

typedef struct struct_page_list {
	unsigned short				page_number;
	PageTable*					subscribe_table[PROCESS_NUM];
	struct struct_page_list*	prev;
	struct struct_page_list*	next;
} free_page_list;

class PhysicalMemory {
	private:
		int						shmid;
		free_page_list*			free_page_list_head;
		free_page_list*			idx_to_page_list[PHYSICAL_MEMORY_PAGE_SIZE];
		unsigned short			backing_store_idx[PHYSICAL_MEMORY_PAGE_SIZE];

	public:
		unsigned short*			memory;
		PhysicalMemory(void);
		~PhysicalMemory(void);

		unsigned short			validPage(unsigned short page_idx, PageTable *pt);
		void					subscribePage(char* page_ptr, PageTable *pt);
};
