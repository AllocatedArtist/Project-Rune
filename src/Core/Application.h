#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <memory>
#include <functional>
#include <vector>

class Application {
public: 
  Application() = default;
  Application(const int& width, const int& height, const char* window_title);
  ~Application();

  enum class SystemType {
    kSystemStart,
    kSystemUpdate,
    kSystemEnd,
  };

  Application& AddSystem(const SystemType& type, const std::function<void(void)>& system);
  void Run();
  void Quit();
public:
  int GetWindowWidth();
  int GetWindowHeight();
  
  static void SetTargetFPS(int target_fps);
private:
  struct GLFWwindow* window_; 

  std::vector<std::function<void(void)>> start_functions_;
  std::vector<std::function<void(void)>> update_functions_;
  std::vector<std::function<void(void)>> end_functions_;
private:
  static double current_time_;
  static double last_time_;

  static double update_time_;
  static double draw_time_;

  static double target_fps_;
};

#endif