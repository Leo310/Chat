#include "Client.h"
#include "Interface.h"

#include <thread>

static std::string rcvMsg;
static bool rcvdMsg = false;
static int crCount;

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

	//CLient logic
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	//std::string choice = chooseChatroom();
	client.createSocket();
	client.connectToSrv("77.191.118.48", 54000);


	//client.sendMsg(choice);
	client.recieve();		//gets number of chatrooms
	crCount = std::stoi(client.getMessage());
	std::cout << "Du kannst mit /contocr auf die Chatraeume von 0" << "-" << crCount-1 <<  " connecten." << std::endl;


	std::thread rcvWorker(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann

	Interface gui;

	if (!gui.init())
		return -1;

	gui.setChatCount(crCount);

	while (!gui.closeProgram())
	{
		if (rcvdMsg == true)
		{
			gui.printRcvdMsg(rcvMsg);
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