#include <utility>
#include <vector>

#include "Projects.hpp"


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
};

void P1_NLI(Project &aProject)
{
  auto &curve = aProject.mCurve;
}


void Project1(Project &aProject)
{
  auto config = aProject.mPrivate.ConstructAndGetIfNotAlready<Project1Config>();

  ImGui::RadioButton("NLI", (int*)(&config->mType), static_cast<int>(Project1Type::NLI)); ImGui::SameLine();
  ImGui::RadioButton("BB", (int*)(&config->mType),  static_cast<int>(Project1Type::BB)); ImGui::SameLine();
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