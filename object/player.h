#pragma once

#include "model.h"

namespace mv1 {

    class player : public model {
    public:
        // コンストラクタ
        player();
        player(const player&) = default; // コピー
        player(player&&) = default; // ムーブ

         // デストラクタ
        virtual ~player() = default;

        void process() override;

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
        void process_forward(const bool is_forward);
        void process_rotate(const double rotate_value);
        void process_attack(const bool start_attack);
        void process_jump(const bool start_jump);
        void process_jump_initialize();
#if defined(_AMG_MATH)
        math::vector4 process_jump_logic_physics();
        math::vector4 process_jump_logic_vector();
        void process_jump_landing(const math::vector4& jump_position);
#else
        VECTOR process_jump_logic_physics();
        VECTOR process_jump_logic_vector();
        void process_jump_landing(const VECTOR& jump_position);
#endif

        double movement;
        double rotate;
        double ground_y;
        double jump_power;
        double jump_angle;
        double jump_timer;

        bool is_jump;
        bool is_attack;

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
