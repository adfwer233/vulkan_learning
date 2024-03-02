#pragma once

#include "vkl/vkl_camera.hpp"
#include "vkl/vkl_object.hpp"

#include "GLFW/glfw3.h"

class KeyboardCameraController {
  private:
    static inline Camera *camera;

    static inline bool is_mouse_pressing = false;
    static inline bool mouse_flag = true;

    static inline float last_x = 1024 / 2.0f;
    static inline float last_y = 1024 / 2.0f;

    static float mouse_x_pos;
    static float mouse_y_pos;

    static std::vector<VklObject*> objects_;

  public:
    static void setCamera(Camera &t_camera);

    static void processInput(GLFWwindow *window, float deltaTime);

    static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

    static void mouse_button_callback(GLFWwindow *window, int button, int state, int mod);

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);

    static void set_objects(decltype(objects_) object);
};