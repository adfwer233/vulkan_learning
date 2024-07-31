#include "bezier_editor_panel_ui.hpp"

#include "../../ui_manager.hpp"

#include "nlohmann/json.hpp"

#include "../../../scripts/containment_script.hpp"
#include "../../../scripts/render_script_base.hpp"

using json = nlohmann::json;

BezierEditorPanelUI::BezierEditorPanelUI(VklScene &scene, UIManager &uiManager) : scene_(scene), uiManager_(uiManager) {
}

void BezierEditorPanelUI::renderImgui() {

    ImGui::Begin("Bezier Panel");
    {
        ImGui::BeginChild("Bezier Editor Panel");

        if (ImGui::Button("Load the Bezier curve")) {
            std::string full_path = std::format("{}/{}", DATA_DIR, "bezier/shape_order_25.json");

            std::cout << full_path << std::endl;
            std::ifstream f(full_path);
            json data = json::parse(f);

            auto get_point = [&](json &point_json) -> std::pair<float, float> {
                return {point_json["x"].get<float>(), point_json["y"].get<float>()};
            };

            for (auto path_json : data) {
                for (auto bezier : path_json) {
                    std::vector<std::array<float, 2>> boundary_data;
                    auto straight = false;
                    for (auto points : bezier) {
                        if (points.is_boolean()) {
                            straight = true;
                        }
                    }
                    if (straight) {
                        auto [x1, y1] = get_point(bezier[0]);
                        auto [x2, y2] = get_point(bezier[3]);
                        boundary_data.push_back({x1, y1});
                        boundary_data.push_back({x2, y2});
                    } else {
                        for (auto pt : bezier) {
                            auto [x, y] = get_point(pt);
                            boundary_data.push_back({x, y});
                        }
                    }
                    uiManager_.bezier_editor_curves.push_back(
                        std::move(std::make_unique<BezierCurve2D>(std::move(boundary_data))));
                }
            }
        }

        if (ImGui::Button("Save Bezier Curve")) {
            json root = json::array();
            json path = json::array();
            json curve = json::array();

            auto control_pts = uiManager_.bezier_editor_curves.front()->control_point_vec2;
            for (auto vert : control_pts) {
                json vert_json = json::object();
                vert_json["x"] = vert.x;
                vert_json["y"] = vert.y;
                curve.push_back(vert_json);
            }

            path.push_back(curve);
            root.push_back(path);

            std::ofstream ofs("bezier_result.json");
            ofs << root.dump();
        }

        if (ImGui::Button("Render Script")) {
            ContainmentQueryScript scriptBase(uiManager_.device_);
            scriptBase.renderResult();
        }

        if (ImGui::Button("Export To File")) {
            if (uiManager_.bezierRender != nullptr) {
                uiManager_.bezierRender->exportCurrentImageToPPM();
            }
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

BezierEditorPanelUI::~BezierEditorPanelUI() {
}
