#ifndef TIME_H_
#define TIME_H_

class Time {
public:
  static double GetDeltaTime();
  static double GetElapsedTime();
private:
  static void SetDeltaTime(const double& delta_time);
  static double delta_time_;

  friend class Application;
};

#endif