#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include <fstream>

#include "Input.h"

Application::Application(const int& width, const int& height, const char* window_title) {

  //Clear contents of file
  std::ofstream logfile("EngineLog.txt");
  logfile.close();

#ifdef DEBUG 
  plog::init(plog::debug, "EngineLog.txt");
#elif RELEASE
  plog::init(plog::warning, "EngineLog.txt");
#endif

  int glfw_result = glfwInit();

  if (!glfw_result) {
    const char* description = nullptr;
    glfwGetError(&description);
    PLOG_FATAL << "Error could not initialize GLFW: " << description;
    assert(glfw_result && "Could not initialize GLFW");
    return;
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window_ = glfwCreateWindow(width, height, "Project Rune", nullptr, nullptr);

  if (!window_) {
    const char* description = nullptr;
    glfwGetError(&description);
    PLOG_FATAL << "Error could not create window: " << description;
    assert(window_ && "Could not create window");
    return;
  }

  glfwMakeContextCurrent(window_);

  glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
  });

  glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      Input::SetKeyState(key, GLFW_PRESS);
    } else if (action == GLFW_RELEASE) {
      Input::SetKeyState(key, GLFW_RELEASE);
    }
  });

  int glad_result = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

  if (!glad_result) {
    PLOG_ERROR << "Error could not load GLAD context for OpenGL";
    assert(glad_result && "Could not load GLAD context for OpenGL");
    return;
  }
  
  glViewport(0, 0, width, height);

  PLOG_DEBUG << "Initialized successfully";
}

Application::~Application() {
  PLOG_DEBUG << "Terminated application";
  glfwDestroyWindow(window_);
  glfwTerminate();
}

Application& Application::AddSystem(const SystemType& type, const std::function<void(void)>& system) {
  switch (type) {
    case SystemType::kSystemStart:
      PLOG_DEBUG << "Start System added";
      start_functions_.push_back(system);
      break;
    case SystemType::kSystemUpdate:
      PLOG_DEBUG << "Update System added";
      update_functions_.push_back(system);
      break;
    case SystemType::kSystemEnd:
      PLOG_DEBUG << "End System added";
      end_functions_.push_back(system);
      break;
  }

  return *this;
}

void Application::Run() {
  std::for_each(start_functions_.cbegin(), start_functions_.cend(), [](const auto& fn) { fn(); });

  while (!glfwWindowShouldClose(window_)) {
    glfwPollEvents();
    std::for_each(update_functions_.cbegin(), update_functions_.cend(), [](const auto& fn) { fn(); });
    glfwSwapBuffers(window_);
  }

  std::for_each(end_functions_.cbegin(), end_functions_.cend(), [](const auto& fn) { fn(); });
}



