#pragma once

#include "vkl_device.hpp"
#include "vkl_window.hpp"

class Application {
  private:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    vklWindow window_{WIDTH, HEIGHT};
    vklDevice device_;

  public:
    Application()
        : device_(window_){

          };
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void run();
};