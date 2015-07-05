#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket.h"

int
jr_ipaddr_init(struct sockaddr_in *addr,
    const char *ip, unsigned short port)
{
  bzero(addr, sizeof(struct sockaddr_in));

  addr->sin_family = AF_INET;
  addr->sin_port = htons(port);

  return inet_pton(AF_INET, ip, &addr->sin_addr);
}


int
jr_tcp_open(struct sockaddr_in *addr)
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    return -1;
  }

  if (addr != NULL) {
    int err = bind(fd, (struct sockaddr*)addr, sizeof(struct sockaddr_in));
    if (err == -1) {
      close(fd);
      return -1;
    }
  }

  return fd;
}


int
jr_tcp_listen(int fd, int backlog)
{
  return listen(fd, backlog);
}


int
jr_tcp_connect(int fd, struct sockaddr_in *addr)
{
  return connect(fd, (struct sockaddr*)addr, sizeof(struct sockaddr_in));
}


int
jr_tcp_accept(int fd, struct sockaddr_in *addr)
{
  bzero(addr, sizeof(struct sockaddr_in));
  socklen_t len = sizeof(struct sockaddr_in);

  return accept(fd, (struct sockaddr*)addr, &len);
}


int
jr_tcp_close(int fd)
{
  return close(fd);
}

