#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"

typedef int16_t offsetptr;
#define NIL -1
#define NOTNIL(x) (x != -1)
#define get(base,field) (field == NIL ? NULL : (((void *) base) + field))
#define to_offset(base,p) (p == NULL ? NIL : (int16_t) (((void *)p) - ((void *)base)))

typedef struct node
{
  void *element;
  offsetptr next;
} node_t;

typedef struct opaque_list
{
  offsetptr first;
  offsetptr last;
  offsetptr frontier;
  int64_t capacity;
} list_t;

list_t *list_mk(int64_t size)
{
  list_t *result = calloc(1, sizeof(list_t) + (size + 1) * sizeof(node_t));
  result->first = NIL;
  result->last = NIL;
  result->frontier = sizeof(list_t);
  result->capacity = size + 2;
  return result;
}

static inline node_t *node_mk(list_t *l, void *e, offsetptr n)
{
  assert(l->capacity > 0);
  node_t *tmp = (node_t*)(((void *)l) + l->frontier);
  l->frontier = l->frontier + sizeof(node_t);
  --l->capacity;
  if (tmp)
    {
      tmp->element = e;
      tmp->next = n;
    }
  return tmp;
}

void dump_list_compact(list_t *l, char* msg) 
{
  printf("Dumping @ %s [ ", msg);
  int guard = 20;
  node_t *n = get(l, l->first);
  while (guard-- && n) {
    printf(" %p,", n->element);
    n = get(n, n->next);
  }
  printf("\b ]\n");
}

void list_append(list_t *l, void *e)
{
  printf("Appending %p\n", e);
  dump_list_compact(l, "<<before append>>");
  if (NOTNIL(l->first))
    {
      node_t *tmp = get(l, l->last);
      node_t *new = node_mk(l, e, NIL);
      tmp->next = to_offset(tmp, new);
      l->last = to_offset(l, new);

      assert(get(l, l->last) == get(tmp, tmp->next));
    }
  else
    {
      node_t *new = node_mk(l, e, NIL);
      l->first = to_offset(l, new);
      l->last = to_offset(l, new);
    }
  dump_list_compact(l, "<<after append>> ");
}

void list_prepend(list_t *l, void *e)
{
  assert(false);
  if (NOTNIL(l->first))
    {
      l->first = to_offset(l, node_mk(l, e, l->first));
    }
  else
    {
      l->first = l->last = to_offset(l, node_mk(l, e, NIL));
    }
}

static inline node_t *find(list_t *l, node_t *n, int64_t i)
{
  while (i-- && n) n = get(n, n->next);
  return i ? n : NULL;
}

void *list_get(list_t *l, int64_t i)
{
  node_t *holder = find(l, get(l, l->first), i);
  return holder ? holder->element : NULL;
}

bool list_set(list_t *l, int64_t i, void *e)
{
  node_t *holder = find(l, get(l, l->first), i);
  if (holder)
    {
      holder->element = e;
      return true;
    }
  else
    {
      return false;
    }
}

void list_forall_seq(list_t *l, mapfun f, void *s)
{
  for (node_t *n = get(l, l->first);
       n;
       n = get(n, n->next)) {
    f(n->element, s);
  }
}

static inline node_t *shift_one(node_t **tmp, node_t *last)
{
  /* if ((*tmp)->next)  */
  /*   { */
  /*     (*tmp)->element = (*tmp)->next->element; */
  /*     return shift_one(&((*tmp)->next), *tmp); */
  /*   }  */
  /* else  */
  /*   { */
  /*     *tmp = NULL;  */
  /*     return last; */
  /*   } */
  return NULL;
}

void *list_delete(list_t *l, int64_t i)
{
  printf("Delete %lld\n", i);
  dump_list_compact(l, "<<before delete>>");
  assert(i >= 0);

  node_t *to_address = i ? find(l, get(l, l->first), i) : get(l, l->first);
  node_t *from_address = get(to_address, to_address->next);

  printf("to_address %p with %lld\n", to_address, (int64_t) to_address->element);
  printf("to_address %p with %p\n", to_address, get(to_address, to_address->next));
  printf("from_address %p with %p\n", from_address, get(from_address, from_address->next));
  
  l->last -= sizeof(node_t);

  if (from_address)
    {
      memmove(to_address, from_address, l->frontier - sizeof(list_t));
    }
  else
    {
      node_t *prev = to_address-1;
      prev->next = NIL;
    }
  ++l->capacity;
  --l->frontier;
  dump_list_compact(l, "<<after delete>> ");
  return NULL;
}

void dump_list(list_t *l) 
{
  printf("Dumping list\n");
  int guard = 20;
  node_t *n = get(l, l->first);
  while (guard-- && n) {
    printf("%p\n", n->element);
    n = get(n, n->next);
  }
}
