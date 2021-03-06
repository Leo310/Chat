#include "Interface.h"

Interface::Interface()
{
}

Interface::~Interface()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool Interface::init()
{
	if (!glfwInit())
		return false;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow(3840, 2160, "Chat", NULL, NULL);
	if (!m_Window)
	{
		return false;
	}

	displaySize = { 960, 540 };

	glfwMakeContextCurrent(m_Window);

	if (glewInit() != GLEW_OK)
		return false;

	IMGUI_CHECKVERSION();
	// Setup ImGui binding
	ImGui::CreateContext();
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();


	// Enable Keyboard Controls
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 26.0f, NULL, io.Fonts->GetGlyphRangesDefault());

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	return true;
}

void Interface::update()
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = displaySize;

	/*float time = (float)glfwGetTime();
	io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
	m_Time = time;*/
	//Render here
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();
	//static bool show = true;
	//ImGui::ShowDemoWindow(&show);
	ImGui::PushStyleColor(ImGuiCol_Button, { m_Color[0],m_Color[1],m_Color[2], 255 });

	if (m_ScreensActivated)
		showScreens();

	if (m_ChatConnectionsActivated)
		showChatConnections();

	if (m_LogActivated)
		showLog();

	if (m_ColorSettingsActivated)
		showColorSettings();

	if (m_ExitActivated)
	{
		showExit();
	}

	if (m_LoginActivated)
	{
		showLogin();
	}

	if (m_SendMsgActivated)
	{
		showSendMsg();
	}

	if (m_RcvdMsgActivated)
	{
		showRcvdMsg();
	}
	ImGui::PopStyleColor();

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//Swap front and back buffers
	glfwSwapBuffers(m_Window);
	//Poll for and process events
	glfwPollEvents();
}

void Interface::reset(const std::string& reason)
{
	log(reason);
	m_LogActivated = true;
	m_ColorSettingsActivated = true;
	m_ScreensActivated = false;
	m_ExitActivated = true;
	m_LoginActivated = true;
	m_Logined = false;
	m_SendMsgActivated = false;
	m_RcvdMsgActivated = false;
	m_ChatConnectionsActivated = false;
}

void Interface::log(const std::string& msg)
{
	m_Logs.push_back(msg);
}

void Interface::setChatCount(int crCount)
{
	m_CrCount = crCount;
}

int Interface::ConnectTo()
{
	return m_ConnectTo;
}

void Interface::showScreens()
{
	ImGui::Begin("Show Screens", &m_ScreensActivated, 0);
	if (ImGui::Button("Receiving Message", { ImGui::GetWindowWidth() - 20 , ImGui::GetWindowHeight()/6-12 }))
		m_RcvdMsgActivated = !m_RcvdMsgActivated;
	if(ImGui::Button("Sending Message", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / 6 - 12 }))
		m_SendMsgActivated = !m_SendMsgActivated;
	if(ImGui::Button("Connect to Chatrooms", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / 6 - 12 }))
		m_ChatConnectionsActivated = !m_ChatConnectionsActivated;
	if (ImGui::Button("Exit", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / 6 - 12 }))
		m_ExitActivated = !m_ExitActivated;
	if (ImGui::Button("Log", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / 6 - 12 }))
		m_LogActivated = !m_LogActivated;
	if (ImGui::Button("Color Settings", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / 6 - 12 }))
		m_ColorSettingsActivated = !m_ColorSettingsActivated;
	ImGui::End();
}

void Interface::showExit()
{
	ImGui::Begin("Exit", &m_ExitActivated, 0);
	
	if (ImGui::Button("Exit", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() - 50 }))
	{
		m_Exit = true;
	}

	ImGui::End();
}

void Interface::showLogin()
{
	ImGui::Begin("Login", &m_LoginActivated, 0);
	ImGui::InputText("Username", m_LoginUserName, sizeof(m_LoginUserName));
	ImGui::InputText("Server IP", m_ServerIp, sizeof(m_ServerIp));
	ImGui::InputText("Server Port", m_ServerPort, sizeof(m_ServerPort));
	ImGui::Dummy({ 0.0f, 20.0f });
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::Button("Login", { ImGui::GetWindowWidth() - 20, 40 }) || io.KeysDownDuration[GLFW_KEY_ENTER] == 0.0f)
	{
		m_Logined = true;
		m_SendMsgActivated = true;
		m_RcvdMsgActivated = true;
		m_ScreensActivated = true;
		m_ChatConnectionsActivated = true;
		m_LoginActivated = false;
	}
	else
		m_Logined = false;
	ImGui::End();
}

