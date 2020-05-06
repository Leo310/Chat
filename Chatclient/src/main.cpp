#include "Client.h"

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	client.createSocket();
	client.connectToSrv("192.168.1.4", 54000);

	std::string userInput;

	while (true)
	{
		//Prompt the user for some text
		std::cout << "> ";
		std::getline(std::cin, userInput);
		if (userInput == "exit")
		{
			break;
		}
		if (client.sendMsg(userInput))
		{
			if (client.recieve())
				std::cout << client.getMessage() << std::endl;
		}
	}
	return 0;
}