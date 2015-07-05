#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "alloc.h"
#include "rbtree.h"
#include "poller.h"


static void jr__poller_update(jr_poller_t*, jr_handler_t*, int);


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
jr_poller_ctl(jr_poller_t *poller,
    int fd, int32_t event, jr_handler_func_t func, void *ctx);
{
  int i;

  for (i = 0; i < len; ++i) {
    int found = 0;
    jr_poller_ctl_t *ctl = pollctl_array + len;
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
      jr_handler_t *handler = container_of(p, jr_handler_t, rbnode);
      if (event == 0) {
        // Del the event
        jr_rbtree_delete(&poller->fdset, handler->rbnode);
        jr_handler_free(handler);

        jr__poller_update(poller, handler, EPOLL_CTL_DEL);
      } else if (handler->event != event) {
        // Mod the event
        handler->event = event;
        handler->callback = func;

        jr__poller_update(poller, handler, EPOLL_CTL_MOD);
      }
    } else {
      // Add the event
      jr_handler_t *handler = jr_handler_alloc(fd, event, ctx);
      handler->callback = func;
      jr_rbtree_insert(poller->fdset, &handler->rbnode);

      jr__poller_update(poller, handler, EPOLL_CTL_MOD);
    }

  }

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
    int event = 0;
    struct epoll_event ev = poller->events[i];
    jr_handler_t *handler = (jr_handler_t*)ev.data.ptr;
    if (ev.events & EPOLLIN) event |= POLL_READ;
    if (ev.events & EPOLLOUT) event |= POLL_WRITE;

    handler->callback(handler->fd, event, handler->ctx);
  }
}


static void
jr__poller_update(jr_poller_t *poller,
    jr_handler_t *handler, int op)
{
  struct epoll_event event;
  event.events = handler->event;
  event.data.ptr = handler;

  if (epoll_ctl(poller->epfd, op, handler->fd, &event) == -1) {
    abort();
  }
}
