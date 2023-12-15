#include "paging.hpp"

free_page_list*	createFreePageList(unsigned short page_cnt) {
	free_page_list* result = new free_page_list;
	result->page_number = 0;
	result->prev = NULL;
	result->next = NULL;
	for (int idx = 0; idx < PROCESS_NUM; idx++)
		result->subscribe_table[idx] = NULL;

	free_page_list* tmp = result;
	for (int i = 1; i < page_cnt; i++) {
		tmp->next = new free_page_list;
		tmp->next->page_number = i;
		for (int idx = 0; idx < PROCESS_NUM; idx++)
			tmp->next->subscribe_table[idx] = NULL;
		tmp->next->prev = tmp;
		tmp->next->next = NULL;
		tmp = tmp->next;
	}
	return result;
}

void pushListNodeToLast(free_page_list* target) {
	if (target->next) {
		if (target->prev) target->prev->next = target->next;
		target->next->prev = target->prev;
		free_page_list* last = target;
		while (last->next != NULL) last = last->next;
		last->next = target;
		target->prev = last;
		target->next = NULL;
	}
}

free_page_list*	findListNode(free_page_list* begin, unsigned short target) {
	while (begin && begin->page_number == USHRT_MAX) begin = begin->next;
	while (begin) {
		if (begin->page_number == target) break;
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
			target->subscribe_table[idx]->makePageInvalid(target->page_number);
			target->subscribe_table[idx] = NULL;
		}

	}
}

void freeList(free_page_list* target) {
	free_page_list* tmp;
	while (target != NULL) {
		tmp = target->next;
		delete target;
		target = tmp;
	}
}
