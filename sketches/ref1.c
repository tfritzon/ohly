#include <stdlib.h>
#include <stdio.h>

struct Node
{
  struct Node * next;
  int value;
};

#define LISTSIZE 32768

int
main( int argc, char ** argv )
{
  int i;
  struct Node * list= calloc( 1, sizeof(struct Node));
  struct Node * last= list;
  struct Node * elem;
  int sum= 0;

  for( i= 0; i < LISTSIZE; i++ )
    {
      elem= calloc( 1, sizeof(struct Node) );
      elem->value= i;
      last->next= elem;
      last = elem;
    }

  for( i= 0; i < 10000; i++ )
    {
      sum= 0;
      elem= list;
      while( elem != NULL )
	{
	  sum++;
	  elem= elem->next;
	}
    }
}
