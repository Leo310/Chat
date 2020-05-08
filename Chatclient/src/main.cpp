#include "Client.h"

#include <thread>

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//change color of console
static std::string userInput;
static std::string serverChoice;

void waitingForMsg(Client client)
{

	while (true)
	{
		client.recieve();
		SetConsoleTextAttribute(hConsole, 10);		//change color of console
		std::cout << "				" << client.getMessage() << std::endl;
		SetConsoleTextAttribute(hConsole, 12);		//change color of console
	}

}

bool chooseChatroom(Client& client)
{
	std::cout << "Du kannst auf die Chatraeume von 0-3 connected." << std::endl;
	std::cout << "Tippe nun deine Wahl ein." << std::endl;
	std::getline(std::cin, userInput);
	if (userInput == "0")
		serverChoice = "0";
	else if (userInput == "1")
		serverChoice = "1";
	else if (userInput == "2")
		serverChoice = "2";
	else if (userInput == "3")
		serverChoice = "3";
	else
	{
		std::cout << "Dieser Chatraum existiert nicht." << std::endl;
		return false;
	}
	return true;
}

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	while (!chooseChatroom(client));

	client.createSocket();
	client.connectToSrv("78.55.167.172", 54000);
	client.sendMsg(serverChoice);
	client.recieve();
	std::cout << client.getMessage() << "!" << std::endl;

	std::thread worker(waitingForMsg, std::ref(client));

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