#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"

#include <Windows.h>	//for SecureZeroMemory

#include <iostream>
#include <string>
#include <vector>
#include <tuple>


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
	void showColorSettings();
	bool m_ColorSettingsActivated = true;
	float m_Color[3] = { 0.176503f, 0.089224f, 0.863850f };

	void showLog();
	bool m_LogActivated = true;
	std::vector<std::string> m_Logs;

	void showScreens();
	bool m_ScreensActivated = false;


	void showExit();
	bool m_ExitActivated = true;
	bool m_Exit = false;

	void showLogin();
	bool m_LoginActivated = true;
	char m_LoginUserName[4096] = "";
	char m_ServerIp[4096] = "";
	char m_ServerPort[4096] = "";
	bool m_Logined = false;


	void showSendMsg();
	bool m_SendMsgActivated = false;
	char m_SendTxt[4096] = "";
	bool m_Send = false;
	std::vector<std::string> m_SendedMessages;


	void showRcvdMsg();
	bool m_RcvdMsgActivated = false;
	std::vector<std::string> m_RcvdMessages;

	std::vector<std::tuple<std::string, int>> m_RcvdSendMessages;	//1. msg, ob msg rcvd oder gesendet

	float m_Scaling = 1.0f;

	void showChatConnections();
	bool m_ChatConnectionsActivated = false;
	int m_CrCount = 0;
	int m_ConnectToCr = -1;


	GLFWwindow* m_Window;

	float m_Time = 0.0f;
	ImVec2 displaySize;
};