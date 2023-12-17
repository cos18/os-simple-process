#pragma once

#include "paging.hpp"

typedef struct {
	unsigned short				page_number;
	unsigned short 				page_offset;
} va;

enum msg_type {
	TYPE_RUN_CPU_PROCESS,
	TYPE_PAGE_REQUEST,
	TYPE_PAGE_FAULT,
	TYPE_PAGE_HIT,
	TYPE_CHILD_READY,
	TYPE_CHILD_WAITING,
	TYPE_CHILD_END,
	TYPE_CHILD_INFO,
	TYPE_TERMINATE_CHILD,
	TYPE_KILL_PROCESS
};

typedef struct {
	long			mtype;
	int				send_pid;
	unsigned short	page_idx[MEMORY_ACCESS_REQUEST_SIZE];
	unsigned short	physical_idx[MEMORY_ACCESS_REQUEST_SIZE];
	msg_type		type;
} msg_load;

class PageTable {
	private:
		unsigned short			logical_memory_start_idx;
		unsigned short			page_number[PAGE_TABLE_SIZE];
		bool					valid[PAGE_TABLE_SIZE];

	public:
		PageTable(void);
		PageTable(unsigned short start_idx);

		bool	checkPageValid(msg_load &msg, PhysicalMemory &pm, ofstream &log_file_stream);
		void	makePageInvalid(unsigned short page_number);
};
