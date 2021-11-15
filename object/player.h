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
        double movement;
        double rotate;

#if defined(_AMG_MATH)
        math::vector4 direction;
        math::vector4 moved;
        math::vector4 last_position;
        math::matrix44 y_rotate;
#else
        VECTOR direction;
        VECTOR moved;
        VECTOR last_position;
        MATRIX y_rotate;
#endif
    };
}
