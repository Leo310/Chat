#pragma once

#include <WS2tcpip.h>				//Headerfile welches die Funktionsprotypen/Deklarationen enthält welche in der Library enthaten sind
#pragma comment(lib, "ws2_32.lib")	//um auf die Windows Socket Methoden zugreifen zu können brauchen wir die Library

#include <vector>
#include <iostream>					//um dinge in der Konsole auszugeben

//#define MAX_CLIENTS	16

class Chatroom		//ein virtueller chatroom
{
public:
	Chatroom(int chatRoomId);	//konstruktor jeder chatroom hat eine eindeutig identifizierbare ID die vom server zugeordnet wird
	~Chatroom();


	bool add(SOCKET& client);			//& ist egal weil pointer = int = SOCKET aber trotzdem schöner
	bool remove(SOCKET& client);
	bool inChatroom(SOCKET& client);

	std::vector<SOCKET> getClients();
	std::vector<SOCKET> sendMsg(SOCKET& client);

private:
	int m_ChatroomId;

	std::vector<SOCKET> m_Clients;	//speichert alle CLients die auf den Chatraum verbunden sind
};