#include "Server.h"

Server::Server(std::string ip, int port)	//konstruktor initialisiert alles um Warnings des Compilers zu entfernen
	: m_IpAddress(ip), m_Port(port), m_AddrOfClient(), m_Client(0), m_Listening(0), m_Maxsd(0), m_Rdy(0), m_Readfds(), m_Sd(0)
{
}

Server::~Server()	//Destruktor der beim Aufruf alles aufräumt
{
	cleanUp();
}

bool Server::init()
{
	//initialisiert Windowssockets
	WSAData data;
	return !WSAStartup(MAKEWORD(2, 2), &data);
}

int Server::recieve()		//returns -1 rcv command, returns 0 error, returns 1 rcv msg
{
	int rcv = 0;	//return wert
	for (unsigned int i = 0; i < m_Clients.size(); i++)	//geht alle clients die sich mit dem Server verbunden haben durch
	{
		m_Sd = m_Clients[i];	//m_Sd (Socket descriptor = SOCKET ) 
		if (FD_ISSET(m_Sd, &m_Readfds))	//FD_ISSET checkt ob auf dem Socket eine Aktivität aufgezeichnet wurde, in m_Readfds sind die Status aller Sockets gespeichert
		{
			char buf[4096];	
			int received = recv(m_Sd, buf, 4096, 0); //empfangene Nachricht wird in buf gespeichert

			getpeername(m_Sd, (sockaddr*)&m_AddrOfClient, &m_ClientSize);	//speichert Addresse vom SOCKET
			char hostName[NI_MAXHOST];
			inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);	//macht netzwerk Adresse zu einer lesbaren und speichert lesbare IPv4 in hostName

			//checks if client disconnected
			if (received == SOCKET_ERROR || received == 0)
			{
				for (Chatroom* cr : m_Chatrooms)	//geht alle chatrooms durch
				{
					if (cr->inChatroom(m_Sd))		//wenn client im chatraum
						if (cr->remove(m_Sd))		//enferne client aus chatraum
						{
							std::cout << "Removed Client " << hostName << ":" << htons(m_AddrOfClient.sin_port) << " succesfully from Chatroom" << std::endl;
						}
				}
				std::cout << hostName << ":" << htons(m_AddrOfClient.sin_port) << " Client disconnected" << std::endl;
				m_Clients.erase(m_Clients.begin() + i);	//entferne client aus array
				closesocket(m_Sd);						//schließe client socket
				rcv = 0;
			}
			else
				rcv = 1;			

			//checks if client wants to connect on chatroom
			std::string cmd = buf;
			if (cmd.substr(0, 8) == "/contocr")	//wenn client Nachricht schickt die mit /contocr anfängt:
			{
				int crCon = std::stoi(cmd.substr(9, 1));	//10tes Zeichen ist die Nummer auf welchen CHatraum der Client connecten möchte

				//if already in a chatroom, diconnect from this one
				for (Chatroom* cr : m_Chatrooms)
				{
					if (cr->inChatroom(m_Sd))
					{
						//wenn der client nicht schon in dem Chatraum ist entferne ihn erst von dem jetztigen
						if (cr != m_Chatrooms[crCon])
						{
							std::vector<SOCKET> sendTo = cr->sendMsg(m_Sd);
							for (SOCKET client : sendTo)
							{
								sendMsgTo(client, (std::string)"Server: " + hostName + " disconnected from Chatroom");
							}
							if (cr->remove(m_Sd))
							{
								std::cout << "Removed Client " << hostName << ":" << htons	(m_AddrOfClient.sin_port) << " succesfully from Chatroom " << crCon << std::endl;
							}
							else
							{
								std::cout << "Client " << hostName << ":" << htons(m_AddrOfClient.sin_port) << " is not in this chatroom" << std::endl;
							}
							break;
						}
					}
				}
				if (m_Chatrooms[crCon]->add(m_Sd))	//jetzt kann man den client zum neuen chatraum hinzufügen
				{
					//prints on srv side
					std::cout << hostName << ":" << ntohs(m_AddrOfClient.sin_port) << " did successfully connect to Chatroom " << crCon << std::endl;
					//sends msg to client who wants to connect
					sendMsgTo(m_Sd, (std::string)"Server: Successfully connected to Chatroom " + std::to_string(crCon));
					//sends msg to all other clients on the chatroom
					std::vector<SOCKET> sendTo = m_Chatrooms[crCon]->sendMsg(m_Sd);
					for (SOCKET client : sendTo)
					{
						sendMsgTo(client, (std::string)"Server: " + hostName + " connected to Chatroom");
					}
				}
				else	//wenn client schon auf dem chatraum ist
				{
					//prints on srv side
					std::cout << "Client " << hostName << ":" << htons(m_AddrOfClient.sin_port) << " is already on this chatroom" << std::endl;
					//sends msg to client who wants to connect
					sendMsgTo(m_Sd, (std::string)"Server: Already on this Chatroom ");
				}
				rcv = -1;
			}
			if(rcv > 0)
				m_RcvMsg = std::make_tuple(buf, m_Sd);	//füge message zu RcvMsg hinzu wenns kein command war
		}
	}
	return rcv;
}

