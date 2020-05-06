#include <iostream>
#include "Server.h"

#include <thread>

int main()
{
	Server srv("192.168.1.4", 54000);

	if (!srv.init())	
		std::cout << "Couldnt Init Winsock" << std::endl;
	
	if (srv.createListeningSocket())
	{
		while (true)
		{
			srv.waitForConnection();
			if (srv.recieve())
			{
				std::cout << srv.getMessage() << std::endl;
				srv.sendMsg(srv.getMessage());
			}
		}
	}


	std::cin.get();
	return 0;
}