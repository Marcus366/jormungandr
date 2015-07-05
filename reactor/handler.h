#ifndef __JOR_REACTOR_HANDLER_H__
#define __JOR_REACTOR_HANDLER_H__


#include <stdint.h>
#include "rbtree.h"


#define M_READ (1 << 0)
#define M_WRITE (1 << 1)
#define M_EXCEPT (1 << 2)
#define M_ACCEPT (1 << 3)
#define M_CONNECT (1 << 4)


typedef int (*jr_handler_func_t)(int, int, void*);

typedef struct jr_handler_vf_s {
  jr_handler_func_t handle_read;
  jr_handler_func_t handle_write;
  jr_handler_func_t handle_close;

  jr_handler_func_t handle_expection;
} jr_handler_vf_t;

typedef struct jr_handler_s {
  jr_handler_vf_t vtable;
  jr_rbtree_node_t rbnode;

  int fd;
  uint32_t mask;

  void *ctx;

} jr_handler_t;


jr_handler_t *jr_handler_alloc(int fd, uint32_t event, void *ctx);


void jr_handler_free(jr_handler_t *handler);


#endif

