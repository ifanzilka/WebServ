#include "tools.hpp"

off_t getFdLen(int fd)
{
	struct stat buff;
	off_t	len = 0;
	if (fstat(fd, &buff) == -1)
		len = 0;
	else
		len = buff.st_size;
	return (len);
}

int  checkCgi(const std::multimap<std::string, std::string> &CGI, std::string fPath)
{
	int			res = 0;
	std::string ext = "." + fPath.substr(fPath.find_last_of('.') + 1);

	std::multimap<std::string, std::string>::const_iterator i = CGI.begin();
	for (; i != CGI.end(); i++)
	{
		if (ext == i->first)
		{
			if (access(i->second.c_str(), X_OK))
				return (-1);
			res++;
		}
	}
	return (res);
}

static std::string	buildPathToFile(std::string const &fullPath, const LocationData *location, std::string fileName)
{
	std::string	resultPath;
	std::string	tmp;
	std::size_t	pos;

	pos = fullPath.rfind(location->GetRoot());
	if (fileName == "."
		or fileName == ".." or pos == std::string::npos)
		return ".";

	tmp = fullPath.substr(pos + location->GetRoot().length());
	if (!tmp.length())
		tmp = "/";
	pos = tmp.find(location->GetLocationPath());
	if (pos == std::string::npos)
		return ".";
	if (tmp ==  "/")
		return fileName;
	return (tmp.substr(pos) + "/" + fileName);
}

char	*filesListing(std::string const &path, uint64_t &bodySize, uint32_t &statusCode,
	const LocationData *location)
{
	std::string		htmlBody;
	DIR				*dirPtr;
	struct dirent	*dirent;
	std::string		pathToFile;

	if (!location)
		return nullptr;
	dirPtr = opendir(path.c_str()); // открытие директории по _url
	if (!dirPtr)
	{
		statusCode = 403;
		return nullptr;
	}

	htmlBody = "<!DOCTYPE html>\n";
	htmlBody += "<html>\n";
	htmlBody += "<head><title>AutoIndexON</title></head>\n";
	htmlBody += "<body>\n<h1>Files in current directory</h1>\n";
	dirent = readdir(dirPtr);
	while (dirent)
	{
		pathToFile = buildPathToFile(path, location, dirent->d_name);
		if (pathToFile != ".")
		{
			htmlBody += "<div><a href=\"" + pathToFile + "\"><h2>"
						+ dirent->d_name + "</a></h2>\n";
		}
		dirent = readdir(dirPtr);
	}
	closedir(dirPtr);
	htmlBody += "</body>\n</html>\n";
	bodySize = htmlBody.length();
	statusCode = 200;
	return strdup(htmlBody.c_str());
}

std::map <int, std::string> &error_map()
{
	static  std::map <int, std::string> error_map;
	if (!error_map.size())
	{
		error_map.insert(std::pair<int, std::string>(200, " Ok"));
		error_map.insert(std::pair<int, std::string>(201, " Created"));
		error_map.insert(std::pair<int, std::string>(204, " No Content"));
		error_map.insert(std::pair<int, std::string>(301, " Moved Permanently"));
		error_map.insert(std::pair<int, std::string>(400, " Bad Request"));
		error_map.insert(std::pair<int, std::string>(403, " Forbidden"));
		error_map.insert(std::pair<int, std::string>(404, " Not Found"));
		error_map.insert(std::pair<int, std::string>(405, " Method Not Allowed"));
		error_map.insert(std::pair<int, std::string>(413, " Payload Too Large"));
		error_map.insert(std::pair<int, std::string>(500, " Internal Server Error"));
		error_map.insert(std::pair<int, std::string>(502, " Bad Gateway"));
		error_map.insert(std::pair<int, std::string>(503, " Service Unavailable"));
	}
	return error_map;
}

std::string	ft_itoa(int x)
{
	std::stringstream	buff;
	std::string			tmp;
	buff << x;
	buff >> tmp;

	return (tmp);
}

