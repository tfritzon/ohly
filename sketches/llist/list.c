#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"

typedef struct node
{
  void *element;
  struct node *next;
} node_t;

typedef struct opaque_list
{
  node_t *first;
  node_t *last;
} list_t;

node_t **nodes;


#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))

void *padding;
static inline node_t *node_mk(void *e, node_t *n)
{
  // TODO: should be able to give probability of same cache
  // line locality, to determine how often padding should
  // happen
  padding = malloc(64);
  node_t *tmp = malloc(sizeof(node_t));

  if (tmp)
    {
      tmp->element = e;
      tmp->next = n;
    }
  return tmp;
}

list_t *list_mk(int64_t size)
{
  return calloc(1, sizeof(list_t));
}

void list_append(list_t *l, void *e)
{
  if (l->first)
    {
      l->last = l->last->next = node_mk(e, NULL);
    }
  else
    {
      l->first = l->last = node_mk(e, NULL);
    }
}

void list_prepend(list_t *l, void *e)
{
  if (l->first)
    {
      l->first = node_mk(e, l->first);
    }
  else
    {
      l->first = l->last = node_mk(e, NULL);
    }
}

static inline node_t *find(node_t *n, int64_t i)
{
  while (i-- && n) n = n->next;
  return i ? n : NULL;
}

void *list_get(list_t *l, int64_t i)
{
  node_t *holder = find(l->first, i);
  return holder ? holder->element : NULL;
}

bool list_set(list_t *l, int64_t i, void *e)
{
  node_t *holder = find(l->first, i);
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
  for (node_t *n = l->first;
       n;
       n = n->next) {
    f(n->element, s);
  }
}

void *list_delete(list_t *l, int64_t i)
{
  assert(i >= 0);

  node_t *tmp_node;
  void *tmp_element;

  if (i)
    {
      node_t *prev = find(l->first, i - 1);
      if (prev)
        {
          tmp_node = prev->next;
          tmp_element = tmp_node->element;
          prev->next = prev->next->next;
        }
      else
        {
          return NULL;
        }
    }
  else
    {
      tmp_node = l->first;
      tmp_element = tmp_node->element;
      l->first = l->first->next;
    }
  free(tmp_node);
  return tmp_element;
}

void dump_list(list_t *l)
{
  printf("Dumping list\n");
  node_t *n = l->first;
  while (n) {
    printf("%p\n", n->element);
    n = n->next;
  }
}
