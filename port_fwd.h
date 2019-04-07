#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

int forward_socket_create(struct sockaddr_in fwd_addr);
int listen_socket_create(int port_listen);
struct sockaddr_in parseAddr(char *line, int *srcPrtPT);
void listen_on_socket(struct sockaddr_in fwd_addr, int port);
void send_packet(int src_sd, int dst_sd);
void intHandler(int dummy);
