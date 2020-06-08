#include "Client.h"
#include "Interface.h"

//Threading um mehrere Blocking IOs non-blocking zu machen
//Der client wartet zum einen auf Nachrichten vom Server, muss aber gleichzeitig Userinput verarbeiten und
//Nachrichten an den Server senden
#include <thread>	

Client client;
Interface gui;	//musste leider in Globalen Scope damit ich im client auch im Interface Log einträge machen kann

//global in dieser Datei (static) damit ich von mehreren Threads auf die selben Variablen zugreifen kann
static std::string rcvMsg;	//speichert die Nachricht von Server in diesem String
static bool rcvdMsg = false;	//ist eine Nachricht angekommen?

static std::string stopOptimizeThisWhileLoop; //benutzte den string damit die while true loop im release mode beim builden vom Projekt nicht weg optimized wird

//Methode die auf dem anderen Thread laufen soll, wartet auf Nachrichten vom Server
//nimmt als Parameter ein client reference (erstellt keine copie, übernimmt nur pointer) 
void waitingForMsg(Client& client)
{
	while (true)	//solanger der thread nicht detached oder "beendet" wird, warte auf Nachrichten vom Server
	{
		client.recieve();	//blocking Input, blockt solange bis eine Nachricht angekommen ist
		rcvMsg = client.getMessage();
		rcvdMsg = true;					//eine Nachricht ist angekommen
		while (rcvdMsg == true)			//wartet solange auf anderen Thread bis er die Nachricht
										//verarbeitet hat und rcvdMsg auf false setzt
			stopOptimizeThisWhileLoop = "Hope it wont get optimized";	//die while loop würde im release mode vom compiler weg optimiert werden
	}
}


int main()
{
	//lässt Konsole verschwinden
	FreeConsole();

	bool connected = false;

	if (!gui.init())	// Initializiert Interface/GUI
		return -1;

	if (!client.init())	// initialisiert Client
		gui.log("Couldnt init Winsock");

	std::thread rcvWorker;	//deklariert anderen Thread

	while (!gui.closeProgram())	//solgane das Programm nicht geschlossen wird mache:
	{
		if (gui.logined())		//wenn sich der client schon eingelogt hat:
		{
			if (!connected)		//wenn er noch nicht mit dem Server verbunden ist:
			{
				client.createSocket(); //client erstellt Socket mit dem er sich mit dem Server verbindet
				int connect = client.connectToSrv(gui.getServerIp(), gui.getServerPort()); //connected sich mit dem Server und speichert den verbindungstatus
				switch (connect)	//fragt status ab
				{
				case INVALID_IP:	//falsche IP eingegeben
					gui.reset("Invalid IP");	//reset Interface, mit Error nachricht
					gui.update();				//update Userinterface
					connected = false;			//nicht connected
					break;
				case INVALID_PORT:
					gui.reset("Invalid Port");	//reset Interface, mit Error nachricht
					gui.update();
					connected = false;
					break;
				case COULDNT_CONNECT:
					gui.reset("Couldnt connect to srv");	//reset Interface, mit Error nachricht
					gui.update();
					connected = false;
					break;
				case true:		//wenn connected:
					rcvWorker = std::thread(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann)
					connected = true;	//hat sich verbunden, = true
				}
			}	
			else	//wenn client schon mit einem Server verbunden ist
			{
				if (gui.sendButtonPressed())	//wenn im Interface auf den senden button oder enter gedrückt wurde:
				{
					std::string msg = (std::string)gui.getUserName() + ": " + gui.getSendMsg();	//setzt Nachricht zusammen, mit vorher festgelegtem Benutzternamen
					client.sendMsg(msg);	//client senden diese Nachricht an den Server
				}
				if (gui.ConnectToCr() >= 0)	//wenn der Benutzter auf ein Chatroom verbinden möchte
				{
					client.sendMsg("/contocr " + std::to_string(gui.ConnectToCr()));	//sendet Nachricht an Server mit /contocr (connect to chatroom) command und der zu verbindenden Chatraumnummer
				}
			}
		}
		if (rcvdMsg == true)	//wenn im anderen Thread eine Nachricht angekommen ist
		{
			if (rcvMsg.substr(0, 9) == "Chatroom:")	//wenn Nachricht vom Server mit chatroom beginnt:
			{
				gui.setChatCount(std::stoi(rcvMsg.substr(9, 10)));	//speicher Anzahl der Chatrooms
				gui.log("Chatroom Number: " + rcvMsg.substr(9, 10));//log anzahl an Chaträumen
			}
			else
			{
				gui.printRcvdMsg(rcvMsg);	//sonst gib die Nachricht im Empfangenfenster aus
			}
			rcvdMsg = false;	//Nachricht verarbeitet -> anderer Thread kann wieder Nachrichten empfangen
		}

		gui.update();		//update Interface, Rendering...
	}

	//wenn Programm geschlossen wurde:
	rcvWorker.detach();	//need to "destroy" explicitly, sonst kann es zur Fehlermeldung kommen, da der andere Thread nicht regelkonform beendet wurde
	gui.~Interface();	//rufe destructor vom Interface auf um Cleanup zu starten
	return 0;		//Ende des Programms
}