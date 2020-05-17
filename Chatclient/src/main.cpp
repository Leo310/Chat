#include "Client.h"

#include <thread>
#include <string>

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

int main()
{
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	client.createSocket();
	client.connectToSrv("89.14.163.155", 54000);

	while (true)
	{
		std::cout << "1 Login		2 Registieren" << std::endl;
		std::getline(std::cin, userInput);

		if (userInput == "1") //login
		{
			std::cout << "Tippe deinen Namen ein..." << std::endl;
			std::getline(std::cin, userInput);
			std::string userName = userInput;

			std::cout << "Tippe dein Passwort ein..." << std::endl;
			std::getline(std::cin, userInput);
			std::string password = userInput;
			
			client.sendMsg("/login;" + userName + ";" + "password");
			if (client.recieve())
			{
				if (client.getMessage() == "1")	//successfull login
				{
					User user;
				}
				else if (client.getMessage() == "0")
				{

				}
			}
			break;
		}
		else if (userInput == "2")	//register
		{
			std::cout << "Tippe deinen Namen ein..." << std::endl;
			std::getline(std::cin, userInput);
			std::string userName = userInput;

			break;
		}
		else 
		{
			std::cout << "Tippe entweder die Zahl zum Login oder die Zahl 2 zum Registrieren" std::endl;
		}
	}

	//client.sendMsg(choice);
	client.recieve();		//gets number of chatrooms
	int crCount = std::stoi(client.getMessage());
	std::cout << "Du kannst mit /contocr auf die Chatraeume von 0" << "-" << crCount-1 <<  " connecten." << std::endl;


	std::thread worker(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann

	SetConsoleTextAttribute(hConsole, 12);		//change color of console

	while (true)
	{
		//std::cout << "> ";
		std::getline(std::cin, userInput);
		if (userInput.substr(0, 1) == "/")	//if command
		{
			if (userInput.substr(1, 4) == "exit")
				break;
			else if (userInput.substr(1, 7) == "contocr" && userInput.size() > 9)		//noch nicht perfekt eingefügt, man kann immer noch buchstaben eintippen
			{
				int crCon = std::stoi(userInput.substr(9, 1));
				if (crCon < crCount)
					client.sendMsg(userInput);
				else
					std::cout << "Es gibt nur Chatrooms zwischen 0 und " << crCount - 1 << std::endl;
			}
			else
			{
				std::cout << "Es gibt diesen Command nicht. Es gibt nur /contocr (Chatraumnummer) und /exit" << std::endl;
			}
		}
		else
		{
			std::string msg = (std::string)userName + ": " + userInput;
			client.sendMsg(msg);
		}
			
	}
	worker.detach();	//need to "destroy" explicitly
	return 0;
}