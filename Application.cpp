#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include<iostream>

//must be multiples of 4
#define NUM_LINES 160

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
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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

	float * positions = (float*)alloca((NUM_LINES * 2) * sizeof(float));

	float x_coord = -1.0f;
	for (int i = 0; i < NUM_LINES; i = i + 4) {
		positions[i] = x_coord;
		positions[i + 1] = 1.0f; //y coord of top point stays the same always as we move along the x axis
		positions[i + 2] = x_coord; 
		positions[i + 3] = -1.0f; //y coord of bottom point stays the same always as we move along x axis

		
		x_coord += float(8.0f/(float)NUM_LINES);

	}


	float y_coord = 1.0f;
	for (int i = NUM_LINES; i < NUM_LINES * 2; i = i + 4) {
		positions[i] = -1.0f;
		positions[i + 1] = y_coord;
		positions[i + 2] = 1.0f;
		positions[i + 3] = y_coord;

		
		y_coord += float(-8.0f/(float)NUM_LINES);

	}
		
		unsigned int buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, (NUM_LINES * 2) * sizeof(float), positions, GL_STATIC_DRAW);

		//need to enable 
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
			"void main()\n"
			"{\n"
			"//if we are at the middle x axis\n" 
			"if ((gl_in[0].gl_Position.xy == vec2(-1.0f, 0.0f) && gl_in[1].gl_Position.xy == vec2(1.0f, 0.0f)) || ((gl_in[0].gl_Position.xy == vec2(1.0f, 0.0f) && gl_in[1].gl_Position.xy == vec2(-1.0f, 0.0f))))\n" 
			"{\n"
			"gl_Position = vec4(-1.0f, 0.085f, 0.0f, 1.0f);\n"
			"EmitVertex();\n"
			"gl_Position = vec4(1.0f, 0.085f, 0.0f, 1.0f);\n"
			"EmitVertex();\n"
			"}\n"
		
			"gl_Position = gl_in[0].gl_Position;\n"
			"EmitVertex();\n"
			"gl_Position = gl_in[1].gl_Position;\n"
			"EmitVertex();\n"
			"EndPrimitive();\n"
			"}\n";

		unsigned int shader = CreateShader(vertexShader, fragmentShader, geometryShader);
		glUniform1f(glGetUniformLocation(shader, "x_coord"), int(NUM_LINES / 2));
		glUniform1f(glGetUniformLocation(shader, "y_coord"), int(NUM_LINES / 2));
		glUseProgram(shader);
	

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_LINES, 0, NUM_LINES);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {

	glViewport(0, 0, height, height);
}