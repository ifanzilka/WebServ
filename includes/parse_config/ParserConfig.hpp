#ifndef PARSERCONFIG_HPP
#define PARSERCONFIG_HPP

#include <iostream>
#include <map>
#include "ServerData.hpp"
#include <fstream>

class ParserConfig
{

private:
	/* Ключ - значение (номер сервера) -> Класс{name, port, host, массив путей } */
	std::map<int, ServerData> _servers;
	/* server open переменная для парсера */
	bool _server_open;
	/* открыт ли loc */
	bool _loc_open;
	/* Количсетво серверов */
	int _serv_num;

	void check_brackets(std::string const &buffer);
	void fillServerData(ServerData &s, std::string &buffer);

public:
	ParserConfig();
	ParserConfig(ParserConfig &cpy);
	~ParserConfig();
	ParserConfig &operator=(const ParserConfig &cpy);
	void parse(std::string const &name);
	ServerData parseServer(std::ifstream &configfile, std::string &buffer);
	std::map<int, ServerData> &getServers();

};

#endif
