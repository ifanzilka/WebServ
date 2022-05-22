//
// Created by Josephine Beregond on 5/2/22.
//

#include "Response.hpp"

Response::Response(Request &request)
	:	_request(request),
		_body(nullptr),
		_reqLocation(nullptr),
		_cgi_ptr(nullptr)
{
	t_fileInfo file;
	setErrorPages(); // TODO: заменить

	_content_type = "text/html";
	if ((_status_code = request.GetStatusCode()) == 0)
		CollectStartData();

	if (_status_code < 399 && _status_code != 1 && _method != "PUT" && _method != "DELETE")
	{
		FillFileInfo(_url, &file,  _FILE);
		if (file.fType == DDIR)
			file.fStatus = 404;
		if ((file.fStatus < 200 || file.fStatus > 299) && _status_code != 301)
		{
			_status_code = file.fStatus;
			_url = getErrorPage();
			_content_type = "text/html";
		}
		else if (_status_code != 301 && _method != "PUT" && _method != "DELETE")
		{
			/** заполняет необходимые данные и запускает CGI */
			CollectDataForResponse(file);
		}
	}
	else
		_url = getErrorPage();
	_inProc = false;
}

Response::~Response()
{
	if (_cgi_ptr != nullptr)
		delete _cgi_ptr;
}

char *Response::makeBody(int &readSize)
{
	if (_inProc || _cgi_ptr)
	{
		if (_url != "ERROR" && !_autoindex)
		{
			_body = new char[SEND_BUFFER_SIZE];
			memset(_body, 0, SEND_BUFFER_SIZE);
			if (_cgi_ptr && _cgi_ptr->isReadable())
			{
				readSize = read(_cgi_fds[0], _body, SEND_BUFFER_SIZE);
				if (readSize == 0 || readSize == -1)
					_cgi_ptr->toRead(false);
			}
			else
			{
				_FILE.read(_body, SEND_BUFFER_SIZE);
				readSize = _FILE.gcount();
			}
			if (_FILE.eof())
				_FILE.close();
		}
		else if (_autoindex)
		{
			_body = MakePage(_status_code, _body_size, _url.c_str(), _reqLocation);
			readSize = _body_size;
		}
		else
		{
			_body = MakePage(_status_code, _body_size, nullptr, _reqLocation);
			readSize = _body_size;
		}
	}
	return (_body);
}


const std::string &Response::MakeHeaders()
{
	const std::time_t current_time = std::time(0);
	_headers += "Server: HTTP/1.1" + std::string(CRLF);
	_headers += "Date: " + std::string(ctime(&current_time));
	if (_status_code == 301)
		_headers += "Location: " + _url + std::string(CRLF);
	if (_method == "PUT" || _method == "DELETE")
		_headers += "Content-Location: " + _url + std::string(CRLF);
	if (_status_code < 300 || _status_code > 399)
	{
		_headers += "Content-Type: " + _content_type + std::string(CRLF);
		_headers += "Accept-Ranges: bytes" + std::string(CRLF);
	}
	_headers += "Set-Cookie: lastsess=" + std::string(ctime(&current_time));
	_headers += "Content-Length: " + std::to_string(_body_size) + std::string(CRLF);
	if (_reqHeaders["Connection"].size())
		_headers += "Connection: " + _reqHeaders["Connection"] + std::string(CRLF);
	else
		_headers += "Connection: close" + std::string(CRLF);
	_headers += std::string(CRLF);
	return(_headers);
}

const std::string &Response::MakeStatusLine()
{
	_status_line = "HTTP/1.1 " + std::to_string(_status_code) + " " + GetErrorTypes()[_status_code] + CRLF;
	return (_status_line);
}

