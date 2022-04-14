#include "../includes/ServerSelect.hpp"

namespace ft
{

	void ServerSelect::PrintIpPort()
	{
		std::cout << PURPLE;
		std::cout << "Ip adress: " << _ipaddr << "\n";
		std::cout << "Port: " << _port << F_NONE << "\n";
	}

	/* Constructors */

	ServerSelect::ServerSelect(int port)
	{
		_ipaddr = "127.0.0.1";
		_port = port;
		PrintIpPort();
		Init();
	}


	ServerSelect::ServerSelect(std::string& ipaddr, int port)
	{
		_ipaddr = ipaddr;
		_port = port;
		PrintIpPort();
		Init();
	}

	ServerSelect::ServerSelect(const char *ipaddr, int port)
	{
		_ipaddr = std::string(ipaddr);
		_port = port;
		PrintIpPort();
		Init();
	}

	void ServerSelect::Init()
	{
		/*
			AF_UNIX, AF_LOCAL- Местная связь
			AF_INET- Интернет-протоколы IPv4
			AF_INET6- Интернет-протоколы IPv6
			AF_IPX- протоколы IPX Novell
		*/
		bzero(&_servaddr, sizeof(_servaddr));

		_servaddr.sin_family = SERVER_PROTOCOL;
		
		/* Port host to network short  */
		_servaddr.sin_port = htons(_port);
		/* преобразовать номер порта из порядка байтов хоста */
		
		/* Ip */
		_servaddr.sin_addr.s_addr = inet_addr(_ipaddr.c_str());//127.0.0.1 or htonl(2130706433);

		/* Создаю сокет */
		Create_socket();

		/* Связываю его с ардесом и портом*/
		Binded();

		/* Делаю сокет прослушивающим */
		Listen();
		
	}


	int ServerSelect::Create_socket()
	{
		/**
            int	socket(int domain, int type, int protocol);
            (0) domain:
            AF_UNIX, AF_LOCAL- Местная связь
            AF_INET- Интернет-протоколы IPv4
            AF_INET6- Интернет-протоколы IPv6
            AF_IPX- протоколы IPX Novell

            (1) type:
            typeуказывает, будет ли связь бесконтактной или постоянной.
            Не все typesсовместимы со всеми domains. Некоторые примеры:

            SOCK_STREAM- Двусторонняя надежная связь (TCP)
            SOCK_DGRAM- Без установления соединения, ненадежный (UDP)

            (2) protocol:
            Обычно для каждого protocol'a доступно только одно значение type,
            поэтому можно использовать значение '0'.
	    */

	   //_server_fd = socket(_servaddr.sin_family, SOCK_STREAM, 0);
	   // SERVER_TYPE macro SOCK_STREAM
	   _server_fd = socket(_servaddr.sin_family, SERVER_TYPE, 0);
	   //_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	   if (_server_fd < 0)
	   {	
		   ServerError("socket");
		   throw "Error in create socket\n";
		}
		_max_fd = _server_fd;
		
		std::cout << GREEN << "Success create socket fd(" <<  _server_fd << ") ✅" << F_NONE << "\n"; 
		return (_server_fd);
	}

	int ServerSelect::Binded()
	{
		/**
        
        Когда у нас есть сокет, нам нужно использовать привязку , чтобы назначить IP-адрес и порт сокету
        Создаю имя для сокета (потому нужен unlink )
        
        int bind(int sockfd, const sockaddr *addr, socklen_t addrlen);
        (0) sockfd:
        sockfd относится к файловому дескриптору, которому мы хотим присвоить адрес.
        Для нас это будет файловый дескриптор, возвращаемый socket().

        (1) addr:
        addr - структура, используемая для указания адреса, который мы хотим присвоить сокету.
        Точная структура, которую необходимо использовать для определения адреса, зависит от протокола.
        Поскольку мы собираемся использовать IP для этого сервера, мы будем использовать sockaddr_in

        (2) addrlen
        addrlen - просто size() из addr.
	    
        */
        int bind_ = bind(_server_fd, (const struct sockaddr *)&_servaddr, sizeof(_servaddr));
        if (bind_ < 0)
        {
            ServerError("bind");
		   	throw "Binded\n";
        }
		std::cout << GREEN << "Success bind socket ✅" << F_NONE << "\n"; 
		return bind_;
	}

