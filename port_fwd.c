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

static volatile int keepRunning = 1;

void intHandler(int dummy)
{
    keepRunning = 0;
}

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

//Sets up the socket to listen on, returns the file descripter
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

    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");
#endif
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

//Sets up a listener on the socket
int listen_on_socket(struct sockaddr_in fwd_addr)
{
    struct	sockaddr_in client;
    int	forward_sd, listen_sd, new_sd, client_len;
    int port = ntohs(fwd_addr.sin_port);

    listen_sd = listen_socket_create(port);
    // queue up to 40 connect requests
    listen(listen_sd, 40);
    while (keepRunning)
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
    exit(0);
}

//Sets up the sockaddr to the destination
struct sockaddr_in parseAddr(char *line)
{
    struct sockaddr_in fwd_addr;
    struct hostent *hp;
    char ip[BUFLEN];
    char unused_char;
    int port;

    bzero((char *)&fwd_addr, sizeof(struct sockaddr_in));
    sscanf(line, "%[^,] %c %d", ip, &unused_char, &port);
    fwd_addr.sin_family = AF_INET;
    fwd_addr.sin_port = htons(port);

    if ((hp = gethostbyname(ip)) == NULL)
    {
        fprintf(stderr, "Unknown server address\n");
    }
    // inet_pton(AF_INET, ip, &(fwd_addr.sin_addr));
    bcopy(hp->h_addr, (char *)&fwd_addr.sin_addr, hp->h_length);

    return fwd_addr;
}

int main (int argc, char **argv)
{
    signal(SIGINT, intHandler);

    pid_t childpid, childpid_array[MAX_PORT_NUMBER];
    int number_of_children = 0;

	int	n, bytes_to_read, i;
	int	forward_sd, listen_sd, new_sd, client_len;

	struct	sockaddr_in server, client, fwd_addr;
	char *bp, buf[BUFLEN];
    char line[BUFLEN];
    char *host;

    //Opens the file for reading
    FILE *fp = fopen("dest_ip_ports.txt", "r");
    while (fgets(line, sizeof(line), fp))
    {
        fwd_addr = parseAddr(&line);

        //Set listeners on children
        childpid = fork();
        if (childpid == 0)
        {
            break;
        }
        else
        {
            //Add to children list for parent to terminate later
            childpid_array[number_of_children] = childpid;
            number_of_children++;
        }
    }

    // forward_sd = host_socket_create(host, port);

    if (childpid == 0)
    {
        //Child Process
        listen_on_socket(fwd_addr);

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
        while(keepRunning);
    }

    //Clear all children after parent is signalled to stop
    for (i = 0; i < number_of_children; i++)
    {
        kill(childpid_array[i], SIGKILL);
    }

	exit(0);
}
