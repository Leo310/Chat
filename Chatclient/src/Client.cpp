#include "Client.h"

Client::Client()
{
}

Client::~Client()
{
	cleanup();
}

bool Client::init()
{
	WSAData data;
	return !WSAStartup(MAKEWORD(2, 2), &data);
}

void Client::connectToSrv(const std::string& srvIp, int srvPort)
{
	//filling server addr
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(srvPort);
	inet_pton(AF_INET, srvIp.c_str(), &serverAddr.sin_addr);

	std::cout << "Connecting to srv..." << std::endl;
	int connectSrv = connect(m_Client, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (connectSrv == SOCKET_ERROR)
	{
		std::cout << "Cant connect to Srv	Error code: " << WSAGetLastError() << std::endl;
	}
	else
	{
		std::cout << "Connected successfully to Server " << srvIp << ":" << srvPort << std::endl;
	}
}

bool Client::sendMsg(const std::string& msg)
{
	int sended = send(m_Client, msg.c_str(), msg.size() + 1, 0);
	if (sended == SOCKET_ERROR)
	{
		std::cout << "Couldnt send msg	Error code: " << WSAGetLastError << std::endl;
		return false;
	}
	return true;
}

bool Client::recieve()
{
	int received = recv(m_Client, m_RcvMsg, 4096, 0);
	if (received == SOCKET_ERROR)
		std::cout << "Couldnt receive msg	Error code: " << WSAGetLastError << std::endl;
	else if (received == 0)
		std::cout << "Server diconnected" << std::endl;
	else
		return true;
	return false;
}

std::string Client::getMessage()
{
	return m_RcvMsg;
}

void Client::cleanup()
{
	closesocket(m_Client);
	WSACleanup();
}

void Client::createSocket()
{
	m_Client = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Client == INVALID_SOCKET)
		std::cout << "Couldn't create Socket	Error code: " << WSAGetLastError() << std::endl;
	else 
	{
		std::cout << "Created Socket" << std::endl;
	}
}
