#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <math.h>

int main(int argc, char *argv[])
{
  if (argc < 4)
    {
      printf("Usage: %s elements operations gets_per_set \n", argv[0]);
      puts("elements must be a power of ten, i.e. 10, 100, 1000, 10000, etc.");
    }
  else
    {
      int64_t elements     = atol(argv[1]);
      int64_t operations   = atol(argv[2]);
      int64_t gets_per_set = atol(argv[3]);
      bool should_print    = argc > 4 && strcmp(argv[4], "--print") == 0; 
      fprintf(stderr, "Running with %lld elements, %lld operations and %.2f%% destructive operations\n", elements, operations, 1.0 / gets_per_set * 100);

      // Run a mixture of insertions, deletions and lookups

      list_t *l = list_mk(elements);

      disrupt = true;

      for (uint64_t i = 0; i < elements; ++i)
        list_append(l, (void *)i);

      /* for (uint64_t i = 0; i < elements / 2; ++i) */
      /*        list_prepend(l, (void *)(elements + i)); */

      clock_t t = clock();
      if (t == ((clock_t)-1))
	{
	  fprintf(stderr,"clock() failed in file %s at line # %d\n", __FILE__,__LINE__-3);
	  exit(EXIT_FAILURE);
	}

      uint64_t gets = 1;
      bool add = false;
      for (uint64_t i = 0; i < operations; ++i)
        {
          if (elements == 0 && operations > gets_per_set)
            {
              fprintf(stderr, "NOTE: delete operations have removed all elements!");
              exit(1);
            }
          if (gets % gets_per_set == 0)
            {
	      if (add) 
		{
		  list_append(l, (void *)(20000 + i));
		  add = false;
		}
	      else
		{
		  int x = random() % (elements - 1);
		  list_delete(l, x < 0 ? 0 : x);
		  add = true;
		}
              gets = 1;
            }
          else
            {
              list_get(l, random() % elements);
              ++gets;
            }
        }
      /* void list_forall_seq(list_t *l, mapfun f, void *s); */
      fprintf(stderr, "Done: %lld elements remaining\n", elements);

      t = clock() - t;
      fprintf(stderr, "Time used: %f seconds\n",((float)t)/CLOCKS_PER_SEC);

      if (should_print) dump_list(l);

    }
  return 0;
}
