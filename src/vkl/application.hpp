#pragma once

#include "vkl_device.hpp"
#include "vkl_window.hpp"
#include "vkl_swap_chain.hpp"
class Application {
  private:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    VklWindow window_{WIDTH, HEIGHT};
    VklDevice device_;
    VklSwapChain swapChain_{device_, VkExtent2D(WIDTH, HEIGHT)};
  public:
    Application()
        : device_(window_){

          };
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void run();
};