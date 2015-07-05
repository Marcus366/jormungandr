#ifndef __JOR_SOCKET_H__
#define __JOR_SOCKET_H__


#include <sys/types.h>
#include <sys/socket.h>


int jr_ipaddr_init(struct sockaddr_in *addr,
    const char *ip, unsigned short port);


int jr_tcp_open(struct sockaddr_in *addr);
int jr_tcp_listen(int fd, int backlog);
int jr_tcp_connect(int fd, struct sockaddr_in *addr);
int jr_tcp_accept(int fd, struct sockaddr_in *addr);
int jr_tcp_close(int fd);


#endif

