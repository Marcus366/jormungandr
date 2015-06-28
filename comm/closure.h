#ifndef __JOR_COMM_CLOSURE_H__
#define __JOR_COMM_CLOSURE_H__

typedef int (*jor_closure_func_t)(void*);

typedef struct jor_closure_s {
  void *data;
  jor_closure_func_t func;
} jor_closure_t ;

jor_closure_t jor_make_closure(jor_closure_func_t func, void *data);

int jor_closure_run(jor_closure_t closure);

#endif

