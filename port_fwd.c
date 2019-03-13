/*---------------------------------------------------------------------------------------
--	SOURCE FILE:		port_fwd.c -   A simple port-forwarder using the Berkeley Socket API
--
--	PROGRAM:		portf
--				gcc -Wall -ggdb -o portf port_fwd.c -lpthread
--
--	FUNCTIONS:		Berkeley Socket API
--
--	DATE:			March 12, 2019
--
--	REVISIONS:		(Date and Description)
--
--	DESIGNERS:		Design based on various code snippets provided by Aman Abdulla in BCIT
--				    Modified and Customized: Piyotr Kao, Quincy Lam - March 2019
--
--	PROGRAMMERS:		Piyotr Kao, Quincy Lam
--
--	NOTES:
--	The program will accept TCP connections from client machines.
-- 	The program will read data from the client socket and forward to the user-entered destination host and port.
--	Design is a simple, multi-threaded recv and send operation, substituting as a direct connection.
--	I/O to handle simultaneous inbound, outbound and concurrent connections.
---------------------------------------------------------------------------------------*/
#include "port_fwd.h"

#define SERVER_TCP_PORT		7000	// Default port
#define BUFLEN			80  	// Buffer length
#define TRUE            1

int host_socket_create(char *fwd_host, int fwd_port)
{
    int sd, port;
    char *host;
    struct sockaddr_in server;

    // Create a stream socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror ("Can't create a socket");
		exit(1);
	}

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		perror("Can't bind name to socket");
		exit(1);
	}

    return sd;
}

int main (int argc, char **argv)
{
	int	n, bytes_to_read;
	int	sd, new_sd, client_len, port;
	struct	sockaddr_in server, client;
	char	*bp, buf[BUFLEN];
    char *host;

	switch(argc)
	{
		case 1:
			port = SERVER_TCP_PORT;	// Use the default port
		break;
		case 2:
			port = atoi(argv[1]);	// Get user specified port
		break;
		default:
			fprintf(stderr, "Usage: %s [port]\n", argv[0]);
			exit(1);
	}

	// Listen for connections
    sd = host_socket_create(host, port);

	// queue up to 5 connect requests
	listen(sd, 5);

	while (TRUE)
	{
		client_len= sizeof(client);
		if ((new_sd = accept (sd, (struct sockaddr *)&client, &client_len)) == -1)
		{
			fprintf(stderr, "Can't accept client\n");
			exit(1);
		}

		printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
		bp = buf;
		bytes_to_read = BUFLEN;
		while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFLEN)
		{
			bp += n;
			bytes_to_read -= n;
		}
		printf ("sending:%s\n", buf);

		send (new_sd, buf, BUFLEN, 0);
		close (new_sd);
	}
	close(sd);
	return(0);
}