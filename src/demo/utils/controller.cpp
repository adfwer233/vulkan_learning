#include "demo/utils/controller.hpp"

#include "ray_tracer/ray_picker.hpp"

#include "vkl/scene/vkl_scene.hpp"
#include "vkl/scene/vkl_geometry_model.hpp"

#include <format>
#include <iostream>


KeyboardCameraController *KeyboardCameraController::instance_ = nullptr;

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
    auto controller = instance_;
    controller->camera->process_mouse_scroll(y_offset);
}

void KeyboardCameraController::mouse_button_callback(GLFWwindow *window, int button, int state, int mod) {
    auto controller = instance_;

    auto addBezierControlPoint = [&](float u, float v) {
        if (controller->uiManager_->control_points_model == nullptr) {
            VklPointCloud2D::BuilderFromImmediateData builder;
            builder.vertices.emplace_back(Vertex2D{{u,    v},
                                                   {1.0f, 1.0f, 1.0f},
                                                   {1.0f, 1.0f, 1.0f},
                                                   {1.0f, 1.0f}});
            controller->uiManager_->control_points_model = std::make_unique<VklPointCloud2D>(controller->scene_->get().device_, builder);
            controller->uiManager_->control_points_model->allocDescriptorSets(*controller->scene_->get().setLayout_, *controller->scene_->get().descriptorPool_);

            std::vector<std::array<float, 2>> control_points = {{u, v}};
            controller->uiManager_->bezier_editor_curve = std::make_unique<BezierCurve2D>(std::move(control_points));
        } else {
            auto &model = controller->uiManager_->control_points_model;
            auto &curve = controller->uiManager_->control_points_model->underlyingGeometry;

            std::visit([&](auto &&arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, MeshGeometry>) {
                    model->geometry->vertices.push_back(Vertex2D{{u,    v},
                                                                 {1.0f, 1.0f, 1.0f},
                                                                 {1.0f, 1.0f, 1.0f},
                                                                 {1.0f, 1.0f}});
                }
                model->reallocateVertexBuffer();
            }, curve);

            controller->uiManager_->bezier_editor_curve->add_control_point({u, v});

            auto modelBuffer = VklGeometryModelBuffer<BezierCurve2D>::instance();
            auto curveMesh = modelBuffer->getGeometryModel(controller->scene_->get().device_, controller->uiManager_->bezier_editor_curve.get());

            curveMesh->reallocateMesh();
        }
    };

    if (not controller->in_region)
        return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_PRESS) {
        controller->is_mouse_pressing = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_PRESS) {
        controller->is_mouse_pressing = true;

        if (controller->currentWidgets == DemoWidgets::BezierEditing) {
            auto start_pos = controller->uiManager_->bezier_editor_curve->evaluate(0);
            addBezierControlPoint(start_pos.x, start_pos.y);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT and state == GLFW_PRESS) {
        if (controller->currentWidgets == DemoWidgets::SceneRendering) {
            controller->uiManager_->pickObject(
                    controller->mouse_x_pos - controller->scope_min.x,
                    controller->mouse_y_pos - controller->scope_min.y,
                    controller->scope_max.x - controller->scope_min.x,
                    controller->scope_max.y - controller->scope_min.y);
        } else if (controller->currentWidgets == DemoWidgets::BezierEditing){
            std::cout << "current " << controller->mouse_x_pos - controller->scope_min.x << " " << controller->mouse_y_pos - controller->scope_min.y << std::endl;
            float width = controller->scope_max.x - controller->scope_min.x;
            float height = controller->scope_max.y - controller->scope_min.y;
            float u = (controller->mouse_x_pos - controller->scope_min.x) / width;
            float v = (controller->mouse_y_pos - controller->scope_min.y) / height;

            addBezierControlPoint(u, v);
        }
    }

    if (button == GLFW_MOUSE_BUTTON_MIDDLE and state == GLFW_RELEASE) {
        controller->actionCallBack();
        controller->mouse_flag = true;
        controller->is_mouse_pressing = false;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT and state == GLFW_RELEASE) {
        controller->actionCallBack();
        controller->mouse_flag = true;
        controller->is_mouse_pressing = false;
    }
}

void KeyboardCameraController::mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    auto controller = instance_;

    controller->mouse_x_pos = static_cast<float>(xposIn);
    controller->mouse_y_pos = static_cast<float>(yposIn);

    if (xposIn > controller->scope_min.x and xposIn < controller->scope_max.x and yposIn > controller->scope_min.y and
        yposIn < controller->scope_max.y)
        controller->in_region = true;
    else
        controller->in_region = false;

    if (not controller->is_mouse_pressing)
        return;

    if (controller->mouse_flag) {
        controller->last_x = controller->mouse_x_pos;
        controller->last_y = controller->mouse_y_pos;
        controller->mouse_flag = false;
    }

    float x_offset = controller->mouse_x_pos - controller->last_x;
    float y_offset = controller->last_y - controller->mouse_y_pos; // reversed since y-coordinates go from bottom to top

    controller->last_x = controller->mouse_x_pos;
    controller->last_y = controller->mouse_y_pos;

    if (controller->pressing_shift)
        controller->camera->process_mouse_shift_movement(x_offset, y_offset);
    else
        controller->camera->process_mouse_movement(x_offset, y_offset);
}

void KeyboardCameraController::set_scene(VklScene &scene) {
    scene_ = scene;
}

void KeyboardCameraController::setUIManager(UIManager &uiManager) {
    uiManager_ = &uiManager;
}
