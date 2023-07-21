#ifndef INPUT_H_
#define INPUT_H_

#include <GLFW/glfw3.h>
#include <array>

class Input {
public:
  static void SetKeyState(const int& key, const int& state);
  static bool IsKeyDown(const int& key);
  static bool IsKeyPressed(const int& key);
  static bool IsKeyUp(const int& key);
  static bool IsKeyJustReleased(const int& key);
private:
  static std::array<int, GLFW_KEY_LAST> keys_;
};


#endif