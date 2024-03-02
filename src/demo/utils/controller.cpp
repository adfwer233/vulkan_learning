#include "demo/utils/controller.hpp"

#include "ray_tracer/ray_picker.hpp"

#include <iostream>
#include <format>

float KeyboardCameraController::mouse_x_pos = 0.0;
float KeyboardCameraController::mouse_y_pos = 0.0;

std::vector<VklObject*> KeyboardCameraController::objects_;
std::optional<RayPicker::RayPickingResult> KeyboardCameraController::picking_result;

void KeyboardCameraController::setCamera(Camera &t_camera) {
    camera = &t_camera;
}

void KeyboardCameraController::processInput(GLFWwindow *window, float deltaTime) {
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

void KeyboardCameraController::scroll_callback(GLFWwindow *window, double x_offset, double y_offset) {
    camera->process_mouse_scroll(y_offset);
}

void KeyboardCameraController::mouse_button_callback(GLFWwindow *window, int button, int state, int mod) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_PRESS) {

        std::cout << mouse_x_pos << ' ' << mouse_y_pos << std::endl;

        auto position = camera->position;

        auto up = camera->camera_up_axis;
        auto right = camera->camera_right_axis;

        auto base_on_viewport = camera->position + camera->camera_front * 0.1f - up * 0.0414f - right * 0.0414f;
        up = up * 0.0414f / float(1024 / 2);
        right = right * 0.0414f / float(1024 / 2);
        base_on_viewport = base_on_viewport + up * float(mouse_y_pos) + right * float(mouse_x_pos);

        Ray ray(camera->position, base_on_viewport - camera->position);

        std::cout << std::format("{} {} {}\n", ray.dir.x, ray.dir.y, ray.dir.z);

        RayPicker rayTracer(objects_, ray);
        picking_result = rayTracer.trace();
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS) {
        is_mouse_pressing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_RELEASE) {
        mouse_flag = true;
        is_mouse_pressing = false;
    }
}

void KeyboardCameraController::mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    mouse_x_pos = static_cast<float>(xposIn);
    mouse_y_pos = static_cast<float>(yposIn);
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

    camera->process_mouse_movement(x_offset, y_offset);
}

void KeyboardCameraController::set_objects(decltype(objects_) object) {
    objects_ = object;
}
