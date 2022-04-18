#include <Include_Library.hpp>
//#include <ServerSelect.hpp>

namespace ft
{

	/**
	==============================================
	==        Constructors and Destructor       ==
	==============================================
	*/

	void ServerSelect::Init_Serv()
	{
		_logs << "ServerType: Select 🌐" << std::endl;

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

			
			Logger(BLUE, "Wait select...");
			/* Останавливаю процесс для отловки событий */
			_select = select(_max_fd + 1, &_readfds, NULL, NULL, NULL);
			 
			Logger(B_GRAY, "Select signal is " + std::to_string(_select));
		
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
		CheckAccept();

		/* Проверяю дескрипторы на чтение */
		CheckRead();

		/* Проверяю на запись */
		//CheckWrite();

	}

	void ServerSelect::AddFd(int fd)
	{
		Logger(B_GRAY, "Add fd " + std::to_string(fd));
		fcntl(fd, F_SETFL, O_NONBLOCK);
		
		/* Добавляю во множество */
		FD_SET(fd, &_currfds);

		_clients_fd.insert(std::make_pair(fd, _id));
		_id++;

		//for select
		_max_fd = fd > _max_fd ? fd : _max_fd;
	}

	void ServerSelect::RemoteFd(int fd)
	{
		Logger(B_GRAY, "Remote fd " + std::to_string(fd));
		
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

	void ServerSelect::CheckAccept()
	{
		Logger(BLUE, "Check Accept...");

		int	client_fd;

		/* Если пришло событие на connect */
		if (FD_ISSET(_server_fd, &_readfds))
		{
			client_fd = Accept();
			if (client_fd > 0)
			{
				AddFd(client_fd);
			}//TODO Добавить в массив информацию о клиенте
		}
	}

	void ServerSelect::CheckRead()
	{
		Logger(BLUE, "Check read...");

		std::map<int, int>::iterator	it_begin;
		std::map<int, int>::iterator	it_end;


		it_begin = _clients_fd.begin();
		it_end = _clients_fd.end();

		
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

	void ServerSelect::ReadFd(int fd)
	{
		Logger(GREEN, "Readble is ready: fd(" + std::to_string(fd) + ") ✅");
		
		char buffer[BUFFER_SIZE_RECV];
		std::string full_msg ="";
		bzero(buffer, BUFFER_SIZE_RECV);
		
		int ret = recv(fd, buffer, BUFFER_SIZE_RECV - 1, 0);
		if (ret == 0)
		{
			PrintAllClients();
			Logger(RED, "Disconnect  fd(" + std::to_string(fd) + ") ❌ ");
			RemoteFd(fd);
			PrintAllClients();
			return;

		}
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
		send(fd, "Message has send successfully\n", strlen("Message has send successfully\n"), 0);
	}


	/* Destructor */
	ServerSelect::~ServerSelect()
	{
		Logger(RED, "Call ServerSelect Destructor❌ ");
	}

}