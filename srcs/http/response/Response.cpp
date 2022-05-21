//
// Created by Josephine Beregond on 5/2/22.
//

#include "Response.hpp"

Response::Response(Request &request) :
	_body(nullptr),
	_reqLocation(nullptr),
	_cgiPtr(nullptr)
{
	t_fileInfo file;
	setErrorPages(); // TODO: заменить

	_contentType = "text/html";
	if ((_statusCode = request.getErrorStatus()) == 0)
	{
		std::cout << GREEN"===ZERO CODE==="NORM << std::endl; // TODO: удалить
		_bodySize = 0;
		_reqHeaders = request.getHeaders();
		_method = request.getMethod();
		_url = putDelete(request, _statusCode);
		_reqLocation = request.getLocation();
		_autoindex = _statusCode == 1;
	}

	std::cout << "Response() after if() Status_code: " << _statusCode << std::endl; // TODO: удалить

	if (_statusCode < 399 && _statusCode != 1 && _method != "PUT" && _method != "DELETE")
	{
		urlInfo(_url, &file,  _FILE);
		if (file.fType == DDIR)
			file.fStatus = 404;
		if ((file.fStatus < 200 || file.fStatus > 299) && _statusCode != 301)
		{
			_statusCode = file.fStatus;
			_url = getErrorPage();
			_contentType = "text/html";
		}
		else if (_statusCode != 301 && _method != "PUT" && _method != "DELETE")
		{
			int cgNum;
			if ((cgNum = checkCgi(request.getLocation()->getCgi(), _url)) > 0)
			{
				_cgiPtr = new CGI(request, request.getLocation()->getCgi(), _FILE);
				_contentType = file.fExtension;
				try
				{
					_cgiFd =  _cgiPtr->initCGI(cgNum, _pid);
				}
				catch(RequestException &e)
				{
					std::cerr << e.what() << " due to " << strerror(errno) << std::endl;
					_statusCode = 502;
				}
				_bodySize = file.fLength;
				_contentType = file.fExtension;
				_leftBytes = _bodySize;
			}
			else if (cgNum == -1)
			{
				_statusCode = 502;
				_url = getErrorPage();
			}
			else
			{
				_bodySize = file.fLength;
				_contentType = file.fExtension;
			}
		}
	}
	else
		_url = getErrorPage();
	_inProc = false;
}

Response::~Response() {}

char *Response::makeBody(int &readSize)
{
	if (_inProc || _cgiPtr)
	{
		if (_url != "ERROR" && !_autoindex)
		{
			_body = new char[SEND_BUFFER_SIZZ];
			memset(_body, 0, SEND_BUFFER_SIZZ);
			if (_cgiPtr && _cgiPtr->isReadable())
			{
				readSize = read(_cgiFd[0], _body, SEND_BUFFER_SIZZ);
				if (readSize == 0 || readSize == -1)
					_cgiPtr->toRead(false);
			}
			else
			{
				_FILE.read(_body, SEND_BUFFER_SIZZ);
				readSize = _FILE.gcount();
			}
			if (_FILE.eof())
				_FILE.close();
		}
		else if (_autoindex)
		{
			_body = gen_def_page(_statusCode, _bodySize, _url.c_str(), _reqLocation);
			readSize = _bodySize;
		}
		else
		{
			_body = gen_def_page(_statusCode, _bodySize, nullptr, _reqLocation);
			readSize = _bodySize;
		}
	}
	return (_body);
}


std::string Response::makeHeaders()
{
	const std::time_t current_time = std::time(0);
	_headers += "Server: HTTP/1.1" + std::string(CRLF);
	_headers += "Date: " + std::string(ctime(&current_time));
	if (_statusCode == 301)
		_headers += "Location: " + _url + std::string(CRLF);
	if (_method == "PUT" || _method == "DELETE")
		_headers += "Content-Location: " + _url + std::string(CRLF);
	if (_statusCode < 300 || _statusCode > 399)
	{
		_headers += "Content-Type: " + _contentType + std::string(CRLF);
		_headers += "Accept-Ranges: bytes" + std::string(CRLF);
	}
	_headers += "Set-Cookie: lastsess=" + std::string(ctime(&current_time));
	_headers += "Content-Length: " + ft_itoa(_bodySize) + std::string(CRLF);
	if (_reqHeaders["Connection"].size())
		_headers += "Connection: " + _reqHeaders["Connection"] + std::string(CRLF);
	else
		_headers += "Connection: close" + std::string(CRLF);
	_headers += std::string(CRLF);
	return(_headers);
}

std::string Response::makeStatusLine()
{
	_statusLine = "HTTP/1.1 " + ft_itoa(_statusCode) + " " + error_map()[_statusCode] + CRLF;
	return (_statusLine);
}

