
#include "tools.hpp"

std::uint8_t	isDirOrFile(std::string &path)
{
	std::string curr_path = getcwd(NULL, 0);
	std::string full_path;

	std::cout << "PWD: "<< curr_path << std::endl; //TODO: =----= УДАЛИТЬ
	full_path.append(curr_path);
	full_path.append(path);
	std::cout << "FULL_PATH: "<< full_path << std::endl; //TODO: =----= УДАЛИТЬ

	struct stat	s;

	if (stat(full_path.c_str(), &s) == -1)
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

	//TODO: удалить
	std::cout << "URL: " << _url << std::endl;
	std::cout << "METHOD: " << request.getMethod() << std::endl;
	std::cout << "STATUS: " << statusCode << std::endl;

	if (statusCode == 1) // если автоиндекс
		return (_url);
	// если метод PUT || POST
	else if (request.getMethod() == "PUT" || (request.getMethod() == "POST" && statusCode == 201))
	{
		char *home = getenv("HOME");
		int pos = _url.find(request.getLocation()->GetLocationPath());
		std::cout << "Location pos: " << pos << std::endl; //TODO: удалить
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