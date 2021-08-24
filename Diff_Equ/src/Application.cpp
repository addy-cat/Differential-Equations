#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//must be multiples of 4
#define NUM_LINES 200

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

static unsigned int CompileShader(unsigned int type, const std::string& source) {

	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	//i = integer, v = vector/array
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "Vertex" : (type == GL_FRAGMENT_SHADER ? "fragment" : "geometry")) << "shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader) {

	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
	unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glAttachShader(program, gs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteShader(gs);

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(2160, 2160, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
	}

	std::cout << "Your OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	//allocate how many lines? we are allowed to render
	float* positions = (float*)alloca((NUM_LINES * 2) * sizeof(float));
	//float* equation_positions = (float*)alloca((NUM_LINES * 2) * sizeof(float));

	
	//draw y lines 
	float x_coord = -1.0f;
	for (int i = 0; i < NUM_LINES; i = i + 4) {
		positions[i] = x_coord; //x1
		positions[i + 1] = 1.0f; //y1
		positions[i + 2] = x_coord; //x2
		positions[i + 3] = -1.0f; //y2
		x_coord += float(8.0f / (float)NUM_LINES);
		std::cout << x_coord << std::endl;
	}

	
	//draw x lines 
	float y_coord = 1.0f;
	for (int i = NUM_LINES; i < NUM_LINES * 2; i = i + 4) {
		positions[i] = -1.0f;  //x1
		positions[i + 1] = y_coord;  //y1
		positions[i + 2] = 1.0f; //x2
		positions[i + 3] = y_coord;  //y2
		y_coord += float(-8.0f / (float)NUM_LINES);
		std::cout << y_coord << std::endl;
	}

	//make a test line/ equation before introducing math parser
	float* test_x1 = (float*)malloc(sizeof(float));
	*test_x1 = 0.3f;
	float* test_y1 = (float*)malloc(sizeof(float));
	*test_y1 = 0.5f;
	float* test_x2 = (float*)malloc(sizeof(float));
	*test_x2 = 0.5f;
	float* test_y2 = (float*)malloc(sizeof(float));
	*test_y2 = 1.0f;
	float arr[4] = { *test_x1, *test_y1, *test_x2, *test_y2 };

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"}\n";

	std::string fragmentShader =
		"#version 330 core\n"
		"\n"
		"layout(location = 0) out vec4 color;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	color = vec4(1.0, 0.7529, 0.7960, 1.0);\n"
		"}\n";

	std::string geometryShader =
		"#version 330 core\n"
		"\n"
		"layout(lines) in;\n"
		"layout(line_strip, max_vertices = 256) out;\n"
		"\n"

		"uniform vec2 coord_one;\n"
		"uniform vec2 coord_two;\n"

		"void main()\n"
		"{\n"
		"int x = int(1000 * gl_in[0].gl_Position.x);\n"
		"int y = int(1000 * gl_in[0].gl_Position.y);\n"

		"//if we are at the middle x axis\n"
		"if(x == 0 && y != 0)\n"
		"{\n"
		"gl_Position = gl_in[0].gl_Position + vec4(0.00075f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.00075f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.001f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.001f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.00199f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.00199f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0023f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0023f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0037f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0037f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0045f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0045f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0055f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0055f, 0.0f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"


		"} else if (y == 0 && x != 0)\n"
		"{\n"
		"gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.00075f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.00075f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.001f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.001f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.00199f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.00199f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0033f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.0033f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0042f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.0042f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"

		"gl_Position = gl_in[0].gl_Position + vec4(0.0f, 0.0052f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position + vec4(0.0f, 0.0052f, 0.0f, 0.0f);\n"
		"EmitVertex();\n"
		"}\n"

		"gl_Position = gl_in[0].gl_Position;\n"
		"EmitVertex();\n"
		"gl_Position = gl_in[1].gl_Position;\n"
		"EmitVertex();\n"

		"EndPrimitive();\n"
		"}\n";

	unsigned int shader = CreateShader(vertexShader, fragmentShader, geometryShader);
	glUniform2f(glGetUniformLocation(shader, "coord_one"), float(int(NUM_LINES / 2)), 1.0f);
	glUniform2f(glGetUniformLocation(shader, "coord_two"), float(int(NUM_LINES / 2)), 1.0f);
	glUseProgram(shader);


	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{

		// Render the graph:
		glClear(GL_COLOR_BUFFER_BIT);
		glBufferData(GL_ARRAY_BUFFER, (NUM_LINES * 2) * sizeof(float), positions, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, NUM_LINES);
		//Render the equation/ line:
		glBufferData(GL_ARRAY_BUFFER, (NUM_LINES * 2) * sizeof(float), arr, GL_STATIC_DRAW);
		glDrawArrays(GL_LINES, 0, NUM_LINES);

		//render UI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//Button
		ImGui::Begin("Vector field generator");
		//Input field for first equation
		char Equation_1[256];
		memset(Equation_1, 0, sizeof(Equation_1));
		ImGuiInputTextFlags input_text_flags_1 = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		ImGui::InputText("Differential Equation 1", Equation_1, IM_ARRAYSIZE(Equation_1), input_text_flags_1, 0, 0);

		//Input field for second equation
		char Equation_2[256];
		memset(Equation_2, 0, sizeof(Equation_2));
		ImGuiInputTextFlags input_text_flags_2 = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
		ImGui::InputText("Differential Equation 2", Equation_2, IM_ARRAYSIZE(Equation_2), input_text_flags_2, 0, 0);


		ImGui::SetWindowFontScale(2.0f);


		ImGui::Button("Graph", ImVec2(130.0f, 50.0f));
		
		ImGui::End();
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	free(test_x1);
	free(test_y1);
	free(test_x2);
	free(test_y2);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, height, height);
}