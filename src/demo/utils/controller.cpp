#include "demo/utils/controller.hpp"

#include "ray_tracer/ray_picker.hpp"

#include "vkl/vkl_scene.hpp"

#include <format>
#include <iostream>

float KeyboardCameraController::mouse_x_pos = 0.0;
float KeyboardCameraController::mouse_y_pos = 0.0;

bool KeyboardCameraController::pressing_shift = false;
bool KeyboardCameraController::in_region = false;

std::function<void()> KeyboardCameraController::actionCallBack = []() {};

std::optional<std::reference_wrapper<VklScene>> KeyboardCameraController::scene_;

void KeyboardCameraController::setCamera(Camera &t_camera) {
    camera = &t_camera;
}

void KeyboardCameraController::processInput(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    auto callProcessKeyboard = [&](CameraMovement direction, float deltaTime) {
        actionCallBack();
        camera->process_keyboard(direction, deltaTime);
    };

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        pressing_shift = true;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
        pressing_shift = false;
}

void KeyboardCameraController::scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    camera->process_mouse_scroll(y_offset);
}

void KeyboardCameraController::mouse_button_callback(GLFWwindow *window, int button, int state, int mod) {
    if (not in_region)
        return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_PRESS) {
        is_mouse_pressing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_PRESS) {
        is_mouse_pressing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS) {
        uiManager_->pickObject(mouse_x_pos, mouse_y_pos);
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_RELEASE) {
        actionCallBack();
        mouse_flag = true;
        is_mouse_pressing = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_RELEASE) {
        actionCallBack();
        mouse_flag = true;
        is_mouse_pressing = false;
    }
}

void KeyboardCameraController::mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    mouse_x_pos = static_cast<float>(xposIn);
    mouse_y_pos = static_cast<float>(yposIn);

    if (xposIn > 1024 or yposIn > 1024)
        in_region = false;
    else
        in_region = true;

    if (not is_mouse_pressing)
        return;

    if (mouse_flag) {
        last_x = mouse_x_pos;
        last_y = mouse_y_pos;
        mouse_flag = false;
    }

    float x_offset = mouse_x_pos - last_x;
    float y_offset = last_y - mouse_y_pos; // reversed since y-coordinates go from bottom to top

    last_x = mouse_x_pos;
    last_y = mouse_y_pos;

    if (pressing_shift)
        camera->process_mouse_shift_movement(x_offset, y_offset);
    else
        camera->process_mouse_movement(x_offset, y_offset);
}

void KeyboardCameraController::set_scene(VklScene &scene) {
    scene_ = scene;
}

void KeyboardCameraController::setUIManager(UIManager &uiManager) {
    uiManager_ = &uiManager;
}
