#pragma once

#include "paging.hpp"

class PageTable {
	private:
		unsigned short	logical_memory_start_idx;
		unsigned short	page_number[PAGE_TABLE_SIZE];
		bool			valid[PAGE_TABLE_SIZE];

	public:
		PageTable(void);
		PageTable(unsigned short start_idx);
};
