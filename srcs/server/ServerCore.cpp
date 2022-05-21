#include <Include_Library.hpp>

#include <iostream>

ServerCore::ServerCore(ServerData &server_data)
	: _server_data(server_data)
{}

ServerCore::~ServerCore()
{}

void ServerCore::Start() const
{
	StartWebServer();
}

void ServerCore::StartWebServer() const
{
	//TODO: what to do if there's no ip-address or port in the config data???
	std::string host = _server_data.GetHost();
	int			port = _server_data.GetPort();

//	AbstractServerApi *serverApi = nullptr;
	ServerKqueue *serverApi = nullptr;

//	#ifdef KQUEUE
		std::cout << "KQUEUE\n" << std::endl;
		serverApi = new ServerKqueue(host, port);
//	#endif
//	#ifdef POLL
//		std::cout << "POLL\n" << std::endl;
//		serverApi = new ServerPoll(host, port);
//	#endif
//	#ifdef SELECT
//		std::cout << "SELECT\n" << std::endl;
//		serverApi = new ServerSelect(host, port);
//	#endif

	Messenger messenger(_server_data, *serverApi); //TODO: поменять на KQUEUE

	if (serverApi == nullptr)
	{
		printf(RED"Server object is NULL!\n"NORM);
		return;
	}

	std::cout << PURPLE"Use: " << "http://"<< serverApi->GetHostName() << ":" <<  serverApi->GetPort() << NORM << std::endl;

	while (1)
	{
		int client_fd;
		int event;
		event = serverApi->WaitEvent(client_fd);

		try
		{
			if (!event)
			{
				std::cout << B_YELLOW"WAITING..."NORM << std::endl; //TODO: удалить
			}
			else if (event == EV_EOF)
			{
				std::cout << GREEN"DISCONNECT"NORM << std::endl;
			}
			else if (event == EVFILT_READ)
			{
//				std::cout << YELLOW"READ"NORM << std::endl;
				messenger.ReadRequest(client_fd);
				messenger.ClearValidLocations();
			}
			else if (event == EVFILT_WRITE)
			{
//				std::cout << PURPLE"WRITE"NORM << std::endl;
				messenger.MakeResponse();
			}
			else
			{
				std::cout << B_RED"FUUUUUCK"NORM << std::endl;
			}
		}
		catch (RequestException &e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	delete serverApi;
}
