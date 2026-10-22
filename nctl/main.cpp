#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <cstring>
#include <unistd.h>

#include "libs/tinycli/cli.hpp"

int main (int argc, char **argv)
{
	auto cli = Cli(argc, argv, "nctl", "Command line utility for configuring nwm");
	cli.add_option("-h", "Display help", true);
	cli.add_option("--raw", "Enter raw command", false);

	if (cli.is_command("-h")) 
	{
		cli.print_help();
		return 0;
	}

	
	std::string raw_string = cli.get_option("--raw");

	std::string addr_str = "/tmp/nwm.sock";
	struct sockaddr_un server_addr;
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0)
	{
		std::cout << "unable to init socket.\n";
		return 1;
	}	

	server_addr.sun_family = AF_UNIX;
	std::strcpy(server_addr.sun_path, addr_str.c_str());


	int code = connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (code < 0) 
	{
		std::cout << "failed to connect!\n";
		return 1;
	}

	std::cout << "query: " << raw_string << std::endl;
	if (!raw_string.empty()) send(fd, raw_string.c_str(), raw_string.size(), 0);

	char buf[256]; 
	ssize_t n = recv(fd, buf, sizeof(buf), 0);

	buf[n] = '\0';
	std::cout << "Response: " << buf << std::endl;
	close(fd);
	return 0;
}
