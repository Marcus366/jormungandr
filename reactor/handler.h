#ifndef __JOR_REACTOR_HANDLER_H__
#define __JOR_REACTOR_HANDLER_H__


#include <stdint.h>
#include "rbtree.h"


#define M_READ (1 << 0)
#define M_WRITE (1 << 1)
#define M_EXCEPT (1 << 2)
#define M_ACCEPT (1 << 3)
#define M_CONNECT (1 << 4)


typedef int (*jor_handler_func_t)(int, int, void*);

typedef struct jor_handler_vf_s {
  jor_handler_func_t handle_read;
  jor_handler_func_t handle_write;
  jor_handler_func_t handle_close;

  jor_handler_func_t handle_expection;
} jor_handler_vf_t;

typedef struct jor_handler_s {
  jor_handler_vf_t vtable;
  jor_rbtree_node_t rbnode;

  int fd;
  uint32_t mask;

  void *ctx;

} jor_handler_t;


jor_handler_t *jor_handler_alloc(int fd, uint32_t event, void *ctx);


void jor_handler_free(jor_handler_t *handler);


#endif

