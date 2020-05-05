#include "Client.h"

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	client.createSocket();
	client.connectToSrv("127.0.0.1", 54000);

	std::string userInput;

	while (true)
	{
		//Prompt the user for some text
		std::cout << "> ";
		std::getline(std::cin, userInput);

		if (client.sendMsg(userInput))
		{
			if (client.recieve())
				std::cout << client.getMessage() << std::endl;
		}
	}

	std::cin.get();
	return 0;
}