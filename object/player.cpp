#include "DxLib.h"
#include "player.h"

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

namespace mv1 {

    player::player() {
        movement = 1.0;
        rotate = 1.0;

#if defined(_AMG_MATH)
        direction = math::vector4(0.0, 0.0, -1.0);
        direction.normalized();
#else
        direction = VGet(0.0f, 0.0f, -1.0f);
        direction = VNorm(direction);
        moved = VGet(0.0f, 0.0f, 0.0f);
        y_rotate = MGetIdent();
#endif

        last_position = position;
    }

    void player::process() {
        last_position = position;

        auto is_forward = false;
        auto rotate_value = 0.0;

        if (1 == CheckHitKey(KEY_INPUT_UP)) {
            is_forward = true;
        }

        if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            rotate_value = -rotate;
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            rotate_value = rotate;
        }

        if (rotate_value != 0.0) {
#if defined(_AMG_MATH)
            y_rotate.rotate_y(rotate_value, true);
            direction = direction * y_rotate;
            rotation.add(0.0, rotate_value, 0.0);
#else
            y_rotate = MGetRotY(rotate_value * DEGREE_TO_RADIAN);
            direction = VTransform(direction, y_rotate);
            rotation.y += rotate_value;
#endif
        }

        if (is_forward) {
            if (!is_blend() && 1 != get_main_index()) {
                set_blend(1, 10, true);
            }

#if defined(_AMG_MATH)
            moved = direction * movement;
            position = position + moved;
#else
            moved = VScale(direction, movement);
            position = VAdd(position, moved);
#endif
        }
        else if (!is_blend() && 0 != get_main_index()) {
            set_blend(0, 20, true);
        }

        model::process();
    }
}
