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

	bool createListeningSocket();
	void waitForConnection();

	bool recieve();
	bool sendMsg(const std::string& msg);

	std::string getMessage();

	void cleanUp();

private:
	SOCKET m_Listening;
	
	std::vector<SOCKET> m_Clients;
	SOCKET m_Client;
	SOCKADDR_IN m_AddrOfClient;
	int m_ClientSize;

	std::string m_IpAddress;
	int m_Port;

	char m_RcvMsg[4096];

	//set of socket descriptors  
	fd_set m_Readfds;
	int m_Maxsd;
	int m_Sd;
	int m_Rdy;
};