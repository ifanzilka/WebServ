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

	Messenger messenger(_server_data);
	AbstractServerApi *serverApi = nullptr;

	#ifdef KQUEUE
		std::cout << "KQUEUE\n" << std::endl;
		serverApi = new ServerKqueue(host, port);
	#endif
	#ifdef POLL
		std::cout << "POLL\n" << std::endl;
		serverApi = new ServerPoll(host, port);
	#endif
	#ifdef SELECT
		std::cout << "SELECT\n" << std::endl;
		serverApi = new ServerSelect(host, port);
	#endif

	if (serverApi == nullptr)
	{
		printf(RED"Server object is NULL!\n"NORM);
		return;
	}

	std::cout << PURPLE"Use: " << "http://"<< serverApi->GetHostName() << ":" <<  serverApi->GetPort() << NORM << std::endl;

	int tmp = 0;
	ssize_t read_bytes = 0;
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
			try
			{
				read_bytes = serverApi->ReadFd(client_fd);
				//TODO: чтение может вернуть 0 или -1 - закрыть соединение при ошибке
				if (read_bytes == 0)
				{
					messenger.isClosedConnection = true;
				}
				else if (read_bytes == -1)
					throw RequestException(502, "error during reading data with recv()");

				std::string request = serverApi->GetClientRequest(); // возвращает _client_rqst_msg;
				if (request.empty()) // если всё полученное сообщение пустое
					throw RequestException(500, "ServerCore: serverApi->GetClientRequest() is empty!");
				messenger.StartMessaging(client_fd, request);
				messenger.ClearValidLocations();
			}
			catch (std::exception &e)
			{
				std::cout << e.what() << std::endl;
			}
			continue;
		}
	}

	delete serverApi;
}
