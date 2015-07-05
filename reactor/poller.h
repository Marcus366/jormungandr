#ifndef __JOR_REACTOR_POLLER_H__
#define __JOR_REACTOR_POLLER_H__


#include <stddef.h>
#include <sys/epoll.h>
#include "handler.h"
#include "rbtree.h"


typedef struct jr_poller_s {
  int epfd;
  int size;
  struct epoll_event *events;

  jr_rbtree_tree_t fdset;
  jr_rbtree_node_t fdsentinel;
} jr_poller_t;


jr_poller_t* jr_poller_alloc(int nevents);


void jr_poller_free(jr_poller_t *poller);


int jr_poller_register(jr_poller_t *poller,
    int fd, uint32_t event, jr_handler_func_t func);


int jr_poller_poll(jr_poller_t *poller, int timeout);


#endif

