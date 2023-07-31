#include "Time.h"

#include <GLFW/glfw3.h>

double Time::delta_time_;

void Time::SetDeltaTime(const double& delta_time) {
  delta_time_ = delta_time;
}

double Time::GetDeltaTime() {
  return delta_time_;
}

double Time::GetElapsedTime() {
  return glfwGetTime();
}