#include <iostream>
#include "Server.h"

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
			if (srv.recieve() > 0)
			{
				std::cout << srv.getMessage() << std::endl;
				srv.sendMsgCr();
			}
		}
	}

	std::cin.get();
	return 0;
}