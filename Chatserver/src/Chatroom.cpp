#include "Chatroom.h"

Chatroom::Chatroom(int chatRoomId)
	: m_ChatroomId(chatRoomId)
{
}

Chatroom::~Chatroom()
{
}

bool Chatroom::add(SOCKET& client)	//f�gt clients zum Chatraum hinzu
{
	bool clientInChatroom = false;

	for (int i = 0; i < m_Clients.size(); i++)	//geht alle clients im CHatraum durch
	{
		if (m_Clients[i] == client) // checkt ob der CLient schon im CHatraum ist
		{
			clientInChatroom = true;
			break;
		}
		clientInChatroom = false;
	}
	if (!clientInChatroom) //wenn er noch nicht drin ist f�g ihn zur arraylist hinzu
	{
		m_Clients.push_back(client);
	}
	return !clientInChatroom;	//wenn er noch nicht im chatroom ist gib true zur�ck, da er jetzt erfolgreich hinzugef�gt wurde
}

bool Chatroom::remove(SOCKET& client)	//entfernt client aus chatraum
{
	bool clientInChatroom = true;
	for(int i = 0; i < m_Clients.size(); i++)	//geht alle clients durch
	{
		if (m_Clients[i] == client)	//wenn vorhanden:
		{
			m_Clients.erase(m_Clients.begin() + i);	//l�sche client, .begin() = index[0] um i nicht extra zu einem iterator zu casten
			clientInChatroom = true;
			break;	//break da der gleiche client nicht doppelt in einem CHatraum vorkommen kann und deswegen nicht weiter geschaut werden muss
		}
		clientInChatroom = false;
	}
	return clientInChatroom;
}

bool Chatroom::inChatroom(SOCKET& client)	//schaut ob der client schon im chatraum sit
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

std::vector<SOCKET> Chatroom::getClients()	//gibt alle clients zur�ck die auf dem chatraum verbunden sind
{
	return m_Clients;
}

std::vector<SOCKET> Chatroom::sendMsg(SOCKET& client)	//gibt alle clients zur�ck au�er dem der als Parameter �bergeben wurde damit der Server die Nachricht besser an die anderen CLients im CHatraum senden kann
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

