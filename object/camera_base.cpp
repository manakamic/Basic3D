#include "DxLib.h"
#include "camera_base.h"
#include "vector4.h"
#include "matrix44.h"
#include "utility.h"
#include "dx_utility.h"

namespace {
    constexpr auto DEFAULT_POSITION_Z = -100.0;
    constexpr auto DEFAULT_UP_Y = 1.0;
#undef DEFAULT_NEAR
    constexpr auto DEFAULT_NEAR = 1.0;
#undef DEFAULT_FAR
    constexpr auto DEFAULT_FAR = 10000.0;
    constexpr auto DEFAULT_FOV_ANGLE = 30.0;
}

namespace world {

    camera_base::camera_base(int screen_width, int screen_height) {
        update = nullptr;

        near_value = DEFAULT_NEAR;
        far_value = DEFAULT_FAR;
        fov = math::utility::degree_to_radian(DEFAULT_FOV_ANGLE);
        aspect = static_cast<double>(screen_height) / static_cast<double>(screen_width);

#if defined(_AMG_MATH)
        position = math::vector4(0.0, 0.0, DEFAULT_POSITION_Z);
        target = math::vector4();
        up = math::vector4(0.0, DEFAULT_UP_Y, 0.0);
#else
        position = VGet(0.0f, 0.0f, DEFAULT_POSITION_Z);
        target = VGet(0.0f, 0.0f, 0.0f);
        up = VGet(0.0f, DEFAULT_UP_Y, 0.0f);
#endif
    }

    void camera_base::process() {
        if (update != nullptr) {
            update(this);
        }

#if defined(_AMG_MATH)
        auto view_matrix = get_view_matrix();

        SetCameraViewMatrix(ToDX(view_matrix));

        auto projection_matrix = get_projection_matrix();

        SetupCamera_ProjectionMatrix(ToDX(projection_matrix));
#else
        SetCameraPositionAndTargetAndUpVec(position, target, up);
        SetCameraNearFar(near_value, far_value);
        SetupCamera_Perspective(fov);
#endif
    }

#if defined(_AMG_MATH)
    const math::matrix44 camera_base::get_view_matrix() const {
        math::matrix44 camera_matrix = math::matrix44();

        camera_matrix.look_at(position, target, up);

        return camera_matrix;
    }

    const math::matrix44 camera_base::get_projection_matrix() const {
        math::matrix44 projection_matrix = math::matrix44();

        projection_matrix.perspective(fov, aspect, near_value, far_value);

        return projection_matrix;
    }
#else
    MATRIX camera_base::get_view_matrix() const {
        return GetCameraViewMatrix();
    }

    MATRIX camera_base::get_projection_matrix() const {
        return GetCameraProjectionMatrix();
    }
#endif
}
