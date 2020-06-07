#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <iostream>
#include <string>
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

	int recieve();
	bool sendMsgTo(SOCKET s, std::string msg);

	void addCr(int count);
	void removeCr(int count);
	int getCrCount();
	bool sendMsgCr();

	std::string getMessage();

	void cleanUp();

private:
	std::vector<Chatroom*> m_Chatrooms;

	SOCKET m_Listening;
	
	std::vector<SOCKET> m_Clients;
	SOCKET m_Client;
	SOCKADDR_IN m_AddrOfClient;
	int m_ClientSize = sizeof(m_AddrOfClient);	//need to do this or accept wont work: The returned address is truncated if the buffer provided is too small

	std::string m_IpAddress;
	int m_Port;

	std::tuple<std::string, SOCKET> m_RcvMsg;

	//set of socket descriptors  
	fd_set m_Readfds;
	SOCKET m_Maxsd;
	SOCKET m_Sd;
	int m_Rdy;
};