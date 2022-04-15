#include "../includes/ServerSelect.hpp"

namespace ft
{

	/**
	==============================================
	==        Constructors and Destructor       ==
	==============================================
	*/

	void ServerSelect::Init_Serv()
	{
		_id = 0;
		_max_fd = _server_fd;
	}

	ServerSelect::ServerSelect(int port)
	{
		std::string tmp = "127.0.0.1";
		AbstractServerApi::Init(tmp, port);
		Init_Serv();
	}

	ServerSelect::ServerSelect(std::string& ipaddr, int port)
	{
		AbstractServerApi::Init(ipaddr, port);
		Init_Serv();
	}

	ServerSelect::ServerSelect(const char *ipaddr, int port)
	{
		std::string tmp = std::string(ipaddr);;
		AbstractServerApi::Init(tmp, port);
		Init_Serv();
	}

	ServerSelect::~ServerSelect()
	{
		std::cout << RED"Called (ServerSelect) Destructor"NORM << std::endl;
	}

	/**
	==============================================
	==               StartUp methods            ==
	==============================================
	*/

	void ServerSelect::Start()
	{
		struct timeval 	time;
		int 			_select;

		time.tv_sec = 0;
		time.tv_usec = 0;

		/* Чищу множество */
		FD_ZERO(&_currfds);
		/* Добавил во множество */
		FD_SET(_server_fd, &_currfds);

		while (1)
		{
			/* Множества приравниваю */
			_writefds = _readfds = _currfds;

			std::cout << BLUE << "Wait select..." << NORM << std::endl;
			
			/* Останавливаю процесс для отловки событий */
			//_select = select(_max_fd + 1, &_readfds, &_writefds, NULL, NULL);
			_select = select(_max_fd + 1, &_readfds, NULL, NULL, NULL);
			
			std::cout << "Select signal is " << _select << std::endl;
			if (_select == -1)
			{
				if (errno == EINTR)
				{	/* Нас прервал сигнал*/
					continue;
				}
				else
				{
					AbstractServerApi::ServerError("Select");
				}
			}
			else if (_select == 0)
			{
				std::cout << RED << "TimeOut" << NORM << std::endl;
				/* Fun */
				return;
			}
			else
			{
				/* Check Event Select */
				EventsCheck();
			}
		}	
			
	}


	void ServerSelect::EventsCheck()
	{
		/* Проверяю пришло ли кто на прослушку */
		CheckListen();
	
		/* Проверяю дескрипторы на чтение */
		CheckRead();

		/* Проверяю на запись */
		//CheckWrite();

	}

	void ServerSelect::AddFd(int fd)
	{
		/* Добавляю во множество */
		FD_SET(fd, &_currfds);

		_clients_fd.insert(std::make_pair(fd, _id));
		_id++;

		//for select
		_max_fd = fd > _max_fd ? fd : _max_fd;
	}

	void ServerSelect::RemoteFd(int fd)
	{
		std::map<int,int>::iterator		tmp;
		
		/* Удаляю из множества */
		FD_CLR(fd, &_currfds);

		tmp = _clients_fd.find(fd);
		if (tmp != _clients_fd.end())
		{
			_clients_fd.erase(tmp);
		}
	}

	void ServerSelect::PrintAllClients()
	{
		std::map<int, int>::iterator i;
		std::map<int, int>::iterator end;


		if (_clients_fd.size() == 0)

		std::cout << PURPLE << "Clients List: "NORM << std::endl;

		if (_clients_fd.size() == 0)
		{
			std::cout << "Empty\n";
			return ;
		}

		i = _clients_fd.begin();
		end = _clients_fd.end();
		while (i != end)
		{
			std::cout << "Fd: " << (*i).first << " Id: " << (*i).second << std::endl;
			i++;
		}

	}

	void ServerSelect::CheckListen()
	{
		struct sockaddr_in	clientaddr;
		socklen_t 			len;
		int 				client_fd;

		len = sizeof(clientaddr);
		std::cout << BLUE"Check Listen"NORM"\n";
		
		/* Если пришло событие на connect */
		if (FD_ISSET(_server_fd, &_readfds))
		{
			client_fd = accept(_server_fd,(struct sockaddr *)&clientaddr, &len);
			if (client_fd == -1)
				AbstractServerApi::ServerError("Accept");


			printf(GREEN"New connection fd:%d✅ "NORM"\n", client_fd);
			PrintClientInfo(&clientaddr);
			
			AddFd(client_fd);
			
			//TODO Добавить в массив информацию о клиенте
		}
	}

	void ServerSelect::CheckRead()
	{		
		std::map<int, int>::iterator	it_begin;
		std::map<int, int>::iterator	it_end;


		it_begin = _clients_fd.begin();
		it_end = _clients_fd.end();

		std::cout << BLUE"Check read"NORM << std::endl;
		
		/* Проверяю дескрипторы на то что пришло ли что то чтение */
		while (it_begin != it_end)
		{
			/* message receives from curr_cli */
			if (FD_ISSET((*it_begin).first, &_readfds))
			{
				ReadFd((*it_begin).first);
				return;
			}
			it_begin++;
		}
	}

	void ServerSelect::ReadFd(int clinet_fd)
	{
		ssize_t		ret;
		char		buffer[BUFFER_SIZE];

		std::cout << GREEN << "Listen signal fd(" << clinet_fd << ") ✅ " << NORM << "\n";
		ret = recv(clinet_fd, buffer, BUFFER_SIZE - 1, 0);

		if (ret == 0)
		{	
			PrintAllClients();
			std::cout << RED << "Disconnect fd(" <<  clinet_fd << ") ❌ " << NORM << std::endl;
			
			RemoteFd(clinet_fd);
			PrintAllClients();

		}
		else
		{

			printf("Listen msg in fd(%d)\n", clinet_fd);	
			buffer[ret] = 0;
			write(1, buffer, ret);

			/* Отправляю в ответ то что все хорошо пришло */
			send(clinet_fd, "Message has send successfully\n", strlen("Message has send successfully\n"), 0);

			//HttpParser
		}
	}

	void ServerSelect::CheckWrite()
	{
		int i;

		i = 0;
		std::cout << BLUE"Check Write"NORM << std::endl; //когда нажал ентер
		while (i <= _max_fd)
		{
			/* Сокет для прослушки скипаю */
			if (i == _server_fd)
			{	
				i++;
				continue;
			}
			
			if (FD_ISSET(i, &_readfds))
			{
				std::cout << "Write wait its: " << i << std::endl;
				write(i, "I am writing for you\n", strlen("I am writing for you\n"));
				//while (1)
				//	;
			}
			i++;
		}
	}

	void ServerSelect::PrintClientInfo(struct sockaddr_in *info)
	{
		char ip4[INET_ADDRSTRLEN]; // место для строки IPv4
		int port;
	
		port =  ntohs(info->sin_port); 
		//заполнили ip
		inet_ntop(AF_INET, &(info->sin_addr), ip4, INET_ADDRSTRLEN);
		printf(PURPLE"New client IPv4 address is: %s:%d"NORM"\n", ip4, port);
	}
}


