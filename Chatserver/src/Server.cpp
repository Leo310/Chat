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

void Server::run()
{
	createSocket();
	waitForConnection();
}

bool Server::recieve()
{
	int received = recv(m_Clients, m_RcvMsg, 4096, 0);
	if (received == SOCKET_ERROR)
		std::cout << "Couldnt receive msg	Error code: " << WSAGetLastError << std::endl;
	else if (received == 0)
		std::cout << "Client diconnected" << std::endl;
	else
		return true;
	return false;
}

bool Server::sendMsg(const std::string& msg)
{
	int sended = send(m_Clients, msg.c_str(), msg.size() + 1, 0);
	if (sended == SOCKET_ERROR)
	{
		std::cout << "Couldnt send msg	Error code: " << WSAGetLastError << std::endl;
		return false;
	}
	return true;
}

void Server::cleanUp()
{
	closesocket(m_Listening);
	closesocket(m_Clients);
	WSACleanup();
}

std::string Server::getMessage()
{
	return m_RcvMsg;
}

void Server::createSocket()
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
		}
	}
	else
	{
		std::cout << "Couldnt create listening socket	Error code: " << WSAGetLastError() << std::endl;
	}
}

void Server::waitForConnection()
{
	int listenSocket = listen(m_Listening, SOMAXCONN);
	if (listenSocket != SOCKET_ERROR)
	{
		std::cout << "Server listening..." << std::endl;

		m_ClientSize = sizeof(m_AddrOfClients);	//need to do this or accept wont work
		m_Clients = accept(m_Listening, (sockaddr*)&m_AddrOfClients, &m_ClientSize);

		if (m_Clients == INVALID_SOCKET)
		{
			std::cout << "Couldnt accept clientSocket	Error code: " << WSAGetLastError() << std::endl;
		}

		char hostName[NI_MAXHOST];
		char service[NI_MAXSERV];

		if (getnameinfo((sockaddr*)&m_Clients, sizeof(m_Clients), hostName, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			std::cout << hostName << " connected on port " << service << std::endl;
		}
		else
		{
			std::cout << "Couldn't getnameinfo	Warning code: " << WSAGetLastError() << std::endl;
			inet_ntop(AF_INET, &m_AddrOfClients.sin_addr, hostName, NI_MAXHOST);
			std::cout << hostName << " connected on port " << ntohs(m_AddrOfClients.sin_port) << std::endl;
		}
	}
}
