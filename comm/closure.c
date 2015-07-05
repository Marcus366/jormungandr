#include "closure.h"

jr_closure_t
jr_make_closure(jr_closure_func_t func, void *data)
{
  jr_closure_t closure = {
    .func = func,
    .data = data
  };

  return closure;
}

int
jr_closure_run(jr_closure_t closure)
{
  return closure.func(closure.data);
}

