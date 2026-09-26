#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//#include "tcpserver.h"

#define BACKLOG 10
#define BUFFER_INITIAL_SIZE 1024

enum ServerState
{
	CREATED = 0,
	INITIALIZED = 1,
	LISTENING = 2,
	CLOSED = 3
};
typedef int server_state;
struct tcpserver
{
	char *port;
	int sockfd;
	int accepted_fd;
	server_state state;
};

int init_tcpserver(struct tcpserver *sv)
{
	struct addrinfo hints, *servinfo, *p;
	int sockfd;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // using only ipv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use own ip

	int ok;
	if ((ok = getaddrinfo(NULL, sv->port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "gai error: %s\n", gai_strerror(ok));
		return -1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sockfd < 0)
		{
			perror("socket");
			continue;
		}

		if ((ok = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1},
							 sizeof(int))) != 0)
		{
			perror("setsockopt");
			close(sockfd);
			continue;
		}

		if ((ok = bind(sockfd, p->ai_addr, p->ai_addrlen)) < 0)
		{
			perror("bind");
			close(sockfd);
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);

	if (p == NULL)
	{
		fprintf(stderr, "Server failed to bind\n");
		return -1;
	}

	if ((ok = listen(sockfd, BACKLOG)) == -1)
	{
		perror("listen");
		close(sockfd);
		return -1;
	}
	sv->sockfd = sockfd;
	fprintf(stdout, "Listening on %s...\n", sv->port);
	return 0;
}

int handle_connections(struct tcpserver *sv)
{
	struct sockaddr_storage their_addr;
	socklen_t sin_size = sizeof(their_addr);

	// improve this with fork
	int new_fd = accept(sv->sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd < 0)
	{
		perror("accept");
		return -1;
	}

	char s[INET_ADDRSTRLEN];
	struct sockaddr_in *si = (struct sockaddr_in *)&their_addr;
	inet_ntop(their_addr.ss_family, (struct sockaddr *)&si->sin_addr, s,
			  sizeof(s));
	fprintf(stdout, "Connected to %s\n", s);

	// should be an array of fd, for fork
	sv->accepted_fd = new_fd;

	return 0;
}

void recv_loop(int fd, ssize_t (*on_message_recv)(char *, size_t, int))
{
	int buffer_size = BUFFER_INITIAL_SIZE;
	char *buffer = calloc(buffer_size, sizeof(char));

	int bytes_rcv = 0;
	int total_bytes = 0;
	while (1)
	{
		// buffer filled, realoc
		if (total_bytes == buffer_size)
		{
			size_t prev_buffer_size = buffer_size;
			buffer_size *= 2;
			char *p = realloc(buffer, buffer_size);
			if (p == NULL)
			{
				perror("realloc");
				break;
			}

			memset(p + prev_buffer_size, 0, buffer_size - prev_buffer_size);
			buffer = p;
		}

		bytes_rcv =
			recv(fd, buffer + total_bytes, buffer_size - total_bytes, 0);
		if (bytes_rcv == -1)
		{
			perror("recv");
			break;
		}
		if (bytes_rcv == 0)
		{
			fprintf(stdout, "Received empty msg. Client closed \n");
			break;
		}

		total_bytes += bytes_rcv;

		int bytes_processed = on_message_recv(buffer, total_bytes, fd);
		if (bytes_processed == -1 || bytes_processed > total_bytes)
		{
			fprintf(stderr, "Failed processing received message");
			break;
		}

		// get the not consumed portion of the buffer and
		// move it to the beginning
		char *not_consumed_ptr = buffer + bytes_processed;
		size_t not_consumed_size = total_bytes - bytes_processed;
		memmove(buffer, not_consumed_ptr, not_consumed_size);

		// zero out any garbage data after moving the buffer
		size_t empty_buffer_size = buffer_size - not_consumed_size;
		char *not_consumed_end = buffer + not_consumed_size;
		memset(not_consumed_end, 0, empty_buffer_size);

		total_bytes -= bytes_processed;
	}

	free(buffer);
}

void close_server(struct tcpserver *server)
{
	close(server->accepted_fd);
	close(server->sockfd);
}

ssize_t s_msg(int fd, char *msg, size_t msg_size)
{
	// improve this in the future
	send(fd, msg, msg_size, 0);
	return 0;
}

ssize_t r_msg(char *buf, size_t size, int fd)
{
	size_t msg_size = 0;
	int msg_found = 0;
	for (size_t i = 0; i < size; i++)
	{
		msg_size++;

		if (*(buf + i) == '\n')
		{
			msg_found = 1;
			break;
		}
	}

	if (msg_found)
	{
		char s_buf[msg_size];
		memcpy(s_buf, buf, msg_size);

		int ok;
		if ((ok = s_msg(fd, s_buf, msg_size)) != 0)
		{
			printf("Failed sending msg with len %zu\n", msg_size);
			printf("msg: %s", s_buf);
			return 0;
		}

		printf("Sent msg with len %zu\n", msg_size);
		printf("msg: %s",
			   s_buf); // don't include \n because the message ends with it
		return msg_size;
	}

	return 0;
}

// create a tcp server capable of knowing how to join message chunks into a
// message the definition of a complete message (with how we can say it is
// complete) will be given from the protocol that is built on the server
int main(void)
{
	int ok;
	struct tcpserver sv = {
		.port = "3333", .sockfd = -1, .accepted_fd = -1, .state = CLOSED};

	printf("Starting server at port 3333\n");

	if ((ok = init_tcpserver(&sv)) != 0)
	{
		printf("Failed server init");
		close_server(&sv);
		return -1;
	}

	// fills the accepted fd on server for now
	if ((ok = handle_connections(&sv)) != 0)
	{
		printf("Failed accepting connections");
		close_server(&sv);
		return -1;
	}

	// in the future, test with stdin and out file descriptors
	recv_loop(sv.accepted_fd, r_msg);

	close_server(&sv);
	printf("Server shutting down...\n");
}
