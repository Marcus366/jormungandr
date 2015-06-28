#include "closure.h"

jor_closure_t
jor_make_closure(jor_closure_func_t func, void *data)
{
  jor_closure_t closure = {
    .func = func,
    .data = data
  };

  return closure;
}

int
jor_closure_run(jor_closure_t closure)
{
  return closure.func(closure.data);
}

