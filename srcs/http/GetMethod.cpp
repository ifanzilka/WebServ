#include "Include_Http_Library.hpp"

GetMethod::GetMethod()
{}

GetMethod::~GetMethod()
{}

int GetMethod::SendHttpResponse(int client_fd, std::vector<char>	file_buffer)
{
	int bytes_written = 0;
	//TODO добавить определение Content-Type и Content-Length
	std::string header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");

	header += file_buffer.size();
	header += "\n\n";

	bytes_written += send(client_fd, &header.c_str()[0], header.size(), 0);

//			std::string file_data(buffer.begin(), buffer.end());
	for (int sum = 0, res = 0; sum < file_buffer.size();)
	{
//				res = send(_client_fd, &file_data.c_str()[sum], file_data.length() - sum, 0);
		res = send(client_fd, &file_buffer[sum], file_buffer.size() - sum, 0);
		if (res == -1)
			continue;
		bytes_written += res;
		sum += res;
	}
	printf("HAVE WRITTEN: %d\n", bytes_written);
	return (0);
}