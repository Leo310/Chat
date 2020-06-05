#include "Client.h"
#include "Interface.h"

#include <thread>

Client client;
Interface gui;


static std::string rcvMsg;
static bool rcvdMsg = false;

static std::string stopOptimizeThisWhileLoop;

//other Threads task
void waitingForMsg(Client client)
{
	while (true)
	{
		client.recieve();
		rcvMsg = client.getMessage();
		rcvdMsg = true;
		while (rcvdMsg == true)
			stopOptimizeThisWhileLoop = "Hope it wont get optimized";	//need this because the while loop would get optimized in rls mode
	}
}


int main()
{
	//no Console
	FreeConsole();

	bool connected = false;

	if (!gui.init())
		return -1;

	if (!client.init())
		gui.log("Couldnt init Winsock");

	std::thread rcvWorker;

	while (!gui.closeProgram())
	{
		if (gui.logined())
		{
			if (!connected)
			{
				client.createSocket();
				int connect = client.connectToSrv(gui.getServerIp(), gui.getServerPort());
				switch (connect)
				{
				case INVALID_IP:
					gui.reset("Invalid IP");
					gui.update();
					connected = false;
					break;
				case INVALID_PORT:
					gui.reset("Invalid Port");
					gui.update();
					connected = false;
					break;
				case COULDNT_CONNECT:
					gui.reset("Couldnt connect to srv");
					gui.update();
					connected = false;
					break;
				case true:
					rcvWorker = std::thread(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann)
					connected = true;
				}
			}
			else
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
		}
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

		gui.update();
	}

	rcvWorker.detach();	//need to "destroy" explicitly
	gui.~Interface();
	return 0;
}