//
// Created by Josephine Beregond on 5/2/22.
//

#include "Response.hpp"

Response::Response(Request &request)
	:	_request(request),
		_body(nullptr),
		_req_location(nullptr),
		_cgi_ptr(nullptr)
{
	t_fileInfo file;
	SetErrorPages(); // TODO: заменить

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
			_url = GetErrorPage();
			_content_type = "text/html";
		}
		else if (_status_code != 301 && _method != "PUT" && _method != "DELETE")
		{
			/** заполняет необходимые данные и запускает CGI */
			CollectDataForResponse(file);
		}
	}
	else
		_url = GetErrorPage();
	_inProc = false;
}

Response::~Response()
{
	if (_cgi_ptr != nullptr)
		delete _cgi_ptr;
}

/**
 * 1)
 * */
void Response::SendResponse(int client_fd)
{
	int	read_bytes = 0;

//	std::cout << PURPLE"====================== SendResponse() ======================"NORM << std::endl;
//	std::cout << BLUE"Is file open?: "NORM << (_FILE.is_open() ? "yes" : "no") << std::endl;
//	std::cout << BLUE"StatusCode: "NORM << _status_code << std::endl;
//	std::cout << BLUE"Autoindex: "NORM << (_autoindex ? "on" : "off") << std::endl;
//	std::cout << PURPLE"============================================================"NORM << std::endl;

	if (_inProc == false && (!_cgi_ptr || _cgi_ptr->isReadable()))
	{
		_response.append(MakeStatusLine());
		_response.append(MakeHeadersBlock());
		_left_bytes = _body_size;
		read_bytes = send(client_fd, _response.c_str(), _response.length(), 0);
		if (read_bytes == -1)
			throw RequestException("Sending data error");
//		std::cout << "\n\n" << _response << std::endl << std::endl; //TODO: удалить
		_response = "";
		_inProc = true;
	}
	else if (_FILE.is_open() || _status_code != 200 || _autoindex || _cgi_ptr->isReadable())
	{
		int read_size;
		read_bytes = 0;

		MakeBody(read_size);
		SendBody(client_fd, read_size, read_bytes);
	}
	if (_left_bytes < 1 && !_cgi_ptr)
	{
		_inProc = false;
		_left_bytes = false;
		if (_cgi_ptr)
			delete _cgi_ptr;
		_cgi_ptr = nullptr;
	}
	else if (_left_bytes < 1 && _cgi_ptr)
	{
		CloseCGI();
	}
}

/**
 * 2)
 * */
const std::string &Response::MakeStatusLine()
{
	_status_line = "HTTP/1.1 " + std::to_string(_status_code) + " " + GetErrorTypes()[_status_code] + CRLF;
	return (_status_line);
}

/**
 * 3)
 * */
const std::string &Response::MakeHeadersBlock()
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
	if (_req_headers["Connection"].size())
		_headers += "Connection: " + _req_headers["Connection"] + std::string(CRLF);
	else
		_headers += "Connection: close" + std::string(CRLF);
	_headers += std::string(CRLF);
	return(_headers);
}

/**
 * 4)
 * */
char *Response::MakeBody(int &readSize)
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
			_body = MakePage(_status_code, _body_size, _url.c_str(), _req_location);
			readSize = _body_size;
		}
		else
		{
			_body = MakePage(_status_code, _body_size, nullptr, _req_location);
			readSize = _body_size;
		}
	}
	return (_body);
}

/**
 * 5)
 * */
void Response::SendBody(const int &client_fd, int &read_size, int &read_bytes)
{
	int pos, tries;
	pos = 0, tries = 0;

	try
	{
		while (pos != read_size)
		{
			if (_cgi_ptr && !_cgi_ptr->isReadable())
				read_bytes = write(_cgi_fds[1], &(_body[pos]), read_size);
			else
				read_bytes = send(client_fd, &(_body[pos]), read_size, 0);
			pos += read_bytes;
			if (tries++ == 8 || read_bytes == -1)
			{
				_left_bytes = 0;
				throw RequestException("TOO MANY ATTEMPTS TO SEND DATA"); //TODO увеличить кол-во попыток
			}
		}
		_left_bytes -= read_bytes;
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
		_left_bytes = 1;
		_inProc = false;
		_url = "ERROR";
		std::cerr << e.what() << '\n';
	}
	delete [] _body;
	_body = nullptr;
}

/**
 * 6)
 * */
void Response::CloseCGI(void)
{
	if (!_cgi_ptr->isReadable())
	{
		close(_cgi_fds[1]);
		waitpid(_pid, 0, 0);
		_body_size = GetFdLen(_cgi_fds[0]);
		_left_bytes = _body_size;
		if (_left_bytes == -1)
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

/**
 * ====================================
 * ==      Getters && Setters        ==
 * ====================================
 */

void Response::SetErrorPages()
{
	std::map<int, std::string>	pages;

	pages.insert(std::make_pair(404, "/www/html/404.html"));
	pages.insert(std::make_pair(405, "/www/html/405.html"));

	_error_pages = pages;
}

const std::string &Response::GetErrorPage()
{
	char *def_page = nullptr;

	for (std::map<int, std::string>::iterator i = _error_pages.begin(); i != _error_pages.end(); i++)
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
		def_page = (MakePage(_status_code, _body_size, _url.c_str(), _req_location));
	else
	{
		def_page = (MakePage(_status_code, _body_size, nullptr, _req_location));
		_url = "ERROR";
	}

//	std::cout << BLUE"getErrorPage() DEF_PAGE:"NORM << std::endl; // TODO: удалить
//	std::cout << def_page << std::endl;

	delete def_page; // TODO: зачем удалять, если потом снова вызываем
	return (_url);
}