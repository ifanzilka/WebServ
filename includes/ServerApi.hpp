/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerApi.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bmarilli <bmarilli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/14 00:49:56 by bmarilli          #+#    #+#             */
/*   Updated: 2022/04/15 17:51:23 by bmarilli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_API
# define SERVER_API

#include "Color.hpp"

#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>	/* inet_addr inet_htop */
#include <sys/socket.h>	/* socket */
#include <errno.h>		/* errno */
#include <iostream>		/* std cout */


#define SERVER_PROTOCOL 	AF_INET 	//IpV4
#define	SERVER_TYPE			SOCK_STREAM //TCP
#define MAX_CONNECT_LISTEN 	15			//In Listen
#define	BUFFER_SIZE			1024		//In Read Buffer


namespace ft
{
	class AbstractServerApi
	{
	public:
	
		virtual	void 	Start() = 0;
		
    	
	protected:
		/* Настройка моей сети */
		struct sockaddr_in 	_servaddr;
		std::string 		_ipaddr;
		int 				_port;
		int 				_server_fd;

		/* Говорю что можно переопределить*/
		virtual	void	Init(std::string& ipaddr, int port);
		virtual int 	Create_socket();
		virtual int 	Binded();
    	virtual int 	Listen();

		/* Functional */
		virtual void	AddFd(int fd) = 0;
		virtual void	RemoteFd(int fd) = 0;

		/* Print Errno */
		virtual	void	ServerError(const char *s);
	
	private:
		void 			PrintIpPort();

	};

}

#endif