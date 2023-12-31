#include "paging.hpp"

PageTable::PageTable(void): logical_memory_start_idx(USHRT_MAX) {}

PageTable::PageTable(unsigned short start_idx): logical_memory_start_idx(start_idx) {
	this->logical_memory_start_idx = start_idx;
	for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
		this->page_number[i] = 0;
		this->valid[i] = false;
	}
}

bool PageTable::checkPageValid(msg_load &msg, PhysicalMemory &pm, ofstream &log_file_stream) {
	bool result = true;
	int table_idx;
	for (int i = 0; i < MEMORY_ACCESS_REQUEST_SIZE; i++) {
		table_idx = msg.page_idx[i] - this->logical_memory_start_idx;
		if (result && !this->valid[table_idx]) {
			result = false;
			this->page_number[table_idx] = pm.validPage(msg.page_idx[i], this);
			log_file_stream << "Page Fault at " << msg.page_idx[i] << ", save page idx at physical memory " << this->page_number[table_idx] << endl;
			this->valid[table_idx] = true;
		}
	}
	if (result) {
		log_file_stream << "Page HIT!" << endl;
		for (int i = 0; i < MEMORY_ACCESS_REQUEST_SIZE; i++) {
			table_idx = msg.page_idx[i] - this->logical_memory_start_idx;
			msg.physical_idx[i] = this->page_number[table_idx];
		}
	}
	return result;
}
void PageTable::makePageInvalid(unsigned short page_number) {
	this->page_number[page_number] = 0;
	this->valid[page_number] = false;
}
