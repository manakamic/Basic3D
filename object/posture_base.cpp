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
    posture = MGetIdent();
#endif
}

void posture_base::process_posture() {
#if defined(_AMG_MATH)
    posture.scale(scale.get_x(), scale.get_y(), scale.get_z(), true);
    posture.rotate_z(rotation.get_z(), false);
    posture.rotate_x(rotation.get_x(), false);
    posture.rotate_y(rotation.get_y(), false);
    posture.transfer(position.get_x(), position.get_y(), position.get_z(), false);
#else
    auto radian_x = rotation.x * DEGREE_TO_RADIAN;
    auto radian_y = rotation.y * DEGREE_TO_RADIAN;
    auto radian_z = rotation.z * DEGREE_TO_RADIAN;
    auto rotate = MMult(MMult(MGetRotZ(radian_z), MGetRotX(radian_x)), MGetRotY(radian_y));

    posture = MMult(MMult(MGetScale(scale), rotate), MGetTranslate(position));
#endif
}
