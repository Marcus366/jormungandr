#include <string.h>
#include "handler.h"
#include "alloc.h"

jor_handler_t*
jor_handler_alloc(int fd, uint32_t event, void *ctx)
{
  jor_handler_t *handler = (jor_handler_t*)jor_alloc(sizeof(jor_handler_t));

  bzero(handler, sizeof(jor_handler_t));
  handler->fd = fd;
  handler->mask = event;
  handler->ctx = ctx;

  return handler;
}


void
jor_handler_free(jor_handler_t *handler)
{
  //FIXME Check whether remove from poller.
  jor_free(handler);
}