//TODO: проверить корретность страницы ответа, ибо показываются лишние
// символы из-за добавления 'video_ref'
char	*gen_def_page(uint32_t &statusCode, uint64_t &bodySize,
	const char *path, const LocationData *location)
{
	std::string video_ref = "https://www.youtube.com/watch?v=sbAClADl4C8&ab_channel=RangeOfficial26";

	char *def_page;
	if (!path)
	{
		std::stringstream buff;
		buff << "<html>\n";
		buff << "<head><title>" + ft_itoa(statusCode) + error_map()[statusCode] + "</title>\n";
		buff << "<body>\n";
		buff << "<center><h1>" + ft_itoa(statusCode)  + error_map()[statusCode] + "</h1></center>\n";
//		buff << "<hr><center>SUPER SERVER TEAM</center>\n";
		buff << "<hr><center>Your request is wrong, don't do this again or else..</center>\n";
		buff << "<hr><center>If you want to relax, click ";
		buff << "<a href=\"" + video_ref + "\">here</a></center>\n";
		buff << "</body>\n";
		buff << "</html>\n";
		buff.seekg(0, buff.end);
		bodySize = buff.tellg();
		buff.seekg(0, buff.beg);
		def_page = new char[bodySize];
		buff.read(def_page, bodySize);
	}
	else if (!(def_page = filesListing(std::string(path) , bodySize, statusCode, location)))
		def_page = gen_def_page(statusCode, bodySize, nullptr, location);
	return (def_page);
}

std::string strUpper(const std::string &str)
{
	std::string to_ret = str;
	for (std::string::iterator i = to_ret.begin(); i != to_ret.end() ; i++){
		*i = toupper(*i);
	}
	return to_ret;
}

std::string getExtension(std::string fPath)
{
	static std:: map<std::string, std::string> MIME;
	std::string	ext = "application/octet-stream";

	if (!MIME.size())
	{
		MIME.insert(std::pair<std::string, std::string>("mp3", "audio/mpeg"));
		MIME.insert(std::pair<std::string, std::string>("weba", "audio/webm"));
		MIME.insert(std::pair<std::string, std::string>("aac", "audio/aac"));

		MIME.insert(std::pair<std::string, std::string>("avi", "video/x-msvideo"));
		MIME.insert(std::pair<std::string, std::string>("mpeg", "video/mpeg"));
		MIME.insert(std::pair<std::string, std::string>("ogv", "video/ogg"));
		MIME.insert(std::pair<std::string, std::string>("ts", "video/mp2t"));
		MIME.insert(std::pair<std::string, std::string>("webm", "video/webm"));
		MIME.insert(std::pair<std::string, std::string>("mp4", "video/mp4"));

		MIME.insert(std::pair<std::string, std::string>("gif", "image/gif"));
		MIME.insert(std::pair<std::string, std::string>("jpeg", "image/jpeg"));
		MIME.insert(std::pair<std::string, std::string>("png", "image/png"));
		MIME.insert(std::pair<std::string, std::string>("jpg", "image/jpeg"));
		MIME.insert(std::pair<std::string, std::string>("tiff", "image/tiff"));
		MIME.insert(std::pair<std::string, std::string>("webp", "image/webp"));
		MIME.insert(std::pair<std::string, std::string>("ico", "image/x-icon"));
		MIME.insert(std::pair<std::string, std::string>("bmp", "image/bmp"));

		MIME.insert(std::pair<std::string, std::string>("js", "application/javascript"));
		MIME.insert(std::pair<std::string, std::string>("json", "application/json"));
		MIME.insert(std::pair<std::string, std::string>("pdf", "application/pdf"));
		MIME.insert(std::pair<std::string, std::string>("ps", "application/postscript"));
		MIME.insert(std::pair<std::string, std::string>("doc", "application/msword"));

		MIME.insert(std::pair<std::string, std::string>("css", "text/css"));
		MIME.insert(std::pair<std::string, std::string>("html", "text/html"));
		MIME.insert(std::pair<std::string, std::string>("htm", "text/html"));
		MIME.insert(std::pair<std::string, std::string>("csv", "text/csv"));
		MIME.insert(std::pair<std::string, std::string>("php", "text/php"));
	}

	if (fPath.find_last_of('.') != std::string :: npos)
	{
		std::string find = fPath.substr(fPath.find_last_of('.') + 1);
		for (std::map<std::string, std::string>::iterator i = MIME.begin(); i != MIME.end(); i++)
		{
			if (find == i->first || find == strUpper(i->first))
			{
				ext = i->second;
				break ;
			}
		}
	}
	return (ext);
}

