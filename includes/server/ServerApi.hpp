/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerApi.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ifanzilka <ifanzilka@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 00:49:56 by bmarilli          #+#    #+#             */
/*   Updated: 2022/04/22 04:16:25 by ifanzilka        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_API
# define SERVER_API

#include "Color.hpp"

#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>	/* inet_addr inet_htop */
#include <sys/socket.h>	/* socket */
#include <errno.h>		/* errno */
#include <fcntl.h>		/* open */
#include <unistd.h>		/* write */
#include <fcntl.h>		/* fcntl */

#include <fstream>
#include <iostream>		/* std cout */
#include <ctime>		/* time */
#include <map>			/* map */
#include <vector>		/* vector */

#define SERVER_PROTOCOL 	AF_INET 	//IpV4
#define	SERVER_TYPE			SOCK_STREAM //TCP
#define MAX_CONNECT_LISTEN 	15			//In Listen
#define	BUFFER_SIZE_RECV	4096		//In Read Buffer !!!!( > 2)
#define	BUFFER_SIZE_SEND	2			//

#define LOGGER_ENABLE		0			//1 - ON, 0 - OFF


namespace ft
{
	class Client
	{
		public:
			Client(int fd_client, sockaddr_in 	addrinfo_client)
			{
				fd = fd_client;
				addrinfo = addrinfo_client;
			};

			int getFd() const { return (fd); }
			struct sockaddr_in getAddrInfo() const { return (addrinfo); }
		private:
			struct sockaddr_in 	addrinfo;
			int					fd;
	};

	class AbstractServerApi
	{
		public:

			virtual	int			WaitEvent() = 0;
			virtual int			CheckAccept() = 0;
			virtual	int 		CheckRead() = 0;
			//virtual int		CheckWrite() = 0;
			virtual	int			ReadFd(int fd);

			virtual std::string	GetClientRequest() const;

			std::string			GetHostName();
			int					GetPort();

			static void 		PrintSockaddrInfo(struct sockaddr_in *info);

			/* Destructor */
			virtual				 ~AbstractServerApi();

		protected:
			/* Настройка моей сети */
			struct sockaddr_in 	_servaddr;
			std::string 		_ipaddr;
			std::ofstream 		_logs;
			int 				_port;
			int 				_server_fd;
			int					_fd_log_file;

			/*  Подключенные клиенты и их информация */
			std::vector<class ft::Client>		_clients;
			std::string							_client_rqst_msg;

			/* Говорю что можно переопределить*/
			virtual	void		Init(std::string& ipaddr, int port);
			virtual int 		Create_socket();
			virtual int 		Binded();
			virtual int 		Listen();
			virtual	int 		Accept();

			void				AddClient(int fd, struct sockaddr_in addrclient);
			void				RemoteClient(int fd);

			/* Print Errno */
			virtual	void	ServerError(const char *s);
			void 			Logger(std::string msg);
			void			Logger(std::string color,std::string msg);
		private:
			void 			PrintIpPort();

	};

}

#endif