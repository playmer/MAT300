#define NOMINMAX

#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <map>
#include <chrono>

#include "glm/geometric.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/common.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include <glm/gtc/type_ptr.hpp>


#include "Rendering.hpp"
#include "Projects.hpp"

static void error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}

static void ShowHelpMarker(const char* desc)
{
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered())
  {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(450.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void OptionsWindow(Project &aProject)
{
  ImGui::Begin("Options Window", nullptr);

  if (ImGui::Button("Reset Camera"))
  {
    aProject.mPosition = { 5.24f, 0.0f, 7.39f };
  }

  ImGui::SliderInt("Control Points", &aProject.mControlPoints, 2, 21);
  ImGui::SameLine(); ShowHelpMarker("or, d + 1");

  if (aProject.mControlPoints != static_cast<int>(aProject.mPoints.size()))
  {
    aProject.mPoints.clear();
    aProject.mPoints.resize(aProject.mControlPoints, 1.0f);
  }

  for (auto[point, i] : enumerate(aProject.mPoints))
  {
    int d = static_cast<int>(i);
    ImGui::PushID(d);
    ImGui::VSliderFloat("##v", ImVec2(10, 160), &(*point), -3.0f, 3.0f, "");
    
    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
    {
      ImGui::SetTooltip("Control Point %d, at y = %f", d, *point);
    }

    if (i != (aProject.mPoints.size() - 1))
    {
      ImGui::SameLine();
    }

    ImGui::PopID();
  }

  static int item{ 0 };
  ImGui::Combo("Project", &item, aProject.mProjectNames.data(), static_cast<int>(aProject.mProjectNames.size()));

  if (-1 < item && static_cast<size_t>(item) < aProject.mProjectNames.size())
  {
    aProject.aProjectFunctions[item].second(aProject);
  }

  ImGui::End();
}
void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
  printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    type, severity, message);
}

struct CompilerOptions
{
#ifdef _DEBUG
  using Release = std::integral_constant<bool, false>;
  using Debug = std::integral_constant<bool, true>;
#else
  using Release = std::integral_constant<bool, true>;
  using Debug = std::integral_constant<bool, false>;
#endif
};


bool whereIntersectRayPlane
(
  glm::vec3 const & orig, glm::vec3 const & dir,
  glm::vec3 const & planeOrig, glm::vec3 const & planeNormal,
  glm::vec3 & intersectionPoint
)
{
  glm::vec3::value_type d = glm::dot(dir, planeNormal);
  glm::vec3::value_type Epsilon = std::numeric_limits<glm::vec3::value_type>::epsilon();

  if (d < -Epsilon)
  {
    auto distance = glm::dot(planeOrig - orig, planeNormal) / d;
    intersectionPoint = (distance * dir)  + orig;
    return true;
  }

  return false;
}

bool viewToWorldCoordTransform(Project &aProject, int aMouseX, int aMouseY, glm::vec3 &intersection)
{
  auto width = static_cast<float>(aProject.mWindowSize.x);
  auto height = static_cast<float>(aProject.mWindowSize.y);

  float x = (2.0f * aMouseX) / width - 1.0f;
  float y = 1.0f - (2.0f * aMouseY) / height;
  float z = 1.0f;
  glm::vec3 ray_nds = glm::vec3(x, y, z);

  glm::vec4 ray_clip = glm::vec4(glm::vec2(ray_nds.x, ray_nds.y), -1.0, 1.0);

  

  auto projection = glm::perspective(glm::radians(45.0f),
                                     width / height,
                                     0.1f,
                                     100.0f);

  glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
  ray_eye = glm::vec4(glm::vec2(ray_eye.x, ray_eye.y), -1.0, 0.0);

  

  auto view = NicksViewMatrix({ 1.0f, 0.0f, 0.0f },
                              { 0.0f, 1.0f, 0.0f }, 
                              { 0.0f, 0.0f, -1.0f }, 
                              aProject.mPosition);

  glm::vec4 ray_wor4 = (glm::inverse(view) * ray_eye);
  glm::vec3 ray_wor = { ray_wor4.x, ray_wor4.y , ray_wor4.z };

  // don't forget to normalize the vector at some point
  ray_wor = glm::normalize(ray_wor);

  glm::vec3 planeOrigin{ 0.0f, 0.0f,0.0f };
  glm::vec3 planeNormal{ 0.0f, 0.0f, 1.0f };

  return whereIntersectRayPlane(aProject.mPosition, ray_wor, planeOrigin, planeNormal, intersection);
}


float Vector2DDistance(glm::vec2 &pVec0, glm::vec2 &pVec1)
{
  float deltaX = pVec0.x - pVec1.x;
  float deltaY = pVec0.y - pVec1.y;

  return (float)sqrt((deltaX * deltaX) + (deltaY * deltaY));
}



