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

    void world_base::process() {
        if (camera_index >= 0 && camera_index < camera_list.size()) {
            camera_list[camera_index]->process();
        }

        for (auto primitive : primitive_list) {
            primitive->process();
        }

        for (auto model : model_list) {
            model->process();
        }
    }

    bool world_base::render() {
        if (pre_render != nullptr) {
            pre_render();
        }

        for (auto primitive : primitive_list) {
            primitive->render();
        }

        for (auto model : model_list) {
            model->render();
        }

        if (post_render != nullptr) {
            post_render();
        }

        return true;
    }

}