void Response::SendResponse(int client_fd)
{
	int	res = 0;

//	std::cout << PURPLE"====================== SendResponse() ======================"NORM << std::endl;
//	std::cout << BLUE"Is file open?: "NORM << (_FILE.is_open() ? "yes" : "no") << std::endl;
//	std::cout << BLUE"StatusCode: "NORM << _status_code << std::endl;
//	std::cout << BLUE"Autoindex: "NORM << (_autoindex ? "on" : "off") << std::endl;
//	std::cout << PURPLE"============================================================"NORM << std::endl;

	if (_inProc == false && (!_cgi_ptr || _cgi_ptr->isReadable()))
	{
		_response.append(MakeStatusLine());
		_response.append(MakeHeaders());
		_leftBytes = _body_size;
		res = send(client_fd, _response.c_str(), _response.length(), 0);
		if (res == -1)
			throw RequestException("Sending data error");
//		std::cout << "\n\n" << _response << std::endl << std::endl; //TODO: удалить
		_response = "";
		_inProc = true;
	}
	else if (_FILE.is_open() || _status_code != 200 || _autoindex || _cgi_ptr->isReadable())
	{
		int to_send, pos, tries;
		res = 0, pos = 0, tries = 0;
		makeBody(to_send);
		try
		{
			while (pos != to_send)
			{
				if (_cgi_ptr && !_cgi_ptr->isReadable())
					res = write(_cgi_fds[1], &(_body[pos]), to_send);
				else
					res = send(client_fd, &(_body[pos]) , to_send, 0);
				pos += res;
				if (tries++ == 8 || res == -1)
				{
					_leftBytes = 0;
					throw RequestException("TOO MANY ATTEMPTS TO SEND DATA"); //TODO увеличить кол-во попыток
				}
			}
			_leftBytes -= res;
		}
		catch(const std::exception& e)
		{
			if (_cgi_ptr)
			{
				delete _cgi_ptr;
				_cgi_ptr = nullptr;
				waitpid(_pid, 0, WNOHANG);
			}
			_status_code = 502;
			_leftBytes = 1;
			_inProc = false;
			_url = "ERROR";
			std::cerr << e.what() << '\n';
		}
		delete [] _body;
		_body = nullptr;
	}
	if (_leftBytes < 1 && !_cgi_ptr)
	{
		_inProc = false;
		_leftBytes = false;
		if (_cgi_ptr)
			delete _cgi_ptr;
		_cgi_ptr = nullptr;
	}
	else if (_leftBytes < 1 && _cgi_ptr)
	{
		if (!_cgi_ptr->isReadable())
		{
			close(_cgi_fds[1]);
			waitpid(_pid, 0, 0);
			_body_size = GetFdLen(_cgi_fds[0]);
			_leftBytes = _body_size;
			if (_leftBytes == -1)
			{
				_status_code = 500;
				delete _cgi_ptr;
				_cgi_ptr = nullptr;
				_inProc = false;
				close(_cgi_fds[0]);
			}
			else
				_cgi_ptr->toRead(true);
		}
		else
		{
			close(_cgi_fds[0]);
			delete _cgi_ptr;
			_cgi_ptr = nullptr;
			_inProc = false;
		}
	}
}

void Response::setErrorPages()
{
	std::map<int, std::string>	pages;

	pages.insert(std::make_pair(404, "/www/html/404.html"));
	pages.insert(std::make_pair(405, "/www/html/405.html"));

	_errorPages = pages;
}

std::string Response::getErrorPage()
{
	char *def_page;

	for (std::map<int, std::string>::iterator i = _errorPages.begin(); i != _errorPages.end(); i++)
	{
		if (i->first == static_cast<int>(_status_code))
		{
			t_fileInfo file;
			FillFileInfo(i->second, &file, _FILE);
			if (file.fStatus == 200)
			{
				_body_size = file.fLength;
				_content_type = file.fExtension;
				return i->second;
			}
		}
	}
	if (_autoindex)
		def_page = (MakePage(_status_code, _body_size, _url.c_str(), _reqLocation));
	else
	{
		def_page = (MakePage(_status_code, _body_size, nullptr, _reqLocation));
		_url = "ERROR";
	}

	std::cout << BLUE"getErrorPage() DEF_PAGE:"NORM << std::endl; // TODO: удалить
	std::cout << def_page << std::endl;

	delete def_page; // TODO: зачем удалять, если потом снова вызываем
	return (_url);
}