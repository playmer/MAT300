#pragma once

#include <cmath>

#include <chrono>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"

#include "glm/common.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"


GLuint LoadAndCompileShader(const char *aSource, GLenum shaderType);
GLuint CreateProgram(const char *aVertShaderSource, const char *aFragShaderSource);

struct Project;

struct Vertex
{
  Vertex(glm::vec2 aPosition, glm::vec4 aColor)
    : mPosition(aPosition, 0.0f, 1.0f)
    , mColor(aColor)
  {

  }

  Vertex(glm::vec3 aPosition, glm::vec4 aColor)
    : mPosition(aPosition, 1.0f)
    , mColor(aColor)
  {

  }

  glm::vec4 mPosition;
  glm::vec4 mColor;
};



struct CurveBuilder
{
  CurveBuilder(Project *aProject);
  void Draw();
  void AddPoint(glm::vec3 aPoint);
  void AddPoint(glm::vec2 aPoint);
  void Clear();

  std::vector<Vertex> mVertices;
  GLuint mVertexArrayObject;
  GLuint mVertexBufferObject;
  GLuint mShaderProgram;
  GLuint mProjectionLocation;
  GLuint mViewLocation;
  GLuint mModelLocation;
  glm::vec4 mColor;
  glm::vec3 mScale;
  Project *mProject;
  bool mShouldClear;
};