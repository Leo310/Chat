#pragma once

#include <iostream>
#include <string>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
	Client();
	~Client();

	bool init();
	void connectToSrv(const std::string& srvIp, int srvPort);

	bool sendMsg(const std::string& msg);
	bool recieve();

	std::string getMessage();

	void cleanup();

	void createSocket();

private:
	SOCKET m_Client;

	char m_RcvMsg[4096];

};