#include "Client.h"

#include <thread>

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//change color of console

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

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	client.createSocket();
	client.connectToSrv("78.55.167.172", 54000);

	std::thread worker(waitingForMsg, std::ref(client));

	std::string userInput;

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