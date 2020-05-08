#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <iostream>
#include <tuple>
#include "Chatroom.h"

class Server
{
public:
	Server(std::string ip, int port);
	~Server();

	bool init();

	bool createListeningSocket();
	void waitForConnection();

	bool recieve();
	bool sendMsg();

	std::string getMessage();

	void cleanUp();

private:
	//std::vector<Chatroom> m_Chatrooms;
	Chatroom cr = Chatroom(1);

	SOCKET m_Listening;
	
	std::vector<SOCKET> m_Clients;
	SOCKET m_Client;
	SOCKADDR_IN m_AddrOfClient;
	int m_ClientSize;

	std::string m_IpAddress;
	int m_Port;

	std::tuple<std::string, SOCKET> m_RcvMsg;

	//set of socket descriptors  
	fd_set m_Readfds;
	int m_Maxsd;
	SOCKET m_Sd;
	int m_Rdy;
};