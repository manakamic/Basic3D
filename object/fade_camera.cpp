#include "DxLib.h"
#include "fade_camera.h"

namespace world {

    fade_camera::fade_camera(int screen_width, int screen_height) : camera_base(screen_width, screen_height) {
        ortho = false;
        // 185.0f はパースペクティブ設定(オースの時は何でも良い)
        position = VGet(0.0f, 0.0f, 185.0f);
    }

    void fade_camera::process() {
        if (update != nullptr) {
            update(this);
        }

        SetCameraPositionAndTargetAndUpVec(position, target, up);
        SetCameraNearFar(near_value, far_value);

        if (ortho) {
            // transition.mv1 は 100 x 100 サイズの頂点なので
            SetupCamera_Ortho(100.0f);
        }
    }
}