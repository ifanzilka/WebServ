//
// Created by Josephine Beregond on 4/30/22.
//

#include "Request.hpp"

Request::Request(std::multimap<std::string, LocationData> &locations)
		:	_allLocations(locations),
			_location(nullptr),
			_buffer(new char[RECV_BUFFER_SIZE + 1])
{
}

Request::~Request()
{
	delete [] _buffer;
}

char	*Request::GetBuffer(void) const
{
	return (_buffer);
}

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

size_t	Request::GetHeaderEndPos(std::string &client_request)
{
	size_t	pos;

	pos = client_request.find("\n\r\n");
	pos = (pos == std::string::npos) ? client_request.size() : pos;
	return (pos);
}

void Request::FillHeadersMap(std::string &request, HttpData &client_data)
{
	size_t header_size = GetHeaderEndPos(request);
	size_t end_of_line;

	int colon_id = 0;
	std::string new_line;
	for (int i = 0; i < header_size; i++)
	{
		end_of_line = request.find("\n");
		new_line = request.substr(0, end_of_line);
		colon_id = new_line.find(':');
		if (colon_id == std::string::npos)
			throw RequestException(400, "BadRequest");
		client_data._headers.insert(std::make_pair(new_line.substr(0, colon_id),
			new_line.substr(colon_id + 2, new_line.length() - 1)));
		request.erase(0, end_of_line + 1);
		i += end_of_line;
	}

}

void Request::CheckHeaders(HttpData &client_data)
{
	std::unordered_map<std::string, std::string>::iterator var_it;

	if (client_data._headers.find("Host") == client_data._headers.end())
		throw RequestException(400, "BadRequest");
	var_it = client_data._headers.find("Content-Length");
	if (var_it != client_data._headers.end())
	{
		client_data._hasBody = true;
		client_data._body_length = static_cast<std::uint32_t>(std::atol(var_it->second.c_str()));
		std::cout << "CheckHeaders. BodyLength: " << client_data._body_length << std::endl;
	}
	var_it = client_data._headers.find("Transfer-Encoding");
	if (var_it != client_data._headers.end())
		client_data._transfer_encoding = var_it->second;
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

void Request::FillBodyData(HttpData &client_data, std::string &request_text)
{
	if (request_text.length() > client_data._body_length)
		throw RequestException(413, "Request Entity Too Large");
	client_data._body.append(request_text);
	request_text.clear();
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

	FillHeadersMap(request_text, client_data);
	CheckHeaders(client_data);

	if (client_data._hasBody)
	{
		/** удаление остатка разделительной строки между заголовками и телом запроса */
		request_text.erase(0, request_text.find('\n') + 1);
		FillBodyData(client_data, request_text);
	}
}