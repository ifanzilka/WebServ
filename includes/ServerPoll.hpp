#ifndef SERVER_POLL
#define SERVER_POLL

namespace ft
{
	class ServerPoll
	{
	public:

		/* Constructor */
		ServerPoll(int port);

		~ServerPoll();
		
	private:
		/* Настройка моей сети */
		struct sockaddr_in 	_servaddr;
		std::string 		_ipaddr;
		int 				_port;
		int 				_server_fd;





	};
}

#endif