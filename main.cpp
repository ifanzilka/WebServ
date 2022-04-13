

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
		int connect_fd;

		socklen_t addr_len = sizeof(server_info);
		connect_fd = accept(sock_fd, (struct sockaddr *) &server_info, &addr_len);
		if (connect_fd < 0) {
			perror("Accept error: ");
			throw std::runtime_error("accept error\n");
		}
		printf("A new connection with fd %d has been accepted\n", connect_fd);

		char client_buff[1024];
		int		byte_readed;

			byte_readed = read(connect_fd, client_buff, 1024);
			client_buff[byte_readed] = '\0';
			printf("%s\n", client_buff);

		int byte_wrote;

		std::string	header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");
		std::string body = "<iframe width=\"1200\" height=\"800\"\nsrc=https://www.youtube.com/embed/YXna1vdzg1c?mute=1&amp;autoplay=1>\n</iframe>";
		header += body.length();
		header += "\n\n";
		byte_wrote = write(connect_fd, header.c_str(), header.size());
		byte_wrote = write(connect_fd, body.c_str(), body.length());
	}
}
