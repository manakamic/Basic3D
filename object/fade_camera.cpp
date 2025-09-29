#include "DxLib.h"
#include "fade_camera.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "dx_utility.h"
#endif

namespace world {

    fade_camera::fade_camera(int screen_width, int screen_height) : camera_base(screen_width, screen_height) {
        ortho = false;
        // 185.0f はパースペクティブ設定(オースの時は何でも良い)
#if defined(_AMG_MATH)
        position = math::vector4(0.0, 0.0, 185.0);
#else
        position = VGet(0.0f, 0.0f, 185.0f);
#endif
    }

    void fade_camera::process() {
        if (update != nullptr) {
            update(this);
        }

#if defined(_AMG_MATH)
        SetCameraPositionAndTargetAndUpVec(ToDX(position), ToDX(target), ToDX(up));
#else
        SetCameraPositionAndTargetAndUpVec(position, target, up);
#endif
        SetCameraNearFar(near_value, far_value);

        if (ortho) {
            // transition.mv1 は 100 x 100 サイズの頂点なので
            SetupCamera_Ortho(100.0f);
        }
    }
}