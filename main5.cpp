#include <Include_Library.hpp>
//#include <ParserConfig.hpp>

int	main(int argc, char **argv)
{
	if (argc == 2)
	{
		ServerCore server(argv[1]);
		server.Start();
	}
	else
	{
		std::cout << "You need to put only one additional parameter!\n";
	}
}