void Interface::showRcvdMsg()
{
	ImGui::Begin("Receiving Message", &m_RcvdMsgActivated, 0);
	for (std::tuple<std::string, int> msg : m_RcvdSendMessages)
	{
		if (std::get<1>(msg) == RCVDCMSG)
		{
			for (int i = 0; i < std::get<0>(msg).size(); i += 45)
			{
				ImGui::Text(std::get<0>(msg).substr(i, 45).c_str());
			}
			ImGui::Dummy({ 0.0f, 10.0f });
		}
		else if (std::get<1>(msg) == RCVDSMSG)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { m_Color[0],m_Color[1],m_Color[2], 255 });
			for (int i = 0; i < std::get<0>(msg).size(); i += 45)
			{
				ImGui::Text(std::get<0>(msg).substr(i, 45).c_str());
			}
			ImGui::PopStyleColor();
			ImGui::Dummy({0.0f, 10.0f});
		}
		else if (std::get<1>(msg) == SEND)
		{
			for (int i = 0; i < std::get<0>(msg).size(); i += 30)
			{
				ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 3 / 7);
				ImGui::Text(std::get<0>(msg).substr(i, 30).c_str());
			}
			ImGui::Dummy({ 0.0f, 10.0f });
		}
		ImGui::SetScrollHere(0.999f);
	}
	ImGui::End();
}

void Interface::showChatConnections()
{
	ImGui::Begin("Connect to Chatrooms", &m_ChatConnectionsActivated, 0);
	ImGui::Text("Connect to... ");
	for (int i = 0; i < m_CrCount; i++)
	{
		std::string cr = "Chatroom " + std::to_string(i);
		if (ImGui::Button(cr.c_str(), { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / m_CrCount - 20 }))
		{
			m_ConnectTo = i;	//client wants to connect to chatroom
			break;
		}
		else
		{
			m_ConnectTo = -1;	// client wants to connect to nothing
		}
	}

	ImGui::End();
}

void Interface::showSendMsg()
{
	ImGui::Begin("Sending Message", &m_SendMsgActivated, 0);
	ImGui::InputText("", m_SendTxt, sizeof(m_SendTxt));
	ImGui::SameLine();
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::Button("Send") || io.KeysDownDuration[GLFW_KEY_ENTER] == 0.0f)
	{
		ImGui::SetKeyboardFocusHere(0);
		m_Send = true;
		m_SendedMessages.push_back(m_SendTxt);
		m_RcvdSendMessages.push_back(std::make_tuple(m_SendTxt, SEND));
		SecureZeroMemory(m_SendTxt, sizeof(m_SendTxt));
	}
	else {
		m_Send = false;
	}

	/*for (std::string msg : m_SendedMessages)
	{
		ImGui::Text(msg.c_str());
	}*/
	ImGui::End();
}

void Interface::printRcvdMsg(const std::string& rcvdMsg)
{
	m_RcvdMessages.push_back(rcvdMsg);
	if(rcvdMsg.substr(0,7) == "Server:")
		m_RcvdSendMessages.push_back(std::make_tuple(rcvdMsg, RCVDSMSG));
	else
		m_RcvdSendMessages.push_back(std::make_tuple(rcvdMsg, RCVDCMSG));
}

bool Interface::sendButtonPressed()
{
	return m_Send;
}

std::string Interface::getSendMsg()
{
	return m_SendedMessages.back();
}

bool Interface::closeProgram()
{
	return glfwWindowShouldClose(m_Window) || m_Exit;
}

void Interface::showColorSettings()
{
	ImGui::Begin("Color Settings", &m_ColorSettingsActivated, 0);
	ImGui::ColorPicker3("Set Color", m_Color, 0);
	/*ImGuiStyle& style = ImGui::GetStyle();
	if (ImGui::Button("+"))
	{
		m_Scaling += 0.2f;
		style.ScaleAllSizes(m_Scaling);
	}
	if (ImGui::Button("-"))
	{
		m_Scaling -= 0.2f;
		style.ScaleAllSizes(1.0f/m_Scaling);
	}*/
	ImGui::End();
}

void Interface::showLog()
{
	ImGui::Begin("Log", &m_LogActivated, 0);
	for (std::string msg : m_Logs)
	{
		ImGui::Text(msg.c_str());
		ImGui::SetScrollHere(0.999f);
	}
	ImGui::End();
}

bool Interface::logined()
{
	return m_Logined;
}

std::string Interface::getUserName()
{
	return m_LoginUserName;
}

std::string Interface::getServerIp()
{
	return m_ServerIp;
}

int Interface::getServerPort()
{
	try
	{
		return std::stoi(m_ServerPort);
	}
	catch (std::invalid_argument error)
	{
		return -1;
	}
}
