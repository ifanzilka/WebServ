#include <Include_Library.hpp>
//#include "ServerData.hpp"

ServerData::ServerData(/* args */) : _server_name(""), _port(0), _host("")
{
}

ServerData::ServerData(const ServerData &cpy)
{
	*this = cpy;
}

ServerData::~ServerData()
{
}

ServerData &ServerData::operator=(const ServerData &cpy)
{
	if (this != &cpy)
	{
		this->_server_name = cpy._server_name;
		this->_port = cpy._port;
		this->_host = cpy._host;
		this->_loc = cpy._loc;
	}
	return (*this);
}

const std::string &ServerData::GetServerName() const
{
	return (this->_server_name);
}

int ServerData::GetPort() const
{
	return (this->_port);
}

const std::string &ServerData::GetHost() const
{
	return (this->_host);
}

const std::multimap<std::string, LocationData> &ServerData::GetLocationData() const
{
	return (this->_loc);
}

void ServerData::AddLocationData(const std::pair<std::string, LocationData> loc_pair)
{
	_loc.insert(loc_pair);
}

void ServerData::SetServerName(std::string const &name)
{
	this->_server_name = name;
}

void ServerData::SetPort(int port)
{
	this->_port = port;
}

void ServerData::SetHost(std::string const &host)
{
	this->_host = host;
}


std::ostream &operator<<(std::ostream &out, const ServerData &tmp)
{
	std::multimap<std::string, LocationData> loc;
	loc = tmp.GetLocationData();

	out << "ServName: " << tmp.GetServerName() << std::endl;
	out << "Port: " << tmp.GetPort() << std::endl;
	out << "Host: " << tmp.GetHost() << std::endl;

	for (std::multimap<std::string, LocationData>::iterator it = loc.begin(); it != loc.end(); ++it)
	{
		std::cout << "PATH: " << it->first << std::endl <<
			it->second << std::endl;
	}
	return (out);
}