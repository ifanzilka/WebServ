#include "../includes/Messenger.hpp"

namespace ft
{
	Messenger::Messenger(const int client_fd)
		: _client_fd(client_fd)
	{}

	Messenger::~Messenger() {}

//	int HTTPParser::SetClientFd(const int client_fd)
//	{
//		int ret = -1;
//
//		if (client_fd >= 0)
//		{
//			this->_client_fd = client_fd;
//			ret = 1;
//		}
//		else
//			printf("Incorrect client_fd!\n");
//		return (ret);
//	}

	void Messenger::GetRequest(const int client_fd)
	{
		char	client_buff[1024];
		int		byte_readed;

		byte_readed = read(client_fd, client_buff, 1024);
		client_buff[byte_readed] = '\0';
		printf("%s\n", client_buff);

		SendResponse(client_fd);
	}

	void Messenger::SendResponse(const int client_fd)
	{
		int byte_wrote = 0;

		std::string body = "<iframe width=\"1200\" height=\"800\"\nsrc=https://www.youtube.com/embed/YXna1vdzg1c?mute=1&amp;autoplay=1>\n</iframe>";
		std::string	header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");
		header += body.length();
		header += "\n\n";
		byte_wrote = write(client_fd, header.c_str(), header.size());
		byte_wrote = write(client_fd, body.c_str(), body.length());
	}

}