bool Server::sendMsgTo(SOCKET s, std::string msg)	//senden Nachricht an einen bestimmten client
{
	int sended = send(s, msg.c_str(), msg.size() + 1, 0);
	if (sended == SOCKET_ERROR)
	{
		std::cout << "Couldnt send msg	Error code: " << WSAGetLastError() << std::endl;
		sended = false;
	}
	sended = true;
	return sended;
}

void Server::addCr(int count) //fügt neuen chatraum hinzu
{
	for (int i = 0; i < count; i++)
	{
		m_Chatrooms.push_back(new Chatroom(m_Chatrooms.size()));
	}
	for (SOCKET client : m_Clients)
	{
		sendMsgTo(client, "Chatroom:" + std::to_string(m_Chatrooms.size()));	//send new Chatroomcount to all connected clients
	}
}

void Server::removeCr(int count)	//entfernt CHatraum
{
	if (m_Chatrooms.size() >= count)	//checkt nur ob man mehr clients entfernen möchte als eigentlich existieren
	{
		int oldSize = m_Chatrooms.size();	//speichert alter size da sich .size() dynamisch verändert da man Chaträume aus der Liste entfernt
		for (int i = 1; i <= count; i++)	//entfernt alle chaträume
		{
			auto clientsOnCr = m_Chatrooms[oldSize - i]->getClients();
			for(SOCKET client : clientsOnCr)
				sendMsgTo(client, "Server: Chatroom " + std::to_string(oldSize - i) + " got closed");	//send disconnect
			std::cout << "Chatroom " + std::to_string(oldSize - i) + " got closed" << std::endl;
			m_Chatrooms.erase(m_Chatrooms.begin() + oldSize - i);	//entfernt chatraum aus Chatraumliste
		}
		for (SOCKET client : m_Clients)
		{
			sendMsgTo(client, "Chatroom:" + std::to_string(m_Chatrooms.size()));	//send new Chatroomcount to all connected clients
		
		}
	}
	else
	{
		std::cout << "There are only " + std::to_string(m_Chatrooms.size()) + " Chatrooms left" << std::endl;
	}
}

int Server::getCrCount()
{
	return m_Chatrooms.size();
}

bool Server::sendMsgCr()	//sends rcv message to other clients
{
	bool sended = false;
	std::vector<SOCKET> sendTo;
	//figuering out on which Chatroom the client is
	for (Chatroom* cr : m_Chatrooms)
	{
		if (cr->inChatroom(std::get<1>(m_RcvMsg)))
		{
			sendTo = cr->sendMsg(std::get<1>(m_RcvMsg));	//speichert clients an die die Nachricht gesendet werden soll
			break;
		}
	}

	for (unsigned int i = 0; i < sendTo.size(); i++)
	{
		sendMsgTo(sendTo[i],std::get<0>(m_RcvMsg));	
	}
	return sended;
}

void Server::cleanUp()	//räumt alles auf
{
	closesocket(m_Listening);
	for (unsigned int i = 0; i < m_Clients.size(); i++)
	{
		closesocket(m_Clients[i]);
	}
	WSACleanup();
}

std::string Server::getMessage()	//gibt die letzte empfangene Nachricht zurück
{
	return std::get<0>(m_RcvMsg);
}