bool StaticPointToStaticCircle(glm::vec2 &aP, glm::vec2 &aCenter, float Radius, float &distance)
{
  distance = Vector2DDistance(aP, aCenter);
  if (Radius >= distance)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool gMouseDown{ false };
int gSelectedPoint{ -1 };


int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    return 1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif
  GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
  glfwMakeContextCurrent(window);
  gl3wInit();

  glEnable(GL_MULTISAMPLE);

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  if constexpr (CompilerOptions::Debug::value)
  {
    glDebugMessageCallback((GLDEBUGPROC)MessageCallback, 0);
  }

  // Setup ImGui binding
  ImGui_ImplGlfwGL3_Init(window, true);
  
  ImVec4 clear_color = ImColor(44, 44, 44);

  Project project;

  std::chrono::time_point<std::chrono::high_resolution_clock> mBegin = std::chrono::high_resolution_clock::now();
  std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrame = mBegin;

  // Main loop
  while (!glfwWindowShouldClose(window))
  {
    std::chrono::duration<float> timeSpan =
      std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::high_resolution_clock::now() - mLastFrame);
    mLastFrame = std::chrono::high_resolution_clock::now();

    //if (totalTime > PI)
    //{
    //  totalTime = 0.0f;
    //}

    float dt = timeSpan.count();

    glfwPollEvents();

    glfwGetWindowSize(window, &project.mWindowSize.x, &project.mWindowSize.y);

    ImGui_ImplGlfwGL3_NewFrame();

    ImGui::SetNextWindowPos(ImVec2(350, 20), ImGuiSetCond_FirstUseEver);

    OptionsWindow(project);

    float dx{ 0.0f };
    float dy{ 0.0f };
    float dz{ 0.0f };

    constexpr float cameraMoveSpeed = 2.0f;

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
    {
      dy += dt * 1.0f * cameraMoveSpeed;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
    {
      dy -= dt * 1.0f * cameraMoveSpeed;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
    {
      dx += dt * 1.0f * cameraMoveSpeed;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
    {
      dx -= dt * 1.0f * cameraMoveSpeed;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_PAGE_UP))
    {
      dz -= dt * 1.0f * cameraMoveSpeed;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
    {
      dz += dt * 1.0f * cameraMoveSpeed;
    }

    if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1))
    {
      double x, y;
      glfwGetCursorPos(window, &x, &y);

      float pixelData[4];

      glReadPixels(static_cast<int>(x), 
                   project.mWindowSize.y -  static_cast<int>(y), 
                   1, 
                   1, 
                   GL_RGBA, 
                   GL_FLOAT, 
                   pixelData);

      glm::vec4 pixel;
      pixel.x = pixelData[0];
      pixel.y = pixelData[1];
      pixel.z = pixelData[2];
      pixel.a = pixelData[3];

      glm::vec3 intersection{};
      auto success = viewToWorldCoordTransform(project, static_cast<int>(x), static_cast<int>(y), intersection);
      if (success && false == gMouseDown)
      {
        glm::vec3 scale = { 1.0f / project.mXAxis.mScale.x,
                            1.0f / project.mYAxis.mScale.y,
                            1.0f / project.mZAxis.mScale.z };

        glm::mat4 model{};
        model = glm::scale(model, scale);

        float lowestDistance = std::numeric_limits<float>::max();
        for (size_t i{ 0 }; i < project.mPointDrawer.mVertices.size(); ++i)
        {
          glm::vec2 point{ project.mPointDrawer.mVertices[i].mPosition.x, 
                           project.mPointDrawer.mVertices[i].mPosition.y };

          auto scaledIntersection = model * glm::vec4{ intersection, 1.0f };

          glm::vec2 intersection2D{ scaledIntersection.x, scaledIntersection.y };
          float distance;
          
          if (StaticPointToStaticCircle(intersection2D, point, 0.06f, distance) && distance < lowestDistance)
          {
            lowestDistance = distance;
            gSelectedPoint = i;
          }
        }
      }

      if (gMouseDown && gSelectedPoint >= 0 && success)
      {
        glm::vec3 intersection2{};

        //printf("x: %f, y: %f, z: %f\n",
        //       intersection.x,
        //       intersection.y,
        //       intersection.z); 

        project.mPoints[gSelectedPoint] = intersection.y;
      }


      gMouseDown = true;
    }
    else
    {
      gMouseDown = false;
      gSelectedPoint = -1;
    }

    project.mPosition.x += dx;
    project.mPosition.y += dy;
    project.mPosition.z += dz;

    if (project.mPosition.z < 0.1f)
    {
      project.mPosition.z = 0.1f;
    }

    // Rendering
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    project.RenderAxis();

    ImGui::Render();
    glfwSwapBuffers(window);
  }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();

  return 0;
}
