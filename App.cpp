#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//#define ASSERT(x) if (!(x)) DEBUG_BREAK;
//
//#define GLCall(x) GLClearError();\
//	x;\
//	ASSERT(GLLogCall());

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

    enum class ShaderType {
      NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
      if (line.find("#shader") != std::string::npos) {
        if (line.find("vertex") != std::string::npos) {
          type = ShaderType::VERTEX;
        } else if (line.find("fragment") != std::string::npos) {
          type = ShaderType::FRAGMENT;
        }
      } else {
        ss[(int) type] << line << '\n';
      }
    }
    return {
    	ss[0].str(),
    	ss[1].str()
    };
}

static unsigned int CompileShader(unsigned int type, std::string& source) {
  unsigned int id = glCreateShader(type);
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    glDeleteShader(id);
    return 0;
  }
  return id;
}

static void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall() {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
		return false;
	}
	return true;
}

static unsigned int Createshader(std::string& vertexShader, std::string& fragmentShader) {
  unsigned int shader = glCreateProgram();
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

  glAttachShader(shader, vs);
  glAttachShader(shader, fs);
  glLinkProgram(shader);
  glValidateProgram(shader);

  glDeleteShader(vs);
  glDeleteShader(fs);

  return shader;
}

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int main(void)
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();

    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    float positions[] = {
      -0.5f, -0.5f,
	   0.5f, -0.5f,
	   0.5f,  0.5f,
	  -0.5f,  0.5f,
    };

    unsigned int indices[] {
    	0, 1, 2,
		2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    ShaderProgramSource sources = ParseShader("shaders/shader.shader");

    unsigned int shader = Createshader(sources.VertexSource, sources.FragmentSource);
    glUseProgram(shader);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        GLClearError();
        GLLogCall();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}
