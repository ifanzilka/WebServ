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

	std::string create_html_output_for_binary(const std::string &full_path);
	std::vector<char> read_file(std::string file_path, std::string read_method);

	//TODO: не отображает большой файл
	void Messenger::SendResponse()
	{
		std::vector<char> buffer;
		std::string method;
		std::string file_data;
		std::string file_path;

		method = "binary";
//		method = "text";

		if (method.compare("text") == 0)
		{
			file_path = "./resources/index.html";
			buffer = read_file(file_path, "r");
		}
		else
		{
//			file_path = "./resources/mario_2.png";
			file_path = "./resources/linux.png";
			buffer = read_file(file_path, "rb");
		}

		int bytes_written = 0;
		if (_http_method.compare("GET") == 0)
		{
			std::string header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");

			header += buffer.size();
			header += "\n\n";

			std::string file_data(buffer.begin(), buffer.end());
			bytes_written += write(_client_fd, header.c_str(), header.size());
			bytes_written += write(_client_fd, file_data.c_str(), file_data.length());
		}
	}

	std::vector<char> read_file(std::string file_path, std::string read_method)
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


	void reopen_file(std::string full_path);

	std::string create_html_output_for_binary(const std::string &full_path)
	{
		std::vector<char> buffer;

		FILE* file_stream = fopen(full_path.c_str(), "rb");

//		string file_str;

		size_t file_size;

		if (file_stream != nullptr)
		{
			fseek(file_stream, 0, SEEK_END);
			long file_length = ftell(file_stream);
			rewind(file_stream);

			buffer.resize(file_length);

			file_size = fread(&buffer[0], 1, file_length, file_stream);
			fclose(file_stream);
		}
		else
		{
			printf("file_stream is null! file name -> %s\n", full_path.c_str());
		}

		std::string html = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n" + std::string("FILE NOT FOUND!!");
		if (!buffer.empty())
		{
			// HTTP/1.0 200 OK
			// Server: cchttpd/0.1.0
			// Content-Type: image/gif
			// Content-Transfer-Encoding: binary
			// Content-Length: 41758

			std::string binary_str(buffer.begin(), buffer.end());

			std::string file_size_str = std::to_string(binary_str.length());

			html = "HTTP/1.1 200 Okay\r\nContent-Type: image/png; Content-Transfer-Encoding: binary; Content-Length: " + file_size_str + ";charset=ISO-8859-4 \r\n\r\n" + binary_str;


//			reopen_file("./resources/marr.png");

//			fwrite (buffer.data(), 1 , buffer.size() , stdout);
//			printf("\n\nHTML -> %s\n\nfile_str -> %ld\n\n\n", html.c_str(), binary_str.length());
//			write(1, html.c_str(), binary_str.length());
		}
		return html;
	}

	void reopen_file(std::string file_path)
	{
		int stdout_dupfd;
		FILE *temp_out;

		/* duplicate stdout */
		stdout_dupfd = dup(1);

		temp_out = fopen(file_path.c_str(), "w");

		/* replace stdout with our output fd */
		dup2(fileno(temp_out), 1);
		/* output something... */
		printf("Woot!\n");
		/* flush output so it goes to our file */
		fflush(stdout);
		fclose(temp_out);
		/* Now restore stdout */
		dup2(stdout_dupfd, 1);
		close(stdout_dupfd);
	}
}