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

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(5.0f);

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

	if (m_ScreensActivated)
		showScreens();

	if (m_ChatConnectionsActivated)
		showChatConnections();

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
	
	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	//Swap front and back buffers
	glfwSwapBuffers(m_Window);
	//Poll for and process events
	glfwPollEvents();
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
	ImGui::Begin("All Screens", &m_ScreensActivated, 0);

	if (ImGui::Button("Receiving Message"))
		m_RcvdMsgActivated = true;
	if(ImGui::Button("Sending Message"))
		m_SendMsgActivated = true;
	if(ImGui::Button("Connect to Chatrooms"))
		m_ChatConnectionsActivated = true;

	ImGui::End();
}

void Interface::showLogin()
{
	ImGui::Begin("Login", &m_LoginActivated, 0);
	ImGui::InputText("Username", m_LoginUserName, sizeof(m_LoginUserName));
	ImGui::SameLine();
	if (ImGui::Button("Login"))
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
		if (std::get<1>(msg) == RCVD)
		{
			ImGui::Text(std::get<0>(msg).c_str());
		}
		else if (std::get<1>(msg) == SEND)
		{
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() * 4/5);
			ImGui::Text(std::get<0>(msg).c_str());
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
		if (ImGui::Button(cr.c_str()))
		{
			std::cout << "sdkagjhflk" << std::endl;
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

	if (ImGui::Button("Send"))
	{
		m_Send = true;
		m_SendedMessages.push_back(m_SendTxt);
		m_RcvdSendMessages.push_back(std::make_tuple(m_SendTxt, SEND));
		std::memset(m_SendTxt, 0, sizeof(m_SendTxt));
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

void Interface::printRcvdMsg(std::string rcvdMsg)
{
	m_RcvdMessages.push_back(rcvdMsg);
	m_RcvdSendMessages.push_back(std::make_tuple(rcvdMsg, RCVD));
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
	return glfwWindowShouldClose(m_Window);
}

bool Interface::logined()
{
	return m_Logined;
}

std::string Interface::getUserName()
{
	return m_LoginUserName;
}