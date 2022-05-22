#include "Common_Header.hpp"

/**
 *
 * */
void	Request::SaveChunkedBody(std::string &req_data)
{
	while (_parse_state != END_STATE)
	{
		if (!_isChunkedData)
			TryToFindChunk(req_data);
		else if (_isChunkedData && _parse_state != END_STATE)
			ParseChunkedBody(req_data);
	}
}

/**
 *
 * */
void	Request::TryToFindChunk(std::string &req_data)
{
	std::stringstream	str_stream;
	std::size_t			EOL;

	if (!req_data.length())
	{
		_parse_state = END_STATE;
		return;
	}
	EOL = req_data.find(LF);
	if (EOL == std::string::npos)
		return;
	str_stream << std::hex << req_data.substr(0, EOL);
	str_stream >> _chunk_size;
	if (_chunk_size == 0)
		_parse_state = END_STATE;
	_isChunkedData = true;
	req_data.erase(0, EOL + 1);
}

/**
 *
 * */
void	Request::ParseChunkedBody(std::string &data)
{
	std::size_t	i;

	i = 0;
	while (i < data.length() && _chunk_size)
	{
		if (data[i] == '\n' && (i - 1 >= 0 && data[i - 1] == '\r'))
			_body.push_back('\n');
		else if (data[i] != '\r')
			_body.push_back(data[i]);
		i++;
		_chunk_size--;
	}
	if (_chunk_size == 0)
	{
		_isChunkedData = false;
		i += 2;
	}
	data.erase(0, i);
}

/**
 *
 * */
void	Request::SaveCommonBody(std::string &data)
{
	if (_body_size > _max_body_size)
		throw RequestException(413, "Request Entity Too Large");
	if (_body.length() + data.length() > _max_body_size)
		throw RequestException(413, "Request Entity Too Large");

	_body.append(data);
	data.clear();
	if (_body.length() == _body_size)
		_parse_state = END_STATE;
}