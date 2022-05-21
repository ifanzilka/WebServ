#include <Include_Library.hpp>
//#include <ServerSelect.hpp>

/**
==============================================
==        Constructors and Destructor       ==
==============================================
*/

void ServerSelect::Init_Serv()
{
	_logs << "ServerType: Select 🌐 " << std::endl;

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

int	ServerSelect::WaitEvent(int &client_fd)
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

/* Проверяю на чтение если нету возвращаю 0, иначе fd откуда читать */
int	ServerSelect::CheckRead()
{
	Logger(BLUE, "Check read...");

	std::vector<Client>::iterator	it_begin;
	std::vector<Client>::iterator	it_end;


	it_begin = _clients.begin();
	it_end = _clients.end();


	/* Проверяю дескрипторы на то что пришло ли что то чтение */
	while (it_begin != it_end)
	{
		/* message receives from curr_cli */
		if (FD_ISSET((*it_begin).getFd(), &_readfds))
		{

			//ReadFd((*it_begin).first);
			return ((*it_begin).getFd());
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
			return (client_fd);
		}
		return (client_fd);

	}
	return (0);
}


void	ServerSelect::AddFd(int fd)
{
	Logger(B_GRAY, "Add fd " + std::to_string(fd));

	/* Добавляю во множество */
	FD_SET(fd, &_currfds);

	//for select
	_max_fd = fd > _max_fd ? fd : _max_fd;
}


void ServerSelect::RemoveFd(int fd)
{
	Logger(B_GRAY, "Remove fd " + std::to_string(fd));

	/* Удаление из множества */
	FD_CLR(fd, &_currfds);

	RemoveClient(fd); //TODO: переименовать в Remove
}

int ServerSelect::ReadFd(int fd)
{
	Logger(GREEN, "Readble is ready: fd(" + std::to_string(fd) + ") ✅ ");

	char buffer[RECV_BUFFER_SIZE];
	bzero(buffer, RECV_BUFFER_SIZE);

	int ret = recv(fd, buffer, RECV_BUFFER_SIZE - 1, 0);
	if (ret == 0)
	{
		Logger(RED, "Disconnect  fd(" + std::to_string(fd) + ") ❌ ");
		RemoveFd(fd);
		return (0);
	}

	_client_rqst_msg.resize(0);
	_client_rqst_msg += buffer;

	Logger(PURPLE, "Recv read " + std::to_string(ret) + " bytes");
	Logger(B_GRAY, "buf:" + _client_rqst_msg);
	while (ret == RECV_BUFFER_SIZE - 1)
	{
		ret = recv(fd, buffer, RECV_BUFFER_SIZE - 1, 0);
		if (ret == -1)
			break;

		buffer[ret] = 0;
		_client_rqst_msg += buffer;
		Logger(B_GRAY, "subbuf:" + std::string(buffer));
		Logger(PURPLE, "Replay Recv read " + std::to_string(ret) + " bytes");
	}

	Logger(GREEN, "Data is read is " + std::to_string(_client_rqst_msg.size()) + " bytes  ✅ ");
	Logger(B_GRAY, _client_rqst_msg);
	send(fd, "Message has send successfully\n", strlen("Message has send successfully\n"), 0);

	return (_client_rqst_msg.size());
}


/* Destructor */
ServerSelect::~ServerSelect()
{
	//TODO: закрытие сокета
	Logger(RED, "Call ServerSelect Destructor❌ ");
}