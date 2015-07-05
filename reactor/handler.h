#ifndef __JOR_REACTOR_HANDLER_H__
#define __JOR_REACTOR_HANDLER_H__


#include <stdint.h>
#include "rbtree.h"


#define POLL_READ (1 << 0)
#define POLL_WRITE (1 << 1)
#define POLL_EXCEPT (1 << 2)
#define POLL_ACCEPT (1 << 3)
#define POLL_CONNECT (1 << 4)


typedef int (*jr_handler_func_t)(int, int32_t, void*);


typedef struct jr_handler_s {
  jr_handler_func_t callback;
  jr_rbtree_node_t rbnode;

  int fd;
  uint32_t event;

  void *ctx;

} jr_handler_t;


jr_handler_t *jr_handler_alloc(int fd, uint32_t event, void *ctx);


void jr_handler_free(jr_handler_t *handler);


#endif

