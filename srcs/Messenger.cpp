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

			SendResponse();
		}
		return (rv);
	}

	void Messenger::SendResponse()
	{
		int bytes_written = 0;
		std::ifstream file("./resources/index.html");
		std::string file_data;

		if (file.is_open())
		{
			std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(),
					  std::back_inserter(file_data));

			printf("\033[32mMessenger successfully read %ld bytes from a file 👍 \033[0m\n", file_data.length());

			std::cout << file_data << std::endl;

			file.close();
		}
		else
			printf("\033[31mUnable to open file! 😔 \033[0m\n");

		if (_http_method.compare("GET") == 0)
		{
			std::string header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");

			header += file_data.length();
			header += "\n\n";

			bytes_written += write(_client_fd, header.c_str(), header.size());
			bytes_written += write(_client_fd, file_data.c_str(), file_data.length());
		}
	}

}