/** заполнение информации о файле по полученнуму URL */
bool	urlInfo(std::string fPath, t_fileInfo *fStruct, std::ifstream &FILE)
{
	struct stat buff;
	int res;

	res = stat(fPath.c_str(), &buff);
	if (fStruct != nullptr && res != -1)
	{
		// S_ISREG() - возвращает true, если аргумент является обычным файлом
		// st.mode -  тип файла */
		fStruct->fType = static_cast<fileType>(S_ISREG(buff.st_mode)); // файл, папка или none
		if (fStruct->fType == NONEXIST)
			return (false);
		FILE.open(fPath);
		fStruct->fLength = buff.st_size;
		fStruct->fExtension = getExtension(fPath);
		if (FILE.is_open())
			fStruct->fStatus = 200;
		else
		{
			fStruct->fStatus = 403;
			FILE.close();
		}
	}
	else if (res < 1)
	{
		fStruct->fStatus = 404;
		return (false);
	}
	return (true); // TODO: удалить
}

std::uint8_t	isDirOrFile(std::string &path)
{
//	std::string curr_path = getcwd(NULL, 0);
//	std::string full_path;

//	std::cout << "PWD: "<< curr_path << std::endl; //TODO: =----= УДАЛИТЬ
//	full_path.append(curr_path);
//	full_path.append(path);
//	std::cout << "PATH: "<< path << std::endl; //TODO: =----= УДАЛИТЬ

	struct stat	s;

	if (stat(path.c_str(), &s) == -1)
		return NOT_FOUND;
	if (s.st_mode & S_IFDIR)
		return DIR_MODE;
	if (s.st_mode & S_IFREG)
		return FILE_MODE;
	return UNKNOWN_MODE;
}

std::string putDelete(Request &request, uint32_t &statusCode)
{
	std::string _url = request.getUrl(statusCode);

//	//TODO: удалить
//	std::cout << "URL: " << _url << std::endl;
//	std::cout << "METHOD: " << request.getMethod() << std::endl;
//	std::cout << "STATUS: " << statusCode << std::endl;

	if (statusCode == 1) // если автоиндекс
		return (_url);
	// если метод PUT || POST
	else if (request.getMethod() == "PUT" || (request.getMethod() == "POST" && statusCode == 201))
	{
		char *home = getenv("HOME");
		int pos = _url.find(request.getLocation()->GetLocationPath());
		if (home)
			_url =  std::string(home) + "/Downloads" + request.getLocation()->GetLocationPath() + _url.substr(_url.find_last_of('/') + 1);
		else
			_url =  "/var/www/Downloads" + _url.substr(pos);
		std::ofstream newFile(_url);
		if (!newFile.is_open())
			statusCode = 204;
		else
		{
			std::string const &body(request.getBody());
			newFile.write(body.c_str(), body.size());
			newFile.close();
			statusCode = 201;
		}
	}
	else if (request.getMethod() == "DELETE")
	{
		if (access(_url.c_str() , W_OK) == -1 || remove(_url.c_str()) == -1)
			statusCode = 403;
		else
			statusCode = 204;
		_url = _url.substr(_url.find(request.getLocation()->GetRoot()));
	}
	return _url;
}

std::size_t	skipWhiteSpaces(std::string const &str, std::size_t start)
{
	if (start >= str.length())
		return str.length();
	while (start < str.length() and isCharWhiteSpace(str[start]))
		start++;
	return start;
}

bool	isCharWhiteSpace(unsigned char c)
{
	return std::isspace(c);
}