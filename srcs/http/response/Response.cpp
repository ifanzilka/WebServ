//
// Created by Josephine Beregond on 5/2/22.
//

#include "Response.hpp"

Response::Response(Request &request)
{
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

	if (_statusCode >= 400)
		_url = getErrorPage();
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

	delete def_page;
	return (_url);
}
