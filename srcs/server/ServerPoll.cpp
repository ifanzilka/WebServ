#include <Include_Library.hpp>
//#include "ServerPoll.hpp"

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
		_logs << "ServerType: Poll 🌐 " << std::endl;

		struct pollfd fd_serv;

		fd_serv.fd = _server_fd;
		fd_serv.events = POLLIN;
		fd_serv.revents = 0;

		_pollfds.push_back(fd_serv);
	}

	/* Start */
	int ServerPoll::WaitEvent()
	{
		int result;

		Logger(BLUE, "Wait Event...");
		result = poll(& _pollfds[0], _pollfds.size(), -1);
		Logger(B_GRAY, "Poll return " + std::to_string(result));

		if (result == -1)
		{
			ServerError("Poll");
		}
		else if (result == 0)
		{
			Logger(RED, "TimeOut 🕐 ");
		}
		return result;
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

	int ServerPoll::CheckAccept()
	{
		Logger(BLUE, "Check Accept...");
		int 				client_fd;

		if( _pollfds[0].revents == 0)
        	return (0);
		
		client_fd = Accept();
		if (client_fd < 0)
		{
			if (errno != EWOULDBLOCK)
				ServerError("Accept");
			return -1;
		}
		AddFd(client_fd);
		return (client_fd);
	}

	int ServerPoll::CheckRead()
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

			return(it->fd); 
			//ReadFd(it->fd);
			it++;
		}
		return (0);
	}
	
	//TODO: вынести этот метод в абстрактный класс, ибо реализация везде одинаковая
	int ServerPoll::ReadFd(int client_fd)
	{
		Logger(GREEN, "Readble is ready: fd(" + std::to_string(client_fd) + ") ✅ ");
		
		char buffer[BUFFER_SIZE_RECV];
		bzero(buffer, BUFFER_SIZE_RECV);
		
		int ret = recv(client_fd, buffer, BUFFER_SIZE_RECV - 1, 0);
		if (ret == 0)
		{
			Logger(RED, "Disconnect fd(" + std::to_string(client_fd) + ") ❌ ");
			RemoteFd(client_fd);
			return (0);
		}

		_client_rqst_msg.resize(0);
		_client_rqst_msg += buffer;
		
		Logger(PURPLE, "Recv read " + std::to_string(ret) + " bytes");
		Logger(B_GRAY, "buff:" + _client_rqst_msg);
		while (ret == BUFFER_SIZE_RECV - 1)
		{
			ret = recv(client_fd, buffer, BUFFER_SIZE_RECV - 1, 0);
			if (ret == -1)
				break;
			
			buffer[ret] = 0;
			_client_rqst_msg += buffer;
			Logger(B_GRAY, "subbuf:" + std::string(buffer));
			Logger(PURPLE, "Replay Recv read " + std::to_string(ret) + " bytes");
		}
		// _client_rqst_msg.pop_back();

		Logger(GREEN, "Data is read is " + std::to_string(_client_rqst_msg.size()) + " bytes  ✅ ");
		Logger(B_GRAY, _client_rqst_msg);

		return (_client_rqst_msg.size());
	}

	std::string ServerPoll::GetClientRequest() const
	{
		return (_client_rqst_msg);
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
		//TODO: закрытие сокета
		Logger(RED, "Call ServerPoll Destructor ❌ ");
	}
	
}