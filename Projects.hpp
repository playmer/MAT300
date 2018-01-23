#pragma once

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include "PrivateImplementation.hpp"
#include "Rendering.hpp"



struct Project
{
  Project();

  using ProjectFunction = void(*)(Project&);
  static std::vector<std::pair<std::string, ProjectFunction>> aProjectFunctions;
  static std::vector<const char*> mProjectNames;

  void RenderAxis()
  {
    mXAxis.Draw();
    mYAxis.Draw();

    if (m3D)
    {
      //mZAxis.Draw();
    }

    mCurve.Draw();

    mPointDrawer.FromYValues(mPoints);
    mPointDrawer.ToGPU();
    mPointDrawer.Draw();
  }

  CurveBuilder mCurve;

  LineDrawer mXAxis;
  LineDrawer mYAxis;
  LineDrawer mZAxis;
  PointDrawer mPointDrawer;

  glm::mat4 ProjectionMatrix;
  glm::mat4 ViewMatrix;
  glm::vec3 mPosition;

  PrivateImplementationDynamic mPrivate;
  int mControlPoints;

  std::vector<float> mPoints;

  glm::ivec2 mWindowSize;

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
