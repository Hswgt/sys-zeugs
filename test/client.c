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
#include <netinet/in.h>
#include <arpa/inet.h>

#include "messages.h"

/**
 * sock -> STDOUT
 * STDIN -> sock
 **/

static void testmessage(int sock)
{

	// Struktur erstellen
	struct LOGINRESPONSEOK response;
	response.header.type = 1;
	response.header.length = sizeof(response);
	response.clientid = 5;

	// Debug
	printf("%d %d \n", response.header.length, sizeof(response));


	printf("Versuche, Daten an den Server zu senden ...\n");

	char buf[] = "Hallo";
	printf("Size of buf: %d \n", sizeof(response));

	int ret = 0;
	ret = write(sock, &response, sizeof(response));
	printf("Gesendet: %d\n", ret);



}


// Sendet den Login-Request an den Server
// 12.04.2011
int SendLoginRequest(int sock, char * player) {
	
	// Erstelle Struktur
	struct LOGINREQUEST request;
	request.header.type = 1;
	request.header.length = 3 + strlen(player);
		
	// unsigned char buf[512];
	memcpy(request.name, player, strlen(player));
	request.name[strlen(player)] =  0;
	
	printf("%s\n", request.name);

	int ret = 0;
	ret = write(sock, &request, request.header.length);

	printf("Gesendet: %d\n", ret);

	return 0;
}




static void echo_loop(int sock)
{
	fd_set set;
	int ret;
	static char buf[512]; // Sende-Empfangspuffer
	
	while (1) {
		int max;
		FD_ZERO(&set);
		FD_SET(sock, &set);
		FD_SET(STDIN_FILENO, &set);
		max = (sock>STDIN_FILENO)?sock:STDIN_FILENO;
		
		/* RTFM: select */
		ret = select(max+1, &set, NULL, NULL, NULL);
		if (ret <= 0) {
			perror("select");
			break;
		}
		
		
 

		/**
		 * sock -> STD_OUT
		 **/

		// Socket >> Standardausgabe
		// Im Puffer steht, was vom Socket gelesen wurde
		if (FD_ISSET(sock, &set)) {
			ret = read(sock, buf, sizeof(buf));
			if (ret == 0) {
				break;
			}
			if (ret < 0) {
				perror("read");
				break;
			}
			if (write(STDOUT_FILENO, buf, ret) < ret) {
				perror("write");
				break;
			}
		}


		/**
		 * STDIN -> sock
		 **/
		// Standardeingabe >> Socket
		// Werte der Standardeingabe werden an Socket übergeben
		if (FD_ISSET(STDIN_FILENO, &set)) {
			
			ret = read(STDIN_FILENO, buf, sizeof(buf));
			
			if (ret == 0) {
				break;
			}
			if (ret < 0) {
				perror("read");
				break;
			}
			if (write(sock, buf, ret) < ret) {
				perror("write");
				break;
			}
		}
	}  
}


int main(int argc, char ** argv)
{
        struct addrinfo *addr_info, *p, hints;
	int ret;

	if (argc <= 2)
	{
		printf("Simple Echo-Client\nUsage: %s [SERVER] [PORT]\n", argv[0]);
		printf("\nExample: %s www.sixxs.net 80\n", argv[0]);
		exit(1);
	}


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = 0 /* | AI_ADDRCONFIG */;

	/* RTFM: getaddrinfo */
        ret = getaddrinfo(argv[1], argv[2], &hints, &addr_info);
        if (ret)
	{
		printf("getaddrinfo: %s\n", gai_strerror(ret));
		exit(1);
	}

	printf("\n");
        p = addr_info;
        while (p)
        {
		int s;
		char dst[INET6_ADDRSTRLEN];

		/* Create socket for found family */		
                s = socket(p->ai_family, p->ai_socktype, 0);

		/* RTFM: getnameinfo */
		getnameinfo(p->ai_addr,
			p->ai_addrlen,
			dst,
			sizeof(dst),
			NULL,
			0,
			NI_NUMERICHOST);

		printf("Trying %s ... ",dst);
		fflush(stdout);

		/* Try to connect */
                if (connect(s, p->ai_addr, p->ai_addrlen) == 0)
                {
                        printf("Connected\n");
			





			/* Do stuff when connected */
			// echo_loop(s);
		

			
			//testmessage(s);		
			SendLoginRequest(s, "Hallo");
			SendLoginRequest(s, "Sandra");
			SendLoginRequest(s, "und");
			SendLoginRequest(s, "Steffen");
			SendLoginRequest(s, "ICH BRAUCHE");
			SendLoginRequest(s, "KAFFFFEEEEE");
			SendLoginRequest(s, ":-) :-)");

			close(s);
			break;
                } else {
			perror("FAILED");
		}
		close(s);

		p = p->ai_next;
        }

        freeaddrinfo(addr_info);
        
	return 0;
}

