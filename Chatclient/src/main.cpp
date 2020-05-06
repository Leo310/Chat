#include "Client.h"

#include <thread>


void waitingForMsg(Client client)
{
	while (true)
	{
		client.recieve();
		std::cout << client.getMessage() << std::endl;
	}

}

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	client.createSocket();
	client.connectToSrv("127.0.0.1", 54000);

	std::thread worker(waitingForMsg, std::ref(client));

	std::string userInput;

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