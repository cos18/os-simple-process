#include "paging.hpp"

free_page_list*	createFreePageList() {
	free_page_list* result = (free_page_list *)malloc(sizeof(free_page_list));
	result->physical_page_number = 0;
	result->backing_store_idx = USHRT_MAX;
	result->prev = NULL;
	result->next = NULL;
	for (int idx = 0; idx < PROCESS_NUM; idx++)
		result->subscribe_table[idx] = NULL;

	free_page_list* tmp = result;
	for (int i = 1; i < PHYSICAL_MEMORY_PAGE_SIZE; i++) {
		tmp->next = (free_page_list *)malloc(sizeof(free_page_list));
		tmp->next->physical_page_number = i;
		tmp->next->backing_store_idx = USHRT_MAX;
		for (int idx = 0; idx < PROCESS_NUM; idx++)
			tmp->next->subscribe_table[idx] = NULL;
		tmp->next->prev = tmp;
		tmp->next->next = NULL;
		tmp = tmp->next;
	}
	return result;
}

free_page_list*	findListNode(free_page_list* begin, unsigned short target) {
	while (begin && begin->backing_store_idx == USHRT_MAX) begin = begin->next;
	while (begin) {
		if (begin->backing_store_idx == target) break;
		begin = begin->next;
	}
	return begin;
}
void subscribeTableToNode(free_page_list* target, PageTable *pt) {
	for (int idx = 0; idx < PROCESS_NUM; idx++) {
		if (target->subscribe_table[idx] == NULL) {
			target->subscribe_table[idx] = pt;
			break;
		}
	}
}

void resetListNode(free_page_list* target) {
	for (int idx = 0; idx < PROCESS_NUM; idx++) {
		if (target->subscribe_table[idx]) {
			target->subscribe_table[idx]->makePageInvalid(target->physical_page_number);
			target->subscribe_table[idx] = NULL;
		}

	}
}

void printList(free_page_list* target, ofstream &log_file_stream) {
	while (target != NULL) {
		log_file_stream << target->physical_page_number << " ";
		target = target->next;
	}
	log_file_stream << endl;
}

void freeList(free_page_list* target) {
	free_page_list* tmp;
	while (target != NULL) {
		tmp = target->next;
		delete target;
		target = tmp;
	}
}
