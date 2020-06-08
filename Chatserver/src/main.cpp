#include "Server.h"

//Threading um mehrere Blocking IOs non-blocking zu machen
//Der Server wartet zum einen auf Verbindungen von Clients, muss aber gleichzeitig Userinput verarbeiten und
//Nachrichten an die Clients senden
#include <thread>


//läuft auf anderem Thread, für UserInput zuständig
void waitingForUserInput(Server& srv)
{
	std::string userInput;	//speichert hier die Benutzereingabe
	while (true)
	{
		std::getline(std::cin, userInput);	 //solanger der thread nicht detached oder "beendet" wird, warte auf Nachrichten vom Server
		if (userInput.substr(0, 4) == "/add")	//wenn input mit /add beginnt
		{
			srv.addCr(std::stoi(userInput.substr(5, 1)));	//stoi = string to int, userInout in Anzahl der Chaträume die hinzugefügt werden sollen (int)
			std::cout << "Chatrooms: " + std::to_string(srv.getCrCount()) << std::endl; //gibt in der Konsole die Anzahl der virtuellen Chaträume
		}
		else if (userInput.substr(0, 7) == "/remove")	//wenn input mit /remove beginnt
		{
			srv.removeCr(std::stoi(userInput.substr(8, 1)));	//Anzahl an Chaträumen die entfernt werden soll
			std::cout << "Chatrooms: " + std::to_string(srv.getCrCount()) << std::endl;
		}
		else //wenn server die eingabe in die Konsole nicht verarbeiten kann
		{
			std::cout << "You can only add a chatroom with /add and remove a chatroom with /remove" << std::endl;
		}
	}
}

int main()
{

	//Fragt zunächst IPv4 Adresse und Port an auf dem der Server-Dienst laufen soll
	std::string serverIp;
	std::string serverPort;

	bool bindSrv = false;	//Speichert ob der Server auf der eingegebe Ip und Port laufen kann
	while (!bindSrv)
	{
		//Input der IPv4 Adresse
		char tmpBuf[4096];
		std::cout << "Type in the network ip of your device to bind the srv to that ip. The clients can than connect to this ip." << std::endl;
		std::cout << "Server-IP: ";
		std::getline(std::cin, serverIp);
		if (inet_pton(AF_INET, serverIp.c_str(), tmpBuf) <= 0)	//checkt ob die eingegebene Adresse gültig
		{
			std::cout << "Thats not a valid ip address" << std::endl;
			bindSrv = false;
			continue;
		}
		else
			bindSrv = true;

		//Input des Ports
		std::cout << "Type in a portnumber between 49152 - 65535. The clients can than connect to this port." << std::endl;
		std::cout << "Server-Port: ";
		std::getline(std::cin, serverPort);
		int srvPort = std::stoi(serverPort);	//stoi, = string to int
		if (srvPort == -1 && srvPort >= 49152 && srvPort <= 65535)
		{
			std::cout << "Thats not a valid port" << std::endl;
			bindSrv = false;
			continue;
		}
		else
			bindSrv = true;
	}


	Server srv(serverIp, std::stoi(serverPort));	//erstellt srv objekt mit eingegeber Ip und Port
	
	//Initialisiert Server
	if (!srv.init())	
		std::cout << "Couldnt Init Winsock" << std::endl;

	std::cout << "You can add a chatroom with /add and remove a chatroom with /remove" << std::endl;
	std::thread userInputWorker = std::thread(waitingForUserInput, std::ref(srv));	//startet Thread und ergibt srv als reference an die Methode die auf dem anderen Thread läuft

	if (srv.createListeningSocket())	//wenn der Listening Socket, welcher auf eingehende verbindungen warten soll erfolgreich erstellt wurde:
	{
		while (true)	//solange das fenster nicht geschlossen wird
		{
			srv.waitForConnection();	//wartet auf eingehende Verbindungen
			if (srv.recieve() > 0)		//wartet auf eingehende Nachrichten
			{
				std::cout << srv.getMessage() << std::endl;	//gibt engegangene Nachricht in der Konsole aus
				srv.sendMsgCr();	//sendet die eingegangene Nachricht an alle anderen Clients die in dem virtuellen Chatraum verbunden sind
			}
		}
	}

	userInputWorker.detach();	//need to "destroy" explicitly, sonst kann es zur Fehlermeldung kommen, da der andere Thread nicht regelkonform beendet wurde
	srv.cleanUp();	//Cleanup
	return 0;
}