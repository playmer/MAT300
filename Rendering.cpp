#include "Rendering.hpp"


// Compile a shader
GLuint LoadAndCompileShader(const char *aSource, GLenum shaderType)
{
  // Load a shader from an external file

  // Compile the shader
  GLuint shader = glCreateShader(shaderType);
  glShaderSource(shader, 1, &aSource, NULL);
  glCompileShader(shader);

  // Check the result of the compilation
  GLint test;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &test);

  if (!test)
  {
    std::cerr << "Shader compilation failed with this message:" << std::endl;
    std::vector<char> compilation_log(512);
    glGetShaderInfoLog(shader, static_cast<int>(compilation_log.size()), NULL, &compilation_log[0]);
    std::cerr << &compilation_log[0] << std::endl;
    glfwTerminate();
    exit(-1);
  }

  return shader;
}

// Create a program from two shaders
GLuint CreateProgram(const char *aVertShaderSource, const char *aFragShaderSource)
{
  // Load and compile the vertex and fragment shaders
  GLuint vertexShader = LoadAndCompileShader(aVertShaderSource, GL_VERTEX_SHADER);
  GLuint fragmentShader = LoadAndCompileShader(aFragShaderSource, GL_FRAGMENT_SHADER);

  // Attach the above shader to a program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  // Flag the shaders for deletion
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  return shaderProgram;
}

void LinkProgram(GLuint aProgram)
{
  // Link
  glLinkProgram(aProgram);
}


const char* lineVertexShader = R"foo(
#version 330

//uniform mat4 ProjectionMatrix, ViewMatrix, ModelMatrix;

in vec4 inPosition;
in vec4 inColor;

out vec4 outColor;

void main()
{
  outColor = inColor;

//  gl_Position = ProjectionMatrix * 
//                ViewMatrix * 
//                ModelMatrix * 
//                inPosition;
  gl_Position = inPosition;
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


CurveBuilder::CurveBuilder()
  : mScale{1.0f, 1.0f, 1.0f}
{
  mShaderProgram = CreateProgram(lineVertexShader, lineFragmentShader);

  // Get the location of the attributes that enters in the vertex shader
  glBindAttribLocation(mShaderProgram, 0, "inPosition");
  glBindAttribLocation(mShaderProgram, 1, "inColor");

  LinkProgram(mShaderProgram);

  // Use a Vertex Array Object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);

  // Create a Vector Buffer Object that will store the vertices on video memory
  glGenBuffers(1, &mVertexBufferObject);

  //Clean
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void CurveBuilder::Draw()
{
  glm::mat4 modelMatrix{};
  glm::mat4 ModelMatrix = glm::scale(modelMatrix, mScale);

  //int loc;
  //uniform mat4 ProjectionMatrix, ViewMatrix, ModelMatrix;
  //loc = glGetUniformLocation(mShaderProgram, "ProjectionMatrix");
  //glUniformMatrix4fv(loc, 1, GL_TRUE, &ModelMatrix[0][0]);
  //loc = glGetUniformLocation(mShaderProgram, "ViewMatrix");
  //glUniformMatrix4fv(loc, 1, GL_TRUE, &ModelMatrix[0][0]);
  //loc = glGetUniformLocation(mShaderProgram, "ModelMatrix");
  //glUniformMatrix4fv(loc, 1, GL_TRUE, &ModelMatrix[0][0]);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

  glUseProgram(mShaderProgram);

  glBindVertexArray(mVertexArrayObject);
  glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(mVertices.size()));

  Clear();
}

void CurveBuilder::AddPoint(glm::vec2 aPoint)
{
  mVertices.emplace_back(aPoint, mColor);
}

void CurveBuilder::AddPoint(glm::vec3 aPoint)
{
  mVertices.emplace_back(aPoint, mColor);
}

void CurveBuilder::Clear()
{
  mVertices.clear();
}