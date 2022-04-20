#include <Include_Library.hpp>

#include <iostream>

ServerCore::ServerCore(const char *precessing_method)
		: _processing_method(std::string(precessing_method))
{}

ServerCore::~ServerCore()
{}

void ServerCore::Start() const
{
	if (_processing_method.compare("--kqueue") == 0
		|| _processing_method.compare("--poll") == 0
		|| _processing_method.compare("--select") == 0)
		StartWebServer();
}

void ServerCore::StartWebServer() const
{
	ParserConfig config;
	//TODO: добавить прием пути к конфигу через аргументы
	config.parse("default.conf");
	std::map<int, ServerData> servers = config.getServers();

	//TODO: what to do if there's no ip-address or port in the config data???
	ServerData data = servers[0];
	std::string host = data.getHost();
	int			port = data.getPort();


	ft::AbstractServerApi *serverApi;
	ft::Messenger *messenger = new ft::Messenger();
	if (_processing_method.compare("--kqueue") == 0)
	{
		std::cout << "KQUEUE" << std::endl;
		serverApi = new ft::ServerKqueue(host, port);
	}
	else if (_processing_method.compare("--poll") == 0)
	{
		std::cout << "POLL" << std::endl;
		serverApi = new ft::ServerPoll(host, port);
	}
	else if (_processing_method.compare("--select") == 0)
	{
		std::cout << "SELECT" << std::endl;
		serverApi = new ft::ServerSelect(host, port);
	}

	while (1)
	{
		int client_fd;
		serverApi->WaitEvent();

		if ( serverApi->CheckAccept() != 0)
		{
			continue;
		}
		client_fd = serverApi->CheckRead();
		if (client_fd != 0)
		{
			serverApi->ReadFd(client_fd);
			//TODO: чтение может вернуть 0 или -1
			std::string request = serverApi->GetClientRequest();
			messenger->SetRequest(client_fd, request);
			continue;
		}
	}

	delete serverApi;
	delete messenger;
}