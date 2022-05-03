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

	std::multimap<std::string, LocationData> _loc;
//	std::vector<LocationData> _loc; // массив локейшенов данного сервера со всеми данными
	// std::map<int, LocationData>    _loc; // массив локейшенов данного сервера со всеми данными

	/* data */
public:
	ServerData(/* args */);
	ServerData(const ServerData &cpy);
	ServerData &operator=(const ServerData &cpy);
	~ServerData();

	/* Geters */
	const std::string 				&GetServerName() const;
	int 							GetPort() const;
	const std::string 				&GetHost() const;
	const std::multimap<std::string, LocationData> &GetLocationData() const;

	void AddLocationData(const std::pair<std::string, LocationData> loc_pair);

	/* Setters */
	void SetServerName(std::string const &name);
	void SetPort(int port);
	void SetHost(std::string const &host);
};

std::ostream &operator<<(std::ostream &out, const ServerData &sd);

#endif