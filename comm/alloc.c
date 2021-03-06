#include <stdlib.h>
#include "alloc.h"


void*
jr_malloc(size_t size)
{
  void *p = malloc(size);

  if (p == NULL) {
    abort();
  }

  return p;
}


void
jr_free(void *p)
{
  free(p);
}

