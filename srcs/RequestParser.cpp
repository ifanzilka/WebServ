#include "../includes/RequestParser.hpp"

namespace ft
{
	std::string RequestParser::GetHttpMethod(std::string &request)
	{
		std::string http_method;
		size_t pos;

		pos = request.find(" ");
		http_method = request.substr(0, pos);
		request.erase(0, ++pos);
		if (http_method.compare("GET") != 0 && http_method.compare("POST") != 0
			&& http_method.compare("DELETE") != 0)
			http_method.resize(0);
		return (http_method);
	}

	std::string RequestParser::GetFilePath(std::string &request)
	{
		std::string file_path;
		size_t pos;

		pos = request.find(" ");
		file_path = request.substr(0, pos);
		request.erase(0, ++pos);
		return (file_path);
	}

	std::vector<std::string> RequestParser::GetHeaders(std::string &request)
	{
		std::vector<std::string> headers;
		size_t header_size = GetHeaderEndPos(request);
		size_t	end_of_line;

		for (int i = 0; i < header_size; i++)
		{
			end_of_line = request.find("\n");
			headers.push_back(request.substr(0, end_of_line - 1));
			request.erase(0, end_of_line + 1);
			i += end_of_line;
		}
		return (headers);
	}

	size_t	RequestParser::GetHeaderEndPos(std::string &client_request)
	{
		size_t	pos;

		pos = client_request.find("\n\r\n");
		pos = (pos == std::string::npos) ? client_request.size() : pos;
		return (pos);
	}
}