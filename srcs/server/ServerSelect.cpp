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
		_logs << "ServerType: Select 🌐 " << std::endl;

		_id = 0;
		_max_fd = _server_fd;
		
		/* Чищу множество */
		FD_ZERO(&_currfds);
		/* Добавил во множество */
		FD_SET(_server_fd, &_currfds);
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

	int	ServerSelect::WaitEvent()
	{
		struct timeval 	time;
		int 			_select;

		time.tv_sec = 0;
		time.tv_usec = 0;

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
				return (-1);
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
			return (0);
		}
		return (_select);
	}

	int	ServerSelect::CheckRead()
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

				//ReadFd((*it_begin).first);
				return ((*it_begin).first);
			}
			it_begin++;
		}

		return (0);
	}

	/* Проверяю событие на Подключение если полюкчился возвращаю fd клиента иначе 0*/
	int	ServerSelect::CheckAccept()
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
			}
			return (client_fd);
		}
		return (0);
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

	//TODO: вынести этот метод в абстрактный класс, ибо реализация везде одинаковая
	int ServerSelect::ReadFd(int fd)
	{
		Logger(GREEN, "Readble is ready: fd(" + std::to_string(fd) + ") ✅ ");
		
		char buffer[BUFFER_SIZE_RECV];
		bzero(buffer, BUFFER_SIZE_RECV);
		
		int ret = recv(fd, buffer, BUFFER_SIZE_RECV - 1, 0);
		if (ret == 0)
		{
			PrintAllClients();
			Logger(RED, "Disconnect  fd(" + std::to_string(fd) + ") ❌ ");
			RemoteFd(fd);
			PrintAllClients();
			return (0);

		}
		
		_client_rqst_msg.resize(0);
		_client_rqst_msg += buffer;
		
		Logger(PURPLE, "Recv read " + std::to_string(ret) + " bytes");
		Logger(B_GRAY, "buf:" + _client_rqst_msg);
		while (ret == BUFFER_SIZE_RECV - 1)
		{
			ret = recv(fd, buffer, BUFFER_SIZE_RECV - 1, 0);
			if (ret == -1)
				break;
			
			buffer[ret] = 0;
			_client_rqst_msg += buffer;
			Logger(B_GRAY, "subbuf:" + std::string(buffer));
			Logger(PURPLE, "Replay Recv read " + std::to_string(ret) + " bytes");
		}
		//full_msg.pop_back();

		Logger(GREEN, "Data is read is " + std::to_string(_client_rqst_msg.size()) + " bytes  ✅ ");
		Logger(B_GRAY, _client_rqst_msg);
		send(fd, "Message has send successfully\n", strlen("Message has send successfully\n"), 0);

		return (_client_rqst_msg.size());
	}

	std::string ServerSelect::GetClientRequest() const
	{
		return (_client_rqst_msg);
	}

	/* Destructor */
	ServerSelect::~ServerSelect()
	{
		//TODO: закрытие сокета
		Logger(RED, "Call ServerSelect Destructor❌ ");
	}

}