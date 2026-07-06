/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef DLIST_H
#define DLIST_H

/* Doubly linked list. */

#define DEBUG_DLIST

struct dlist;

struct dlist_link {
#ifdef DEBUG_DLIST
    struct dlist *head;
#endif
    struct dlist_link *next;
    struct dlist_link *prev;
};

struct dlist {
    struct dlist_link head;
};

#define dlist_container_of(ptr, type, member)          \
    ((type *)(((char *) ptr) - offsetof(type, member)))

static inline void
dlist_init(struct dlist *list)
{
    list->head.next = &list->head;
    list->head.prev = &list->head;
}

static inline void
dlist_link_init(struct dlist_link *link)
{
#ifdef DEBUG_DLIST
    link->head = NULL;
#endif
}

static inline void
dlist_add_tail(struct dlist *list, struct dlist_link *link)
{
#ifdef DEBUG_DLIST
    if (link->head) {
	while(true) {}
    }
    link->head = list;
#endif
    link->next = &list->head;
    link->prev = list->head.prev;
    list->head.prev->next = link;
    list->head.prev = link;
}

static inline struct dlist_link *
dlist_get_head(struct dlist *list)
{
    if (list->head.next == &list->head)
	return NULL;
    return list->head.next;
}
#define dlist_get_head_container(list, type, member) \
    (dlist_get_head(list)				     \
     ? dlist_container_of((list)->head.next, type, member)   \
     : NULL)

static inline void
dlist_remove_link(struct dlist *list, struct dlist_link *link)
{
#ifdef DEBUG_DLIST
    if (link->head != list) {
	while(true) {}
    }
    link->head = NULL;
#endif
    link->next->prev = link->prev;
    link->prev->next = link->next;
}

static inline bool
dlist_empty(struct dlist *list)
{
    return list->head.next == &list->head;
}

#endif /* DLIST_H */
