#include "Server.h"

#include <thread>


//other Threads task
void waitingForUserInput(Server& srv)
{
	std::string userInput;
	while (true)
	{
		std::getline(std::cin, userInput);
		if (userInput.substr(0, 4) == "/add")
		{
			srv.addCr(std::stoi(userInput.substr(5, 1)));
			std::cout << "Chatrooms: " + std::to_string(srv.getCrCount()) << std::endl;
		}
		else if (userInput.substr(0, 7) == "/remove")
		{
			srv.removeCr(std::stoi(userInput.substr(8, 1)));
			std::cout << "Chatrooms: " + std::to_string(srv.getCrCount()) << std::endl;
		}
		else
		{
			std::cout << "You can only add a chatroom with /add and remove a chatroom with /remove" << std::endl;
		}
	}
}

int main()
{
	std::string serverIp;
	std::string serverPort;

	bool bindSrv = false;
	while (!bindSrv)
	{
		char tmpBuf[4096];
		std::cout << "Type in the network ip of your device to bind the srv to that ip. The clients can than connect to this ip." << std::endl;
		std::cout << "Server-IP: ";
		std::getline(std::cin, serverIp);
		if (inet_pton(AF_INET, serverIp.c_str(), tmpBuf) <= 0)
		{
			std::cout << "Thats not a valid ip address" << std::endl;
			bindSrv = false;
			continue;
		}
		else
			bindSrv = true;
		std::cout << "Type in a portnumber between 49152 - 65535. The clients can than connect to this port." << std::endl;
		std::cout << "Server-Port: ";
		std::getline(std::cin, serverPort);
		int srvPort = std::stoi(serverPort);
		if (srvPort == -1 && srvPort >= 49152 && srvPort <= 65535)
		{
			std::cout << "Thats not a valid port" << std::endl;
			bindSrv = false;
			continue;
		}
		else
			bindSrv = true;
	}


	Server srv(serverIp, std::stoi(serverPort));

	if (!srv.init())	
		std::cout << "Couldnt Init Winsock" << std::endl;

	std::cout << "You can add a chatroom with /add and remove a chatroom with /remove" << std::endl;
	std::thread userInputWorker = std::thread(waitingForUserInput, std::ref(srv));

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
	userInputWorker.detach();
	srv.cleanUp();
	return 0;
}