#ifndef SERVERDATA_HPP
#define SERVERDATA_HPP

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "LocationData.hpp"

// данные содержащиеся в сервере из конфиг файла (серверов может и должно быть несколько)

class ServerData
{
private:

	/* Example:  webserv_80 */
	std::string _server_name;

	/* Port Example 80 8010 8050 8070 */
	int _port;

	/* Example:  127.0.0.1 нужно будет перевести в int возможно, если не использовать IFADDR_ANY */
	std::string _host;

	std::vector<LocationData> _loc; // массив локейшенов данного сервера со всеми данными
	// std::map<int, LocationData>    _loc; // массив локейшенов данного сервера со всеми данными

	/* data */
public:
	ServerData(/* args */);
	ServerData(const ServerData &cpy);
	ServerData &operator=(const ServerData &cpy);
	~ServerData();

	/* Geters */
	const std::string 				&getServerName() const;
	int 							getPort() const;
	const std::string 				&getHost() const;
	const std::vector<LocationData> &getLocationData() const;

	void addLocationData(LocationData const &locationData);

	/* Setters */
	void setServerName(std::string const &name);
	void setPort(int port);
	void setHost(std::string const &host);
};

std::ostream &operator<<(std::ostream &out, const ServerData &sd);

#endif