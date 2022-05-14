#include "Request.hpp"

Request::Request(std::multimap<std::string, LocationData> &locations)
		:	_allLocations(locations),
			_location(nullptr),
			_bodySize(0),
			_chunkSize(0),
			_parseState(START_LINE),
			_buffer(new char[RECV_BUFFER_SIZE + 1]),
			_errorStatus(0),
			_isReqDone(false),
			_isChunkSize(false)
{}

Request::~Request()
{
	delete [] _buffer;
}

char	*Request::GetBuffer(void) const
{
	return _buffer;
}

void	Request::setErrorStatus(const int s)
{
	_errorStatus = s;
}

void	Request::parsePercent(std::string &strRef)
{
	std::stringstream	ss;
	std::string			tmp;
	int					c;

	for (std::size_t i = 0; i < strRef.length(); i++)
	{
		if (strRef[i] == '%')
		{
			try
			{
				ss << std::hex << strRef.substr(i + 1, 2);
				ss >> c;
				tmp = strRef.substr(i + 3);
				strRef.erase(i);
				strRef.push_back(static_cast<char>(c));
				strRef.append(tmp);
				ss.clear();
			}
			catch(std::exception &e)
			{
				throw RequestException(400, "Bad Request");
			}
		}
		else if (strRef[i] == '+')
			strRef = strRef.substr(0, i) + " " + strRef.substr(i + 1);
	}
	return;
}

void	Request::parseUri(void)
{
	std::size_t	pos;

	pos = _uri.find("?");
	if (pos != std::string::npos)
	{
		_query = _uri.substr(pos + 1);
		_uri.erase(pos);
	}

	parsePercent(_uri);
	return;
}

const LocationData	*Request::getLoc(void)
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
			if (tmp == tmp1) {
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

void	Request::validateStartLine(void)
{
	_location = getLoc(); /** получение location под _uri */
	if (!_location)
		throw RequestException(404, "Not Found");
//	std::vector<std::string>::const_iterator i = _location->GetMethods().begin();
//	for (; i != _location->methods.end(); i++)
//	{
//		if (i->first == _method) {
//			if (!i->second)
//				throw RequestException(405, "Method Not Allowed");
//			break;
//		}
//	}
//	if (i == _location->methods.end())
//		throw RequestException(400, "Bad Request");
	//TODO: протокол может быть 1.0
	if (_protocol != HTTP_PROTOCOL)
		throw RequestException(505, "Http Version Not Supported");
	_maxBodySize = _location->GetClientBufferBodySize();
	parseUri();
	return;
}

void	Request::saveStartLine(std::string startLine)
{
	std::size_t	lfPos;

	if (!startLine.length())
		throw RequestException(400, "Bad Request");
	lfPos = startLine.find(' '); // поиск первого пробела из строки 'GET / HTTP/1.1'
	if (lfPos == std::string::npos)
		throw RequestException(400, "Bad Request");
	_method = startLine.substr(0, lfPos); // вырезание типа метода (GET)
	startLine.erase(0, skipWhiteSpaces(startLine, lfPos)); // удаление сохраненного

	lfPos = startLine.find(' '); // поиск следующего пробела
	if (lfPos == std::string::npos)
		throw RequestException(400, "Bad Request");
	_uri = startLine.substr(0, lfPos); // вырезание URI из строки '/ HTTP/1.1'
	startLine.erase(0, skipWhiteSpaces(startLine, lfPos)); // удаление сохраненного
	_protocol = startLine;
	// удаление пустых символов
	_protocol.erase(std::remove_if(_protocol.begin(),
								   _protocol.end(), &isCharWhiteSpace), _protocol.end());

	validateStartLine(); /** проверка стартовой линии */
	_parseState = HEADER_LINE;
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

void Request::saveStartLineHeaders(std::string &data)
{
	std::size_t	newLinePos;

	newLinePos = data.find(LF);
	while (newLinePos != std::string::npos and (_parseState != BODY_LINE and _parseState != END_STATE))
	{
		/** */
		if (_parseState == START_LINE)
		{
			saveStartLine(data.substr(0, newLinePos));
			data.erase(0, newLinePos + 1);
		}
		/**  */
		if (_parseState == HEADER_LINE)
		{
			newLinePos = data.find(LF);
			saveHeaderLine(data.substr(0, newLinePos));
			data.erase(0, newLinePos + 1);
		}
		newLinePos = data.find(LF);
	}
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
		_isChunkSize = false;
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
	_isChunkSize = true;
	data.erase(0, pos + 1);
	return;
}

void	Request::saveChunkedBody(std::string &data)
{
	while (_parseState != END_STATE)
	{
		if (!_isChunkSize)
			parseChunkSize(data);
		if (_isChunkSize and _parseState != END_STATE)
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

bool Request::saveRequestData(size_t data_size)
{
	std::string	data;

	data = _tmpBuffer;

	_buffer[data_size] = '\0';
	data.append(_buffer, data_size);

	if (_parseState == START_LINE or _parseState == HEADER_LINE)
		saveStartLineHeaders(data);
	if (_parseState == BODY_LINE)
	{
		if (_transferEncoding == "chunked")
			saveChunkedBody(data);
		else
			saveSimpleBody(data);
	}
	_tmpBuffer = data;
	if (_parseState == END_STATE)
		_isReqDone = true;
	return _isReqDone;
}

void Request::PrintAllRequestData()
{
	std::cout << "REQUEST DATA PARSING RESULT" << std::endl;

	std::cout << "ParseState: " << (_parseState == END_STATE ? "END_STATE"
		: (_parseState == BODY_LINE) ? "BODY_LINE"
		: (_parseState == HEADER_LINE) ? "HEADER_LINE"
		: (_parseState == START_LINE) ? "START_LINE" : "ERROR") << std::endl;
	std::cout << "IsReqDone? : " << (_isReqDone == false ? "false" : "true") << std::endl;

	std::cout << "ErrorStatus: " << _errorStatus << std::endl;

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
	std::cout << "_isChunkSize: " << _isChunkSize << std::endl;
	std::cout << "_query: " << (_query.empty() ? "empty" : _query) << std::endl;

}