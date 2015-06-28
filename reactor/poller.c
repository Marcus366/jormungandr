#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "rbtree.h"
#include "poller.h"
#include "alloc.h"


jor_poller_t*
jor_poller_alloc(int nevents)
{
  jor_poller_t *poller = (jor_poller_t*)jor_malloc(sizeof(jor_poller_t));
  if (poller == NULL) {
    goto end;
  }

  poller->epfd = epoll_create1(EPOLL_CLOEXEC);
  if (poller->epfd == -1) {
    goto fail;
  }

  poller->size = nevents;
  poller->events = (struct epoll_event*)
    jor_malloc(sizeof(struct epoll_event) * nevents);
  if (poller->events == NULL) {
    goto fail;
  }

  jor_rbtree_init(&poller->fdset, &poller->fdsentinel,
      jor_rbtree_insert_value);

  return poller;

fail:
  jor_free(poller);
  poller = NULL;
end:
  return poller;
}


void
jor_poller_free(jor_poller_t *poller)
{
  close(poller->epfd);
  jor_free(poller->events);
  jor_free(poller);
}


int
jor_poller_register(jor_poller_t *poller,
    int fd, int event, jor_handler_func_t func, void *arg)
{
  int found = 0;
  jor_rbtree_node_t *p = poller->fdset->root;

  while (p != poller->fdset->sentinel) {
    if (fd < p->key) {
      p = p->left;
    } else if (fd > p->key) {
      p = p->right;
    } else {
      found = 1;
      break;
    }
  }

  if (found) {
    //TODO replace
  } else {
    jor_handler_t *handler = jor_handler_alloc(fd, event, ctx);
    if (event & M_READ) {
      handler->vtable->handle_read = func;
    }
    if (event & M_WRITE) {
      handler->vtable->handle_write = func;
    }

    jor_rbtree_insert(poller->fdset, &handler->rbnode);
  }
  
  //TODO epoll ctl

  return 0;
}

int jor_poller_poll(jor_poller_t *poller, int timeout)
{
  int i, nfds = epoll_wait(poller->epfd, poller->events, poller->size, timeout);
  if (nfds == -1) {
    if (errno == EINTR) {
      return -1;
    } else {
      abort();
    }
  }

  for (i = 0; i < nfds; ++i) {
    struct epoll_event ev = poller->events[i];
    jor_handler_t *handler = (jor_handler_t*)ev.data.ptr;
    if (ev.events & EPOLLIN) {
      handler->vtable->handle_read(handler->fd, 0, handler->ctx);
    }
    if (ev.events & EPOLLOUT) {
      handler->vtable->handle_write(handler->fd, 0, handler->ctx);
    }
  }
}

