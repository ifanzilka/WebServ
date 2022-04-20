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

const std::string &ServerData::getServerName() const
{
	return (this->_server_name);
}

int ServerData::getPort() const
{
	return (this->_port);
}

const std::string &ServerData::getHost() const
{
	return (this->_host);
}

const std::vector<LocationData> &ServerData::getLocationData() const
{
	return (this->_loc);
}

void ServerData::addLocationData(const LocationData &locationData)
{
	_loc.push_back(locationData);
}

void ServerData::setServerName(std::string const &name)
{
	this->_server_name = name;
}

void ServerData::setPort(int port)
{
	this->_port = port;

}

void ServerData::setHost(std::string const &host)
{
	this->_host = host;
}


std::ostream &operator<<(std::ostream &out, const ServerData &tmp)
{
	std::vector<LocationData> loc;
	loc = tmp.getLocationData();

	out << "ServName: " << tmp.getServerName() << std::endl;
	out << "Port: " << tmp.getPort() << std::endl;
	out << "Host: " << tmp.getHost() << std::endl;

	for (std::vector<LocationData>::iterator it = loc.begin();
		 it != loc.end(); ++it)
		std::cout << *it << std::endl;
	return (out);
}