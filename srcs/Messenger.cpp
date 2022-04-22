#include "Messenger.hpp"
#include <sys/socket.h>

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
		std::vector<char>	file_buffer;
		std::string			method;
		std::string			file_path;

		method = "binary";
//		method = "text";

		//TODO сделать распределение по типам файла
		// и получение пути к файла извне
		if (method.compare("text") == 0)
		{
			file_path = "./resources/index.html";
			file_buffer = ReadFile(file_path, "r");
		}
		else
		{
//			file_path = "./resources/Audi.jpeg";
			//file_path = "./resources/fera.jpeg";
			file_path = "./resources/tmp.mp4";
//			file_path = "./resources/mario_2.png";
//			file_path = "./resources/linux.png";
			file_buffer = ReadFile(file_path, "rb");
		}

		if (_http_method.compare("GET") == 0)
		{
			GetMethod getMethod = GetMethod();
			//TODO отослать буффер по ссылке
			getMethod.SendHttpResponse(_client_fd, file_buffer);
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