#include "../includes/Messenger.hpp"

namespace ft
{
	Messenger::Messenger()
	{}

	Messenger::~Messenger() {}

	void Messenger::SetClientFd(const int client_fd)
	{
		this->_client_fd = client_fd;
	}

	int Messenger::SetRequest(const int client_fd, std::string request)
	{
		int rv = 1;
		//TODO: if the request is incorrect - return -1
		// else 1
		printf("%s\n", request.c_str());
		SetClientFd(client_fd);

		SendResponse(_client_fd);
		return (rv);
	}

	void Messenger::SendResponse(const int client_fd)
	{
		int byte_wrote = 0;
		std::string	header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");
		std::string body = "<iframe width=\"1200\" height=\"800\"\nsrc=https://www.youtube.com/embed/C3LQfH-YGKM?start=4>\n</iframe>";
		header += body.length();
		header += "\n\n";
		byte_wrote = write(client_fd, header.c_str(), header.size());
		byte_wrote = write(client_fd, body.c_str(), body.length());
	}

}