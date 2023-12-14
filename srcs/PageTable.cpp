#include "paging.hpp"

PageTable::PageTable(void): logical_memory_start_idx(USHRT_MAX) {}

PageTable::PageTable(unsigned short start_idx): logical_memory_start_idx(start_idx) {
	this->logical_memory_start_idx = start_idx;
	for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
		this->page_number[i] = 0;
		this->valid[i] = false;
	}
}

bool PageTable::checkPageVaild(unsigned short *page_idx_arr, unsigned short g_timer) {
	bool result = true;
	int table_idx;
	for (int i = 0; i < MEMORY_ACCESS_REQUEST_SIZE; i++) {
		table_idx = page_idx_arr[i] - this->logical_memory_start_idx;
		if (!this->valid[table_idx]) result = false;
		// TODO: check LRU
		(void)g_timer;
	}
	return result;
}