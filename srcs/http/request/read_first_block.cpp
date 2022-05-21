#include "Common_Header.hpp"

/**
 * 1)
 */
void Request::ReadFirstBlock(std::string &data)
{
	std::size_t	newLinePos;

	newLinePos = data.find(LF);
	while (newLinePos != std::string::npos &&
		(_parseState != BODY_LINE && _parseState != END_STATE))
	{
		if (_parseState == START_LINE)
		{
			ReadStartLine(data.substr(0, newLinePos));
			data.erase(0, newLinePos + 1);
		}
		if (_parseState == HEADER_LINE)
		{
			newLinePos = data.find(LF);
			saveHeaderLine(data.substr(0, newLinePos));
			data.erase(0, newLinePos + 1);
		}
		newLinePos = data.find(LF);
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
	_maxBodySize = _location->GetClientBufferBodySize();

	ParseURIData();

	_parseState = HEADER_LINE;
}

/**
 *
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
 *
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
 *
 * */
void	Request::SaveProtocol(std::string &fst_line, std::size_t &space_ind)
{
	_protocol = fst_line;
	// удаление пустых символов
	_protocol.erase(std::remove_if(_protocol.begin(),
		_protocol.end(), &isCharWhiteSpace), _protocol.end());
	//TODO: протокол может быть 1.0
	if (_protocol != HTTP_PROTOCOL)
		throw RequestException(505, "Http Version Not Supported");
}

/**
 *
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
 *
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