	int ServerSelect::Listen()
	{
		int _listen;


		/*
            listen помечает сокет как пассивный. т.е. сокет будет использоваться для приема соединений.
            
            Функция listen используется сервером, чтобы информировать ОС, что он ожидает ("слушает") запросы связи на данном сокете
            
            int listen(int socket_fd, int backlog);
                sockfd - файловый дескриптор сокета.

                backlog - максимальное количество подключений, которые будут поставлены в очередь,
                прежде чем в подключениях будет отказано.
        */
        
        _listen = listen(_server_fd, MAX_CONNECT_LISTEN);

        if (_listen < 0)
        {
			ServerError("listen");
			throw "listen bad\n";
        }

		std::cout << GREEN << "Success listenen socket ✅" << F_NONE << "\n";
		return (_listen);
	}

	void ServerSelect::Start()
	{
		struct timeval 	time;
		int 			_select;

		time.tv_sec = 0;
		time.tv_usec = 0;


		/* Очищаю множество */
		FD_ZERO(&_currfds);
		/* Добавил во множество */
		FD_SET(_server_fd, &_currfds);

		while (1)
		{
			/* Множества приравниваю */
			_writefds = _readfds = _currfds;


			std::cout << BLUE << "Wait select..." << F_NONE << std::endl;
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
					ServerError("select");
					throw "SelectError";
				}
			}
			else if (_select == 0)
			{
				std::cout << RED << "TimeOut" << F_NONE << std::endl;
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

	void ServerSelect::CheckListen()
	{
		struct sockaddr_in	clientaddr;
		socklen_t 			len;
		int 				client_fd;

		len = sizeof(clientaddr);
		std::cout << BLUE"Check Listen"F_NONE"\n";
		
		/* Если пришло событие на connect */
		if (FD_ISSET(_server_fd, &_readfds))
		{
			client_fd = accept(_server_fd,(struct sockaddr *)&clientaddr, &len);
			if (client_fd == -1)
			{
				ServerError("accept");
				throw "AcceptError";
			}
			printf(GREEN"New connection fd:%d✅"F_NONE"\n", client_fd);
			PrintClintInfo(&clientaddr);
			
			/* Добавляю во множество */
			FD_SET(client_fd, &_currfds);
			_max_fd = client_fd > _max_fd ? client_fd : _max_fd;

			//TO_DO Добавить в массив информацию о клиенте 
		}
	}

	void ServerSelect::CheckRead()
	{
		ssize_t		size;
		ssize_t		ret;
		char		buffer[1024];
		int i;

		i = 0;

		std::cout << BLUE"Check read"F_NONE"\n";
		/* Проверяю дескрипторы на то что пришло ли что то чтение */
		while (i <= _max_fd)
		{
			/* Сокет для прослушки скипаю */
			if (i == _server_fd)
			{	
				i++;
				continue;
			}

			/* message receives from curr_cli */
			if (FD_ISSET(i, &_readfds))
			{
				std::cout << GREEN << "Listen signal fd(" << i << ") ✅" << F_NONE << "\n"; 
				ret = recv(i, buffer, 1024 - 1, 0);
				if (ret == 0)
				{	
					std::cout << RED << "Disconnect fd(" <<  i << ") ❌"F_NONE << std::endl;

					/* Удаляю из множества */
					FD_CLR(i, &_currfds);
				}
				else
				{
					printf("Listen msg in fd(%d)\n", i);
					buffer[ret] = 0;
					write(1, buffer, ret);
					
					/* Отравляю в ответ то что все хорошо пришло */
					send(i, "Succsec send\n", strlen("Succsec send\n"), 0);
				}
			}
			i++;
		}
	}

	void ServerSelect::CheckWrite()
	{
		int i;

		i = 0;
		std::cout << BLUE"Check Write"F_NONE"\n"; //когда нажал ентер
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
				write(i, "I am write for you\n", strlen("I am write for you\n"));
				//while (1)
				//	;
			}

			i++;
		}
	}

	void ServerSelect::PrintClintInfo(struct sockaddr_in *info)
	{
		char ip4[INET_ADDRSTRLEN]; // место для строки IPv4
		int port;
	
		port =  ntohs(info->sin_port); 
		//заполнли ip
		inet_ntop(AF_INET, &(info->sin_addr), ip4, INET_ADDRSTRLEN);
		printf(PURPLE"The IPv4 address is: %s:%d"F_NONE"\n", ip4, port);    	
	}

	void ServerSelect::ServerError(const char *s)
	{
		std::cout << RED << std::endl;
		perror(s);
		std::cout << F_NONE << "\n";
		exit(42);
	}

	ServerSelect::~ServerSelect()
	{
		std::cout << RED;
		std::cout << "Call (ServerSelect) Destructor\n";
		std::cout << F_NONE;
	}

}


