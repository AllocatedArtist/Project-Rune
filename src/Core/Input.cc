#include "Input.h"

std::array<int, GLFW_KEY_LAST> Input::keys_;

float Input::mouse_x_ = 0.f;
float Input::mouse_y_ = 0.f;
float Input::mouse_delta_x_ = 0.f;
float Input::mouse_delta_y_ = 0.f;
bool Input::mouse_is_moving_ = false;
Input::CursorState Input::cursor_state_ = Input::CursorState::kCursorStateNormal;


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

void Input::SetMouseStateX(const float& x) {
  mouse_x_ = x;
}

void Input::SetMouseStateY(const float& y) {
  mouse_y_ = y;
}

void Input::SetMouseStateDeltaX(const float& x) {
  mouse_delta_x_ = x;
}

void Input::SetMouseStateDeltaY(const float& y) {
  mouse_delta_y_ = y;
}

float Input::GetMouseX() {
  return mouse_x_;
}

float Input::GetMouseY() {
  return mouse_y_;
}

float Input::GetMouseDelta_X() {
  return mouse_delta_x_;
}

float Input::GetMouseDelta_Y() {
  return mouse_delta_y_;
}

void Input::SetMouseMoving(const bool& moving) {
  mouse_is_moving_ = moving;
}

bool Input::IsMouseMoving() {
  bool is_mouse_moving = mouse_is_moving_;
  mouse_is_moving_ = false;
  return is_mouse_moving;
}

void Input::SetCursorState(const Input::CursorState& cursor_state) {
  cursor_state_ = cursor_state;
}

Input::CursorState Input::GetCursorState() {
  return cursor_state_;
}

