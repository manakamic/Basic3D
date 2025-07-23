#include "DxLib.h"
#include "posture_base.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#endif

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

posture_base::posture_base() {
#if defined(_AMG_MATH)
    scale = math::vector4(1.0, 1.0, 1.0);
#else
    position = VGet(0.0f, 0.0f, 0.0f);
    rotation = VGet(0.0f, 0.0f, 0.0f);
    scale = VGet(1.0f, 1.0f, 1.0f);
    scale_matrix = MGetIdent();
    rotate_matrix = MGetIdent();
    transfer_matrix = MGetIdent();
    posture_matrix = MGetIdent();
#endif
    update_posture_matrix = true;
}

void posture_base::process_posture() {
    if (update != nullptr) {
        update(this);
    }

#if defined(_AMG_MATH)
    scale_matrix.scale(scale.get_x(), scale.get_y(), scale.get_z(), true);
    rotate_matrix.rotate_x(rotation.get_x(), true);
    rotate_matrix.rotate_y(rotation.get_y(), false);
    rotate_matrix.rotate_z(rotation.get_z(), false);
    transfer_matrix.transfer(position.get_x(), position.get_y(), position.get_z(), true);

    if (update_posture_matrix) {
        posture_matrix = scale_matrix * rotate_matrix * transfer_matrix;
    }
#else
    auto radian_x = rotation.x * DEGREE_TO_RADIAN;
    auto radian_y = rotation.y * DEGREE_TO_RADIAN;
    auto radian_z = rotation.z * DEGREE_TO_RADIAN;
    auto rot_x = MGetRotX(radian_x);
    auto rot_y = MGetRotY(radian_y);
    auto rot_z = MGetRotZ(radian_z);

    scale_matrix = MGetScale(scale);
    rotate_matrix = MMult(MMult(rot_x, rot_y), rot_z);
    transfer_matrix = MGetTranslate(position);

    if (update_posture_matrix) {
        posture_matrix = MMult(MMult(scale_matrix, rotate_matrix), transfer_matrix);
    }
#endif

    if (update_after != nullptr) {
        update_after(this);
    }
}
