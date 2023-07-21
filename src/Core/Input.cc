#include "Input.h"

std::array<int, GLFW_KEY_LAST> Input::keys_;

void Input::SetKeyState(const int& key, const int& state) {
  keys_[key] = state;
}

bool Input::IsKeyDown(const int& key) {
  return keys_[key] == GLFW_PRESS;
}

bool Input::IsKeyPressed(const int& key) {
  int state = keys_[key];
  keys_[key] = GLFW_RELEASE;
  return state == GLFW_PRESS;
}

bool Input::IsKeyUp(const int& key) {
  return keys_[key] == GLFW_RELEASE;
}

bool Input::IsKeyJustReleased(const int& key) {
  static std::array<int, GLFW_KEY_LAST> states{0};
  if (IsKeyDown(key)) states[key] = GLFW_PRESS; 
  if (IsKeyUp(key) && (states[key] == GLFW_PRESS)) {
    states[key] = GLFW_RELEASE;
    return true;
  }
  return false;
}

