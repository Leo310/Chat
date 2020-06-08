#pragma once

#include <iostream>					//um dinge in der Konsole auszugeben
#include <string>					//um die String Klasse der C++ Stanartlibrary zu benutzen

#include <WS2tcpip.h>				//Headerfile welches die Funktionsprotypen/Deklarationen enthält welche in der Library enthaten sind
#pragma comment(lib, "ws2_32.lib")	//um auf die Windows Socket Methoden zugreifen zu können brauchen wir die Library

#include "Interface.h"	//nur um auf die log Methode zugreifen zu können

#define INVALID_IP -1
#define INVALID_PORT 0
#define COULDNT_CONNECT 2


class Client
{
public:
	Client();
	~Client();

	bool init();
	int connectToSrv(const std::string& srvIp, int srvPort);

	bool sendMsg(const std::string& msg);
	bool recieve();

	std::string getMessage();

	void cleanup();

	void createSocket();

private:
	SOCKET m_Client;

	char m_RcvMsg[4096];

};