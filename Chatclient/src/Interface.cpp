#include "Interface.h"

Interface::Interface()
{
}

Interface::~Interface()	//Destruktor welche alles regelgerecht terminiert
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

bool Interface::init()	//initialisierung des Interface, benutzen OpenGL als API um auf die Grafikkartenfunktionen zugreifen zu können
{
	//Initialisierung von GLFW, GLFW erstellt für uns ein Applikationsfenster und kümmert sich schon um die Benutztereingaben
	if (!glfwInit())
		return false;

	//Hier geben wir die Version an
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	
	m_Window = glfwCreateWindow(3840, 2160, "Chat", NULL, NULL); //Hier erstellen wir das Fenster

	if (!m_Window)		//Falls das erstellen fehlgeschlagen ist gebe false zurück
	{
		return false;
	}

	//da OpenGL eine State Maschine ist setzten wir vor der update/render Methode alle Werte
	glfwMakeContextCurrent(m_Window);	//Hier setzten wir den OpenGL Kontext, welcher Informationen über Texturen und weiteres beeinhaltet 

	if (glewInit() != GLEW_OK)	//initialisieren GLEW
		return false;

	IMGUI_CHECKVERSION();
	// Setup ImGui binding
	ImGui::CreateContext();
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();


	// set Font
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/Arial.ttf", 26.0f, NULL, io.Fonts->GetGlyphRangesDefault());

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);	//and init glfw	
	ImGui_ImplOpenGL3_Init("#version 130");		//set OpenGL version and init

	return true;
}

void Interface::update()	//wird so oft es geht ausgeführt, komplett Non-blocking
{
	//"säubert" das Fenster
	glClear(GL_COLOR_BUFFER_BIT);

	//Erstellt Frames in denen die Grafiken gerendert werden und auf Userinput gewartet wird
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//Setzt die Farbe der Buttons
	ImGui::PushStyleColor(ImGuiCol_Button, { m_Color[0],m_Color[1],m_Color[2], 255 });

	//wenn das Fenster gezeigt werden soll, zeige ihn
	if (m_ScreensActivated)
		showScreens();

	if (m_ChatConnectionsActivated)
		showChatConnections();

	if (m_LogActivated)
		showLog();

	if (m_ColorSettingsActivated)
		showColorSettings();

	if (m_ExitActivated)
		showExit();

	if (m_LoginActivated)
		showLogin();

	if (m_SendMsgActivated)
		showSendMsg();

	if (m_RcvdMsgActivated)
		showRcvdMsg();

	ImGui::PopStyleColor();	//Farbe für die buttons wieder entfernen um sie für die nächste loop wieder neu konfigurieren zu können

	//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Render();	//rendert alle Fenster indem es alle Vertexbuffer abspeichert
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());		//hier werden die buffer ins Fenster gerendert

	//damit diese Buffer nicht live gerendert werden, benutzt man andere Buffer 
	// um die fertig gerenderten Vertexbuffer in diesem zu speichern und auszutauschen. 
	// Dadurch können die fertig gerenderten Buffer auf dem Monitor "gezeichnet" werden
	glfwSwapBuffers(m_Window);

	//Poll for and process events (Benutzereingabe im Fenster)
	glfwPollEvents();
}

