#include "DxLib.h"
#include "world_base.h"
#include "model_base.h"
#include "primitive_base.h"
#include "camera_base.h"

namespace world {

    world_base::world_base() {
        camera_index = -1;
    }

    void world_base::process() {
        for (auto primitive : primitive_list) {
            primitive->process();
        }

        for (auto model : model_list) {
            model->process();
        }

        if (camera_index >= 0 && camera_index < camera_list.size()) {
            camera_list[camera_index]->process();
        }
    }

    bool world_base::render() {
        for (auto model : model_list) {
            model->render();
        }

        for (auto primitive : primitive_list) {
            primitive->render();
        }

        return true;
    }

}
