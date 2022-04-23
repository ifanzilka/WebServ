#include <Include_Library.hpp>
#include <iterator>

ParserConfig::ParserConfig() : _server_open(false), _loc_open(false), _serv_num(0)
{
}

ParserConfig::~ParserConfig()
{
}

ParserConfig::ParserConfig(ParserConfig &cpy)
{
	*this = cpy;
}

ParserConfig &ParserConfig::operator=(const ParserConfig &cpy)
{
	if (this != &cpy)
	{
		_servers = cpy._servers;
		_server_open = cpy._server_open;
		_loc_open = cpy._loc_open;
		_serv_num = cpy._serv_num;
	}
	return (*this);
}

std::map<int, ServerData> &ParserConfig::GetServers()
{
	return (this->_servers);
}

void ParserConfig::CheckBrackets(std::string const &buffer)
{
	if ((buffer.find('{', 0) != std::string::npos) && !_server_open)
		_server_open = true;
	else if (((buffer.find('{', 0) != std::string::npos) &&
			  _server_open))
		_loc_open = true;
	if (((buffer.find('}', 0) != std::string::npos) && _loc_open))
	{
		_loc_open = false;
	}
	else if (((buffer.find('}', 0) != std::string::npos) && !_loc_open))
	{
		_server_open = false;
	}
}

void ParserConfig::FillServerData(ServerData &s, std::string &buffer)
{
	size_t next = 0;
	if ((next = buffer.find("port ", 0)) != std::string::npos)
	{
		next += 5;
		s.SetPort(std::stoi(buffer.substr(next, (buffer.find(';') - next)))); // ищем точку с запятой, вырезаем от пробела до точи с запятой, коертируем в строку си, переводим в инт и записываем в значение порта.
	}
	else if ((next = buffer.find("host ", 0)) != std::string::npos)
	{
		next += 5;
		s.SetHost(buffer.substr(next, (buffer.find(';') - next)));
	}
	else if ((next = buffer.find("server_name ", 0)) != std::string::npos)
	{
		next += 12;
		s.SetServerName(buffer.substr(next, (buffer.find(';') - next)));
	}
}

// в буффер передается первая строчка каждого блока 'location'
LocationData ParseLocations(std::ifstream &configfile, std::string &buffer, size_t next)
{
	int fst_space = buffer.find(' ');
	int lst_space = buffer.rfind(' ');
	LocationData l;

	l.SetExactPath(buffer[fst_space + 1] == '=');
	l.SetLocationPath(buffer.substr(next, lst_space - next));

	while (getline(configfile, buffer))
	{
		if (buffer.find('}') != std::string::npos)
		{
			if (!l.GetLocationPath().empty() && !l.GetRoot().empty() &&
				l.GetFullPath().empty())
			{
				l.SetFullPath(l.GetRoot(), l.GetLocationPath());
			}
			else
				l.SetFullPath("", l.GetLocationPath());
			return (l);
		}
		if ((next = buffer.find("root ") != std::string::npos))
		{
			next += 5;
			l.SetRoot(buffer.substr(next + 1, buffer.rfind(';') - next - 1));
		}
		else if ((next = buffer.find("\tindex ")) != std::string::npos)
		{
			next += 6;
			l.SetIndex(buffer.substr(next + 1, buffer.rfind(';') - next - 1));
		}
		else if ((next = buffer.find("method ")) != std::string::npos)
		{
			next += 7;
			l.SetMethods(buffer.substr(next, buffer.rfind(';') - next));
		}
		else if ((next = buffer.find("cgi_path ")) != std::string::npos)
		{
			next += 9;
			l.SetCgiPath(buffer.substr(next, buffer.rfind(';') - next));
		}
		else if ((next = buffer.find("redirect ")) != std::string::npos)
		{
			next += 9;
			l.SetRedirect(buffer.substr(next, buffer.rfind(';') - next));
		}
		else if ((next = buffer.find("cgi_extension ")) != std::string::npos)
		{
			next += 14;
			l.SetCgiExtension(buffer.substr(next, buffer.rfind(';') - next));
		}
		else if ((next = buffer.find("client_body_buffer_size ")) !=
				 std::string::npos)
		{
			next += 24;
			l.SetClientBufferBodySize(
					std::stoi(buffer.substr(next, buffer.rfind(';') - next)));
		}
		else if ((next = buffer.find("autoindex ")) != std::string::npos)
		{
			next += 10;
			l.SetAutoindex(
					(buffer.substr(next, buffer.rfind(';') - next) == "on"));
		}

	}
	// std::cout << "I`m here!!!1" << std::endl;
	// if (!l.getLocationPath().empty() && !l.getRoot().empty() && l.getFullPath().empty())
	// {
	//         std::cout << "I`m here!!!2" << std::endl;
	//         l.setFullPath(l.getRoot(), l.getLocationPath());
	// }
	return (l);
}

ServerData ParserConfig::ParseServer(std::ifstream &configfile, std::string &buffer)
{
	ServerData s;
	size_t context_pos;

	while (getline(configfile, buffer))
	{
		if (buffer.find('}') != std::string::npos)
		{
			_server_open = false;
			return (s);
		}
		FillServerData(s, buffer);
		if ((context_pos = buffer.find("location ")) != std::string::npos)
		{
			int path_ind = buffer.find('/');
			s.AddLocationData(ParseLocations(configfile, buffer, path_ind));
		}
	}
	return (s);
}

void ParserConfig::Parse(std::string const &name)
{

	_server_open = false;
	_loc_open = false;
	_servers.clear();
	std::ifstream configfile(name);
	std::string buffer;
	size_t next = 0;
	bool server_find = false;
	_serv_num = 0;

	if (!configfile)
	{
		std::cerr << "Uh oh, " << name
				  << " config file could not be opened for reading!"
				  << std::endl;
		exit(1);
	}
	while (getline(configfile, buffer))
	{
		next = buffer.find("server ", 0);
		/* Если нашли слово server */
		/* Если не конец строки*/
		if (next != std::string::npos)
		{
			server_find = true;
		}
		if (server_find)
		{
			CheckBrackets(buffer);
			if (_server_open && _serv_num < 4)
			{
				_servers.insert(std::pair<int, ServerData>(_serv_num,
					ParseServer(configfile, buffer)));
				_serv_num += 1;
				server_find = false;
			}
		}
	}

//	for (std::map<int, ServerData>::iterator it = _servers.begin(); it != _servers.end(); ++it)
//	{
//		ServerData &tmp = it->second;
//		std::cout << tmp << std::endl;
//		std::cout << std::endl;
//	}
}