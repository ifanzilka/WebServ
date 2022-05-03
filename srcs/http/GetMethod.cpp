#include "Include_Http_Library.hpp"

GetMethod::GetMethod() {}
GetMethod::~GetMethod() {}

int GetMethod::SendHttpResponse(int client_fd, std::string &file_buffer,
	std::string _http_code, HttpData *_client_data)
{
	int bytes_written = 0;
	std::string http_version = "HTTP/1.1 ";
	std::string http_code = _http_code + "\n";

	//TODO добавить определение Content-Type и Content-Length
	std::string header(http_version + http_code + "Content-Type: text\nContent-Length: ");
//	std::string header(http_version + http_code);

//	header += "Content-Type: text/html\n";
//	header += "Connection: keep-alive\n";

	header += "\n\n";

//	std::cout << header << std::endl;

	bytes_written += send(client_fd, &header.c_str()[0], header.size(), 0);

	for (int sum = 0, res = 0; sum < file_buffer.size();)
	{
		res = send(client_fd, &file_buffer[sum], file_buffer.size() - sum, 0);
		if (res == -1)
			continue;
		bytes_written += res;
		sum += res;
	}
	printf("\033[32mGetMethod::SendHttpResponse: HAVE WRITTEN: \033[34m%d\033[0m\n", bytes_written);
	return (0);
}