void Interface::reset(const std::string& reason)
{
	//setzt Anfangseinstellungen des Interface, zeigt also die Screens an die am Anfang gezeigt werden sollen
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

void Interface::log(const std::string& msg)	//speichert alle logs in einem log array ab um sie später zu rendern
{
	m_Logs.push_back(msg);
}

void Interface::setChatCount(int crCount)	//speichert Anzahl der Chaträume
{
	m_CrCount = crCount;
}

int Interface::ConnectToCr()	//speichert Chatroomnummer auf den sich der Benutzter verbinden möchte
{
	return m_ConnectToCr;
}

void Interface::showScreens()	//Fenster um status der anderen Fenster einzustellen
{
	ImGui::Begin("Show Screens", &m_ScreensActivated, 0);	//neues Fenster beginnt immer mit ImGui::Begin
	if (ImGui::Button("Receiving Message", { ImGui::GetWindowWidth() - 20 , ImGui::GetWindowHeight()/6-12 }))
		m_RcvdMsgActivated = !m_RcvdMsgActivated;			//wenn auf den Knopf gedrückt wird soll das Fenster geschlossen/geöffnet werden
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

void Interface::showExit()	//zeigt exit button
{
	ImGui::Begin("Exit", &m_ExitActivated, 0);
	
	if (ImGui::Button("Exit", { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() - 50 }))
	{
		m_Exit = true;	//wenn der exit button gedrückt wurde setze exit auf true
	}

	ImGui::End();
}

void Interface::showLogin()	//zeigt login fenster
{
	ImGui::Begin("Login", &m_LoginActivated, 0);
	ImGui::InputText("Username", m_LoginUserName, sizeof(m_LoginUserName));	// input des Benutzers wird in m_LoginUserName gespeichert
	ImGui::InputText("Server IP", m_ServerIp, sizeof(m_ServerIp));
	ImGui::InputText("Server Port", m_ServerPort, sizeof(m_ServerPort));
	ImGui::Dummy({ 0.0f, 20.0f });					//leeres widget um das layout zu gestalten also ein bestimmten Abstand zum Loginbutton zu schaffen
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::Button("Login", { ImGui::GetWindowWidth() - 20, 40 }) || io.KeysDownDuration[GLFW_KEY_ENTER] == 0.0f)	
	{
		//hat sich eingelogt -> m_Logined = true
		m_Logined = true;
		//Wenn der login button gedrückt wurde zeige folgende Fenster
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

void Interface::showRcvdMsg()	//zeigt Empfangene Nachrichten an
{
	ImGui::Begin("Receiving Message", &m_RcvdMsgActivated, 0);
	//geht kompletten cache aller gespeicherten empfangenen Nachrichten durch
	for (std::tuple<std::string, int> msg : m_RcvdSendMessages)
	{
		if (std::get<1>(msg) == RCVDCMSG)	//checkt ob die Nachricht von nem anderen Client gekommen ist
		{
			for (int i = 0; i < std::get<0>(msg).size(); i += 45)	//diese For-loop ist dafür den Text nach einem Bestimmten abstand zu trennen und in der nächsten Zeile weiter zu schreiben
			{
				ImGui::Text(std::get<0>(msg).substr(i, 45).c_str());
			}
			ImGui::Dummy({ 0.0f, 10.0f });	//nur fürs layout
		}
		else if (std::get<1>(msg) == RCVDSMSG)	//checkt ob die Nachricht vom Server gekommen ist
		{
			ImGui::PushStyleColor(ImGuiCol_Text, { m_Color[0],m_Color[1],m_Color[2], 255 });	//highlighted Color vom Server
			for (int i = 0; i < std::get<0>(msg).size(); i += 45)
			{
				ImGui::Text(std::get<0>(msg).substr(i, 45).c_str());
			}
			ImGui::PopStyleColor();
			ImGui::Dummy({0.0f, 10.0f});
		}
		else if (std::get<1>(msg) == SEND)	//checkt ob der Client die Nachricht gesendet hat
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

void Interface::showChatConnections()	//zeigt alle Chaträume an die sich der Client verbinden kann
{
	ImGui::Begin("Connect to Chatrooms", &m_ChatConnectionsActivated, 0);
	ImGui::Text("Connect to... ");
	for (int i = 0; i < m_CrCount; i++)	//geht Chaträume durch und erstellt für jeden einen Button um sich auf ihn verbinden zu können
	{
		std::string cr = "Chatroom " + std::to_string(i);	
		if (ImGui::Button(cr.c_str(), { ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight() / m_CrCount - 20 }))
		{
			m_ConnectToCr = i;	//client wants to connect to chatroom
			break;
		}
		else
		{
			m_ConnectToCr = -1;	// client wants to connect to nothing
		}
	}

	ImGui::End();
}

void Interface::showSendMsg()	//zeigt Fenster indem der Benutzer seine Nachricht eintippen und absenden kann
{
	ImGui::Begin("Sending Message", &m_SendMsgActivated, 0);
	ImGui::InputText("", m_SendTxt, sizeof(m_SendTxt));
	ImGui::SameLine();
	ImGuiIO& io = ImGui::GetIO();	//Keyboard event
	if (ImGui::Button("Send") || io.KeysDownDuration[GLFW_KEY_ENTER] == 0.0f)	//wenn der Send button oder die enter tasten gedückt wurde:
	{
		ImGui::SetKeyboardFocusHere(0);	//setzte Tastaturfokus direkt wieder auf den Textinput
		m_Send = true;
		m_SendedMessages.push_back(m_SendTxt);
		m_RcvdSendMessages.push_back(std::make_tuple(m_SendTxt, SEND));
		SecureZeroMemory(m_SendTxt, sizeof(m_SendTxt));			//leere den Buffer wieder um eine leere Eingabe zu ermöglichen
																//benutzen Secure, da es sein kann dass es im release Mode optimiert wird und dadurch nicht mehr funktioniert 
	}
	else {
		m_Send = false;
	}
	ImGui::End();
}

void Interface::printRcvdMsg(const std::string& rcvdMsg)	//speichert empfangene Nachrichten ab
{
	m_RcvdMessages.push_back(rcvdMsg);		//ein cashe der alle Nachrichten abspeichert (wird nicht weiter gebraucht)
	if(rcvdMsg.substr(0,7) == "Server:")	//wenn die Nachricht vom Server kommt: 
		m_RcvdSendMessages.push_back(std::make_tuple(rcvdMsg, RCVDSMSG));
	else									//wenn die Nachricht vom Client kommt:
		m_RcvdSendMessages.push_back(std::make_tuple(rcvdMsg, RCVDCMSG));
}

bool Interface::sendButtonPressed()	//wurde sendenbutton gedrückt?
{
	return m_Send;
}

std::string Interface::getSendMsg()	//gibt letzte gesendete Nachricht zurück
{
	return m_SendedMessages.back();
}

bool Interface::closeProgram()	//Soll da Programm geschlossen werden? ja/nein
{
	return glfwWindowShouldClose(m_Window) || m_Exit;
}

void Interface::showColorSettings()	//Fenster für Farbeinstellungen
{
	ImGui::Begin("Color Settings", &m_ColorSettingsActivated, 0);
	ImGui::ColorPicker3("Set Color", m_Color, 0);	//Farbeinstellung wird in m_Color gespeichert
	ImGui::End();
}

void Interface::showLog()	//Fenster welches den Log anzeigt
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

int Interface::getServerPort()	//gibt Serverport zurück
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
