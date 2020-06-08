#include "Client.h"

extern Interface gui;	//um auf log Methode zugreifen zu können, damit kann diese Klasse Dinge im Interface(gui) ausgeben

Client::Client()	//Konstruktor
	: m_Client(0), m_RcvMsg("")
{
}

Client::~Client()	//wenn der Destruktor aufgerufen wird (wenn das Client Objekt im Stack außerhalb vom Scope kommt) soll er die cleanup Methode aufrufen
{
	cleanup();
}

bool Client::init()
{
	// initialisiere winsock um auf die Winsocket funktionen ausführen zu können
	WSAData data;
	return !WSAStartup(MAKEWORD(2, 2), &data);	//wenn erfoglreich initialisiert gebe true zurück
}

int Client::connectToSrv(const std::string& srvIp, int srvPort)	//verbindet sich mit einen
{
	//Serverdaten füllen mit denen man sich verbinden will
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;	//setzt IP "Familiy" auf IPv4
	if (inet_pton(AF_INET, srvIp.c_str(), &serverAddr.sin_addr) <= 0)	//checkt ob die ip eine gültige IPv4 Adresse ist
	{
		closesocket(m_Client);			//da die ip nicht gültig ist terminiere den erstellten socket
		gui.log("Destroyed Socket");	
		return INVALID_IP;
	}

	if (srvPort != -1)							//checkt ob der angegeben port gültig ist
		serverAddr.sin_port = htons(srvPort);	//htons = unsigned short integer HostShort from host byte order TO Network byte order
	else										//wenn der angegebene port ungültig ist, terminiere erstellten socket
	{
		closesocket(m_Client);
		gui.log("Destroyed Socket");
		return INVALID_PORT;
	}

	gui.log("Connecting to srv...");	
	int connectSrv = connect(m_Client, (sockaddr*)&serverAddr, sizeof(serverAddr));	//Verbindet sich hier mit dem Server

	if (connectSrv == SOCKET_ERROR)	//wenn die Verbindung fehlgeschlagen ist
	{
		gui.log("Cant connect to Srv	Error code: " + WSAGetLastError());
		closesocket(m_Client);
		gui.log("Destroyed Socket");
		return COULDNT_CONNECT;
	}
	else
	{
		gui.log("Connected successfully to Server " + srvIp + ":" + std::to_string(srvPort));
		return true;
	}
}

bool Client::sendMsg(const std::string& msg)	//sendet eine Nachricht an den Server
{
	int sended = send(m_Client, msg.c_str(), msg.size() + 1, 0);	//Sendet Nachricht
	if (sended == SOCKET_ERROR)	//Wenn Nachricht senden fehl geschlagen ist
	{
		gui.log("Couldnt send msg	Error code: " + WSAGetLastError());
		return false;
	}
	return true;
}

bool Client::recieve()	//empfängt nachrichten vom Server
{
	int received = recv(m_Client, m_RcvMsg, 4096, 0);	//empfängt Nachrichten und speicher sie in dem Buffer m_RcvMsg
	if (received == SOCKET_ERROR)						//wenn das Empfangen fehl geschlagen ist
		gui.log("Couldnt receive msg	Error code: " + WSAGetLastError());
	else if (received == 0)
		gui.log("Server diconnected");
	else
		return true;
	return false;
}

std::string Client::getMessage()
{
	return m_RcvMsg;	//gibt empfangene Nachricht zurück
}

void Client::cleanup()	//Räumt das Programm auf
{
	closesocket(m_Client);	
	WSACleanup();
	gui.log("CleanedUp");
}

void Client::createSocket()	//erstellt Socket mit dem sich der Client mit dem Server verbindet
{
	m_Client = socket(AF_INET, SOCK_STREAM, 0);	//erstellt ein Socket, AF_INET = IPv4, SOCK_STREAM = TCP Protocoll
	if (m_Client == INVALID_SOCKET)				//wenn das erstellen des Sockets fehl geschlagen sit
		gui.log("Couldn't create Socket	Error code: " + WSAGetLastError());
	else 
	{
		gui.log("Created Socket");
	}
}
