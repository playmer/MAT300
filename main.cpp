#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <map>
#include <chrono>

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
  ImGui::Begin("MainWindow", nullptr);

  ImGui::SliderInt("Control Points", &aProject.mControlPoints, 2, 80);
  ImGui::SameLine(); ShowHelpMarker("CTRL+click to input value.");

  if (aProject.mControlPoints != static_cast<int>(aProject.mPoints.size()))
  {
    aProject.mPoints.clear();
    aProject.mPoints.resize(aProject.mControlPoints, 1.0f);
  }

  for (auto[point, i] : enumerate(aProject.mPoints))
  {
    int d = static_cast<int>(i);
    ImGui::PushID(i);
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



int main(int, char**)
{
  // Setup window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
  {
    return 1;
  }


  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  #endif
  GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
  glfwMakeContextCurrent(window);
  gl3wInit();

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback((GLDEBUGPROC)MessageCallback, 0);

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

    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_UP))
    {
      dy += dt * 1.0f * 40.0f;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_DOWN))
    {
      dy -= dt * 1.0f * 40.0f;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_RIGHT))
    {
      dx += dt * 1.0f * 40.0f;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_LEFT))
    {
      dx -= dt * 1.0f * 40.0f;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_PAGE_UP))
    {
      dz -= dt * 1.0f * 40.0f;
    }
    if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_PAGE_DOWN))
    {
      dz += dt * 1.0f * 40.0f;
    }

    project.mPosition.x += dx;
    project.mPosition.y += dy;
    project.mPosition.z += dz;

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
