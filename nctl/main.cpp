#include <exception>
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <cstring>
#include <unistd.h>
#include "libs/argparse/include/argparse/argparse.hpp"

int main (int argc, char **argv)
{
	argparse::ArgumentParser program("nctl");
	program.add_description("command line utility for configuring nwm");
	
	program.add_argument("-r", "--raw") 
		.default_value("")
		.help("Enter raw commands");

	argparse::ArgumentParser bind_subcommand("bind");
	bind_subcommand.add_description("use this to add a keybinding");

	program.add_subparser(bind_subcommand);

	try 
	{
		program.parse_args(argc, argv);
	} 
	catch (const std::exception &e) 
	{
		std::cout << "Failed to parse command line arguments.\n";
		return 1;
	}

	std::string raw_string = program.get("--raw");

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


	int code = connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (code < 0) 
	{
		std::cout << "failed to connect!\n";
		return 1;
	}

	if (!raw_string.empty()) send(fd, raw_string.c_str(), raw_string.size(), 0);
	close(fd);
	return 0;
}
