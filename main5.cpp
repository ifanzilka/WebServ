#include <Include_Library.hpp>

int	main(int argc, char **argv)
{
	if (argc == 2)
	{
		ParserConfig config;
		std::string conf_path = "./resources/conf/" + std::string(argv[1]);

		config.Parse(conf_path);
		//TODO сделать вывод ошибки при невалидном конфиге
		//TODO: добавить прием пути к конфигу через аргументы
		std::map<int, ServerData> servers_data = config.GetServers();

		//TODO: сделать запуск нескольких серверов
		// (возможно многопоточность)
		for (int i = 0; i < servers_data.size(); i++)
		{
			ServerCore server(servers_data[i]);
			server.Start();
		}
	}
	else
	{
		std::cout << "./webserv <default.conf>\n";
	}
}