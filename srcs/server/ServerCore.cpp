#include <Include_Library.hpp>

#include <iostream>

ServerCore::ServerCore(std::string &processing_method, ServerData &server_data)
		: _processing_method(std::string(processing_method)),
		  _server_data(server_data)
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
	//TODO: what to do if there's no ip-address or port in the config data???
	std::string host = _server_data.GetHost();
	int			port = _server_data.GetPort();

	ft::Messenger messenger(_server_data);
	ft::AbstractServerApi *serverApi;
	if (_processing_method.compare("--kqueue") == 0)
		serverApi = new ft::ServerKqueue(host, port);
	else if (_processing_method.compare("--poll") == 0)
		serverApi = new ft::ServerPoll(host, port);
	else if (_processing_method.compare("--select") == 0)
		serverApi = new ft::ServerSelect(host, port);

	while (1)
	{
		int client_fd;
		serverApi->WaitEvent();

		if (serverApi->CheckAccept() != 0)
		{
			continue;
		}
		client_fd = serverApi->CheckRead();
		if (client_fd != 0)
		{
			serverApi->ReadFd(client_fd);
			//TODO: чтение может вернуть 0 или -1
			std::string request = serverApi->GetClientRequest();
			messenger.SetRequest(client_fd, request);
			continue;
		}
	}

	delete serverApi;
//	delete messenger;
}