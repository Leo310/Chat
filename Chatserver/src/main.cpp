#include <iostream>
#include "Server.h"

int main()
{
	Server srv("192.168.1.4", 54000);

	if (!srv.init())	
		std::cout << "Couldnt Init Winsock" << std::endl;

	srv.addCr(2);
	if (srv.createListeningSocket())
	{
		while (true)
		{
			srv.waitForConnection();
			if (srv.recieve() > 0)
			{
				std::cout << srv.getMessage() << std::endl;
				srv.sendMsgCr();	//when rcv a msg send to all the other clients in the chatroom
			}
		}
	}

	std::cin.get();
	return 0;
}