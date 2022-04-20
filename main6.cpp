#include <Include_Library.hpp>

int main()
{
    ft::AbstractServerApi *serverApi;
	
	// if (_processing_method.compare("--kqueue") == 0)
	// {
	// 	std::cout << "KQUEUE" << std::endl;
	// 	serverApi = new ft::ServerKqueue(host, port);
	// }
	// else if (_processing_method.compare("--poll") == 0)
	// {
	// 	std::cout << "POLL" << std::endl;
	// 	serverApi = new ft::ServerPoll(host, port);
	// }
	// else if (_processing_method.compare("--select") == 0)
	// {
	// 	std::cout << "SELECT" << std::endl;
	// 	serverApi = new ft::ServerSelect(host, port);
	// }

    serverApi = new ft::ServerSelect("127.0.0.1", 8082);
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
			continue;
		}
	}
}