#pragma once

#include "paging.hpp"

free_page_list*	createFreePageList();
free_page_list*	findListNode(free_page_list* begin, unsigned short target);
void			subscribeTableToNode(free_page_list* target, PageTable *pt);
void			resetListNode(free_page_list* target);
void			freeList(free_page_list* target);

