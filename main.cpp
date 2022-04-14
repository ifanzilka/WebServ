

#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

// http://localhost:8000

#define SERVER_PORT    "8000"
#define MAX_CONNECTION 10

#include "../includes/Messenger.hpp"

int main()
{
	int sock_fd;
	struct sockaddr_in server_info;

	server_info.sin_family = AF_INET;
	server_info.sin_addr.s_addr = INADDR_ANY;
	server_info.sin_port = htons(atoi(SERVER_PORT));

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
		throw std::runtime_error("setsockopt error\n");
	printf("server socket id: %d\n", sock_fd);

	int yes = 1;
	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
		throw std::runtime_error("setsockopt error\n");

	if (bind(sock_fd, (struct sockaddr *)&server_info, sizeof(server_info)) < 0)
		throw std::runtime_error("bind error\n");

	if (listen(sock_fd, MAX_CONNECTION) < 0)
		throw std::runtime_error("listen error\n");
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(server_info.sin_addr), ip, INET_ADDRSTRLEN);
	printf("Server %s:%d is listening...\n", ip, atoi(SERVER_PORT));

	while (1)
	{
		ft::Messenger *parser;
		int connect_fd;
		int rv;

		socklen_t addr_len = sizeof(server_info);
		connect_fd = accept(sock_fd, (struct sockaddr *) &server_info, &addr_len);
		if (connect_fd < 0) {
			perror("Accept error: ");
			throw std::runtime_error("accept error\n");
		}
		printf("A new connection with fd %d has been accepted\n", connect_fd);

		parser = new ft::Messenger(connect_fd);
		parser->GetRequest(connect_fd);
		delete parser;
	}
}
