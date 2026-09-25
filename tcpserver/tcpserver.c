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

#define BACKLOG 10

typedef int server_state;
struct tcp_server
{
	char *port;
	int sockfd;
    int accepted_fd;
    server_state state;
};


// struct message
// {
// };
//
// void start_listen(struct server *sv, (void)(*on_request)(struct message msg))
// {}

int create_server(struct tcp_server *sv)
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
		exit(1);
	}

	if ((ok = listen(sockfd, BACKLOG)) == -1)
	{
		perror("listen");
		close(sockfd);
		exit(1);
	}
	fprintf(stdout, "Listening on %s...\n", sv->port);

	struct sockaddr_storage their_addr;
	socklen_t sin_size;
    // improve this with fork
	int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if (new_fd < 0)
	{
		perror("accept");
		close(sockfd);
		exit(1);
	}

	char s[INET_ADDRSTRLEN];
	struct sockaddr_in *si = (struct sockaddr_in *)&their_addr;
	inet_ntop(their_addr.ss_family, (struct sockaddr *)&si->sin_addr, s,
			  sizeof(s));
	fprintf(stdout, "Connected to %s\n", s);

	sv->sockfd = sockfd;
    sv->accepted_fd = new_fd;

	return 0;
}

void recv_loop(struct tcp_server *server, int (*on_message_recv)(char *, size_t))
{
    int buffer_size = 1024;
    char *buffer = calloc(buffer_size, sizeof(char));

    int bytes_rcv = 0;
    int total_bytes = 0;
    while(1)
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

        bytes_rcv = recv(server->accepted_fd, buffer, buffer_size, 0);
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

        //TODO create on_message_recv arg and a http func of this
        int bytes_processed = on_message_recv(buffer, total_bytes);
        if (bytes_processed == -1)
        {
            fprintf(stderr, "Failed processing received message");
            break;
        }

        // get the not consumed portion of the buffer and 
        // move it to the beginning
        char *not_consumed_ptr = buffer + bytes_processed;
        size_t not_consumed_size = buffer_size - bytes_processed;
        memmove(buffer, not_consumed_ptr, not_consumed_size);

        // zero out any garbage data after moving the buffer
        size_t empty_buffer_size = buffer_size - not_consumed_size;
        char *not_consumed_end = buffer + not_consumed_size;
        memset(not_consumed_end, 0, empty_buffer_size);

        total_bytes -= bytes_processed;
    }

    free(buffer);
}

void close_server(struct tcp_server *server)
{
    close(server->accepted_fd);
    close(server->sockfd);
}

int r_msg(char *buf, size_t size)
{
    printf("%s\n", buf);
    return size;
}

// create a tcp server capable of knowing how to join message chunks into a
// message the definition of a complete message (with how we can say it is
// complete) will be given from the protocol that is built on the server
int main(void)
{
    printf("Starting server at port 3333\n");

    struct tcp_server sv;
    sv.port = "3333";

    // in the future, test with stdin and out file descriptors
    create_server(&sv);
    recv_loop(&sv, r_msg);
    close_server(&sv);
    printf("Server shutting down...\n");
}
