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

	bool sendMsgCr();

	std::string getMessage();

	void cleanUp();

private:
	//std::vector<Chatroom> m_Chatrooms;
	Chatroom cr0 = Chatroom(0);
	Chatroom cr1 = Chatroom(1);
	Chatroom cr2 = Chatroom(2);
	//Chatroom cr3 = Chatroom(3);

	std::vector<Chatroom*> m_Chatrooms = { &cr0, &cr1, &cr2 };

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
	int m_Maxsd;
	SOCKET m_Sd;
	int m_Rdy;
};