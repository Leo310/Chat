#pragma once

#include <iostream>

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
	Client();
	~Client();

	bool init();
	void connectToSrv(const std::string& srvIp, int srvPort);

	void cleanup();

	void createSocket();
private:

	SOCKET m_Client;

};