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

    static void setCamera(Camera &t_camera) {
        camera = &t_camera;
    }

    static void processInput(GLFWwindow *window, float deltaTime) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera->process_keyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera->process_keyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera->process_keyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera->process_keyboard(RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera->process_keyboard(UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            camera->process_keyboard(DOWN, deltaTime);
    }

    static void scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
        camera->process_mouse_scroll(y_offset);
    }

    static void mouse_button_callback(GLFWwindow *window, int button, int state, int mod) {
        if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS)
            is_mouse_pressing = true;
        if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_RELEASE) {
            mouse_flag = true;
            is_mouse_pressing = false;
        }
    }

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
        if (not is_mouse_pressing)
            return;

        float x_pos = static_cast<float>(xposIn);
        float y_pos = static_cast<float>(yposIn);

        if (mouse_flag) {
            last_x = x_pos;
            last_y = y_pos;
            mouse_flag = false;
        }

        float x_offset = x_pos - last_x;
        float y_offset = last_y - y_pos; // reversed since y-coordinates go from bottom to top

        last_x = x_pos;
        last_y = y_pos;

        camera->process_mouse_movement(x_offset, y_offset);
    }
};