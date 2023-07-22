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

  static void SetMouseStateX(const float& x); //DOES NOT SET MOUSE POSITION
  static void SetMouseStateY(const float& y); //IN THE WINDOW. ONLY USED FOR CALLBACK
  static void SetMouseStateDeltaX(const float& x); //DOES NOT SET MOUSE DELTA
  static void SetMouseStateDeltaY(const float& y); //USE FOR CALLBACK

  //Don't use this. Only use for mouse callback
  static void SetMouseMoving(const bool& moving); 

  static float GetMouseX();
  static float GetMouseY();
  static float GetMouseDelta_X();
  static float GetMouseDelta_Y();
  static bool IsMouseMoving();

  enum class CursorState {
    kCursorStateNormal,
    kCursorStateHidden,
    kCursorStateDisabled,
  };

  static void SetCursorState(const CursorState& cursor_state);
  static CursorState GetCursorState();
private:
  static std::array<int, GLFW_KEY_LAST> keys_;
  static float mouse_x_;
  static float mouse_y_;
  static float mouse_delta_x_;
  static float mouse_delta_y_;
  static bool mouse_is_moving_;
  static CursorState cursor_state_;
};


#endif