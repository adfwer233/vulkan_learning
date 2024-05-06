#include "bezier_editor_panel_ui.hpp"

#include "../../ui_manager.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

BezierEditorPanelUI::BezierEditorPanelUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void BezierEditorPanelUI::renderImgui() {

    ImGui::Begin("Bezier Panel");
    {
        ImGui::BeginChild("Bezier Editor Panel");

        if (ImGui::Button("Save Bezier Curve")) {
            json root = json::array();
            json path = json::array();

            auto control_pts = uiManager_.control_points_model.get()->geometry->vertices;
            for (auto vert: control_pts) {
                json vert_json = json::object();
                vert_json["x"] = vert.position.x;
                vert_json["y"] = vert.position.y;
                path.push_back(vert_json);
            }

            root.push_back(path);

            std::ofstream ofs("bezier_result.json");
            ofs << root.dump();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

BezierEditorPanelUI::~BezierEditorPanelUI() {
}
