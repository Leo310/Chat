#include "Client.h"
#include "Interface.h"

#include <thread>

Client client;
Interface gui;


static std::string rcvMsg;
static bool rcvdMsg = false;

//other Threads task
void waitingForMsg(Client client)
{
	while (true)
	{
		client.recieve();
		rcvMsg = client.getMessage();
		rcvdMsg = true;
		while (rcvdMsg == true);
	}
}


int main()
{
	//no Console
	FreeConsole();

	if (!gui.init())
		return -1;

	if (!client.init())
		gui.log("Couldnt init Winsock");

	//std::string choice = chooseChatroom();
	client.createSocket();
	client.connectToSrv("77.191.144.48", 54000);

	std::thread rcvWorker(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann


	while (!gui.closeProgram())
	{
		if (rcvdMsg == true)
		{
			if (rcvMsg.substr(0, 9) == "Chatroom:")
			{
				gui.setChatCount(std::stoi(rcvMsg.substr(9, 10)));
				gui.log("Chatroom Number: " + rcvMsg.substr(9, 10));
			}
			else
			{
				gui.printRcvdMsg(rcvMsg);
			}
			rcvdMsg = false;
		}
		if (gui.logined())
		{
			if (gui.sendButtonPressed())
			{
				std::string msg = (std::string)gui.getUserName() + ": " + gui.getSendMsg();
				client.sendMsg(msg);
			}
			if (gui.ConnectTo() >= 0)
			{
				client.sendMsg("/contocr " + std::to_string(gui.ConnectTo()));
			}
		}

		gui.update();
	}

	rcvWorker.detach();	//need to "destroy" explicitly
	gui.~Interface();
	return 0;
}