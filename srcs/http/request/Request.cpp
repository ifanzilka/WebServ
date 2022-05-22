#include "Request.hpp"

Request::Request(std::multimap<std::string, LocationData> &locations, ServerKqueue &server_api)
	:	_allLocations(locations), _location(nullptr),
		_server_api(server_api),
		_parse_state(START_LINE),
		_buffer(new char[RECV_BUFFER_SIZE + 1])
{
	_client_fd = 0;
	_body_size = 0;
	_chunk_size = 0;
	_isChunkedData = false;
	_status_code = 0;
}

Request::~Request() { delete [] _buffer; }

void Request::ReadRequestData(const size_t &data_size)
{
	std::string	req_data;

	_buffer[data_size] = '\0';
	req_data.append(_buffer, data_size);

	if (_parse_state == END_STATE)
	{
		ClearRequestData();
		_server_api.enableWriteEvent(_client_fd, &_server_api);
	}
	if (_parse_state == START_LINE || _parse_state == HEADER_LINE)
		ReadFirstBlock(req_data); /** чтение первой строки и заголовков */
	if (_parse_state == BODY_LINE)
	{
		if (_transfer_encoding == "chunked")
			SaveChunkedBody(req_data);
		else
			SaveCommonBody(req_data);
	}
}

/**
 * ====================================
 * ==             Setters            ==
 * ====================================
 */

void	Request::SetClientFd(const std::uint8_t &fd) { _client_fd = fd; }
void	Request::SetStatusCode(const std::uint32_t &status_code) { _status_code = status_code; }

/**
 * ====================================
 * ==             Getters            ==
 * ====================================
 */

std::string									Request::GetURIParameters(void) { return (_uri_parameters); }
const std::map<std::string, std::string>	&Request::GetHeaders(void) const { return (_headers); }
const std::uint32_t							&Request::GetStatusCode(void) const { return (_status_code); }
const std::string							&Request::GetMethod(void) const { return (_method); }
const std::string							&Request::GetBody(void) { return (_body); }
const LocationData							*Request::GetLocation(void) const { return (_location); }
char										*Request::GetBuffer(void) const { return (_buffer); }

void	Request::GetUrlEncodedBody(std::map<std::string, std::string> &queryBody)
{
	std::string							tmp;
	std::size_t							size;
	std::size_t							pos;
	std::pair<std::string, std::string>	pair;

	tmp = _body;
	size = std::count(tmp.begin(), tmp.end(), '&');
	for (std::size_t i = 0; i < size; i++)
	{
		pos = tmp.find("&");
		pair.first = tmp.substr(0, tmp.find("="));
		ParsePercentData(pair.first);
		pair.second = tmp.substr(tmp.find("=") + 1, pos - pair.first.length() - 1);
		ParsePercentData(pair.second);
		queryBody.insert(pair);
		tmp.erase(0, pos + 1);
	}
}

/** получение полного пути location + _uri */
const std::string	Request::GetUrl(std::uint32_t &status)
{
	std::string		full_path;
	std::uint8_t	mode;
	std::string		tmp;

	/** REDIRECT */
	if (_location && (!_location->GetRedirect().empty()))
	{
		status = 301; //TODO: добавить статус код для редиректа
		return (_location->GetRedirect());
	}
	status = 200;
	full_path = _location->GetRoot() + _uri;
	// удаление лишних слешей
	for (std::size_t i = 0; i < full_path.length() - 1; i++)
	{
		if (full_path[i] == '/' && full_path[i + 1] == '/')
			full_path.erase(i + 1, 1);
	}
	// если последний символ в пути - слеш - удалить
	if (full_path[full_path.length() - 1] == '/')
		full_path.pop_back();

	mode = GetTypeOfData(full_path);
	if (mode == NOT_FOUND && _method != "POST" && _method != "PUT")
	{
		status = 404;
		return ("unknown url");
	}
	return (ValidateUrl(full_path, status, mode));
}

/**
 * ====================================
 * ==   Вспомогательные методы       ==
 * ====================================
 */

std::string	Request::ValidateUrl(std::string &full_path, std::uint32_t &status_code, std::uint8_t mode)
{
	std::string	tmp;

	if (mode == DIR_MODE) /** DIRECTORY */
	{
		if (_method == "PUT")
		{
			status_code = 201;
			return (full_path);
		}
		tmp = full_path + "/" + _location->GetIndex(); // получение полного пути с страницей (index.html)
		if (GetTypeOfData(tmp) == FILE_MODE)
		{
			if (!access(tmp.c_str(), R_OK))
				return (tmp); // возврат пути к файлу в дирректории
		}
		if (_location->IsAutoindex()) // если есть автоиндекс
		{
			status_code = 1;
			return (full_path); // вернуть полный путь (location + _uri)
		}
		if (errno == EACCES)
			status_code = 403;
		else
			status_code = 404;
		return (tmp);
	}
	else if (mode == FILE_MODE) /** FILE */
	{
		if (!access(full_path.c_str(), R_OK))
			return (full_path); // если файл доступен - вернуть весь путь
		if (errno == EACCES)
			status_code = 403;
		else
			status_code = 404;
		status_code = 403;
		return (full_path);
	}
	if (_method == "POST")
		status_code = 201;
	else
		status_code = 404;
	return (full_path);
}

void	Request::ClearRequestData(void)
{
	_headers.clear();
	_max_body_size = 0;
	_body_size = 0;
	_chunk_size = 0;
	_parse_state = START_LINE;
	_transfer_encoding.clear();
	_method.clear();
	_protocol.clear();
	_uri.clear();
	_uri_parameters.clear();
	_body.clear();
	_isChunkedData = false;
	_status_code = 0;
	_location = nullptr;
}

void Request::PrintAllRequestData(void)
{
	std::cout << "REQUEST DATA PARSING RESULT" << std::endl;

	std::cout << "ParseState: " << (_parse_state == END_STATE ? "END_STATE"
		: (_parse_state == BODY_LINE) ? "BODY_LINE"
		: (_parse_state == HEADER_LINE) ? "HEADER_LINE"
		: (_parse_state == START_LINE) ? "START_LINE" : "ERROR") << std::endl;

	std::cout << "StatusCode: " << _status_code << std::endl;

	std::cout << "StarLine: " <<
		_method << " " << _uri << " " << _protocol << std::endl;

	std::cout << "=========================HEADERS=========================\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		std::cout << it->first << ":" << it->second << std::endl;
	}
	std::cout << "=========================================================\n\n";

	std::cout << "=========================BODY=== BODYSIZE: " << _body_size << " ========\n";
	std::cout << _body << std::endl;
	std::cout << "=========================BODY=========================\n\n";

	std::cout << "Additional information: " << std::endl;
	std::cout << "TransferEncoding: " << (_transfer_encoding.empty() ? "empty" : _transfer_encoding) << std::endl;
	std::cout << "_maxBodySize: " << _max_body_size << std::endl;
	std::cout << "_chunk_size: " << _chunk_size << std::endl;
	std::cout << "_isChunkedData: " << _isChunkedData << std::endl;
	std::cout << "_uri_parameters: " << (_uri_parameters.empty() ? "empty" : _uri_parameters) << std::endl;
}