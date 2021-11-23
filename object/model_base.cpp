#include "DxLib.h"
#include "model_base.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#include "dx_utility.h"
#endif

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

namespace mv1 {

    model_base::model_base() {
        handle = -1;
#if defined(_AMG_MATH)
        scale = math::vector4(1.0, 1.0, 1.0);
#else
        position = VGet(0.0f, 0.0f, 0.0f);
        rotation = VGet(0.0f, 0.0f, 0.0f);
        scale = VGet(1.0f, 1.0f, 1.0f);
        posture = MGetIdent();
#endif
    }

    model_base::~model_base() {
        unload();
    }

    bool model_base::load(const TCHAR* fileName) {
        unload();

        handle = MV1LoadModel(fileName);

        auto ret = (-1 != handle);

        if (ret) {
            // Z バッファを有効化
            MV1SetUseZBuffer(handle, TRUE);
            MV1SetWriteZBuffer(handle, TRUE);
        }

        return ret;
    }

    bool model_base::unload() {
        if (-1 != handle) {
            return false;
        }

        return (-1 != MV1DeleteModel(handle));
    }

    void model_base::process() {
        process_posture();
    }

    void model_base::process_posture() {
#if defined(_AMG_MATH)
        posture.scale(scale.get_x(), scale.get_y(), scale.get_z(), true);
        posture.rotate_z(rotation.get_z(), false);
        posture.rotate_x(rotation.get_x(), false);
        posture.rotate_y(rotation.get_y(), false);
        posture.transfer(position.get_x(), position.get_y(), position.get_z(), false);

        MV1SetMatrix(handle, ToDX(posture));
#else
        auto radian_x = rotation.x * DEGREE_TO_RADIAN;
        auto radian_y = rotation.y * DEGREE_TO_RADIAN;
        auto radian_z = rotation.z * DEGREE_TO_RADIAN;
        auto rotate = MMult(MMult(MGetRotZ(radian_z), MGetRotX(radian_x)), MGetRotY(radian_y));

        posture = MMult(MMult(MGetScale(scale), rotate), MGetTranslate(position));

        MV1SetMatrix(handle, posture);
#endif
    }

    bool model_base::render() {
        if (handle == -1) {
            return false;
        }

        return (-1 != MV1DrawModel(handle));
    }
}
