#include "Chatroom.h"

Chatroom::Chatroom(int chatRoomId)
	: m_ChatroomId(chatRoomId)
{
}

Chatroom::~Chatroom()
{
}

bool Chatroom::add(SOCKET& client)
{
	getpeername(client, (sockaddr*)&m_AddrOfClient, &m_ClientSize);	//TODO check return value
	char hostName[NI_MAXHOST];
	inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);//funktioniert irgendwie noch nicht so richtig

	bool clientInChatroom = false;

	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i] == client)
		{
			clientInChatroom = true;
			break;
		}
		clientInChatroom = false;
	}
	if (!clientInChatroom)
	{
		m_Clients.push_back(client);
	}
	return !clientInChatroom;
}

bool Chatroom::remove(SOCKET& client)
{
	getpeername(client, (sockaddr*)&m_AddrOfClient, &m_ClientSize);	//TODO check return value
	char hostName[NI_MAXHOST];
	inet_ntop(AF_INET, &m_AddrOfClient.sin_addr, hostName, NI_MAXHOST);

	bool clientInChatroom = true;
	for(int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i] == client)
		{
			m_Clients.erase(m_Clients.begin() + i);
			clientInChatroom = true;
			break;
		}
		clientInChatroom = false;
	}
	return clientInChatroom;
}

bool Chatroom::inChatroom(SOCKET& client)
{
	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i] == client)
		{
			return true;
		}
	}
	return false;
}

std::vector<SOCKET> Chatroom::getClients()
{
	return m_Clients;
}

std::vector<SOCKET> Chatroom::sendMsg(SOCKET& client)
{
	std::vector<SOCKET> sendTo;
	for (int i = 0; i < m_Clients.size(); i++)
	{
		if (m_Clients[i] != client)
		{
			sendTo.push_back(m_Clients[i]);
		}	
	}
	return sendTo;
}

