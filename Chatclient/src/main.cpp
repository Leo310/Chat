#include "Client.h"

#include <thread>

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//change color of console
static std::string userInput;


//other Threads task
void waitingForMsg(Client client)
{
	while (true)
	{
		client.recieve();
		SetConsoleTextAttribute(hConsole, 10);		//change color of console
		std::cout << client.getMessage() << std::endl;
		SetConsoleTextAttribute(hConsole, 12);		//change color of console
	}
}

std::string chooseChatroom()
{
	while (true) {
		std::cout << "Du kannst auf die Chatraeume von 0-3 connected." << std::endl;
		std::cout << "Tippe nun deine Wahl ein." << std::endl;
		std::getline(std::cin, userInput);

		if (userInput == "0")
			return "0";
		else if (userInput == "1")
			return "1";
		else if (userInput == "2")
			return "2";
		else if (userInput == "3")
			return "3";
		else
		{
			std::cout << "Dieser Chatraum existiert nicht." << std::endl;
		}
	}
}

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	std::string choice = chooseChatroom();
	client.createSocket();
	client.connectToSrv("89.14.163.155", 54000);

	client.sendMsg(choice);
	client.recieve();		//wartet auf antwort des srv ob das joinen auf den chatroom geklappt hat
	std::cout << client.getMessage() << "!" << std::endl;
		
	std::thread worker(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann

	SetConsoleTextAttribute(hConsole, 12);		//change color of console

	while (true)
	{
		//std::cout << "> ";
		std::getline(std::cin, userInput);
		if (userInput == "exit")
			break;
		client.sendMsg(userInput);
	}
	worker.detach();	//need to "destroy" explicitly
	return 0;
}