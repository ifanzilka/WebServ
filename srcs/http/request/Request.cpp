#include "Request.hpp"

Request::Request(std::multimap<std::string, LocationData> &locations, ServerKqueue &server_api)
	:	_allLocations(locations), _location(nullptr),
		_server_api(server_api),
		_parseState(START_LINE),
		_buffer(new char[RECV_BUFFER_SIZE + 1])
{
	_client_fd = 0;
	_bodySize = 0;
	_chunkSize = 0;
	_hasChunk = false;
	_status_code = 0;
}

Request::~Request() { delete [] _buffer; }
void										Request::SetClientFd(const std::uint8_t &fd) { _client_fd = fd; }
void										Request::SetStatusCode(const std::uint32_t &status_code) { _status_code = status_code; }
std::string									Request::GetURIParameters(void) { return (_uri_parameters); }
const std::map<std::string, std::string>	&Request::GetHeaders(void) const { return (_headers); }
const std::uint32_t							&Request::GetStatusCode(void) const { return (_status_code); }
const std::string							&Request::GetMethod(void) const { return (_method); }
const std::string							&Request::GetBody(void) { return (_body); }
const LocationData							*Request::GetLocation(void) const { return (_location); }
char										*Request::GetBuffer(void) const { return (_buffer); }

void	Request::getUrlEncodedBody(std::map<std::string, std::string> &queryBody)
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

void Request::ReadRequestData(size_t data_size)
{
	std::string	data;

	data = _tmpBuffer;

	_buffer[data_size] = '\0';
	data.append(_buffer, data_size);

	if (_parseState == END_STATE)
		resetRequest();
	if (_parseState == START_LINE || _parseState == HEADER_LINE)
		ReadFirstBlock(data); /** чтение первой строки и заголовков */
	if (_parseState == BODY_LINE)
	{
		if (_transferEncoding == "chunked")
			saveChunkedBody(data);
		else
			saveSimpleBody(data);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE)
	{
		_server_api.enableWriteEvent(_client_fd, &_server_api);
	}
}

std::string	Request::validateUrl(std::string &fullPath, std::uint32_t &status, std::uint8_t mode)
{
	std::string	tmp;

	if (mode == DIR_MODE) /** DIRECTORY */
	{
		if (_method == "PUT")
		{
			status = 201;
			return fullPath;
		}
		tmp = fullPath + "/" + _location->GetIndex(); // получение полного пути с страницей (index.html)
		if (isDirOrFile(tmp) == FILE_MODE)
		{
			if (!access(tmp.c_str(), R_OK))
				return (tmp); // возврат пути к файлу в дирректории
		}
		if (_location->IsAutoindex()) // если есть автоиндекс
		{
			status = 1;
			return fullPath; // вернуть полный путь (location + _uri)
		}
		if (errno == EACCES)
			status = 403;
		else
			status = 404;
		return tmp;
	}
	else if (mode == FILE_MODE) /** FILE */
	{
		if (!access(fullPath.c_str(), R_OK))
			return fullPath; // если файл доступен - вернуть весь путь
		if (errno == EACCES)
			status = 403;
		else
			status = 404;
		status = 403;
		return fullPath;
	}
	if (_method == "POST")
		status = 201;
	else
		status = 404;
	return fullPath;
}

/** получение полного пути location + _uri */
std::string	Request::getUrl(std::uint32_t &status)
{
	std::string		fullPath;
	std::uint8_t	mode;
	std::string		tmp;

	/** REDIRECT */
	if (_location && (!_location->GetRedirect().empty()))
	{
		status = 301; //TODO: добавить статус код для редиректа
		return _location->GetRedirect();
	}
	status = 200;
	fullPath = _location->GetRoot() + _uri;
	// удаление лишних слешей
	for (std::size_t i = 0; i < fullPath.length() - 1; i++)
	{
		if (fullPath[i] == '/' && fullPath[i + 1] == '/')
			fullPath.erase(i + 1, 1);
	}
	// если последний символ в пути - слеш - удалить
	if (fullPath[fullPath.length() - 1] == '/')
		fullPath.pop_back();

	mode = isDirOrFile(fullPath);
	if (mode == NOT_FOUND && _method != "POST" && _method != "PUT")
	{
		status = 404;
		return "unknown url";
	}
	return (validateUrl(fullPath, status, mode));
}

const LocationData	*Request::GetValidLocation(void)
{
	std::string	tmp;
	std::string	tmp1;
	std::size_t	lastSlashPos;
	std::size_t	len;
	bool		isLastSlash;

	isLastSlash = false;
	if (_uri[_uri.length() - 1] != '/')
	{
		isLastSlash = true;
		_uri.push_back('/');
	}
	lastSlashPos = _uri.find_last_of("/");
	if (lastSlashPos == std::string::npos)
		throw RequestException(400, "Bad Request");
	tmp = _uri.substr(0, lastSlashPos);
	len = std::count(_uri.begin(), _uri.end(), '/');
	for (std::size_t i = 0; i < len; i++)
	{
		std::multimap<std::string, LocationData>::const_iterator j = _allLocations.begin();
		for (; j != _allLocations.end(); j++)
		{
			if (!tmp.length())
				tmp = "/";
			(j->first != "/" and j->first[j->first.length() - 1] == '/') ?
				tmp1 = j->first.substr(0, j->first.find_last_of("/")) : tmp1 = j->first;
			if (tmp == tmp1)
			{
				if (isLastSlash)
					_uri.pop_back();
				return &j->second;
			}
		}
		lastSlashPos = tmp.find_last_of("/", lastSlashPos);
		tmp = tmp.substr(0, lastSlashPos);
	}
	return nullptr;
}

