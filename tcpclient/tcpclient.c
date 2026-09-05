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

int main(int argc, char *argv[])
{
	struct addrinfo hints, *servinfo, *p;
	int sockfd;
	int ok;
	int connected = 0;

	if (argc != 3)
	{
		printf("Wrong arg count. Usage: ./tcpclient host port\n");
		return 1;
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((ok = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0)
	{
		printf("gai error: %s\n", gai_strerror(ok));
		return 1;
	}

	for (p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			perror("socket");
			continue;
		}

		char addrtext[INET6_ADDRSTRLEN];
		struct sockaddr *addr = p->ai_addr;
		struct sockaddr_in *sa = (struct sockaddr_in *)addr;
		inet_ntop(addr->sa_family, &(sa->sin_addr), addrtext, p->ai_addrlen);
		printf("trying to connect to: %s\n", addrtext);

		if ((ok = connect(sockfd, p->ai_addr, p->ai_addrlen)) != 0)
		{
			perror("connect");
			close(sockfd);
			continue;
		}

		printf("connected!\n");
		connected = 1;
		break;
	}
	freeaddrinfo(servinfo);

	if (!connected)
	{
		printf("failed connecting\n");
	}

	char message[1024];
	size_t msg_len = snprintf(message, sizeof(message),
							  "GET / HTTP/1.1\r\n"
							  "Host: %s\r\n"
							  "Connection: close\r\n\r\n", argv[1]);
	// size_t msg_len = strlen(message);
	printf("Trying to get: %.*s\n", (int)(msg_len - 4),
		   message); // removing the carriage return

	size_t sent = 0;
	while (sent != msg_len)
	{
		int bytes_sent = send(sockfd, message, msg_len, 0);
		if (bytes_sent == -1)
		{
			perror("send");
			continue;
		}

		sent += bytes_sent;
	}

	printf("receiving response after request\n");
	size_t tot_recv = 0;
	char buffer[100];

	size_t msg_buf_size = 100;
	char *complete_msg = calloc(msg_buf_size, sizeof(char));

	int retry_count = 0;

	if (complete_msg == NULL)
	{
		while (retry_count < 3)
		{
			perror("calloc");
			complete_msg = calloc(msg_buf_size, sizeof(char));
			retry_count++;
		}
	}
	if (complete_msg == NULL)
	{
		perror("calloc");
		close(sockfd);
		return 1;
	}

	while (1)
	{
		int bytes_recv = recv(sockfd, buffer, sizeof(buffer), 0);
		if (bytes_recv == 0)
			break;
		int prev = tot_recv;
		tot_recv += bytes_recv;

		if (tot_recv >= msg_buf_size)
		{
			msg_buf_size *= 2;

			// won't retry for simplicity
			char *new_msg = realloc(complete_msg, msg_buf_size);
			if (new_msg == NULL)
			{
				perror("realloc");
				close(sockfd);
				return 1;
			}

			complete_msg = new_msg;
		}

		memcpy(&complete_msg[prev], buffer, sizeof(buffer));
	}

	printf("finished request with %zu bytes received.\n", tot_recv);
	printf("message recv: %s\n", complete_msg);
	free(complete_msg);
	close(sockfd);
}
