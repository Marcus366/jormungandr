#include <stdlib.h>
#include "alloc.h"


void*
jor_malloc(size_t size)
{
  void *p = malloc(size);

  if (p == NULL) {
    abort();
  }

  return p;
}


void
jor_free(void *p)
{
  free(p);
}

