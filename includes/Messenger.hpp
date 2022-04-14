#ifndef HTTPPARSER_HPP
#define HTTPPARSER_HPP

#include <unistd.h>
#include <iostream>
#include "Color.hpp"
#include <string>

namespace ft
{
	//TODO: определиться сколько мессенджеров создавать
	class Messenger
	{
		public:
			Messenger();
			Messenger(const int client_fd);
			~Messenger();
//			int SetClientFd(const int client_fd);
			void GetRequest(const int client_fd);
		private:
			const int	_client_fd;
			void	SendResponse(const int client_fd);
	};
}

#endif
