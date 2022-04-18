#include <Include_Library.hpp>
//#include <ServerKqueue.hpp>

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		ft::ServerKqueue serv("127.0.0.1", atoi(argv[1]));
		serv.Start();
	}
	else
	{
		std::cout << "Use: webserv port\n";
	}
	return (0);
}