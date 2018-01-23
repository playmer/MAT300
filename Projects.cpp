#include <utility>
#include <vector>

#include "Projects.hpp"

Project::Project()
  : m3D(false)
  , mCurve(this)
  , mXAxis(this)
  , mYAxis(this)
  , mZAxis(this)
  , mPosition(5.24f, 0.0f, 7.39f)
  , mControlPoints(2)
  , mPointDrawer(this)
{
  mXAxis.mColor = glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f };
  mXAxis.AddLine({ -100.0f, 0.0f }, { 100.0f, 0.0f });
  mXAxis.mScale = { 9.0, 1.0f, 1.0f };

  for (int i{ -100 }; i < 100; ++i)
  {
    float fI = static_cast<float>(i);
    for (int j{ 1 }; j < 10; ++j)
    {
      float tick = (0.1f * j) + fI;
      mXAxis.AddLine({ tick, -0.05f, 0.0f },
                     { tick,  0.05f, 0.0f });
    }
      
    mXAxis.AddLine({ fI, -0.1f, 0.0f },
                   { fI,  0.1f, 0.0f });
  }

  mYAxis.mColor = glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };
  mYAxis.AddLine({ 0.0f, -100.0f }, { 0.0f,  100.0f });
    
  for (int i{ 100 }; i > -100; --i)
  {
    float fI = static_cast<float>(i);

    for (int j{ 1 }; j < 10; ++j)
    {
      float tick = (0.1f * j) + (fI - 1);
      mYAxis.AddLine({ -0.05f, tick, 0.0f },
                     {  0.05f, tick, 0.0f });
    }
      
    mYAxis.AddLine({ -0.1f, fI, 0.0f },
                    {  0.1f, fI, 0.0f });
  }

  
  //for (int i{ 0 }; i > -100; --i)
  //{
  //  float fI = static_cast<float>(i);
  //
  //  for (int j{ 1 }; j < 10; ++j)
  //  {
  //    float tick = (-0.1f * j) + fI;
  //    mYAxis.AddLine({ -0.05f, tick, 0.0f },
  //                   {  0.05f, tick, 0.0f });
  //  }
  //    
  //  mYAxis.AddLine({ -0.1f, fI, 0.0f },
  //                 {  0.1f, fI, 0.0f });
  //}


  mZAxis.mColor = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
  mZAxis.AddLine({ 0.0f, 0.0f, -100.0f }, { 0.0f, 0.0f,  100.0f });

  mXAxis.ToGPU();
  mYAxis.ToGPU();
  mZAxis.ToGPU();

  mCurve.mColor = { 0.0f, 255.0f, 255.0f, 1.0f };
  mCurve.mShouldClear = true;

  mPointDrawer.mColor = { 1.0f, 0.1f, 1.0f, 1.0f };
  mPoints.resize(mControlPoints, 1.0f);
}

void NotImplemented()
{
  ImGui::Text("Not implemented yet!");
}

enum class Project1Type : int
{
  NLI = 0,
  BB = 1
};

struct Project1Config
{
  Project1Config()
    : mType(Project1Type::NLI)
  {

  }

  Project1Type mType;
  std::vector<float> mQ;
};

void P1_BB(Project &aProject)
{
  auto &curve = aProject.mCurve;
}

glm::vec2 P1_NLI_GetPoint(Project &aProject, Project1Config &aConfig, float u)
{
  glm::vec2 toReturn{ u, 0.0f };
  auto &Q = aConfig.mQ;
  auto n = aProject.mControlPoints - 1;

  Q = aProject.mPoints;

  // From: https://pages.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/de-casteljau.html
  //Input: array P[0:n] of n + 1 points and real number u in[0, 1]
  //Output : point on curve, C(u)
  //Working : point array Q[0:n]
  //
  //for i : = 0 to n do
  //  Q[i] : = P[i]; // save input 
  //for k : = 1 to n do
  //  for i : = 0 to n - k do
  //    Q[i] : = (1 - u)Q[i] + u Q[i + 1];
  //return Q[0];

  for (size_t k{ 1 }; k <= n; ++k)
  {
    auto nMinusK{ n - k };

    for (size_t i{ 0 }; i <= nMinusK; ++i)
    {
      Q[i] = (1.0f - u) * Q[i] + u * Q[i + 1];
    }
  }

  toReturn.y = Q[0];

  return toReturn;
}

void P1_NLI(Project &aProject)
{
  auto config = aProject.mPrivate.ConstructAndGetIfNotAlready<Project1Config>();

  auto &curve = aProject.mCurve;

  curve.Clear();


  auto offset = 1.0f / (200 - 1);

  for (size_t i{0}; i < 200; ++i)
  {
    curve.AddPoint(P1_NLI_GetPoint(aProject, *config, i * offset));
  }
}

void Project1(Project &aProject)
{
  auto config = aProject.mPrivate.ConstructAndGetIfNotAlready<Project1Config>();

  ImGui::RadioButton("NLI", (int*)(&config->mType), static_cast<int>(Project1Type::NLI)); ImGui::SameLine();
  ImGui::RadioButton("BB", (int*)(&config->mType),  static_cast<int>(Project1Type::BB)); ImGui::SameLine();

  switch (config->mType)
  {
    case Project1Type::NLI: 
    {
      P1_NLI(aProject);
      break;
    }
    case Project1Type::BB:
    {
      P1_NLI(aProject);
      break;
    }
  }
}

void Project2(Project &aProject)
{
  NotImplemented();
}

void Project3(Project &aProject)
{
  NotImplemented();
}

void Project4(Project &aProject)
{
  NotImplemented();
}

void Project5(Project &aProject)
{
  NotImplemented();
}

void Project6(Project &aProject)
{
  NotImplemented();
}

void Project7(Project &aProject)
{
  NotImplemented();
}

void Project8(Project &aProject)
{
  NotImplemented();
}

void ECProject1(Project &aProject)
{
  NotImplemented();
}

void ECProject2(Project &aProject)
{
  NotImplemented();
}

void ECProject3(Project &aProject)
{
  NotImplemented();
}


std::vector<std::pair<std::string, Project::ProjectFunction>> Project::aProjectFunctions = {
  { "1.) De Casteljau Algorithm for Polynomial Functions", Project1 },
  { "2.) De Casteljau Algorithm for Bezier Curves", Project2 },
  { "3.) Interpolation with Polynomials", Project3 },
  { "4.) Interpolation with Cubic Splines", Project4 },
  { "5.) De Boor Algorithm: Spline Functions", Project5 },
  { "6.) De Boor Algorithm: Polynomial Curves", Project6 },
  { "7.) De Boor Algorithm: B-Spline Curves", Project7 },
  { "8.) 3D Curves: De Casteljau Algorithm for Polynomial Functions", Project8 },
  { "EC 1.) Hermite Interpolation (Osculation)", ECProject1 },
  { "EC 2.) Best Fit Line and Parabola", ECProject2 },
  { "EC 3.) Audio Signals with Bernstein Polynomials", ECProject3 },
};

std::vector<const char*> PairsToVector(std::vector<std::pair<std::string, Project::ProjectFunction>> &aProjectFunctions)
{
  std::vector<const char*> toReturn;

  for (auto &item : aProjectFunctions)
  {
    toReturn.push_back(item.first.c_str());
  }

  return toReturn;
}

std::vector<const char*> Project::mProjectNames = PairsToVector(Project::aProjectFunctions);