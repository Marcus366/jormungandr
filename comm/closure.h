#ifndef __JOR_COMM_CLOSURE_H__
#define __JOR_COMM_CLOSURE_H__

typedef int (*jr_closure_func_t)(void*);

typedef struct jr_closure_s {
  void *data;
  jr_closure_func_t func;
} jr_closure_t ;

jr_closure_t jr_make_closure(jr_closure_func_t func, void *data);

int jr_closure_run(jr_closure_t closure);

#endif

