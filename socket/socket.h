#ifndef __JOR_SOCKET_H__
#define __JOR_SOCKET_H__


#include <sys/types.h>
#include <sys/socket.h>


int jor_ipaddr_init(struct sockaddr_in *addr,
    const char *ip, unsigned short port);


int jor_tcp_open(struct sockaddr_in *addr);
int jor_tcp_listen(int fd, int backlog);
int jor_tcp_connect(int fd, struct sockaddr_in *addr);
int jor_tcp_accept(int fd, struct sockaddr_in *addr);
int jor_tcp_close(int fd);


#endif

