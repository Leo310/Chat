#include <iostream>
#include "Server.h"

int main()
{
	Server srv("192.168.1.7", 54010);

	if (!srv.init())	
		std::cout << "Couldnt Init Winsock" << std::endl;

	srv.run();

	std::cin.get();

	return 0;
}