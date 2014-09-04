#include <stdlib.h>
#include <stdio.h>
#include "two.h"

struct Node
{
  int value;
  struct Node * next;
};

struct List
{
  struct Node * first;
  struct Node * last;
};

#define NEW(x) calloc(1, sizeof(struct x))

void
traverse(struct List * list)
{
  struct Node * node;
  int i;
  int sum= 0;
  int len= 0;
  TSETUP;

  T0;
  PT0;
  for( i= 0; i < LOOPS; i++ )
    {
      node= list->first;
      while( node != NULL )
	{
	  len++;
	  node= node->next;
	}
    }
  T1;
  PT1;
  PDT;
}

int
main( int argc, char ** argv )
{
  int i;
  struct Node * node;
  void * slask;
  struct List * list= NEW(List);

  for( i= 0; i < LISTSIZE; i++ )
    {
      node= NEW(Node);
      node->value= i;
      if( list->first == NULL )
	{
	  list->first= node;
	  list->last= node;
	}
      else
	{
	  list->last->next= node;
	  list->last= node;
	}
      slask= calloc(1, 65536);
    }

  traverse(list);
}
