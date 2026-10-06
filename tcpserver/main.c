#include "tcpserver.h"

#include <stdio.h>
#include <unistd.h>

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
		fprintf(stdout, "Failed server init");
		close_server(&sv);
		return -1;
	}

	while (1)
	{
		// fills the accepted fd on server for now
		if ((ok = handle_connections(&sv, r_msg)) != 0)
		{
			fprintf(stdout, "Failed accepting connections\n");
			break;
		}
	}

	close(sv.sockfd);
	printf("Server shutting down...\n");
}
