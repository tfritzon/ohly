#include <stdlib.h>
#include <stdio.h>
#include "two.h"

struct Node
{
  short next;
  //struct Node * next;
  int value;
};

#define PTR(b, o) (b)+(o)

void
traverse(struct Node * list)
{
  struct Node * elem;
  int i;
  int sum;
  TSETUP;

  T0;
  PT0;
  for( i= 0; i < LOOPS; i++ )
    {
      sum= 0;
      elem= list;
      while( elem->next != 0 )
	//while( elem->next != NULL )
	{
	  sum++;
	  elem= PTR(list, elem->next);
	  //elem= elem->next;
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
  struct Node * list= calloc( LISTSIZE, sizeof(struct Node));
  struct Node * elem;
  int sum= 0;

  for( i= 0; i < LISTSIZE; i++ )
    {
      elem= list+i;
      elem->value= i;
      (elem-1)->next= elem-list;
      //(elem-1)->next= elem;
      
    }

  traverse(list);
}
