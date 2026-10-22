#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "../include/log.h"
#include "../include/ipc.h"
#include "../include/client.h"

void *new_socket_session(void *arg) 
{
	FILE *logger = new_logger();
	char *addr_str = "/tmp/nwm.sock";
	unlink(addr_str);

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);	
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, addr_str);
	
	int code = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
	if (code < 0) 
	{
		write_log(logger, "unable to bind socket!");
		unlink(addr_str);
		close(fd);
		return 0;
	}

	code = listen(fd, 5);
	if (code < 0) 
	{
		write_log(logger, "unable to listen!");
		unlink(addr_str);
		close(fd);
		return 0;
	}

	int addrlen = sizeof(addr);
	write_log(logger, "listening for connections at %s", addr_str);
	while (1) 
	{
		char buf[1024];
		int client_fd = accept(fd, (struct sockaddr *) &addr, (socklen_t *) &addrlen);
		ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
		if (n <= 0)
		{
			return NULL;

		}


		int len = strlen(buf); 
		buf[len] = '\0';



		write_log(logger, "from client %d:%s ", client_fd, buf);


		if (strcmp(buf, "exit") == 0) 
		{
			write_log(logger, "received exit signal...\n");
			char b[] = "received exit signal...";
			send(client_fd, b, strlen(b), 0);
			unlink(addr_str);
			close(fd);
			exit(0);
		}

		else if (strcmp(buf, "windows") == 0)
		{
			int count = 0; 
			for (client_t *it = get_client_head(); it != NULL; it = it->next)
			{
				count++;	
			}


			char b[100]; 
			snprintf(b, sizeof(b), "number of windows open: %d", count);
			send(client_fd, b, sizeof(b), 0);
		}
		else {
			char b[2020];
			snprintf(b, sizeof(b), "invalid command: %s", buf);
			send(client_fd, b, strlen(b), 0);

		}

		
		memset(buf, 0, sizeof(buf));

		close(client_fd);
	}

	close_logger(logger);	
}
