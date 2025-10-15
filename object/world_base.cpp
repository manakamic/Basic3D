#include "DxLib.h"
#include "world_base.h"
#include "model_base.h"
#include "primitive_base.h"
#include "camera_base.h"

namespace world {

    world_base::world_base() {
        camera_index = -1;
        pre_render = nullptr;
        post_render = nullptr;
    }

    int world_base::add_camera(const std::shared_ptr<camera_base>& camera) {
        auto index = camera_list.size();

        camera_list.emplace_back(camera);

        if (camera_index < 0) {
            camera_index = 0;
        }

        return index;
    }

    void world_base::process_camera() {
        if (camera_index >= 0 && camera_index < camera_list.size()) {
            camera_list[camera_index]->process();
        }
    }

    void world_base::process() {
        process_camera();

        for (const auto& primitive : primitive_list) {
            primitive->process();
        }

        for (const auto& model : model_list) {
            model->process();
        }
    }

    void world_base::render_primitive() const {
        for (auto primitive : primitive_list) {
            primitive->render();
        }
    }

    void world_base::render_model() const {
        for (auto model : model_list) {
            model->render();
        }
    }

    bool world_base::render() {
        if (pre_render != nullptr) {
            pre_render();
        }

        render_primitive();
        render_model();

        if (post_render != nullptr) {
            post_render();
        }

        return true;
    }

}
