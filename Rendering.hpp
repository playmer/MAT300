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

bool nearlyEqual(float a, float b, float epsilon);

struct floatComparison
{
  static inline bool vec4Cmp(const glm::vec4 &a, const glm::vec4 &b)
  {
    const float *i = &a[0];
    const float *j = &b[0];
    size_t count{ 0 };

    while ((count < 4) && *i == *j)
    {
      ++i;
      ++j;
      ++count;
    }

    if (*i == *j)
    {
      return 0;
    }
    else
    {
      return *i - *j;
    }
  }

  bool operator()(const glm::vec4 &a, const glm::vec4 &b) const
  {
    auto x = nearlyEqual(a.x, b.x, 0.00625f);
    auto y = nearlyEqual(a.y, b.y, 0.00625f);
    auto z = nearlyEqual(a.z, b.z, 0.00625f);

    return (x && y && z) ? false : vec4Cmp(a, b);
  }
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

  void SetUpFloatToPoints();

  std::vector<Vertex> mVertices;
  std::map<glm::vec4, size_t, floatComparison> mFloatToPoints;
  //std::map<float, size_t> mFloatToPoints;
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
