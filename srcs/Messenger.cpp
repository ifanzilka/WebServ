#include "Messenger.hpp"

namespace ft
{
	Messenger::Messenger()
	{
	}

	Messenger::~Messenger() {}

	void Messenger::SetClientFd(const int client_fd)
	{
		this->_client_fd = client_fd;
	}

	int Messenger::SetRequest(const int client_fd, std::string request)
	{
		int rv = 1;
		ft::RequestParser requestParser = ft::RequestParser();

		if (request.empty()
			|| (_http_method = requestParser.GetHttpMethod(request)).empty())
			rv = -1;
		else
		{
			SetClientFd(client_fd);
			_file_path = requestParser.GetFilePath(request);
			//TODO: убедиться в том, что версия HTTP протокола нам не нужна
			// (строка ниже удаляет эту информацию)
			request.erase(0, request.find("\n") + 1);
			_headers = requestParser.GetHeaders(request);

			SendResponse(_client_fd);
		}
		return (rv);
	}

	void Messenger::SendResponse(const int client_fd)
	{
		int byte_wrote = 0;
		std::string	header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");
		std::string first_body = "<div style=\"text-align: center;\"><iframe width=\"1600\" height=\"800\"\nsrc=https://www.youtube.com/embed/C3LQfH-YGKM?start=4>\n</iframe></div>";
		std::string second_body = "<div style=\"text-align: center;\"><iframe width=\"1600\" height=\"800\"\nsrc=https://www.youtube.com/embed/C3LQfH-YGKM?start=4>\n</iframe></div>";
		header += first_body.length();
		header += second_body.length();
		header += "\n\n";
		byte_wrote = write(client_fd, header.c_str(), header.size());
		byte_wrote = write(client_fd, first_body.c_str(), first_body.length());
		byte_wrote = write(client_fd, second_body.c_str(), second_body.length());
	}

}