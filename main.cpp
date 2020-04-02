#include <iostream>
#include<numeric>
#include<algorithm>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include<vector>
#include<cmath>
#include<fstream>
// GLFW
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;

// Shaders
const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"gl_PointSize=1.5f;\n"
"}\0";
const GLchar* fragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"uniform vec4 u_Color;\n"
"void main()\n"
"{\n"
"color = u_Color;\n"
"}\n\0";

GLuint VBO, VAO;
constexpr int MAXVERTS = 1000;
// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);

	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	const char* glsl_version = "#version 130";


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	bool show_demo_window = true;
	bool show_another_window = false;

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, screenWidth, screenHeight);


	// Build and compile our shader program
	// Vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check for compile time errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Link shaders
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	// Set up vertex data (and buffer(s)) and attribute pointers
	std::ifstream fin("plot.csv");
	std::vector<double> numbers;

	char ch;
	double num;

	while (fin >> num)           // read a number
	{
		numbers.push_back(num);  // store the number 
		fin >> ch;               // read and discard the comma
	}
	// display the result
	std::cout << numbers.size();


	// Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	GLfloat xaxis[4] = { -1.0f,0.f,1.0f,0.f };

	// Game loop


	GLuint AxesVAO,AxesVBO;
	glGenBuffers(1, &AxesVBO);
	glGenVertexArrays(1, &AxesVAO);
	glBindVertexArray(AxesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, AxesVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(xaxis), xaxis,GL_STATIC_DRAW);
	glBindVertexArray(0);
	
	glUseProgram(shaderProgram);
	glEnable(GL_PROGRAM_POINT_SIZE);
	GLuint location = glGetUniformLocation(shaderProgram, "u_Color");
	ImVec4 clear_color = ImVec4(0.82f, 0.0f, 0.23f, 1.00f);
	ImVec4 bg_color = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);

	while (!glfwWindowShouldClose(window))
	{
		static float f = 0.3f;
		static int counter = 0;

		std::vector <GLfloat> vertices;
		for (int i = 0;i < numbers.size();i++) {

			float x = 2 * (float)i / numbers.size() - 1.f + 3*(float)counter/numbers.size();
			float max = *std::max_element(numbers.begin(), numbers.end());
			float y = f * (float)numbers[i] / max;
			vertices.push_back(x);
			vertices.push_back(y);

		}
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)* vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0);
		glUniform4f(location, clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		glBindVertexArray(AxesVAO);
		glClearColor(bg_color.x, bg_color.y, bg_color.z, bg_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_LINES, 0, 4);
		glBindVertexArray(0);
		// Draw our first triangle
		
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, vertices.size());
		glBindVertexArray(0);
		ImGui::Begin("OpenGL CSV Plotter");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Plots the CSV vs Sample number");           
		ImGui::ColorEdit3("BG color", (float*)& bg_color);
		ImGui::SliderFloat("Y Scale", &f, 0.f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("FG color", (float*)& clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Phase++"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		if (ImGui::Button("Phase--"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter--;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Properly de-allocate all resources once they've outlived their purpose
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return EXIT_SUCCESS;
}

