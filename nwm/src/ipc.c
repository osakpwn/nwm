#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "../include/ipc.h"

void *new_socket_session() 
{
	char *addr_str = "nwm.sock";
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);	
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, addr_str);
	
	int code = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (code < 0) 
	{
		printf("unable to bind socket!\n");
		return 0;
	}

	code = listen(fd, 5);
	if (code < 0) 
	{
		printf("unable to listen!\n");
		return 0;
	}

	int addrlen = sizeof(addr);
	printf("listening for connections at %s\n", addr_str);
	while (1) 
	{
		char buf[1024];
		int client_fd = accept(fd, (struct sockaddr *) &addr, (socklen_t *) &addrlen);
		recv(client_fd, buf, sizeof(buf), 0);
		printf("res: %s\n", buf);


		if (strcmp(buf, "exit\n") == 0) 
		{
			printf("received exit signal...\n");
			exit(0);
		}
		memset(buf, 0, sizeof(buf));
	}
	
}
