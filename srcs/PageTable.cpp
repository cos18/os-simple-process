#include "paging.hpp"

PageTable::PageTable(void): logical_memory_start_idx(USHRT_MAX) {}

PageTable::PageTable(unsigned short start_idx): logical_memory_start_idx(start_idx) {
	this->logical_memory_start_idx = start_idx;
	for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
		this->page_number[i] = 0;
		this->valid[i] = false;
	}
}