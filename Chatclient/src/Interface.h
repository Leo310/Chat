#pragma once

#include <GL/glew.h>	//GLEW ist eine Library die nur dafür, da ist alle Deklarationen für die Funktionen welche auf der Grafikkarte vorhanden sind bereitzustellen
#include <GLFW/glfw3.h>	//GLFW kümmert sich um den UserInput und darum ein Fenster zu erstellen, indem die ganzen Vertexbuffer gerendert werden

#include "imgui.h"		//wir benutzten die ImGui library, ist für das einfache erstellen von Fenstern zuständig
#include "examples/imgui_impl_opengl3.h"	//enthält ImGUi Methoden für OpenGL
#include "examples/imgui_impl_glfw.h"		//enthält ImGUi Methoden für GLFW

#include <Windows.h>	//für SecureZeroMemory, um textbuffer zu überschreiben welche sonst im Interface gerendert werden

#include <iostream>		//um dinge in der Konsole ausgeben zu können
#include <string>		//Stringklasse der Standartlibrary
#include <vector>		//vector = Arraylist in Java
#include <tuple>		//um 2 Werte welche zueinander gehören in zum Beispiel einem array speichern zu können


#define SEND 0
#define RCVDCMSG 1
#define RCVDSMSG 2


class Interface
{
public:
	Interface();
	~Interface();

	bool init();
	void update();

	void reset(const std::string& reason);

	void log(const std::string& msg);

	void setChatCount(int crCount);
	int ConnectToCr();

	void printRcvdMsg(const std::string& rcvdMsg);

	bool logined();
	std::string getUserName();
	std::string getServerIp();
	int getServerPort();

	bool sendButtonPressed();
	std::string getSendMsg();

	bool closeProgram();

private:
	//das Fenster welches uns GLFW bereitstellt
	GLFWwindow* m_Window;

	//erstellt Fenster, welches die Farbeinstellungen zeigt
	void showColorSettings();
	bool m_ColorSettingsActivated = true;	//diese Variable ist bei allen Fenstern vorhanden um den den Status zu speichern ob das Fenster geöffnet oder geschlossen ist
	float m_Color[3] = { 0.176503f, 0.089224f, 0.863850f };	//Farben die am Anfang ausgewählt sein können

	//erstellt Fenster, welches den Log anzeigt, hier werden die Texte angezeigt, welche durch die log() Methode im Buffer m_Logs gespeichert werden
	void showLog();
	bool m_LogActivated = true;
	std::vector<std::string> m_Logs;

	//erstellt Fenster, welches alle anderen Fenster anzeigt um sie zu schließen oder erneut zu öffnen
	void showScreens();
	bool m_ScreensActivated = false;

	//erstellt Fenster, welches ein Exit Button rendert um das Programm zu schließen
	void showExit();
	bool m_ExitActivated = true;
	bool m_Exit = false;

	//erstellt Fenster, welches den Login rendert und auf userInput wartet
	void showLogin();
	bool m_LoginActivated = true;
	char m_LoginUserName[4096] = "";
	char m_ServerIp[4096] = "";
	char m_ServerPort[4096] = "";
	bool m_Logined = false;

	//erstellt Fenster, welches für das Senden von Nachrichten zuständig ist
	void showSendMsg();
	bool m_SendMsgActivated = false;
	char m_SendTxt[4096] = "";
	bool m_Send = false;
	std::vector<std::string> m_SendedMessages;	//speichert gesendete Nachrichten ab

	//erstellt Fenster, welches die Empfangenen Nachrichten anzeigt
	void showRcvdMsg();	
	bool m_RcvdMsgActivated = false;
	std::vector<std::string> m_RcvdMessages; //speichert empfangene Nachrichten

	std::vector<std::tuple<std::string, int>> m_RcvdSendMessages;	//1. msg, ob msg rcvd oder gesendet

	float m_Scaling = 1.0f;	//Größe der Fenster

	//erstellt Fenster, welches alle Chatrooms anzeigt auf denen man sich verbinden kann
	void showChatConnections();	
	bool m_ChatConnectionsActivated = false;
	int m_CrCount = 0;	//Anzahl der vorhandenen Chatrooms
	int m_ConnectToCr = -1;	//auf welchen chatroom sich der Benutzer verbinden möchte

};