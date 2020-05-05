#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <iostream>


class Server
{
public:
	Server(std::string ip, int port);
	~Server();

	bool init();
	void run();
	void cleanUp();

private:
	void createSocket();
	void waitForConnection();

	SOCKET m_Listening;

	SOCKET m_Clients;
	sockaddr_in m_AddrOfClients;
	int m_ClientSize;

	std::string m_IpAddress;
	int m_Port;
};