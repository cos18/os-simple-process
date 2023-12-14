#pragma once

#include "paging.hpp"

typedef struct {
	unsigned short				page_number;
	unsigned short 				page_offset;
} va;

class PageTable {
	private:
		unsigned short			logical_memory_start_idx;
		unsigned short			page_number[PAGE_TABLE_SIZE];
		bool					valid[PAGE_TABLE_SIZE];

	public:
		PageTable(void);
		PageTable(unsigned short start_idx);

		bool	checkPageVaild(unsigned short *page_idx_arr, unsigned short g_timer);
};
