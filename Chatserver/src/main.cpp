#include <iostream>
#include "Server.h"

int main()
{
	Server srv("127.0.0.1", 54000);

	if (!srv.init())	
		std::cout << "Couldnt Init Winsock" << std::endl;

	srv.run();

	while (srv.recieve())
	{
		std::cout << srv.getMessage() << std::endl;
		srv.sendMsg(srv.getMessage());
	}

	std::cin.get();
	return 0;
}