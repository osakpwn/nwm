#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <unistd.h>

int main (int argc, char **argv)
{
	if (argc < 2)
	{
		std::cout << "Not enough arguments!\n";
		return 1;
	}
	std::string addr_str = "../nwm.sock";
	struct sockaddr_un server_addr;
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0)
	{
		std::cout << "unable to init socket.\n";
		return 1;
	}	

	server_addr.sun_family = AF_UNIX;
	std::strcpy(server_addr.sun_path, addr_str.c_str());


	int server_fd = connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	std::string buf = argv[1];
	send(server_fd, buf.c_str(), buf.size(), 0);

	close(fd);
	return 0;
}
