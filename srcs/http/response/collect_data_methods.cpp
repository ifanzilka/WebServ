#include "Common_Header.hpp"

/**
 * 1)
 * */
void Response::CollectStartData(void)
{
	_body_size = 0;
	_reqHeaders = _request.GetHeaders();
	_method = _request.GetMethod();
	_url = MakeFilePath(_request, _status_code);
	_reqLocation = _request.GetLocation();
	_autoindex = _status_code == 1;
}

/**
 * 2)
 * */
std::string Response::MakeFilePath(Request &request, uint32_t &status_code)
{
	char *home = NULL;

	std::string file_path = request.GetUrl(status_code);

	if (status_code == 1) // если автоиндекс
		return (file_path);
	else if (request.GetMethod() == "PUT" || (request.GetMethod() == "POST" && status_code == 201))
	{
		home = getenv("HOME");
		if (home)
			file_path = std::string(home) + "/Downloads" + request.GetLocation()->GetLocationPath()
				+ file_path.substr(file_path.find_last_of('/') + 1);
		else
		{
			file_path = "/var/www/Downloads"
				+ file_path.substr(file_path.find(request.GetLocation()->GetLocationPath()));
		}
		std::ofstream file(file_path);
		if (!file.is_open())
			status_code = 204;
		else
		{
			std::string const &body(request.GetBody());
			file.write(body.c_str(), body.size());
			file.close();
			status_code = 201;
		}
	}
	else if (request.GetMethod() == "DELETE")
	{
		if (access(file_path.c_str() , W_OK) == -1 || remove(file_path.c_str()) == -1)
			status_code = 403;
		else
			status_code = 204;
		file_path = file_path.substr(file_path.find(request.GetLocation()->GetRoot()));
	}
	return (file_path);
}

/**
 * 3)
 * заполнение информации о файле по полученному file_path
 *
 * */
void	Response::FillFileInfo(const std::string &file_path, t_fileInfo *const file_t, std::ifstream &FILE)
{
	struct stat buff;
	int res;

	res = stat(file_path.c_str(), &buff);
	if (file_t != nullptr && res != -1)
	{
		// S_ISREG() - возвращает true, если аргумент является обычным файлом
		// st.mode -  тип файла */
		file_t->fType = static_cast<fileType>(S_ISREG(buff.st_mode)); // файл, папка или none
		if (file_t->fType == NONEXIST)
			return ;
		FILE.open(file_path);
		file_t->fLength = buff.st_size;
		file_t->fExtension = getExtension(file_path);
		if (FILE.is_open())
			file_t->fStatus = 200;
		else
		{
			file_t->fStatus = 403;
			FILE.close();
		}
	}
	else if (res < 1)
		file_t->fStatus = 404;
}

/**
 * 4)
 * */
void Response::CollectDataForResponse(const t_fileInfo &file)
{
	int cgi_amnt = 0;

	if ((cgi_amnt = CGI_Amount(_request.GetLocation()->GetCgi(), _url)) > 0)
	{
		_cgi_ptr = new CGI(_request, _request.GetLocation()->GetCgi(), _FILE);
		try
		{
			_cgi_fds = _cgi_ptr->CGI_Init(cgi_amnt, _pid);
		}
		catch(RequestException &e)
		{
			std::cerr << e.what() << " : " << strerror(errno) << std::endl;
			_status_code = 502;
		}
		_body_size = file.fLength;
		_leftBytes = file.fLength;
		_content_type = file.fExtension;
	}
	else if (cgi_amnt == -1)
	{
		_status_code = 502;
		_url = getErrorPage();
	}
	else
	{
		_body_size = file.fLength;
		_content_type = file.fExtension;
	}
	std::cout << B_CYAN"BODY_SIZE: "NORM << _body_size << std::endl; // TODO: удалить
}

/**
 * 5)
 *
 * @return Возвращает кол-во CGI с расширением как у файла (file_path)
 * */
uint16_t  Response::CGI_Amount(const std::multimap<std::string, std::string> &CGI, const std::string &file_path)
{
	uint16_t	cgi_amnt = 0;
	std::string ext = "." + file_path.substr(file_path.find_last_of('.') + 1);

	std::multimap<std::string, std::string>::const_iterator it = CGI.begin();
	for (; it != CGI.end(); it++)
	{
		if (ext == it->first)
		{
			if (access(it->second.c_str(), X_OK))
				return (-1);
			cgi_amnt++;
		}
	}
	return (cgi_amnt);
}