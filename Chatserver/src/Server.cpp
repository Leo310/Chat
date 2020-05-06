#include "Server.h"

Server::Server(std::string ip, int port)
	: m_IpAddress(ip), m_Port(port)
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

bool Server::recieve()
{
	bool rcv = false;
	for (int i = 0; i < m_Clients.size(); i++)
	{
		m_Sd = m_Clients[i];
		if (FD_ISSET(m_Sd, &m_Readfds))
		{
			int received = recv(m_Clients[i], m_RcvMsg, 4096, 0);
			getpeername(m_Sd, (sockaddr*)&m_AddrOfClient, &m_ClientSize);	//TODO check return value

			char hostName[NI_MAXHOST];
			inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);

			if (received == SOCKET_ERROR)
			{
				std::cout << hostName << ":" << ntohs(m_AddrOfClient.sin_port) << " didnt disconnect succesfully	Error code: " << WSAGetLastError() << std::endl;
				m_Clients.erase(m_Clients.begin() + i);
				closesocket(m_Sd);
			}
			else if (received == 0)
			{
				std::cout << hostName << ":" << ntohs(m_AddrOfClient.sin_port) << " Client diconnected" << std::endl;
				m_Clients.erase(m_Clients.begin() + i);
				closesocket(m_Sd);
			}
			else
				rcv = true;			//könnte bei mehreren clients die gleichzeitig etwas schicken probleme auswerfen
		}
	}
	return rcv;
}

bool Server::sendMsg(const std::string& msg)
{
	bool sended = false;
	for (int i = 0; i < m_Clients.size(); i++)
	{
		int sended = send(m_Clients[i], msg.c_str(), msg.size() + 1, 0);
		if (sended == SOCKET_ERROR)
		{
			std::cout << "Couldnt send msg	Error code: " << WSAGetLastError() << std::endl;
			sended = false;
		}
		sended = true;
	}
	return sended;
}

void Server::cleanUp()
{
	closesocket(m_Listening);
	for (int i = 0; i < m_Clients.size(); i++)
	{
		closesocket(m_Clients[i]);
	}
	WSACleanup();
}

std::string Server::getMessage()
{
	return m_RcvMsg;
}

bool Server::createListeningSocket()
{
	m_Listening = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Listening != INVALID_SOCKET)
	{
		std::cout << "Created Socket" << std::endl;
		SOCKADDR_IN addr;
		ZeroMemory(&addr, sizeof(addr));
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
	for (int i = 0; i < m_Clients.size(); i++)
	{
		//socket descriptor 
		m_Sd = m_Clients[i];

		if (m_Sd > 0)//dont need to do that, checked before, but doppelt hält besser
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
		m_ClientSize = sizeof(m_AddrOfClient);	//need to do this or accept wont work
		m_Client = accept(m_Listening, (sockaddr*)&m_AddrOfClient, &m_ClientSize);

		if (m_Client == INVALID_SOCKET)
		{
			std::cout << "Couldnt accept clientSocket	Error code: " << WSAGetLastError() << std::endl;
		}

		char hostName[NI_MAXHOST];
		char service[NI_MAXSERV];
		
		if (getnameinfo((sockaddr*)&m_Client, sizeof(m_Client), hostName, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			std::cout << hostName << " connected on port " << service << std::endl;
		}
		else
		{
			std::cout << "Couldn't getnameinfo	Warning code: " << WSAGetLastError() << std::endl;
			inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);
			std::cout << hostName << " connected on port " << ntohs(m_AddrOfClient.sin_port) << std::endl;
		}
		m_Clients.push_back(m_Client);
	}

}
