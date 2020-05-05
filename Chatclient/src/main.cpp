#include "Client.h"

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	client.createSocket();
	client.connectToSrv("192.168.1.7", 54010);

	std::cin.get();
	return 0;
}