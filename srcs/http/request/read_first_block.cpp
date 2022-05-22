#include "Common_Header.hpp"

/**
 * 1)
 */
void Request::ReadFirstBlock(std::string &data)
{
	std::size_t	EOL;

	EOL = data.find(LF);
	while (EOL != std::string::npos &&
		(_parse_state != BODY_LINE && _parse_state != END_STATE))
	{
		if (_parse_state == START_LINE)
		{
			ReadStartLine(data.substr(0, EOL));
		}
		else if (_parse_state == HEADER_LINE)
		{
			EOL = data.find(LF);
			SaveHeaderLine(data.substr(0, EOL));
		}
		data.erase(0, EOL + 1);
		EOL = data.find(LF);
	}
}

/**
 * 2)
 */
void	Request::ReadStartLine(std::string fst_line)
{
	std::size_t	space_ind;

	if (!fst_line.length())
		throw RequestException(400, "Bad Request");
	SaveMethod(fst_line, space_ind);

	SaveURI(fst_line, space_ind);

	SaveProtocol(fst_line, space_ind);

	_location = GetValidLocation(); /** получение location под _uri */
	if (!_location)
		throw RequestException(404, "Not Found");
	//TODO: сделать проверку доступных протоколов (405, "Method Not Allowed")
	_max_body_size = _location->GetClientBufferBodySize();

	ParseURIData();

	_parse_state = HEADER_LINE;
}

/**
 * 3)
 * */
void	Request::SaveMethod(std::string &fst_line, std::size_t &space_ind)
{
	space_ind = fst_line.find(' '); // поиск первого пробела из строки 'GET / HTTP/1.1'
	if (space_ind == std::string::npos)
		throw RequestException(400, "Bad Request");
	_method = fst_line.substr(0, space_ind); // получение HTTP метода (GET)
	/** удаление пустых символов */
	if (space_ind >= fst_line.length())
		space_ind = fst_line.length();
	while (space_ind < fst_line.length() && std::isspace(fst_line[space_ind]))
		space_ind++;
	fst_line.erase(0, space_ind);
}

/**
 * 4)
 * */
void	Request::SaveURI(std::string &fst_line, std::size_t &space_ind)
{
	space_ind = fst_line.find(' '); // поиск следующего пробела
	if (space_ind == std::string::npos)
		throw RequestException(400, "Bad Request");
	_uri = fst_line.substr(0, space_ind); // получение URI '/ HTTP/1.1'
	/** удаление пустых символов */
	if (space_ind >= fst_line.length())
		space_ind = fst_line.length();
	while (space_ind < fst_line.length() && std::isspace(fst_line[space_ind]))
		space_ind++;
	fst_line.erase(0, space_ind);
}

/**
 * 5)
 * */
void	Request::SaveProtocol(std::string &fst_line, std::size_t &space_ind)
{
	_protocol = fst_line;
	// удаление пустых символов
	_protocol.erase(std::remove_if(_protocol.begin(),
		_protocol.end(), &IsChar), _protocol.end());
	//TODO: протокол может быть 1.0
	if (_protocol != HTTP_PROTOCOL)
		throw RequestException(505, "Http Version Not Supported");
}

/**
 * 6)
 * */
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
		std::multimap<std::string, LocationData>::const_iterator it = _allLocations.begin();
		for (; it != _allLocations.end(); it++)
		{
			if (!tmp.length())
				tmp = "/";
			(it->first != "/" && it->first[it->first.length() - 1] == '/') ?
				tmp1 = it->first.substr(0, it->first.find_last_of("/")) : tmp1 = it->first;
			if (tmp == tmp1)
			{
				if (isLastSlash)
					_uri.pop_back();
				return (&it->second);
			}
		}
		lastSlashPos = tmp.find_last_of("/", lastSlashPos);
		tmp = tmp.substr(0, lastSlashPos);
	}
	return (nullptr);
}

/**
 * 7)
 * */
void	Request::ParseURIData(void)
{
	std::size_t	position;

	position = _uri.find("?");
	if (position != std::string::npos)
	{
		_uri_parameters = _uri.substr(position + 1);
		_uri.erase(position);
	}
	ParsePercentData(_uri);
}

/**
 * 8)
 * */
void	Request::ParsePercentData(std::string &uri_ref)
{
	std::stringstream	str_stream;
	std::string			str;
	int					symbol;

	for (std::size_t i = 0; i < uri_ref.length(); i++)
	{
		if (uri_ref[i] == '%')
		{
			try
			{
				str_stream << std::hex << uri_ref.substr(i + 1, 2);
				str_stream >> symbol;
				str = uri_ref.substr(i + 3);
				uri_ref.erase(i);
				uri_ref.push_back(static_cast<char>(symbol));
				uri_ref.append(str);
				str_stream.str(std::string()); // чистка stringstream
			}
			catch(std::exception &e)
			{
				throw RequestException(400, "Bad Request");
			}
		}
		else if (uri_ref[i] == '+')
			uri_ref = uri_ref.substr(0, i) + " " + uri_ref.substr(i + 1);
	}
}

/**
 * 9)
 * */
void	Request::SaveHeaderLine(std::string req_data)
{
	std::size_t	colon_pos;
	std::string	key;
	std::string	value;

	if (!CheckHeaderLineState(req_data))
		return ;

	colon_pos = req_data.find(":");
	if (colon_pos == std::string::npos)
		throw RequestException(400, "Bad Request");
	key = req_data.substr(0, colon_pos);
	value = req_data.substr(colon_pos + 1);
	_headers.insert(std::pair<std::string, std::string>(key, value));
	if (key == "Content-Length") // получение размера тела
		_body_size = static_cast<std::uint32_t>(std::atol(value.c_str()));
	if (key == "Transfer-Encoding")
		_transfer_encoding = value; // получение типа кодирования
}

/**
 * 10)
 * */
bool	Request::CheckHeaderLineState(std::string &req_data)
{
	req_data.erase(std::remove_if(req_data.begin(),
		req_data.end(), &IsChar), req_data.end()); // удаление пробелов
	if (!req_data.length())
	{
		if (_headers.find("Host") == std::end(_headers)) // если нет "HOST"
			throw RequestException(400, "Bad Request");
		// если нет "Transfer-Encoding || Content-Length"
		if (_headers.find("Transfer-Encoding") == std::end(_headers)
			&& _headers.find("Content-Length") == std::end(_headers))
			_parse_state = END_STATE;
		else // иначе при наличии
			_parse_state = BODY_LINE;
		return (false);
	}
	return (true);
}