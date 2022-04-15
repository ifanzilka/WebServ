#include "../includes/ServerApi.hpp"

namespace ft
{
	void AbstractServerApi::Init(std::string& ipaddr, int port)
	{
		_ipaddr = ipaddr;
		_port = port;
		PrintIpPort();

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
		//TODO: check; Must be: server_info.sin_addr.s_addr = INADDR_ANY;
		//_servaddr.sin_addr.s_addr = inet_addr(_ipaddr.c_str()); //127.0.0.1 or htonl(2130706433);
		_servaddr.sin_addr.s_addr = INADDR_ANY;//0.0.0.0
		/* Создаю сокет */
		Create_socket();

		/* Связываю его с адресом и портом*/
		Binded();

		/* Делаю сокет прослушивающим */
		Listen();
	}

	int AbstractServerApi::Create_socket()
	{
		/**
			int	socket(int domain, int type, int protocol);
			(0) domain:
			AF_UNIX, AF_LOCAL- Местная связь
			AF_INET- Интернет-протоколы IPv4
			AF_INET6- Интернет-протоколы IPv6
			AF_IPX- протоколы IPX Novell

			(1) type:
			type указывает, будет ли связь бесконтактной или постоянной.
			Не все types совместимы со всеми domains. Некоторые примеры:

			SOCK_STREAM- Двусторонняя надежная связь (TCP)
			SOCK_DGRAM- Без установления соединения, ненадежный (UDP)

			(2) protocol:
			Обычно для каждого protocol'a доступно только одно значение type,
			поэтому можно использовать значение '0'.
		*/

		_server_fd = socket(_servaddr.sin_family, SERVER_TYPE, 0);
		//_server_fd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (_server_fd < 0)
			ServerError("Socket()");
	
		
		std::cout << GREEN << "Socket fd(" <<  _server_fd << ") successfully created ✅ " << NORM << "\n";
		return (_server_fd);
	}

	int AbstractServerApi::Binded()
	{
		int yes = 1;
		if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
			ServerError("Setsokport");

		/**

        Когда у нас есть сокет, нам нужно использовать привязку, чтобы назначить IP-адрес и порт сокету
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
            ServerError("Bind: ");

		std::cout << GREEN << "Success bind socket ✅ " << NORM << "\n";
		return bind_;
	}

	int AbstractServerApi::Listen()
	{
		int _listen;

		/**
            listen помечает сокет как пассивный. т.е. сокет будет использоваться для приема соединений.
            
            Функция listen используется сервером, чтобы информировать ОС,
            что он ожидает ("слушает") запросы связи на данном сокете
            
            int listen(int socket_fd, int backlog);
                sockfd - файловый дескриптор сокета.

                backlog - максимальное количество подключений, которые будут поставлены в очередь,
                прежде чем в подключениях будет отказано.
        */
        
        _listen = listen(_server_fd, MAX_CONNECT_LISTEN);

        if (_listen < 0)
			ServerError("listen");

		std::cout << GREEN << "Server is listening " << MAX_CONNECT_LISTEN << " connect ✅ " << NORM << "\n";
		return (_listen);
	}

	void AbstractServerApi::PrintIpPort()
	{
		std::cout << PURPLE;
		std::cout << "Ip address: " << _ipaddr << "\n";
		std::cout << "Port: " << _port << NORM << "\n";
	}


	void AbstractServerApi::ServerError(const char *s)
	{
		/* Смотрим ошибку из errno */
		char *str_error =  strerror(errno);
		std::string		err(str_error);	
		std::string 	error_type(s);
		
		std::string 	full = "";

		/* Example: select: Bad decriptor */
		full += error_type;
		full += ": ";
		full += err;

		std::cerr << RED << full << NORM << "\n";
		//throw std::runtime_error(full);
		exit(42);
	}

}