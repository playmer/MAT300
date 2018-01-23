#include "Projects.hpp"
#include "Rendering.hpp"
#include <glm/gtc/type_ptr.hpp>

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




glm::mat4 NicksViewMatrix(glm::vec3 aRight,
                          glm::vec3 aUp,
                          glm::vec3 aForward,
                          glm::vec3 aPosition)
{

  glm::vec3 p = aPosition;

  glm::mat4 view;

  // column 0
  view[0][0] = aRight.x;
  view[1][0] = aRight.y;
  view[2][0] = aRight.z;
  view[3][0] = -dot(aRight, p);

  // column 1
  view[0][1] = aUp.x;
  view[1][1] = aUp.y;
  view[2][1] = aUp.z;
  view[3][1] = -dot(aUp, p);

  // column 2
  view[0][2] = -aForward.x;
  view[1][2] = -aForward.y;
  view[2][2] = -aForward.z;
  view[3][2] = dot(aForward, p);

  view[3][3] = 1.0f;

  return view;
}



glm::mat4 NicksProjMatrix(int aWidth, int aHeight)
{
  glm::mat4 proj;

  float rad = glm::radians(45.0f);
  float tanHalfFovy = tan(rad / 2.0f);
  float aspect = (float)aWidth / aHeight;
  float zNear = 0.1f;
  float zFar = 100.0f;

  proj[0][0] = 1.0f / (aspect * tanHalfFovy);
  proj[1][1] = 1.0f / (tanHalfFovy);
  proj[2][2] = -(zFar + zNear) / (zFar - zNear);
  proj[2][3] = -1.0f;
  proj[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);

  return proj;
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


///////////////////////////////////////////////////////////////////////////////////
// LineDrawer
///////////////////////////////////////////////////////////////////////////////////
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

  mProjectionLocation = glGetUniformLocation(mShaderProgram, "Projection");
  mViewLocation = glGetUniformLocation(mShaderProgram, "View");
  mModelLocation = glGetUniformLocation(mShaderProgram, "Model");

  // Use a Vertex Array Object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);

  // Create a Vector Buffer Object that will store the vertices on video memory
  glGenBuffers(1, &mVertexBufferObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

  //Clean
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void CurveBuilder::Draw()
{
  glm::mat4 model{};

  mScale = { mProject->mXAxis.mScale.x,
             mProject->mYAxis.mScale.y,
             mProject->mZAxis.mScale.z };

  model = glm::scale(model, mScale);

  auto width = static_cast<float>(mProject->mWindowSize.x);
  auto height = static_cast<float>(mProject->mWindowSize.y);

  //auto projection = glm::ortho(0.0f, width, height, 0.0f, 0.1f, 100.0f);


  auto projection = glm::perspective(glm::radians(45.0f),
                                     width / height,
                                     0.1f,
                                     100.0f);

  //auto projection = NicksProjMatrix(width, height);
  //const float projection[4][4] =
  //{
  //    { 2.0f / width, 0.0f,           0.0f, 0.0f },
  //    { 0.0f,         2.0f / -height, 0.0f, 0.0f },
  //    { 0.0f,         0.0f,          -1.0f, 0.0f },
  //    {-1.0f,         1.0f,           0.0f, 1.0f },
  //};

  auto view = NicksViewMatrix({ 1.0f, 0.0f, 0.0f },
                              { 0.0f, 1.0f, 0.0f }, 
                              { 0.0f, 0.0f, -1.0f }, 
                              mProject->mPosition);

  //glm::mat4 view;

  glUseProgram(mShaderProgram);

  glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(mViewLocation, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(mModelLocation, 1, GL_FALSE, glm::value_ptr(model));

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
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
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











///////////////////////////////////////////////////////////////////////////////////
// LineDrawer
///////////////////////////////////////////////////////////////////////////////////
LineDrawer::LineDrawer(Project *aProject)
  : mScale{ 1.0f, 1.0f, 1.0f }
  , mProject(aProject)
  , mShouldClear(false)
{
  mShaderProgram = CreateProgram(lineVertexShader, lineFragmentShader);

  // Get the location of the attributes that enters in the vertex shader
  //glBindAttribLocation(mShaderProgram, 0, "inPosition");
  //glBindAttribLocation(mShaderProgram, 1, "inColor");

  LinkProgram(mShaderProgram);

  mProjectionLocation = glGetUniformLocation(mShaderProgram, "Projection");
  mViewLocation = glGetUniformLocation(mShaderProgram, "View");
  mModelLocation = glGetUniformLocation(mShaderProgram, "Model");

  // Use a Vertex Array Object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);

  // Create a Vector Buffer Object that will store the vertices on video memory
  glGenBuffers(1, &mVertexBufferObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

  //Clean
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


void LineDrawer::ToGPU()
{
  glBindVertexArray(mVertexArrayObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
}

void LineDrawer::Draw()
{
  glm::mat4 model{};
  model = glm::scale(model, mScale);

  auto width = static_cast<float>(mProject->mWindowSize.x);
  auto height = static_cast<float>(mProject->mWindowSize.y);

  //auto projection = glm::ortho(0.0f, width, height, 0.0f, 0.1f, 100.0f);


  auto projection = glm::perspective(glm::radians(45.0f),
    width / height,
    0.1f,
    100.0f);

  //auto projection = NicksProjMatrix(width, height);
  //const float projection[4][4] =
  //{
  //    { 2.0f / width, 0.0f,           0.0f, 0.0f },
  //    { 0.0f,         2.0f / -height, 0.0f, 0.0f },
  //    { 0.0f,         0.0f,          -1.0f, 0.0f },
  //    {-1.0f,         1.0f,           0.0f, 1.0f },
  //};

  auto view = NicksViewMatrix({ 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
    mProject->mPosition);

  //glm::mat4 view;

  glUseProgram(mShaderProgram);

  glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(mViewLocation, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(mModelLocation, 1, GL_FALSE, glm::value_ptr(model));

  glBindVertexArray(mVertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);

  glLineWidth(4.5f);
  //glDrawArrays(GL_LINES, 0, static_cast<int>(mVertices.size() / 2));
  glDrawArrays(GL_LINES, 0, static_cast<int>(mVertices.size()));

  if (mShouldClear)
  {
    Clear();
  }

  //Clean
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void LineDrawer::AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2)
{
  mVertices.emplace_back(aPoint1, mColor);
  mVertices.emplace_back(aPoint2, mColor);
}

void LineDrawer::AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2)
{
  mVertices.emplace_back(aPoint1, mColor);
  mVertices.emplace_back(aPoint2, mColor);
}

void LineDrawer::Clear()
{
  mVertices.clear();
}












///////////////////////////////////////////////////////////////////////////////////
// PointDrawer
///////////////////////////////////////////////////////////////////////////////////


const char* pointVertexShader = R"foo(
#version 330

uniform mat4 Projection, View, Model;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;

out vec4 outColor;

void main()
{
  outColor = inColor;

  //gl_PointSize = 15.0f;

  gl_Position = Projection * 
                View * 
                Model * 
                inPosition;
}
)foo";

const char* pointFragmentShader = R"foo(
#version 330

in vec4 outColor;
out vec4 out_color;

void main()
{
  out_color = outColor;
}
)foo";



PointDrawer::PointDrawer(Project *aProject)
  : mScale{ 1.0f, 1.0f, 1.0f }
  , mProject(aProject)
  , mShouldClear(false)
{
  //glEnable(GL_PROGRAM_POINT_SIZE);
  mShaderProgram = CreateProgram(pointVertexShader, pointFragmentShader);

  // Get the location of the attributes that enters in the vertex shader
  //glBindAttribLocation(mShaderProgram, 0, "inPosition");
  //glBindAttribLocation(mShaderProgram, 1, "inColor");

  LinkProgram(mShaderProgram);

  mProjectionLocation = glGetUniformLocation(mShaderProgram, "Projection");
  mViewLocation = glGetUniformLocation(mShaderProgram, "View");
  mModelLocation = glGetUniformLocation(mShaderProgram, "Model");

  // Use a Vertex Array Object
  glGenVertexArrays(1, &mVertexArrayObject);
  glBindVertexArray(mVertexArrayObject);

  // Create a Vector Buffer Object that will store the vertices on video memory
  glGenBuffers(1, &mVertexBufferObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));

  //Clean
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}


void PointDrawer::ToGPU()
{
  glBindVertexArray(mVertexArrayObject);

  // Allocate space and upload the data from CPU to GPU
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
}

void PointDrawer::Draw()
{
  mScale = { mProject->mXAxis.mScale.x,
             mProject->mYAxis.mScale.y,
             mProject->mZAxis.mScale.z };

  glm::mat4 model{};
  model = glm::scale(model, mScale);

  auto width = static_cast<float>(mProject->mWindowSize.x);
  auto height = static_cast<float>(mProject->mWindowSize.y);

  //auto projection = glm::ortho(0.0f, width, height, 0.0f, 0.1f, 100.0f);


  auto projection = glm::perspective(glm::radians(45.0f),
                                     width / height,
                                     0.1f,
                                     100.0f);

  //auto projection = NicksProjMatrix(width, height);
  //const float projection[4][4] =
  //{
  //    { 2.0f / width, 0.0f,           0.0f, 0.0f },
  //    { 0.0f,         2.0f / -height, 0.0f, 0.0f },
  //    { 0.0f,         0.0f,          -1.0f, 0.0f },
  //    {-1.0f,         1.0f,           0.0f, 1.0f },
  //};

  auto view = NicksViewMatrix({ 1.0f, 0.0f, 0.0f },
    { 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, -1.0f },
    mProject->mPosition);

  //glm::mat4 view;

  glUseProgram(mShaderProgram);

  glUniformMatrix4fv(mProjectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
  glUniformMatrix4fv(mViewLocation, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(mModelLocation, 1, GL_FALSE, glm::value_ptr(model));

  glBindVertexArray(mVertexArrayObject);

  glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferObject);
  
  glPointSize(10.0f);
  glDrawArrays(GL_POINTS, 0, static_cast<int>(mVertices.size()));

  if (mShouldClear)
  {
    Clear();
  }

  //Clean
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void PointDrawer::AddPoint(glm::vec3 aPoint)
{
  mVertices.emplace_back(aPoint, mColor);
}

void PointDrawer::AddPoint(glm::vec2 aPoint)
{
  mVertices.emplace_back(aPoint, mColor);
}

void PointDrawer::Clear()
{
  mVertices.clear();
}


void PointDrawer::FromYValues(std::vector<float> &aPoints)
{
  mVertices.clear();

  auto offset = 1.0f / (aPoints.size() - 1);

  for (size_t i{ 0 }; i < aPoints.size(); ++i)
  {
    AddPoint({ i * offset, aPoints[i], 0.0f });
  }
}
