#pragma once

#include <cmath>

#include <algorithm>
#include <chrono>
#include <map>
#include <memory>
#include <thread>
#include <vector>
#include <iostream>
#include <functional>

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


struct LineDrawer
{
  LineDrawer(Project *aProject);
  void Draw();
  void AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2);
  void AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2);

  void ToGPU();
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

struct PointDrawer
{
  PointDrawer(Project *aProject);
  void Draw();
  void AddPoint(glm::vec3 aPoint);
  void AddPoint(glm::vec2 aPoint);

  void FromYValues(std::vector<float> &aPoints);
  void ToGPU();
  void Clear();

  std::vector<Vertex> mVertices;
  GLuint mVertexArrayObject;
  GLuint mVertexBufferObject;
  GLuint mShaderProgram;
  GLuint mProjectionLocation;
  GLuint mViewLocation;
  GLuint mModelLocation;
  glm::vec4 mStartColor;
  glm::vec4 mColor;
  glm::vec3 mScale;
  Project *mProject;
  bool mShouldClear;
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

glm::mat4 NicksViewMatrix(glm::vec3 aRight,
                          glm::vec3 aUp,
                          glm::vec3 aForward,
                          glm::vec3 aPosition);
