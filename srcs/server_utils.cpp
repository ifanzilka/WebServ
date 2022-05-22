#include "server_utils.hpp"

bool	IsChar(unsigned char c) { return std::isspace(c); }

off_t GetFdLen(const int &fd)
{
	struct stat buff;
	off_t len = 0;

	if (fstat(fd, &buff) == -1)
		len = 0;
	else
		len = buff.st_size;
	return (len);
}

std::uint8_t	GetDataType(const std::string &data_path)
{
	struct stat	s;

	if (stat(data_path.c_str(), &s) == -1)
		return (NOT_FOUND);
	if (s.st_mode & S_IFDIR)
		return (DIR_MODE);
	if (s.st_mode & S_IFREG)
		return (FILE_MODE);
	return (UNKNOWN_MODE);
}

static std::string	MakeFullPath(std::string const &file_path, const LocationData *location, const std::string &file_name)
{
	std::string	tmp;
	std::size_t	position;

	position = file_path.rfind(location->GetRoot());
	if (file_name == "."
		|| file_name == ".." || position == std::string::npos)
		return (".");

	tmp = file_path.substr(position + location->GetRoot().length());
	if (!tmp.length())
		tmp = "/";

	position = tmp.find(location->GetLocationPath());
	if (position == std::string::npos)
		return (".");
	if (tmp ==  "/")
		return (file_name);
	return (tmp.substr(position) + "/" + file_name);
}

char	*Autoindex(std::string const &path, uint64_t &bodySize, uint32_t &statusCode, const LocationData *location)
{
	std::string		html_body;
	DIR				*dir_fd;
	struct dirent	*dir_point;
	std::string		file_path;

	if (!location)
		return (nullptr);
	dir_fd = opendir(path.c_str()); // открытие директории по _url
	if (!dir_fd)
	{
		statusCode = 403;
		return (nullptr);
	}

	html_body = "<!DOCTYPE html>\n"
			   "<html>\n"
			   "<head>\n"
			   "<title>Index of </title>\n"
			   "</head>"
			   "<body bgcolor=\"white\">\n"
			   "<h1>Files in current directory</h1>\n";

	dir_point = readdir(dir_fd);
	while (dir_point)
	{
		file_path = MakeFullPath(path, location, dir_point->d_name);
		if (file_path != ".")
		{
			html_body += "<div><a href=\"" + file_path + "\"><h2>"
				+ dir_point->d_name + "</a></h2>\n";
		}
		dir_point = readdir(dir_fd);
	}
	closedir(dir_fd);
	html_body += "</body>\n</html>\n";
	bodySize = html_body.length();
	statusCode = 200;
	return strdup(html_body.c_str());
}

std::map<int, std::string> &GetErrorTypes()
{
	static  std::map <int, std::string> error_types;

	if (!error_types.size())
	{
		error_types.insert(std::pair<int, std::string>(200, " Ok"));
		error_types.insert(std::pair<int, std::string>(201, " Created"));
		error_types.insert(std::pair<int, std::string>(204, " No Content"));
		error_types.insert(std::pair<int, std::string>(301, " Moved Permanently"));
		error_types.insert(std::pair<int, std::string>(400, " Bad Request"));
		error_types.insert(std::pair<int, std::string>(403, " Forbidden"));
		error_types.insert(std::pair<int, std::string>(404, " Not Found"));
		error_types.insert(std::pair<int, std::string>(405, " Method Not Allowed"));
		error_types.insert(std::pair<int, std::string>(413, " Payload Too Large"));
		error_types.insert(std::pair<int, std::string>(500, " Internal Server Error"));
		error_types.insert(std::pair<int, std::string>(502, " Bad Gateway"));
		error_types.insert(std::pair<int, std::string>(503, " Service Unavailable"));
	}
	return (error_types);
}

//TODO: проверить корретность страницы ответа, ибо показываются лишние
// символы из-за добавления 'video_ref'
char	*MakePage(uint32_t &status_code, uint64_t &body_size,
	const char *path, const LocationData *location)
{
	std::string video_ref = "https://www.youtube.com/watch?v=sbAClADl4C8&ab_channel=RangeOfficial26";

	char *def_page;
	if (!path)
	{
		std::stringstream buff;
		buff << "<html>\n";
		buff << "<head><title>" + std::to_string(status_code) + GetErrorTypes()[status_code] + "</title>\n";
		buff << "<body>\n";
		buff << "<center><h1>" + std::to_string(status_code)  + GetErrorTypes()[status_code] + "</h1></center>\n";
		buff << "<hr><center>Your request is wrong, don't do this again or else..</center>\n";
		buff << "<hr><center>If you want to relax, click ";
		buff << "<a href=\"" + video_ref + "\">here </a></center>\n";
		buff << "</body>\n";
		buff << "</html>\n";
		buff.seekg(0, buff.end);
		body_size = buff.tellg();
		buff.seekg(0, buff.beg);
		def_page = new char[body_size];
		buff.read(def_page, body_size);
	}
	else if (!(def_page = Autoindex(std::string(path) , body_size, status_code, location)))
		def_page = MakePage(status_code, body_size, nullptr, location);
	return (def_page);
}

std::string UpperStr(const std::string &str)
{
	std::string to_ret = str;
	for (std::string::iterator i = to_ret.begin(); i != to_ret.end() ; i++)
		*i = toupper(*i);
	return (to_ret);
}

std::string GetFileExtension(const std::string &file_path)
{
	static std::map<std::string, std::string> MIME;
	std::string	ext = "application/octet-stream";

	if (!MIME.size())
	{
		MIME.insert(std::pair<std::string, std::string>("mp3", "audio/mpeg"));
		MIME.insert(std::pair<std::string, std::string>("mp4", "video/mp4"));
		MIME.insert(std::pair<std::string, std::string>("avi", "video/x-msvideo"));
		MIME.insert(std::pair<std::string, std::string>("mpeg", "video/mpeg"));
		MIME.insert(std::pair<std::string, std::string>("webm", "video/webm"));

		MIME.insert(std::pair<std::string, std::string>("gif", "image/gif"));
		MIME.insert(std::pair<std::string, std::string>("png", "image/png"));
		MIME.insert(std::pair<std::string, std::string>("jpeg", "image/jpeg"));
		MIME.insert(std::pair<std::string, std::string>("jpg", "image/jpeg"));
		MIME.insert(std::pair<std::string, std::string>("bmp", "image/bmp"));

		MIME.insert(std::pair<std::string, std::string>("js", "application/javascript"));
		MIME.insert(std::pair<std::string, std::string>("json", "application/json"));
		MIME.insert(std::pair<std::string, std::string>("pdf", "application/pdf"));
		MIME.insert(std::pair<std::string, std::string>("doc", "application/msword"));

		MIME.insert(std::pair<std::string, std::string>("css", "text/css"));
		MIME.insert(std::pair<std::string, std::string>("html", "text/html"));
		MIME.insert(std::pair<std::string, std::string>("php", "text/php"));
	}

	if (file_path.find_last_of('.') != std::string :: npos)
	{
		std::string find = file_path.substr(file_path.find_last_of('.') + 1);
		for (std::map<std::string, std::string>::iterator i = MIME.begin(); i != MIME.end(); i++)
		{
			if (find == i->first || find == UpperStr(i->first))
			{
				ext = i->second;
				break;
			}
		}
	}
	return (ext);
}