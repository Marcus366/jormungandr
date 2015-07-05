#include <string.h>
#include "handler.h"
#include "alloc.h"

jr_handler_t*
jr_handler_alloc(int fd, uint32_t event, void *ctx)
{
  jr_handler_t *handler = (jr_handler_t*)jr_alloc(sizeof(jr_handler_t));

  bzero(handler, sizeof(jr_handler_t));
  handler->fd = fd;
  handler->mask = event;
  handler->ctx = ctx;

  return handler;
}


void
jr_handler_free(jr_handler_t *handler)
{
  //FIXME Check whether remove from poller.
  jr_free(handler);
}

