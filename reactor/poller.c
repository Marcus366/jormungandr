#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "rbtree.h"
#include "poller.h"
#include "alloc.h"


jr_poller_t*
jr_poller_alloc(int nevents)
{
  jr_poller_t *poller = (jr_poller_t*)jr_malloc(sizeof(jr_poller_t));
  if (poller == NULL) {
    goto end;
  }

  poller->epfd = epoll_create1(EPOLL_CLOEXEC);
  if (poller->epfd == -1) {
    goto fail;
  }

  poller->size = nevents;
  poller->events = (struct epoll_event*)
    jr_malloc(sizeof(struct epoll_event) * nevents);
  if (poller->events == NULL) {
    goto fail;
  }

  jr_rbtree_init(&poller->fdset, &poller->fdsentinel,
      jr_rbtree_insert_value);

  return poller;

fail:
  jr_free(poller);
  poller = NULL;
end:
  return poller;
}


void
jr_poller_free(jr_poller_t *poller)
{
  close(poller->epfd);
  jr_free(poller->events);
  jr_free(poller);
}


int
jr_poller_register(jr_poller_t *poller,
    int fd, int event, jr_handler_func_t func, void *arg)
{
  int found = 0;
  jr_rbtree_node_t *p = poller->fdset->root;

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
    jr_handler_t *handler = jr_handler_alloc(fd, event, ctx);
    if (event & M_READ) {
      handler->vtable->handle_read = func;
    }
    if (event & M_WRITE) {
      handler->vtable->handle_write = func;
    }

    jr_rbtree_insert(poller->fdset, &handler->rbnode);
  }
  
  //TODO epoll ctl

  return 0;
}

int jr_poller_poll(jr_poller_t *poller, int timeout)
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
    jr_handler_t *handler = (jr_handler_t*)ev.data.ptr;
    if (ev.events & EPOLLIN) {
      handler->vtable->handle_read(handler->fd, 0, handler->ctx);
    }
    if (ev.events & EPOLLOUT) {
      handler->vtable->handle_write(handler->fd, 0, handler->ctx);
    }
  }
}

