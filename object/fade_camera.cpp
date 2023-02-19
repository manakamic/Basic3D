#include "DxLib.h"
#include "fade_camera.h"

namespace world {

    fade_camera::fade_camera(int screen_width, int screen_height) : camera_base(screen_width, screen_height) {
        ortho = false;
        // 185.0f �̓p�[�X�y�N�e�B�u�ݒ�(�I�[�X�̎��͉��ł��ǂ�)
        position = VGet(0.0f, 0.0f, 185.0f);
    }

    void fade_camera::process() {
        if (update != nullptr) {
            update(this);
        }

        SetCameraPositionAndTargetAndUpVec(position, target, up);
        SetCameraNearFar(near_value, far_value);

        if (ortho) {
            // transition.mv1 �� 100 x 100 �T�C�Y�̒��_�Ȃ̂�
            SetupCamera_Ortho(100.0f);
        }
    }
}