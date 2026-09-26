#ifndef TCPSERVER_H

#define TCPSERVER_H
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

int init_tcpserver(struct tcpserver *sv);
int handle_connections(struct tcpserver *sv);
void recv_loop(int fd, ssize_t (*on_message_recv)(char *, size_t, int));
void close_server(struct tcpserver *server);
ssize_t s_msg(int fd, char *msg, size_t msg_size);
ssize_t r_msg(char *buf, size_t size, int fd);

#endif
