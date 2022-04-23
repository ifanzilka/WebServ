#include "Messenger.hpp"
#include <sys/socket.h>

namespace ft
{
	Messenger::Messenger(ServerData &server_data) : _server_data(server_data) {}
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
		std::vector<LocationData> locations = _server_data.GetLocationData();

//		std::vector<LocationData>::iterator bgn = locations.begin();
//		while (bgn != locations.end())
//		{
//			std::cout << *bgn++ << std::endl;
//		}

		std::vector<char> file_buffer;
		std::string file_path = "./resources";
		std::string	http_code = "200 OK";

		//TODO сделать распределение по типам файла
		file_buffer = ReadFile(file_path + _file_path, "r");
		if (file_buffer.empty())
		{
			http_code = "404 Not Found";
			file_buffer = ReadFile(file_path + "/404.html", "r");
		}

		if (_http_method.compare("GET") == 0)
		{
			GetMethod getMethod = GetMethod();
			getMethod.SendHttpResponse(_client_fd, file_buffer, http_code);
		}
	}

	std::vector<char> Messenger::ReadFile(std::string file_path, std::string read_method)
	{
		FILE	*file;
		size_t	file_size;
		std::vector<char>	buffer;

		file = fopen(file_path.c_str(), read_method.c_str());
		if (file != nullptr)
		{
			fseek(file, 0, SEEK_END);
			long file_length = ftell(file);
			rewind(file);

			buffer.resize(file_length);

			file_size = fread(&buffer[0], 1, file_length, file);
			fclose(file);
			printf("\033[32mMessenger successfully read %ld bytes from a file 👍 \033[0m\n", file_size);
		}
		else
			printf("\033[31mUnable to open file! 😔 \033[0m\n");

		return (buffer);
	}
}