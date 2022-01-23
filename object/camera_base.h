#pragma once
#include <functional>
#if defined(_AMG_MATH)
#include "vector4.h"
#else
struct tagVECTOR;
struct tagMATRIX;
#endif

namespace math {
    class vector4;
    class matrix44;
}

namespace world {

    class camera_base {
    public:
        // コンストラクタ
        camera_base(int screen_width, int screen_height);
        camera_base(const camera_base&) = default; // コピー
        camera_base(camera_base&&) = default; // ムーブ

         // デストラクタ
        virtual ~camera_base() = default;

        virtual void process();

        void set_update(const std::function<void(camera_base*)>& update) { this->update = update; }

        void set_near(const double near_value) { this->near_value = near_value; }
        void set_far(const double far_value) { this->far_value = far_value; }
        void set_fov(const double fov) { this->fov = fov; }

#if defined(_AMG_MATH)
        void set_position(const math::vector4& position) { this->position = position; }
        void set_target(const math::vector4& target) { this->target = target; }
        void set_up(const math::vector4& up) { this->up = up; }
        const math::vector4 get_position() const { return position; }
        const math::vector4 get_target() const { return target; }
        const math::vector4 get_up() const { return up; }
        const math::matrix44 get_view_matrix() const;
        const math::matrix44 get_projection_matrix() const;
        const math::matrix44 get_billboard_matrix() const;
#else
        void set_position(const VECTOR& position) { this->position = position; }
        void set_target(const VECTOR& target) { this->target = target; }
        void set_up(const VECTOR& up) { this->up = up; }
        VECTOR get_position() const { return position; }
        VECTOR get_target() const { return target; }
        VECTOR get_up() const { return up; }
        MATRIX get_view_matrix() const;
        MATRIX get_projection_matrix() const;
        MATRIX get_billboard_matrix() const;
#endif

    protected:
        std::function<void(camera_base*)> update;

        double near_value;
        double far_value;
        double fov;
        double aspect;

#if defined(_AMG_MATH)
        math::vector4 position;
        math::vector4 target;
        math::vector4 up;
#else
        VECTOR position;
        VECTOR target;
        VECTOR up;
#endif
    };

}
