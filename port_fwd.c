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

#define MAX_PORT_NUMBER     65535
#define SERVER_TCP_PORT		7000	// Default port
#define BUFLEN			80  	// Buffer length
#define TRUE            1

// int connect_socket_create(char *fwd_host, int fwd_port)
// {
//     int sd;
//     char *host;
//     struct sockaddr_in server;
//
//     // Create a stream socket
// 	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
// 	{
// 		perror ("Can't create a socket");
// 		exit(1);
// 	}
//
// 	// Bind an address to the socket
// 	bzero((char *)&server, sizeof(struct sockaddr_in));
// 	server.sin_family = AF_INET;
// 	server.sin_port = htons(fwd_port);
// 	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client
//
// 	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
// 	{
// 		perror("Can't bind name to socket");
// 		exit(1);
// 	}
//
//     return sd;
// }

int listen_socket_create(int port_listen)
{
    int sd;
    struct sockaddr_in server;

    // Create a stream socket
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Can't create a socket");
		exit(1);
	}

	// Bind an address to the socket
	bzero((char *)&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(port_listen);
	server.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any client

	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
        perror("Can't bind name to socket");
        exit(1);
	}

    return sd;
}

int listen_on_socket(int port, struct sockaddr_in fwd_addr)
{
    struct	sockaddr_in server, client;
    int	forward_sd, listen_sd, new_sd, client_len;
    listen_sd = listen_socket_create(port);
    // queue up to 40 connect requests
    listen(listen_sd, 40);
    while (TRUE)
    {
        client_len= sizeof(client);
        if ((new_sd = accept (listen_sd, (struct sockaddr *)&client, &client_len)) == -1)
        {
            fprintf(stderr, "Can't accept client\n");
            exit(1);
        }
        fprintf(stdout, "Message from port %d\n", port);
        fflush(stdout);
    }
}

int main (int argc, char **argv)
{
    pid_t childpid;

	int	n, bytes_to_read;
	int	forward_sd, listen_sd, new_sd, client_len, port;

	struct	sockaddr_in server, client, fwd_addr[MAX_PORT_NUMBER];
	char *bp, buf[BUFLEN];
    char *host;
    char line[BUFLEN];
    char ip[BUFLEN];
    char unused_char;

    //Opens the file for reading
    FILE *fp = fopen("dest_ip_ports.txt", "r");
    while (fgets(line, sizeof(line), fp))
    {
        sscanf(line, "%[^,] %c %d", ip, &unused_char, &port);
        fwd_addr[port].sin_family = AF_INET;
        fwd_addr[port].sin_port = htons(port);
        inet_pton(AF_INET, ip, &(fwd_addr[port].sin_addr));
        childpid = fork();
        if (childpid == 0)
        {
            break;
        }
    }

    // forward_sd = host_socket_create(host, port);

    if (childpid == 0)
    {
        //Child Process
        listen_on_socket(port, fwd_addr[port]);

        //
    	// 	printf(" Remote Address:  %s\n", inet_ntoa(client.sin_addr));
    	// 	bp = buf;
    	// 	bytes_to_read = BUFLEN;
    	// 	while ((n = recv (new_sd, bp, bytes_to_read, 0)) < BUFLEN)
    	// 	{
    	// 		bp += n;
    	// 		bytes_to_read -= n;
    	// 	}
    	// 	printf ("sending:%s\n", buf);
        //
    	// 	send (new_sd, buf, BUFLEN, 0);
    	// 	close (new_sd);
    }
    else
    {
        //Parent Process
        while(1);
    }

	close(listen_sd);
	return(0);
}