bool Server::createListeningSocket()	//erstellt Server Socket welcher auf weitere Verbindungsanfragen wartet
{
	m_Listening = socket(AF_INET, SOCK_STREAM, 0);	//erstellt Socket
	if (m_Listening != INVALID_SOCKET)	//wenn das Erstellen nicht fehl geschlagen ist:
	{
		std::cout << "Created Socket" << std::endl;

		//füllt Addressenstruktur aus auf der der Server laufen soll
		SOCKADDR_IN addr;
		SecureZeroMemory(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(m_Port);
		inet_pton(AF_INET, m_IpAddress.c_str(), &addr.sin_addr);	

		int bindSocket = bind(m_Listening, (sockaddr*)&addr, sizeof(addr));	//binded Server an diese Addressstruktur
		if (bindSocket == SOCKET_ERROR)
		{
			std::cout << "Couldnt bind listening socket to address	Error code: " << WSAGetLastError() << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "Couldnt create socket	Error code: " << WSAGetLastError() << std::endl;
		return false;
	}

	int listenSocket = listen(m_Listening, SOMAXCONN);	//setzt Serversocket in listing mode -> wartet durch accept() auf Verbindungsanfragen
	if (listenSocket == SOCKET_ERROR)
	{
		std::cout << "Couldnt create listening socket	Error code: " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Server listening..." << std::endl;
	return true;
}

void Server::waitForConnection()	//wartet auf Verbindungen
{
	//clear the socket set  
	FD_ZERO(&m_Readfds);

	//add listening socket	(Server Socket) to set
	FD_SET(m_Listening, &m_Readfds);
	//sd socket discriptor
	m_Maxsd = m_Listening;	//nicht unbedingt notwendig

	//add clients to set
	for (unsigned int i = 0; i < m_Clients.size(); i++)
	{
		//m_Sd = socket descriptor (id des Sockets)
		m_Sd = m_Clients[i];

		if (m_Sd > 0)//dont need to do that, checked before, but doppelt hält besser
			FD_SET(m_Sd, &m_Readfds);

		//highest file descriptor number, need it for the select function but nicht unbedingt notwendig
		if (m_Sd > m_Maxsd)
			m_Maxsd = m_Sd;
	}
	//first paramter: can be ignored, put a sd in that cant be effected
	//second parameter: An optional pointer to a set of sockets to be checked for readability.
	//andere Parameter auf die die select methode achten kann, wir wollen aber nur auf readability achten
	m_Rdy = select(m_Maxsd + 1, &m_Readfds, NULL, NULL, NULL);	
	//IMPORTANT: Blocking IO, ermöglicht non-blocking mehrerer anderer blocking IO, 
	//eine Blockingfunktion übernimmt, mehrere Blocking Funktionen auf eine wodurch diese Non-blocking werden, 
	//da die erst ausgeführt werden, wenn der Status diese Funktionen bekannt ist 
	// (select speichert Status aller sockets in m_Readfds = ReadFileDescriptorsockets) und somit non-blocking werden

	if (m_Rdy == SOCKET_ERROR);		//dont need to do that cause receive will take care of that later
	else if (m_Rdy == 0)
		std::cout << "Couldnt detect rdy Sockets, because the time limit expired" << std::endl;

	if (FD_ISSET(m_Listening, &m_Readfds))	//wenn am Listening SOcket eine Verbindungsanfrage ankam:
	{
		m_Client = accept(m_Listening, (sockaddr*)&m_AddrOfClient, &m_ClientSize);	
		//accept() ist normalerweise blocking IO aber dadurch dass der Status bekannt ist(Verbindungsanfrage kam) 
		// wird accept() non-blocking und akzeptiert einen client (Verbindung zwischen Server und CLient wurde aufgebaut)

		if (m_Client == INVALID_SOCKET)
		{
			std::cout << "Couldnt accept clientSocket	Error code: " << WSAGetLastError() << std::endl;
		}

		m_Clients.push_back(m_Client);	//fügt client verbundenen client zur clientliste hinzu
		char hostName[NI_MAXHOST];
		inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);
		std::cout << hostName << " connected on port " << ntohs(m_AddrOfClient.sin_port) << std::endl;

		sendMsgTo(m_Client, "Chatroom:" + std::to_string(m_Chatrooms.size()));	//sendet dem CLient die Anzahl an Chaträumen die auf die sich der CLient verbinden kann
	}

}
