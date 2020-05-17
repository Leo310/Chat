#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Client.h"

#include <thread>
#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);	//change color of console
static std::string userInput;
static std::string userName;
static int crCount;

//other Threads task
void waitingForMsg(Client client)
{
	while (true)
	{
		client.recieve();
		SetConsoleTextAttribute(hConsole, 10);		//change color of console
		std::cout << client.getMessage() << std::endl;
		SetConsoleTextAttribute(hConsole, 12);		//change color of console
	}
}

void waitingForUserInput(Client client)
{
	while (true)
	{
		//std::cout << "> ";
		std::getline(std::cin, userInput);
		if (userInput.substr(0, 1) == "/")	//if command
		{
			if (userInput.substr(1, 4) == "exit")
				break;
			else if (userInput.substr(1, 7) == "contocr" && userInput.size() > 9)		//noch nicht perfekt eingefügt, man kann immer noch buchstaben eintippen
			{
				int crCon = std::stoi(userInput.substr(9, 1));
				if (crCon < crCount)
					client.sendMsg(userInput);
				else
					std::cout << "Es gibt nur Chatrooms zwischen 0 und " << crCount - 1 << std::endl;
			}
			else
			{
				std::cout << "Es gibt diesen Command nicht. Es gibt nur /contocr (Chatraumnummer) und /exit" << std::endl;
			}
		}
		else
		{
			std::string msg = (std::string)userName + ": " + userInput;
			client.sendMsg(msg);
		}
	}
}

int main()
{

	//CLient logic
	Client client;

	if (!client.init())
		std::cout << "Couldnt init" << std::endl;

	std::cout << "Tippe deinen Namen ein..." << std::endl;
	std::getline(std::cin, userInput);
	userName = userInput;


	//std::string choice = chooseChatroom();
	client.createSocket();
	client.connectToSrv("77.191.118.48", 54000);


	//client.sendMsg(choice);
	client.recieve();		//gets number of chatrooms
	crCount = std::stoi(client.getMessage());
	std::cout << "Du kannst mit /contocr auf die Chatraeume von 0" << "-" << crCount-1 <<  " connecten." << std::endl;


	std::thread rcvWorker(waitingForMsg, std::ref(client));	//arbeit auf threads aufteilen damit der client den userinput und die nachrichten des srv gleichzeitig empfangen kann
	std::thread userInputWorker(waitingForUserInput, std::ref(client));

	SetConsoleTextAttribute(hConsole, 12);		//change color of console

	//Graphics
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	window = glfwCreateWindow(960, 540, "Chat", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	// Setup ImGui binding
	ImGui::CreateContext();
	//Enable Gamepad Controls
	ImGui_ImplGlfwGL3_Init(window, true);
	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	while (!glfwWindowShouldClose(window))
	{
		//Render here
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplGlfwGL3_NewFrame();

		{
			static float f = 0.0f;
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		//Swap front and back buffers
		glfwSwapBuffers(window);

		//Poll for and process events
		glfwPollEvents();
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	userInputWorker.detach();
	rcvWorker.detach();	//need to "destroy" explicitly
	return 0;
}