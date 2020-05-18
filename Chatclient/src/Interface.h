#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"

#include <iostream>
#include <string>
#include <vector>
#include <tuple>


#define SEND 0
#define RCVD 1


class Interface
{
public:
	Interface();
	~Interface();

	bool init();
	void update();

	void setChatCount(int crCount);
	int ConnectTo();

	void printRcvdMsg(std::string rcvdMsg);

	bool logined();
	std::string getUserName();

	bool sendButtonPressed();
	std::string getSendMsg();

	bool closeProgram();

private:

	void showScreens();
	bool m_ScreensActivated = false;


	void showLogin();
	bool m_LoginActivated = true;
	char m_LoginUserName[4096] = "";
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


	void showChatConnections();
	bool m_ChatConnectionsActivated = false;
	int m_CrCount;
	int m_ConnectTo = -1;


	GLFWwindow* m_Window;

	float m_Time = 0.0f;
	ImVec2 displaySize;
};