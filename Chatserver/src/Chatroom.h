#pragma once

#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <vector>
#include <iostream>

//#define MAX_CLIENTS	16

class Chatroom
{
public:
	Chatroom(int chatRoomId);
	~Chatroom();

	bool add(SOCKET& client);			//& ist egal weil pointer = int = SOCKET aber trotzdem schöner
	bool remove(SOCKET& client);
	bool inChatroom(SOCKET& client);

	std::vector<SOCKET> sendMsg(SOCKET& client);

private:
	int m_ChatroomId;

	std::vector<SOCKET> m_Clients;
	SOCKADDR_IN m_AddrOfClient;
	int m_ClientSize = sizeof(m_AddrOfClient);	//need to do this or getpeername wont work: The returned address is truncated if the buffer provided is too small
};