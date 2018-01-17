#include <stdio.h>

#include <GL/gl3w.h>    
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <cmath>

#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>

#include "glm/common.hpp"

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

void OptionsWindow()
{
  ImGui::Begin("MainWindow", nullptr);
  ImGui::Text("Hello from another window!");

  ImGui::PlotLines("Cos", [](void*data, int idx) { return cosf(idx*0.2f); }, NULL, 100,0,0, FLT_MAX, FLT_MAX, ImVec2(400, 400));
  ImGui::End();
}

struct Vertex
{
  glm::vec4 mPosition;
  glm::vec4 mColor;
};

const char* lineVertexShader = R"foo(
#version 330

uniform mat4 ProjectionMatrix, ViewMatrix, ModelMatrix;

in vec4 inPosition;
in vec4 inColor;

out vec4 outColor;

void main()
{
  outColor = inColor;

  gl_Position = ProjectionMatrix * 
                ViewMatrix * 
                ModelMatrix * 
                inPosition;
}
)foo";

const char* lineFragmentShader = R"foo(
#version 330

in vec4 inColor;
out vec4 out_color;

void main()
{
  out_color = inColor;
}
)foo";

struct CurveBuilder
{
  void AddPoint(glm::vec2 aPoint)
  {

  }

  void Clear()
  {
    mVertices.clear();
    mIndicies.clear();
  }

  std::vector<Vertex> mVertices;
  std::vector<glm::u32> mIndicies;
};

  // Compile a shader
GLuint load_and_compile_shader(const char *aSource, GLenum shaderType)
{
  // Load a shader from an external file

  // Compile the shader
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &aSource, NULL);
  glCompileShader(shader);

  // Check the result of the compilation
  GLint test;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

  if(!test)
  {
    std::cerr << "Shader compilation failed with this message:" << std::endl;
    std::vector<char> compilation_log(512);
    glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
    std::cerr << &compilation_log[0] << std::endl;
    glfwTerminate();
    exit(-1);
  }

  return shader;
}

// Create a program from two shaders
GLuint create_program(const char *aVertShaderSource, const char *aFragShaderSource)
{
  // Load and compile the vertex and fragment shaders
  GLuint vertexShader = load_and_compile_shader(aVertShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShader = load_and_compile_shader(aFragShaderSource, GL_FRAGMENT_SHADER);

  // Attach the above shader to a program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  // Flag the shaders for deletion
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Link and use the program
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  return shaderProgram;
}

void RenderLines(CurveBuilder &aCurve)
{
}

void initialize(GLuint &vao) {
  // Use a Vertex Array Object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // 4 triangles to be rendered
  //GLfloat vertices_position[24] = {
  //
  //  ...
  //
  //};

  // Create a Vector Buffer Object that will store the vertices on video memory
  GLuint vbo;
  glGenBuffers(1, &vbo);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_position), vertices_position, GL_STATIC_DRAW);

  GLuint shaderProgram = create_program("shaders/vert.shader", "shaders/frag.shader");

  // Get the location of the attributes that enters in the vertex shader
  GLint position_attribute = glGetAttribLocation(shaderProgram, "position");

  // Specify how the data for position can be accessed
  glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

  // Enable the attribute
  glEnableVertexAttribArray(position_attribute);
}

int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif
  GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
  glfwMakeContextCurrent(window);
  gl3wInit();

  // Setup ImGui binding
  ImGui_ImplGlfwGL3_Init(window, true);
  
  ImVec4 clear_color = ImColor(114, 144, 154);

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    glfwPollEvents();
    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::SetNextWindowPos(ImVec2(350, 20), ImGuiSetCond_FirstUseEver);

    OptionsWindow();

    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderLines()
    ImGui::Render();
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();

  return 0;
}
