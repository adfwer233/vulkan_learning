#include "geometry_processing_ui.hpp"

#include "material_ui.hpp"

#include "ui_manager.hpp"

#include "geometry_processing/map/gauss_curvature.hpp"
#include "geometry_processing/map/single_source_exact_geodesic_distance.hpp"
#include "geometry_processing/visualization/vertex_scalar_quantity_to_color.hpp"
#include "geometry_processing/variable_manager.hpp"

GeometryProcessingUI::GeometryProcessingUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void GeometryProcessingUI::renderImgui() {
    ImGui::Begin("Geometry Processing");

    ImGui::SeparatorText("Geometry Processing Algorithms");

    if (this->uiManager_.picking_result.has_value()) {

        auto object_index = uiManager_.picking_result->object_index;
        auto model_index = uiManager_.picking_result->model_index;
        auto model = this->scene_.objects[object_index]->models[model_index];

        if (ImGui::Button("Compute Gauss Curvature")) {
            uiManager_.geometryVariableManager.meshToVertexMap<GaussCurvature>(*model);
        }

        if (ImGui::Button("Compute Exact Geodesic Distance")) {
            uiManager_.geometryVariableManager.singleVertexToVertexScalarQuantityMap<SingleSourceExactGeodesicDistance>(*model, uiManager_.picking_result->vertex_index);
        }
    }

    ImGui::SeparatorText("Visualization Configs");

    ImGui::ColorEdit3("Min value color", (float *)&lowColor.x);
    ImGui::ColorEdit3("Max value color", (float *)&highColor.x);
    ImGui::End();

    ImGui::Begin("Model Variables");

    ImGui::SeparatorText("Geometry Processing Variables");

    if (this->uiManager_.picking_result.has_value()) {
        auto object_index = uiManager_.picking_result->object_index;
        auto model_index = uiManager_.picking_result->model_index;
        auto model = this->scene_.objects[object_index]->models[model_index];
        auto modelVariables = uiManager_.geometryVariableManager.getModelScalarQuantities(*model);

        for (int i = 0; auto var: modelVariables) {
            ImGui::RadioButton(std::format("{}: {}", i, var->description).c_str(), &this->variableIndex, i);
            i++;
        }
        if (ImGui::Button("Visualize Chosen Variable")) {
            VertexScalarQuantityToColor::visualize(*model, modelVariables[variableIndex], lowColor, highColor);
        }
    }


    ImGui::End();
}
