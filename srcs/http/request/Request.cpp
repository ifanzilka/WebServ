//
// Created by Josephine Beregond on 4/30/22.
//

#include "Request.hpp"

std::string Request::GetHttpMethod(std::string &request)
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

std::string Request::GetFilePath(std::string &request)
{
	std::string file_path;
	size_t pos;

	pos = request.find(" ");
	file_path = request.substr(0, pos);
	request.erase(0, ++pos);
	return (file_path);
}

std::string Request::GetProtocol(std::string &request)
{
	std::string protocol;

	protocol = request.substr(0, request.find("\n") - 1);
	request.erase(0, request.find("\n") + 1);
	return (protocol);
}

std::vector<std::string> Request::GetHeaders(std::string &request)
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

size_t	Request::GetHeaderEndPos(std::string &client_request)
{
	size_t	pos;

	pos = client_request.find("\n\r\n");
	pos = (pos == std::string::npos) ? client_request.size() : pos;
	return (pos);
}

void Request::CheckFirstLineSyntax(std::string &first_line)
{
	int spaces = 0;
	int	ind = 0;

	/** Проверка на валидность первого слеша */
	ind = first_line.find_first_of(" /");
	if (ind != 0 && first_line[ind - 1] == ' ')
		throw RequestException(400, "BadRequest");
	/** Проверка на кол-во пробелов в строке */
	while (true)
	{
		ind = first_line.find_last_of(" ");
		if (ind != std::string::npos)
		{
			spaces++;
			first_line.resize(ind);
		}
		else
			break;
	}
	if (spaces != 2)
		throw RequestException(400, "BadRequest");
}

void Request::CheckProtocol(const std::string &protocol_info)
{
	int slash = protocol_info.find('/');

	/** Проверка на наличие слеша */
	if (slash == std::string::npos)
		throw RequestException(400, "BadRequest");
	/** Проверка типа протокола */
	std::string protocol_type = protocol_info.substr(0, slash);
	if (protocol_type != "HTTP")
		throw RequestException(400, "BadRequest");
	/** Проверка версии протокола */
	std::string protocol_version
		= &protocol_info[slash + 1];
	if (protocol_version != "1.1" && protocol_version != "1.0")
		throw RequestException(505, "Http Version Not Supported");
}

void Request::FillDataByRequest(HttpData &client_data, std::string request_text)
{
	std::string first_line = request_text.substr(0, request_text.find('\n'));
	CheckFirstLineSyntax(first_line);

	client_data._http_method = GetHttpMethod(request_text);
	if (client_data._http_method.empty())
		throw RequestException(405, "Method Not Allowed");
	client_data._file_path = GetFilePath(request_text);
	client_data._protocol = GetProtocol(request_text);

	CheckProtocol(client_data._protocol);
	client_data._headers = GetHeaders(request_text);
}