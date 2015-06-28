#ifndef __JOR_REACTOR_POLLER_H__
#define __JOR_REACTOR_POLLER_H__


#include <stddef.h>
#include <sys/epoll.h>
#include "handler.h"
#include "rbtree.h"


typedef struct jor_poller_s {
  int epfd;
  int size;
  struct epoll_event *events;

  jor_rbtree_tree_t fdset;
  jor_rbtree_node_t fdsentinel;
} jor_poller_t;


jor_poller_t* jor_poller_alloc(int nevents);


void jor_poller_free(jor_poller_t *poller);


int jor_poller_register(jor_poller_t *poller,
    int fd, uint32_t event, jor_handler_func_t func);


int jor_poller_poll(jor_poller_t *poller, int timeout);


#endif

