#include <stdio.h>
#include <heap.h>

#define HEAP_SIZE 16*1024*1024
#define LIST_SIZE 2048
#define LOOPS 1E6 /* One miilliioon ... */

void * heap;


struct Node
{
  struct Node * next;
  int value;
};


struct List
{
  struct Node * first;
  struct Node * last;
};


void *
list_setup( size_t listsize )
{
  int i;
  struct Node * node;
  struct List * list= new( heap, sizeof(struct List) );
  
  for( i= 0; i < LIST_SIZE; i++ )
    {
      node= (struct Node *)newa( heap, list, sizeof(struct Node) );
      node->next= NULL;
      node->value= i;

      if( list->first == NULL )
	list->first= node;

      if( list->last != NULL )
	list->last->next= node;

      list->last= node;
    }

  return list;
}


void
traverse( struct List * list )
{
  int i;
  struct Node * node;
  int n= 0;
  int sum= 0;

  for( i= LOOPS; i != 0; i-- )
    {
      n= 0;
      node= list->first;
      while( node != NULL )
	{
	  n++;
	  node= node->next;
	}
      sum^= n;
    }
  printf( "List size: %d (%d)\n", sum, n );
}


int
main( int argc, char ** argv )
{
  heap= heap_init( HEAP_SIZE );
  struct List * list= list_setup( LIST_SIZE );
  traverse( list );
}
