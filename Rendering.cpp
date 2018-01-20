#include "Projects.hpp"
#include "Rendering.hpp"


// Compile a shader
GLuint LoadAndCompileShader(const char *aSource, GLenum aShaderType)
{
  // Load a shader from an external file

  // Compile the shader
  GLuint shader = glCreateShader(aShaderType);
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

uniform mat4 Projection, View, Model;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;

out vec4 outColor;

void main()
{
  outColor = inColor;

  gl_Position = Projection * 
                View * 
                Model * 
                inPosition;
}
)foo";

const char* lineFragmentShader = R"foo(
#version 330

in vec4 outColor;
out vec4 out_color;

void main()
{
  //out_color = vec4(1.0f,0.0f,0.0f,1.0f);
  out_color = outColor;
}
)foo";


CurveBuilder::CurveBuilder(Project *aProject)
  : mScale{1.0f, 1.0f, 1.0f}
  , mProject(aProject)
  , mShouldClear(false)
{
  mShaderProgram = CreateProgram(lineVertexShader, lineFragmentShader);

  // Get the location of the attributes that enters in the vertex shader
  //glBindAttribLocation(mShaderProgram, 0, "inPosition");
  //glBindAttribLocation(mShaderProgram, 1, "inColor");

  LinkProgram(mShaderProgram);

  // Use a Vertex Array Object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);

  // Create a Vector Buffer Object that will store the vertices on video memory
  glGenBuffers(1, &mVertexBufferObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(4 * sizeof(float)));

  //Clean
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


static glm::mat4 CreateViewMatrix(glm::vec3 aRight,
                                  glm::vec3 aUp,
                                  glm::vec3 aForward,
                                  glm::vec3 aPosition)
{
  return { glm::vec4{ aRight.x, aRight.y, aRight.z, 0.0f },
    glm::vec4{ aUp.x, aUp.y, aUp.z, 0.0f },
    glm::vec4{ aForward.x, aForward.y, aForward.z, 0.0f },
    glm::vec4{ aPosition.x, aPosition.y, aPosition.z, 1.0f } };
}

void CurveBuilder::Draw()
{
  glm::mat4 model{};
  model = glm::scale(model, mScale);

  auto width = static_cast<float>(mProject->mWindowSize.x);
  auto height = static_cast<float>(mProject->mWindowSize.y);

  auto projection = glm::ortho(0.0f,
                               width,
                               0.0f,
                               height);


  //
  //auto projection = glm::perspective(glm::radians(45.0f),
  //                                   width / height,
  //                                   0.1f,
  //                                   256.0f);

  auto view = CreateViewMatrix({ 1.0f, 0.0f, 0.0f }, 
                               { 0.0f, 1.0f, 0.0f }, 
                               { 0.0f, 0.0f, -1.0f }, 
                               mProject->mPosition);
  
  glUseProgram(mShaderProgram);

  int loc;
  loc = glGetUniformLocation(mShaderProgram, "Projection");
  glUniformMatrix4fv(loc, 1, true, &projection[0][0]);
  loc = glGetUniformLocation(mShaderProgram, "View");
  glUniformMatrix4fv(loc, 1, true, &view[0][0]);
  loc = glGetUniformLocation(mShaderProgram, "Model");
  glUniformMatrix4fv(loc, 1, true, &model[0][0]);

  glBindVertexArray(mVertexArrayObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

  glLineWidth(4.5f);
  glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(mVertices.size()));

  if (mShouldClear)
  {
    Clear();
  }

  //Clean
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
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