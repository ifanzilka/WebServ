#include <Include_Library.hpp>
//#include "ServerKqueue.hpp"

namespace ft
{

	/* Constructor */
	ServerKqueue::ServerKqueue(int port)
	{
		std::string tmp = "127.0.0.1";
		AbstractServerApi::Init(tmp, port);
		Init_Serv();
	}

	ServerKqueue::ServerKqueue(const char *ipaddr, int port)
	{
		std::string tmp = std::string(ipaddr);;
		AbstractServerApi::Init(tmp, port);
		Init_Serv();
	}

	ServerKqueue::ServerKqueue(std::string ipaddr,int port)
	{
		AbstractServerApi::Init(ipaddr, port);
		Init_Serv();
	}

	/* Init */
	void 	ServerKqueue::Init_Serv()
	{
		_logs << "ServerType: Kqueue 🌐" << std::endl;

		
		int res_kevent;
		
		_kq_fd = kqueue();
		if (_kq_fd == -1)
			ServerError("Kqueue");

		/* Добавляю событие прослушки fd сервера */
		/* Задаем события для отслеживания */
    	EV_SET(&evSet, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE , 0, 0, NULL);
	    
		res_kevent = kevent(_kq_fd, &evSet, 1, NULL, 0, NULL);
		if (res_kevent == -1)
			ServerError("Kevent");
		bzero(&evSet,sizeof(evSet));

	}

	void ServerKqueue::Start()
	{
		struct kevent 	evList[KQUEUE_SIZE];
		int	 			new_events;
		int				client_fd;


		while (1)
		{	
			Logger(BLUE, "Wait kevent...");
			new_events = kevent(_kq_fd, NULL, 0, evList, KQUEUE_SIZE, NULL);
			if (new_events == -1)
				ServerError("kevent");

			Logger(B_GRAY, "kevent return " + std::to_string(new_events));


			for (int i = 0; i < new_events; i++)
			{
				int event_fd = evList[i].ident;

				// When the client disconnects an EOF is sent. By closing the file
				// descriptor the event is automatically removed from the kqueue.
				if (evList[i].flags & EV_EOF)
				{
					Logger(RED, "Disconnect  fd(" + std::to_string(event_fd) + ") ❌ ");
					close(event_fd);
				}
				else if (event_fd == _server_fd)
            	{
					CheckAccept();
				}
				else if (evList[i].filter & EVFILT_READ)
				{
					ReadFd(event_fd);
				}
				else if (evList[i].filter == EVFILT_WRITE)
				{
					//to do
				}

			}
		}
	}

	void ServerKqueue::CheckAccept()
	{
		Logger(BLUE, "CheckAccept...");

		int 				client_fd;
		

		client_fd = Accept();
		if (client_fd == -1)
		{
			ServerError("CheckAccept");
			return ;
		}
		/* Добавляю нового клиента в аулл фд*/
		EV_SET(&evSet, client_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(_kq_fd, &evSet, 1, NULL, 0, NULL) < 0)
		{
			ServerError("kevent");
		}
		int byte_wrote = 0;
		std::string	header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");
		std::string body = "<iframe width=\"1200\" height=\"800\"\nsrc=https://www.youtube.com/embed/C3LQfH-YGKM?start=4>\n</iframe>";
		header += body.length();
		header += "\n\n";
		byte_wrote = write(client_fd, header.c_str(), header.size());
		byte_wrote = write(client_fd, body.c_str(), body.length());
		AddFd(client_fd);
	}

	void ServerKqueue::AddFd(int fd)
	{
		Logger(B_GRAY, "Add fd " + std::to_string(fd));
		fcntl(fd, F_SETFL, O_NONBLOCK);

	}	

	void ServerKqueue::ReadFd(int fd)
	{
		Logger(GREEN, "Readble is ready: fd(" + std::to_string(fd) + ") ✅");
		
		char buffer[BUFFER_SIZE_RECV];
		std::string full_msg ="";
		bzero(buffer, BUFFER_SIZE_RECV);
		
		int ret = recv(fd, buffer, BUFFER_SIZE_RECV - 1, 0);
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

		// char *msg = "HTTP/1.1 200 OK \n\n";
		// int res =send(fd, msg, strlen(msg), 0);
		// printf("res send: %d\n", res);

		// int byte_wrote = 0;
		// std::string	header("HTTP/1.1 200 OK\nContent-Type: text\nContent-Length: ");
		// std::string body = "<iframe width=\"1200\" height=\"800\"\nsrc=https://www.youtube.com/embed/C3LQfH-YGKM?start=4>\n</iframe>";
		// header += body.length();
		// header += "\n\n";
		// byte_wrote = write(fd, header.c_str(), header.size());
		// byte_wrote = write(fd, body.c_str(), body.length());
	}

	/* Destrcutor */
	ServerKqueue::~ServerKqueue()
	{
		close(_kq_fd);
		Logger(RED, "Call ServerKqueue Destructor ❌ ");
	}

}