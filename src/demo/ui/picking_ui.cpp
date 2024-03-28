#include "picking_ui.hpp"

#include "ui_manager.hpp"

PickingUI::PickingUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void PickingUI::renderImgui() {
    ImGui::Begin("Picking Result");
    if (uiManager_.picking_result.has_value()) {
        auto &object_picked = scene_.objects[uiManager_.picking_result->object_index];
        auto model_picked = object_picked->models[uiManager_.picking_result->model_index];

        auto [u, v, w] =
            std::tuple{uiManager_.picking_result->u, uiManager_.picking_result->v, uiManager_.picking_result->w};

        if (u < v and u < w) {
            uiManager_.picking_result->vertex_index = model_picked->indices_[uiManager_.picking_result->face_index].i;
        } else if (v < u and v < w) {
            uiManager_.picking_result->vertex_index = model_picked->indices_[uiManager_.picking_result->face_index].j;
        } else if (w < u and w < v) {
            uiManager_.picking_result->vertex_index = model_picked->indices_[uiManager_.picking_result->face_index].k;
        }

        ImGui::SeparatorText("Picking Information");
        ImGui::LabelText("Object Index", "%zu", uiManager_.picking_result->object_index);
        ImGui::LabelText("Model Index", "%zu", uiManager_.picking_result->model_index);
        ImGui::LabelText("Face Index", "%zu", uiManager_.picking_result->face_index);
        ImGui::LabelText("Vertex Index", "%zu", uiManager_.picking_result->vertex_index);
        ImGui::LabelText("U", "%.3f", uiManager_.picking_result->u);
        ImGui::LabelText("V", "%.3f", uiManager_.picking_result->v);
        ImGui::LabelText("W", "%.3f", uiManager_.picking_result->w);

        ImGui::SeparatorText("Object Translation");
        ImGui::SliderFloat("x", &object_picked->modelTranslation.x, -5.0f, 5.0);
        ImGui::SliderFloat("y", &object_picked->modelTranslation.y, -5.0f, 5.0);
        ImGui::SliderFloat("z", &object_picked->modelTranslation.z, -5.0f, 5.0);

        ImGui::SeparatorText("Object Scaling");
        ImGui::SliderFloat("S x", &object_picked->modelScaling.x, -0.0f, 3.0);
        ImGui::SliderFloat("S y", &object_picked->modelScaling.y, -0.0f, 3.0);
        ImGui::SliderFloat("S z", &object_picked->modelScaling.z, -0.0f, 3.0);

        ImGui::SeparatorText("Object Scaling");
        ImGui::SliderFloat("R w", &object_picked->modelRotation.w, -0.0f, 1.0);
        ImGui::SliderFloat("R x", &object_picked->modelRotation.x, -0.0f, 1.0);
        ImGui::SliderFloat("R y", &object_picked->modelRotation.y, -0.0f, 1.0);
        ImGui::SliderFloat("R z", &object_picked->modelRotation.z, -0.0f, 1.0);
    }
    ImGui::End();
}
