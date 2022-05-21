#ifndef DEMULTIPLEXER_HPP
#define DEMULTIPLEXER_HPP

#include <string>

class ServerCore
{
	public:
		ServerCore(ServerData &server_data);
		~ServerCore();
		void Start() const;

	private:
		ServerData 	&_server_data;
		void	StartWebServer() const;
};

#endif
