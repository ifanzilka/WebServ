/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerApi.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ifanzilka <ifanzilka@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 00:49:56 by bmarilli          #+#    #+#             */
/*   Updated: 2022/04/18 02:09:32 by ifanzilka        ###   ########.fr       */
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

#define SERVER_PROTOCOL 	AF_INET 	//IpV4
#define	SERVER_TYPE			SOCK_STREAM //TCP
#define MAX_CONNECT_LISTEN 	15			//In Listen
#define	BUFFER_SIZE_RECV	2			//In Read Buffer !!!!( > 2)
#define	BUFFER_SIZE_SEND	2			//

#define LOGGER_ENABLE		0			//1 - ON, 0 - OFF
				

namespace ft
{
	class AbstractServerApi
	{
	public:
	
		virtual	void 	Start() = 0;
		
    	static void 	PrintSockaddrInfo(struct sockaddr_in *info);
	protected:
		/* Настройка моей сети */
		struct sockaddr_in 	_servaddr;
		std::string 		_ipaddr;
		std::ofstream 		_logs;
		int 				_port;
		int 				_server_fd;
		int					_fd_log_file;
		
		/*  Подключенные клиенты и их информация */
		std::map<int, struct sockaddr_in>	_clients;
		std::map<int, int>					_fd_id;
		int									_max_id;

		/* Говорю что можно переопределить*/
		virtual	void	Init(std::string& ipaddr, int port);
		virtual int 	Create_socket();
		virtual int 	Binded();
    	virtual int 	Listen();
		virtual	int 	Accept();

		/* Functional */
		void AddClientInfo(int fd, struct sockaddr_in client);
		
		//virtual void	AddFd(int fd) = 0;
		//virtual void	RemoteFd(int fd) = 0;

		/* Print Errno */
		virtual	void	ServerError(const char *s);
		void 			Logger(std::string msg);
		void			Logger(std::string color,std::string msg);


		~AbstractServerApi();

	private:
		void 			PrintIpPort();

	};

}

#endif