void Response::SendResponse(int client_fd)
{
	int	res = 0;

//	std::cout << PURPLE"====================== SendResponse() ======================"NORM << std::endl;
//	std::cout << BLUE"Is file open?: "NORM << (_FILE.is_open() ? "yes" : "no") << std::endl;
//	std::cout << BLUE"StatusCode: "NORM << _statusCode << std::endl;
//	std::cout << BLUE"Autoindex: "NORM << (_autoindex ? "on" : "off") << std::endl;
//	std::cout << PURPLE"============================================================"NORM << std::endl;

	if (_inProc == false && (!_cgiPtr || _cgiPtr->isReadable()))
	{
		_response.append(makeStatusLine());
		_response.append(makeHeaders());
		_leftBytes = _bodySize;
		res = send(client_fd, _response.c_str(), _response.length(), 0);
		if (res == -1)
			throw RequestException("Sending data error");
//		std::cout << "\n\n" << _response << std::endl << std::endl;
		_response = std::string(); // TODO: изменить обнуление строки
		_inProc = true;
	}
	else if (_FILE.is_open() || _statusCode != 200 || _autoindex || _cgiPtr->isReadable())
	{
		int to_send, pos, tries;
		res = 0, pos = 0, tries = 0;
		makeBody(to_send);
		try
		{
			while (pos != to_send)
			{
				if (_cgiPtr && !_cgiPtr->isReadable())
					res = write(_cgiFd[1], &(_body[pos]), to_send);
				else
					res = send(client_fd, &(_body[pos]) , to_send, 0);
				pos += res;
				if (tries++ == 8 || res == -1)
				{
					_leftBytes = 0;
					throw RequestException("TOO MANY ATTEMPTS TO SEND DATA");
				}
			}
			_leftBytes -= res;
		}
		catch(const std::exception& e)
		{
			if (_cgiPtr)
			{
				delete _cgiPtr;
				_cgiPtr = nullptr;
				waitpid(_pid, 0, WNOHANG);
			}
			_statusCode = 502;
			_leftBytes = 1;
			_inProc = false;
			_url = "ERROR";
			std::cerr << e.what() << '\n';
		}
		delete [] _body;
	}
	if (_leftBytes < 1 && !_cgiPtr)
	{
		_inProc = false;
		_leftBytes = false;
		if (_cgiPtr)
			delete _cgiPtr;
		_cgiPtr = nullptr;
	}
	else if (_leftBytes < 1 && _cgiPtr)
	{
		if (!_cgiPtr->isReadable())
		{
			close(_cgiFd[1]);
			waitpid(_pid, 0, 0);
			_bodySize = getFdLen(_cgiFd[0]);
			_leftBytes = _bodySize;
			if (_leftBytes == -1)
			{
				_statusCode = 500;
				delete _cgiPtr;
				_cgiPtr = nullptr;
				_inProc = false;
				close(_cgiFd[0]);
			}
			else
				_cgiPtr->toRead(true);
		}
		else
		{
			close(_cgiFd[0]);
			delete _cgiPtr;
			_cgiPtr = nullptr;
			_inProc = false;
		}
	}
}

void Response::setErrorPages()
{
	std::map<int, std::string>	pages;

//	404=/www/html/404.html 405=/www/html/405.html
	pages.insert(std::make_pair(404, "/www/html/404.html"));
	pages.insert(std::make_pair(405, "/www/html/405.html"));

	_errorPages = pages;
}

std::string Response::getErrorPage()
{
	char *def_page;

	for (std::map<int, std::string>::iterator i = _errorPages.begin(); i != _errorPages.end(); i++)
	{
		if (i->first == static_cast<int>(_statusCode))
		{
			t_fileInfo file;
			urlInfo(i->second, &file, _FILE);
			std::cout << BLUE"urlInfo() fStatus return: "NORM << file.fStatus << std::endl; // TODO: удалить
			if (file.fStatus == 200)
			{
				_bodySize = file.fLength;
				_contentType = file.fExtension;
				return i->second;
			}
		}
	}
	std::cout << BLUE"urlInfo() autoindex: "NORM << (_autoindex ? "on" : "off") << std::endl; // TODO: удалить
	if (_autoindex)
		def_page = (gen_def_page(_statusCode, _bodySize, _url.c_str(), _reqLocation));
	else
	{
		def_page = (gen_def_page(_statusCode, _bodySize, nullptr, _reqLocation));
		_url = "ERROR";
	}

	std::cout << BLUE"getErrorPage() DEF_PAGE:"NORM << std::endl; // TODO: удалить
	std::cout << def_page << std::endl;

	delete def_page; // TODO: зачем удалять, если потом снова вызываем
	return (_url);
}