void	Request::saveHeaderLine(std::string headerLine)
{
	std::size_t	colonPos;
	std::string	headerName;
	std::string	headerValue;

	headerLine.erase(std::remove_if(headerLine.begin(),
									headerLine.end(), &isCharWhiteSpace), headerLine.end()); // удаление пробелов
	if (!headerLine.length())
	{
		if (_headers.find("Host") == std::end(_headers)) // если нет "HOST"
			throw RequestException(400, "Bad Request");
		// если нет "Transfer-Encoding || Content-Length"
		if (_headers.find("Transfer-Encoding") == std::end(_headers)
			and _headers.find("Content-Length") == std::end(_headers))
			_parseState = END_STATE;
		else // иначе при наличии
			_parseState = BODY_LINE;
		return;
	}

	colonPos = headerLine.find(":");
	if (colonPos == std::string::npos)
		throw RequestException(400, "Bad Request");
	headerName = headerLine.substr(0, colonPos);
	headerValue = headerLine.substr(colonPos + 1);
	_headers.insert(std::pair<std::string,
			std::string>(headerName, headerValue));
	if (headerName == "Content-Length") // получение размера тела
		_bodySize = static_cast<std::uint32_t>(std::atol(headerValue.c_str()));
	if (headerName == "Transfer-Encoding")
		_transferEncoding = headerValue; // получение типа кодирования
}

void	Request::resetRequest(void)
{
	_headers.clear();
	_maxBodySize = 0;
	_bodySize = 0;
	_chunkSize = 0;
	_parseState = START_LINE;
	_transferEncoding.clear();
	_method.clear();
	_protocol.clear();
	_uri.clear();
	_uri_parameters.clear();
	_body.clear();
	_tmpBuffer.clear();
	_hasChunk = false;
	_status_code = 0;
	_location = nullptr;
}


void	Request::parseChunkedBody(std::string &data)
{
	std::size_t	i;

	i = 0;
	while (i < data.length() and _chunkSize)
	{
		if (data[i] == '\n' and (i - 1 >= 0 and data[i - 1] == '\r'))
			_body.push_back('\n');
		else if (data[i] != '\r')
			_body.push_back(data[i]);
		i++;
		_chunkSize--;
	}
	if (!_chunkSize)
	{
		_hasChunk = false;
		i += 2;
	}
	data.erase(0, i);
}

void	Request::parseChunkSize(std::string &data)
{
	std::stringstream	ss;
	std::size_t			pos;

	if (!data.length())
	{
		_parseState = END_STATE;
		return;
	}
	pos = data.find(LF);
	if (pos == std::string::npos)
		return;
	ss << std::hex << data.substr(0, pos);
	ss >> _chunkSize;
	if (!_chunkSize)
		_parseState = END_STATE;
	_hasChunk = true;
	data.erase(0, pos + 1);
}

void	Request::saveChunkedBody(std::string &data)
{
	while (_parseState != END_STATE)
	{
		if (!_hasChunk)
			parseChunkSize(data);
		if (_hasChunk && _parseState != END_STATE)
			parseChunkedBody(data);
	}
}

void	Request::saveSimpleBody(std::string &data)
{
	std::size_t	bodySize;

	bodySize = static_cast<std::size_t>(std::atol(_headers["Content-Length"].c_str()));
	if (bodySize > _maxBodySize)
		throw RequestException(413, "Request Entity Too Large");
	if (_body.length() + data.length() > _maxBodySize)
		throw RequestException(413, "Request Entity Too Large");

	_body.append(data);
	data.clear();
	if (_body.length() == bodySize)
		_parseState = END_STATE;
	return;
}

void Request::PrintAllRequestData()
{
	std::cout << "REQUEST DATA PARSING RESULT" << std::endl;

	std::cout << "ParseState: " << (_parseState == END_STATE ? "END_STATE"
		: (_parseState == BODY_LINE) ? "BODY_LINE"
		: (_parseState == HEADER_LINE) ? "HEADER_LINE"
		: (_parseState == START_LINE) ? "START_LINE" : "ERROR") << std::endl;

	std::cout << "StatusCode: " << _status_code << std::endl;

	std::cout << "StarLine: " <<
		_method << " " << _uri << " " << _protocol << std::endl;

	std::cout << "=========================HEADERS=========================\n";
	for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
	{
		std::cout << it->first << ":" << it->second << std::endl;
	}
	std::cout << "=========================================================\n\n";

	std::cout << "=========================BODY=== BODYSIZE: " << _bodySize << " ========\n";
	std::cout << _body << std::endl;
	std::cout << "=========================BODY=========================\n\n";

	std::cout << "Additional information: " << std::endl;
	std::cout << "TransferEncoding: " << (_transferEncoding.empty() ? "empty" : _transferEncoding) << std::endl;
	std::cout << "_maxBodySize: " << _maxBodySize << std::endl;
	std::cout << "_chunkSize: " << _chunkSize << std::endl;
	std::cout << "_hasChunk: " << _hasChunk << std::endl;
	std::cout << "_uri_parameters: " << (_uri_parameters.empty() ? "empty" : _uri_parameters) << std::endl;
}