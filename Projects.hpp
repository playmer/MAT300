#pragma once

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "PrivateImplementation.hpp"
#include "Rendering.hpp"



struct Project
{
  Project()
    : m3D(false)
  {
    mXAxis.mColor = glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
    mXAxis.AddPoint(glm::vec2{ -100.0f, 0.0f });
    mXAxis.AddPoint(glm::vec2{ 100.0f, 0.0f });

    mYAxis.mColor = glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };
    mYAxis.AddPoint(glm::vec2{ 0.0f, -100.0f });
    mYAxis.AddPoint(glm::vec2{ 0.0f,  100.0f });

    mZAxis.mColor = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
    mZAxis.AddPoint(glm::vec3{ 0.0f, 0.0f, -100.0f });
    mZAxis.AddPoint(glm::vec3{ 0.0f, 0.0f,  100.0f });
  }

  using ProjectFunction = void(*)(Project&);
  static std::vector<std::pair<std::string, ProjectFunction>> aProjectFunctions;
  static std::vector<const char*> mProjectNames;

  void RenderAxis()
  {
    mXAxis.Draw();
    mYAxis.Draw();

    if (m3D)
    {
      mZAxis.Draw();
    }
  }

  CurveBuilder mCurve;

  // Yeah I know this is overkill, but it makes things easier.
  CurveBuilder mXAxis;
  CurveBuilder mYAxis;
  CurveBuilder mZAxis;

  //glm::mat4 mW

  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;

  PrivateImplementationDynamic mPrivate;
  int mControlPoints;

  bool m3D;
};


void Project1(Project &aProject);
void Project2(Project &aProject);
void Project3(Project &aProject);
void Project4(Project &aProject);
void Project5(Project &aProject);
void Project6(Project &aProject);
void Project7(Project &aProject);
void Project8(Project &aProject);
void ECProject1(Project &aProject);
void ECProject2(Project &aProject);
void ECProject3(Project &aProject);
