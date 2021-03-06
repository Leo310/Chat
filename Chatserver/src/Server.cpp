#include "Server.h"

Server::Server(std::string ip, int port)
	: m_IpAddress(ip), m_Port(port), m_AddrOfClient(), m_Client(0), m_Listening(0), m_Maxsd(0), m_Rdy(0), m_Readfds(), m_Sd(0)
{
}

Server::~Server()
{
	cleanUp();
}

bool Server::init()
{
	WSAData data;
	return !WSAStartup(MAKEWORD(2, 2), &data);
}

int Server::recieve()		//returns -1 rcv command, returns 0 error, returns 1 rcv msg
{
	int rcv = 0;
	for (unsigned int i = 0; i < m_Clients.size(); i++)
	{
		m_Sd = m_Clients[i];
		if (FD_ISSET(m_Sd, &m_Readfds))
		{
			char buf[4096];
			int received = recv(m_Sd, buf, 4096, 0);

			getpeername(m_Sd, (sockaddr*)&m_AddrOfClient, &m_ClientSize);	//TODO check return value
			char hostName[NI_MAXHOST];
			inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);

			//checks if client disconnected or an error occurred
			if (received == SOCKET_ERROR)
			{
				std::cout << hostName << ":" << ntohs(m_AddrOfClient.sin_port) << " didnt disconnect succesfully	Error code: " << WSAGetLastError() << std::endl;
				m_Clients.erase(m_Clients.begin() + i);
				closesocket(m_Sd);
				rcv = 0;
			}
			else if (received == 0)
			{
				std::cout << hostName << ":" << ntohs(m_AddrOfClient.sin_port) << " Client diconnected" << std::endl;
				m_Clients.erase(m_Clients.begin() + i);
				closesocket(m_Sd);
				rcv = 0;
			}
			else
				rcv = 1;			//k�nnte bei mehreren clients die gleichzeitig etwas schicken probleme auswerfen

			//checks if client wants to connect on chatroom
			std::string cmd = buf;
			if (cmd.substr(0, 8) == "/contocr")
			{
				int crCon = std::stoi(cmd.substr(9, 1));

				//if already in a chatroom, diconnect from this one
				for (Chatroom* cr : m_Chatrooms)
				{
					if (cr->inChatroom(m_Sd))
					{
						//client not in same chatroom
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
				if (m_Chatrooms[crCon]->add(m_Sd))
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
				else
				{
					//prints on srv side
					std::cout << "Client " << hostName << ":" << htons(m_AddrOfClient.sin_port) << " is already on this chatroom" << std::endl;
					//sends msg to client who wants to connect
					sendMsgTo(m_Sd, (std::string)"Server: Already on this Chatroom ");
				}
				rcv = -1;
			}
			if(rcv > 0)
				m_RcvMsg = std::make_tuple(buf, m_Sd);	//f�ge message zu RcvMsg hinzu wenns kein command war
		}
	}
	return rcv;
}

bool Server::sendMsgTo(SOCKET s, std::string msg)
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

void Server::addCr(int count)
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

void Server::removeCr(int count)
{
	if (m_Chatrooms.size() >= count)
	{
		int oldSize = m_Chatrooms.size();
		for (int i = 1; i <= count; i++)
		{
			auto clientsOnCr = m_Chatrooms[oldSize - i]->getClients();
			for(SOCKET client : clientsOnCr)
				sendMsgTo(client, "Server: Chatroom " + std::to_string(oldSize - i) + " got closed");	//send disconnect
			std::cout << "Chatroom " + std::to_string(oldSize - i) + " got closed" << std::endl;
			m_Chatrooms.erase(m_Chatrooms.begin() + oldSize - i);
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
	//figuering out on which cr the client is
	for (Chatroom* cr : m_Chatrooms)
	{
		if (cr->inChatroom(std::get<1>(m_RcvMsg)))
		{
			sendTo = cr->sendMsg(std::get<1>(m_RcvMsg));
			break;
		}
	}

	for (unsigned int i = 0; i < sendTo.size(); i++)
	{
		sendMsgTo(sendTo[i],std::get<0>(m_RcvMsg));
	}
	return sended;
}

void Server::cleanUp()
{
	closesocket(m_Listening);
	for (unsigned int i = 0; i < m_Clients.size(); i++)
	{
		closesocket(m_Clients[i]);
	}
	WSACleanup();
}

std::string Server::getMessage()
{
	return std::get<0>(m_RcvMsg);
}

bool Server::createListeningSocket()
{
	m_Listening = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Listening != INVALID_SOCKET)
	{
		std::cout << "Created Socket" << std::endl;
		SOCKADDR_IN addr;
		SecureZeroMemory(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(m_Port);
		inet_pton(AF_INET, m_IpAddress.c_str(), &addr.sin_addr);

		int bindSocket = bind(m_Listening, (sockaddr*)&addr, sizeof(addr));
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

	int listenSocket = listen(m_Listening, SOMAXCONN);
	if (listenSocket == SOCKET_ERROR)
	{
		std::cout << "Couldnt create listening socket	Error code: " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Server listening..." << std::endl;
	return true;
}

void Server::waitForConnection()
{
	//clear the socket set  
	FD_ZERO(&m_Readfds);

	//add listening socket to set
	FD_SET(m_Listening, &m_Readfds);
	//sd socket discriptor
	m_Maxsd = m_Listening;

	//add clients to set
	for (unsigned int i = 0; i < m_Clients.size(); i++)
	{
		//socket descriptor 
		m_Sd = m_Clients[i];

		if (m_Sd > 0)//dont need to do that, checked before, but doppelt h�lt besser
			FD_SET(m_Sd, &m_Readfds);

		//highest file descriptor number, need it for the select function  
		if (m_Sd > m_Maxsd)
			m_Maxsd = m_Sd;
	}
	//first paramter: can be ignored, put a sd in that cant be effected
	//second parameter: An optional pointer to a set of sockets to be checked for readability.
	m_Rdy = select(m_Maxsd + 1, &m_Readfds, NULL, NULL, NULL);
	if (m_Rdy == SOCKET_ERROR);		//dont need to do that cause receive will take care of that later
	else if (m_Rdy == 0)
		std::cout << "Couldnt detect rdy Sockets, because the time limit expired" << std::endl;

	if (FD_ISSET(m_Listening, &m_Readfds))
	{
		m_Client = accept(m_Listening, (sockaddr*)&m_AddrOfClient, &m_ClientSize);

		if (m_Client == INVALID_SOCKET)
		{
			std::cout << "Couldnt accept clientSocket	Error code: " << WSAGetLastError() << std::endl;
		}

		m_Clients.push_back(m_Client);
		char hostName[NI_MAXHOST];
		inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);
		std::cout << hostName << " connected on port " << ntohs(m_AddrOfClient.sin_port) << std::endl;

		sendMsgTo(m_Client, "Chatroom:" + std::to_string(m_Chatrooms.size()));
	}

}
