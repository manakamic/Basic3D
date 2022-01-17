#pragma once

#include <vector>
#include <memory>
#include <tuple>
#include "model.h"

namespace primitive {
    class primitive_base;
    class sphere;
    class cube;
}

namespace math {
    class vector4;
}

namespace mv1 {

    class player : public model {
    public:
        enum class jump_type {
            normal, fall, back
        };

        // コンストラクタ
        player();
        player(const player&) = default; // コピー
        player(player&&) = default; // ムーブ

         // デストラクタ
        virtual ~player() = default;

        void process() override;
        bool render() override;

        void set_collision_primitive(const std::shared_ptr<primitive::primitive_base>& primitive);

        void set_collision_sphere_radius(const double sphere_radius) { collision_sphere_radius = sphere_radius; };
        double get_collision_sphere_radius() const { return collision_sphere_radius; };

        void set_debug(const bool debug) { is_debug = debug; };
        bool get_debug() const { return is_debug; };

        void set_movement(const double movement) { this->movement = movement; };
        double get_movement() const { return movement; };

        void set_rotate(const double rotate) { this->rotate = rotate; };
        double get_rotate() const { return rotate; };

        void set_jump_power(const double jump_power) { this->jump_power = jump_power; };
        double get_jump_power() const { return jump_power; };

        void set_jump_angle(const double jump_angle) { this->jump_angle = jump_angle; };
        double get_jump_angle() const { return jump_angle; };

#if defined(_AMG_MATH)
        void set_direction(const math::vector4& direction) { this->direction = direction; };
        math::vector4 get_direction() const { return direction; };

        math::vector4 get_moved() const { return moved; };
        math::vector4 get_last_position() const { return last_position; };
#else
        void set_direction(const VECTOR direction) { this->direction = direction; };
        VECTOR get_direction() const { return direction; };

        VECTOR get_moved() const { return moved; };
        VECTOR get_last_position() const { return last_position; };
#endif

    private:
        void process_collision();
        bool process_collision_plane(std::shared_ptr<primitive::primitive_base>& primitive);
        bool process_collision_sphere(std::shared_ptr<primitive::primitive_base>& primitive);
        bool process_collision_cube(std::shared_ptr<primitive::primitive_base>& primitive);

        void process_forward();
        void process_press_forward(const math::vector4& normal);
        void process_rotate(const double rotate_value);
        void process_attack(const bool start_attack);
        void process_jump();
        void process_jump_initialize(const jump_type type);
        void process_jump_finished(std::shared_ptr<primitive::primitive_base>& primitive);

#if defined(_AMG_MATH)
        math::vector4 process_jump_logic_physics();
        math::vector4 process_jump_logic_vector();
        bool process_jump_landing(const math::vector4& jump_position, const std::array<math::vector4, 4>& vertices);
#else
        VECTOR process_jump_logic_physics();
        VECTOR process_jump_logic_vector();
        bool process_jump_landing(const VECTOR& jump_position, const std::array<math::vector4, 4>& vertices);
#endif

        void jump_velocity_initialize_normal();
        void jump_velocity_initialize_fall();
        void jump_velocity_initialize_back();
        void jump_velocity_initialize_reflect();

        bool check_sphere_distance(const std::shared_ptr<primitive::sphere>& sphere) const;
        bool check_cube_distance(const std::shared_ptr<primitive::cube>& cube) const;
        bool check_fall(const std::array<math::vector4, 4>& vertices) const;

        math::vector4 make_collision_position() const;
        const std::tuple<math::vector4, math::vector4> make_collision_line() const;
        void set_collision_to_position(math::vector4& collision);

        std::vector<std::shared_ptr<primitive::primitive_base>> collision_list; // コリジョン処理をするプリミティブ
        std::shared_ptr<primitive::primitive_base> collision_ride_on; // 上に乗っているプリミティブ

        double collision_sphere_radius; // コリジョン処理用の球の半径

        double movement;
        double rotate;
        double jump_power;
        double jump_angle;
        double jump_timer;

        bool is_jump;
        bool is_fall;
        bool is_back;
        bool is_forward;
        bool is_attack;
        bool is_debug;

#if defined(_AMG_MATH)
        math::vector4 direction;
        math::vector4 moved;
        math::vector4 jump_velocity;
        math::vector4 jump_start_position;
        math::vector4 last_position;
        math::matrix44 rotate_y;
#else
        VECTOR direction;
        VECTOR moved;
        VECTOR jump_velocity;
        VECTOR jump_start_position;
        VECTOR last_position;
        MATRIX rotate_y;
#endif
    };
}
