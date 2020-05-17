#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <string>

class User
{
public:
	User(std::string userName, std::string pw);
	~User();

private:
	int m_Id;
	std::string m_UserName;
	std::string m_pw;

	SOCKET client;
};