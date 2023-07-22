#ifndef IMGUI_BACKEND_H_
#define IMGUI_BACKEND_H_

#include <imgui/imgui.h>
#include <cpp/imgui_stdlib.h>

class ImGui_Backend {
public:
  static void Start();
  static void End();

  static void NewFrame();
  static void Render();
};

#endif