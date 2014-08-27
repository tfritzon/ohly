#ifndef __list_h
#define __list_h

#include <stdbool.h>
#include <stdint.h>

typedef void(*mapfun)(void *, void *);
typedef struct opaque_list list_t;

list_t *list_mk(int64_t size);
void list_append(list_t *l, void *e);
void list_prepend(list_t *l, void *e);
void *list_get(list_t *l, int64_t i);
bool list_set(list_t *l, int64_t i, void *e);
void list_forall_seq(list_t *l, mapfun f, void *s);
void *list_delete(list_t *l, int64_t i);
void dump_list(list_t *l);

#endif
