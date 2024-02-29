#pragma once

#include "GLFW/glfw3.h"
#include "../vkl_camera.hpp"

class KeyboardCameraController {
private:
    static inline Camera* camera;

    static inline bool is_mouse_pressing = false;
    static inline bool mouse_flag = true;

    static inline float last_x = 800 / 2.0f;
    static inline float last_y = 600 / 2.0f;
public:

    static void setCamera(Camera &t_camera);

    static void processInput(GLFWwindow *window, float deltaTime);

    static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset);

    static void mouse_button_callback(GLFWwindow *window, int button, int state, int mod);

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
};