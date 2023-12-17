#pragma once

#include "paging.hpp"

class PageTable;

typedef struct struct_page_list {
	unsigned short				physical_page_number;
	unsigned short				backing_store_idx;
	PageTable*					subscribe_table[PROCESS_NUM];
	struct struct_page_list*	prev;
	struct struct_page_list*	next;
} free_page_list;

class PhysicalMemory {
	private:
		int						shmid;
		free_page_list*			free_page_list_head;

	public:
		unsigned short*			memory;
		PhysicalMemory(void);
		~PhysicalMemory(void);

		void					storeAllMemory(void);
		unsigned short			validPage(unsigned short page_idx, PageTable *pt);
};
