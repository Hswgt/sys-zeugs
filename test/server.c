/*****************************************
 * Demonstration IPv4/IPv6-Client
 * Mit unterstützung von multihomed Servern
 *****************************************/

#define _POSIX_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "messages.h"


/**
 * struct to pass custom data to echo_thread
 **/
struct client_data {
	int sock;
	struct sockaddr_storage addr;
	socklen_t addrlen;
};


/**
 * Copy all data from fd -> fd
 * using a buffer of 512 Byte
 **/

// Client-Thread
#include "server_clientthread.c"


/**
 * Thread to handle connection in background and run clientthread
 * param: struct client_data*
 **/
// Später Client Thread
void* echo_thread(void* param)
{
	char dst[INET6_ADDRSTRLEN];
	struct client_data * data;
	data = (struct client_data*)param;

	/* RTFM: getnameinfo */
	getnameinfo((struct sockaddr*)&data->addr,
			data->addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

	printf("Connection opened from %s\n",dst);
	clientthread(data->sock);
	close(data->sock);
	printf("Connection closed from %s\n", dst);

	free(data);

	pthread_exit(0);
	return NULL;
}

/**
 * Wait for connection on all available sockets
 * fd: array of sockets in listen state
 * numfd: number of valid sockets in fd
 **/
void accept_loop(int fd[], int numfd)
{
	fd_set set;
	int max, i, ret;

	if (numfd < 1) {
		printf("No sockets available!\n");
		return;
	}

	while (1) {
		max = 0;

		FD_ZERO(&set);

		for (i=0; i<numfd; i++) {
			printf("i = %d, max = %d, numfd = %d, fd[%d] = %d\n", i, max, numfd, i, fd[i]);
			if (fd[i] > max) {
				max = fd[i];
				printf("Max ist nun fd[%d], max = %d\n", i, max);
			}
			FD_SET(fd[i], &set);
		}
		/* fd = Liste der Sockets */

		/* wait for first fd that has data */
		ret = select(max+1, &set, NULL, NULL, NULL);

		if (ret <= 0) {
			perror("select");
			return;
		}

		for (i=0; i<numfd; i++)
			if (FD_ISSET(fd[i], &set)) {
				struct client_data *data;

				data = (struct client_data*)malloc(sizeof(struct client_data));

				data->addrlen = sizeof(data->addr);
				data->sock = accept(fd[i], (struct sockaddr*) &data->addr, &data->addrlen);

				if (data->sock < 0) {
					perror("accept");
					free(data);
				} else {
					pthread_t thread_id;
					/* Background new connection */
					pthread_create(&thread_id, NULL, echo_thread, data);
				}
			}
	}
}



int main(int argc, char ** argv)
{
	char *server, *service;
	struct addrinfo *addr_info, *p, hints;
	int ret, i;
	int sockets[128];
	int numsockets = 0;

	if (argc <= 1)
	{
		printf("Simple Echo-Server\nUsage: %s [BINDADDR] [SERVICE]\n", argv[0]);
		printf("\nExample: %s localhost 8080\n", argv[0]);
		exit(1);
	}
	if (argc == 3) {
		server = argv[1];
		service = argv[2];
	} else {
		server = NULL;
		service = argv[1];
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE | AI_V4MAPPED;

	/* RTFM: getaddrinfo */
	ret = getaddrinfo(server, service, &hints, &addr_info);
	if (ret)
	{
		printf("getaddrinfo: %s\n", gai_strerror(ret));
		exit(1);
	}

	p = addr_info;
	while (p)
	{
		int s;
		char dst[INET6_ADDRSTRLEN];
		char service[INET6_ADDRSTRLEN];
		int on = 1;

		/* Create socket for found family */		
		s = socket(p->ai_family, p->ai_socktype, 0);

		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
			perror("setsockopt"); /* maybe not so fatal, continue */
		}
		if (p->ai_family == AF_INET6) {
			if (setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0) {
				perror("setsockopt");
			}
		}

		/* RTFM: getnameinfo */
		getnameinfo(p->ai_addr,
				p->ai_addrlen,
				dst,
				sizeof(dst),
				service,
				sizeof(service),
				NI_NUMERICHOST | NI_NUMERICSERV);

		printf("Trying %s:%s ... ",dst, service);
		fflush(stdout);

		/* Try to bind socket */
		if (bind(s, p->ai_addr, p->ai_addrlen) == 0) {
			if (listen(s, 1) < 0) {
				perror("listen");
				close(s);
			} else 	{
				printf("bind successful\n");
				sockets[numsockets++] = s;
			}
		} else {
			perror("bind");
			close(s);
		}

		p = p->ai_next;
	}

	freeaddrinfo(addr_info);
	printf("Waiting for connections...\n\n");



	accept_loop(sockets, numsockets);

	for (i=0; i<numsockets; i++) {
		close(sockets[i]);
	}
	return 0;
}

