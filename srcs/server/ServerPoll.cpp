#include "../includes/ServerPoll.hpp"

namespace ft
{
	/* Constructors */
	ServerPoll::ServerPoll(int port)
	{
		std::string tmp = "127.0.0.1";
		AbstractServerApi::Init(tmp, port);
		Init_Serv();
	}

	ServerPoll::ServerPoll(const char *ipaddr, int port)
	{
		std::string tmp = std::string(ipaddr);;
		AbstractServerApi::Init(tmp, port);
		Init_Serv();
	}

	ServerPoll::ServerPoll(std::string& ipaddr, int port)
	{
		AbstractServerApi::Init(ipaddr, port);
		Init_Serv();
	}

	void ServerPoll::Init_Serv()
	{
		_logs << "ServerType: Poll 🌐" << std::endl;

		struct pollfd fd_serv;

		fd_serv.fd = _server_fd;
		fd_serv.events = POLLIN;
		fd_serv.revents = 0;

		_pollfds.push_back(fd_serv);
	}

	/* Start */
	void ServerPoll::Start()
	{
		int result;

		while (1)
		{
			Logger(BLUE, "Wait poll...");
			result = poll(& _pollfds[0], _pollfds.size(), -1);
			Logger(B_GRAY, "Poll return " + std::to_string(result));

			if (result == -1)
			{
				ServerError("Poll");
			}
			else if (result == 0)
			{
				Logger(RED, "TimeOut 🕐");
				continue; ;
			}
			else
			{
				EventsCheck();
			}
		
		}

	}


	void ServerPoll::EventsCheck()
	{
		/* Проверяю на наличие подключений */
		CheckAccept();

		/* Проверяю если что то на чтение */
		CheckRead();
	}


	void ServerPoll::AddFd(int fd)
	{
		Logger(B_GRAY, "Add fd " + std::to_string(fd));
		fcntl(fd, F_SETFL, O_NONBLOCK);
		struct pollfd fd_client;

		fd_client.fd = fd;
		fd_client.events = POLLIN;
		fd_client.revents = 0;

		_pollfds.push_back(fd_client);
	}


	void ServerPoll::CheckAccept()
	{
		Logger(BLUE, "Check Accept...");

		struct sockaddr_in	clientaddr;
		socklen_t 			len;
		int 				client_fd;

		len = sizeof(clientaddr);
		
		if( _pollfds[0].revents == 0)
        	return;
		
		client_fd = accept(_server_fd,(struct sockaddr *)&clientaddr, &len);
		if (client_fd< 0)
		{
			if (errno != EWOULDBLOCK)
				ServerError("Accept");
			return;
		}

		Logger(GREEN, "New connection as fd:(" + std::to_string(client_fd) + ")✅");

		PrintSockaddrInfo(&clientaddr);
		AddFd(client_fd);

	}

	void ServerPoll::CheckRead()
	{
		Logger(BLUE, "Check read...");
		char buffer[BUFFER_SIZE_RECV];

		std::vector<struct pollfd>::iterator	it = _pollfds.begin();
		std::vector<struct pollfd>::iterator	it_end = _pollfds.end();

		while (it != it_end)
		{
			if (it->fd == _server_fd || it->revents  == 0)
			{
				it++;
				continue;
			}

			ReadFd(it->fd);
			it++;
		}

	}
	

	void ServerPoll::ReadFd(int fd)
	{
		Logger(GREEN, "Readble is ready: fd(" + std::to_string(fd) + ") ✅");
		
		char buffer[BUFFER_SIZE_RECV];
		std::string full_msg ="";
		bzero(buffer, BUFFER_SIZE_RECV);
		
		int ret = recv(fd, buffer, BUFFER_SIZE_RECV - 1, 0);
		if (ret == 0)
		{
			Logger(RED, "Disconnect  fd(" + std::to_string(fd) + ") ❌ ");
			RemoteFd(fd);
			return;

		}
		full_msg[ret] = 0;
		full_msg += buffer;
		
		Logger(PURPLE, "Recv read " + std::to_string(ret) + " bytes");
		Logger(B_GRAY, "buf:" + full_msg);
		while (ret == BUFFER_SIZE_RECV - 1)
		{
			ret = recv(fd, buffer, BUFFER_SIZE_RECV - 1, 0);
			if (ret == -1)
				break;
			
			buffer[ret] = 0;
			full_msg += buffer;
			Logger(B_GRAY, "subbuf:" + std::string(buffer));
			Logger(PURPLE, "Replay Recv read " + std::to_string(ret) + " bytes");
		}
		//full_msg.pop_back();

		Logger(GREEN, "Data is read is " + std::to_string(full_msg.size()) + " bytes  ✅");
		Logger(B_GRAY, full_msg);
	}

	void ServerPoll::RemoteFd(int client_fd)
	{
		std::vector<struct pollfd>::iterator it = _pollfds.begin();
		std::vector<struct pollfd>::iterator it_end = _pollfds.end();

		while (it != it_end)
		{
			if (it->fd == client_fd)
			{	
				close(it->fd);
				_pollfds.erase(it);
				return;
			}

			it++;
		}
		
	}

	/* Destructor */
	ServerPoll::~ServerPoll()
	{
		Logger(RED, "Call ServerPoll Destructor❌ ");
	}
	
}