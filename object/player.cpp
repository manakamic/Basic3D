#include "DxLib.h"
#include "player.h"
#include "utility.h"
#include "vector4.h"
#include "matrix44.h"

namespace {
    constexpr auto GRAVITY = 9.8; // 重力加速度
    constexpr auto DEFAULT_JUMP_POWER = 100.0;
    constexpr auto DEFAULT_JUMP_ANGLE = 87.5;
#if !defined(_AMG_MATH)
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
#endif
}

namespace mv1 {

    player::player() : model() {
        movement = 1.0;
        rotate = 1.0;
        ground_y = 0.0;
        jump_power = DEFAULT_JUMP_POWER;
        jump_angle = DEFAULT_JUMP_ANGLE;
        jump_timer = 0.0;
        is_jump = false;
        is_attack = false;

#if defined(_AMG_MATH)
        // 特に初期値が必要なければコンストラクタでの値になる
        direction = math::vector4(0.0, 0.0, -1.0);
        direction.normalized();
#else
        direction = VGet(0.0f, 0.0f, -1.0f);
        direction = VNorm(direction);
        moved = VGet(0.0f, 0.0f, 0.0f);
        jump_velocity = VGet(0.0f, 0.0f, 0.0f);
        jump_start_position = VGet(0.0f, 0.0f, 0.0f);
        rotate_y = MGetIdent();
#endif

        last_position = position;
    }

    void player::process() {
        last_position = position;

        auto start_jump = false;
        auto start_attack = false;
        auto is_forward = false;
        auto rotate_value = 0.0;

        if (1 == CheckHitKey(KEY_INPUT_SPACE)) {
            // jump と attack は排他処理
            if (!is_attack && !is_jump) {
                is_jump = true;
                start_jump = true;
            }
        }

        if (1 == CheckHitKey(KEY_INPUT_V)) {
            // jump と attack は排他処理
            if (!is_jump && !is_attack) {
                is_attack = true;
                start_attack = true;
            }
        }

        if (1 == CheckHitKey(KEY_INPUT_UP)) {
            is_forward = true;
        }

        if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            rotate_value = -rotate;
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            rotate_value = rotate;
        }

        // jump と それ以外は排他処理を行う
        if (is_jump) {
             process_jump(start_jump);
        }
        else {
            // attack と それ以外は排他処理を行う
            if (is_attack) {
                process_attack(start_attack);
            }
            else {
                process_rotate(rotate_value);
                process_forward(is_forward);
            }
        }

        model::process();
    }

    void player::process_forward(const bool is_forward) {
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
        } else if (!is_blend() && 0 != get_main_index()) {
            set_blend(0, 10, true);
        }
    }

    void player::process_rotate(const double rotate_value) {
        if (rotate_value != 0.0) {
#if defined(_AMG_MATH)
            rotate_y.rotate_y(rotate_value, true);
            direction = direction * rotate_y;
            rotation.add(0.0, rotate_value, 0.0);
#else
            rotate_y = MGetRotY(rotate_value * DEGREE_TO_RADIAN);
            direction = VTransform(direction, rotate_y);
            rotation.y += rotate_value;
#endif
        }
    }

    void player::process_attack(const bool start_attack) {
        if (start_attack) {
            // index 2 のアニメーションが終わったら行う処理
            std::function<void(void)> attack_end = [this](void) -> void {
                is_attack = false;
                set_blend(0, 5, true);
            };

            set_blend(2, 5, false, attack_end);
        }
    }

    void player::process_jump(const bool start_jump) {
        if (start_jump) {
            process_jump_initialize();
            set_blend(3, 10, false);
        }

        // ========================================================================================================
        // jump の計算ロジック
        auto jump_position = process_jump_logic_physics();
        //auto jump_position = process_jump_logic_vector();
        // ========================================================================================================

        // 空中にいる間はジャンプ位置をそのまま代入
#if defined(_AMG_MATH)
        if (start_jump || jump_position.get_y() > ground_y) {
#else
        if (start_jump || jump_position.y > ground_y) {
#endif
            position = jump_position;
        }
        else {
            // 着地処理
            process_jump_landing(jump_position);

            set_blend(0, 10, true);
            is_jump = false;
        }
    }

    void player::process_jump_initialize() {
        jump_timer = 0.0;
        jump_start_position = position;

        // jump のベクトルを作成する
        // jump_power の基本のベクトル(水平)を作成して jump_angle の角度で傾ける
        // 最後に player の方向に回転させて向きを合わせる
#if defined(_AMG_MATH)
        auto jump_base = math::vector4(0.0, 0.0, -jump_power);
        auto jump_rotate = math::matrix44();

        jump_rotate.rotate_x(jump_angle, true);
        jump_rotate.rotate_y(rotation.get_y(), false);
        jump_velocity = jump_base * jump_rotate;
#else
        VECTOR jump_base = VGet(0.0f, 0.0f, -jump_power);
        MATRIX jump_rotate = MMult(MGetRotX(jump_angle * DEGREE_TO_RADIAN), MGetRotY(rotation.y * DEGREE_TO_RADIAN));

        jump_velocity = VTransform(jump_base, jump_rotate);
#endif
    }

#if defined(_AMG_MATH)

    math::vector4 player::process_jump_logic_physics() {
        // 放物線の式で計算する
        // y = y0 + v0t - 1/2gt^2
        auto jump_position = jump_start_position + (jump_velocity * jump_timer);
        auto jump_y = (0.5 * GRAVITY * jump_timer * jump_timer);

        jump_position.add(0.0, -jump_y, 0.0);
        jump_timer += 1.0;

        return jump_position;
    }

    math::vector4 player::process_jump_logic_vector() {
        // ベクトルで処理する
        auto jump_position = position + jump_velocity;

        jump_velocity.add(0.0, -GRAVITY, 0.0);

        return jump_position;
    }

    void player::process_jump_landing(const math::vector4& jump_position) {
        // 地面(ground_y)より下になったら、前回の位置と線分を作成し地面平面との交点を求めて、その位置にする
        auto line_start = last_position;
        auto line_end = jump_position;
        auto ground_point = math::vector4(last_position.get_x(), ground_y, last_position.get_z());
        math::plane ground = std::make_tuple(ground_point, math::vector4(0.0, 1.0, 0.0)/*平面の法線*/);
        math::collision hit = std::make_tuple(false, math::vector4());

        if (math::utility::collision_plane_line(ground, line_start, line_end, hit)) {
            position = std::get<1>(hit);
        }
    }

#else

    VECTOR player::process_jump_logic_physics() {
        // 放物線の式で計算する
        // y = y0 + v0t - 1/2gt^2
        VECTOR jump_position = VAdd(jump_start_position, VScale(jump_velocity, jump_timer));
        auto jump_y = (0.5 * GRAVITY * jump_timer * jump_timer);

        jump_position.y -= jump_y;
        jump_timer += 1.0;

        return jump_position;
    }

    VECTOR player::process_jump_logic_vector() {
        // ベクトルで処理する
        VECTOR jump_position = VAdd(position, jump_velocity);

        jump_velocity.y -= GRAVITY;

        return jump_position;
    }

    void player::process_jump_landing(const VECTOR& jump_position) {
        // 地面(ground_y)より下になったら、前回の位置と線分を作成し地面平面との交点を求めて、その位置にする
        auto line_start = math::vector4(last_position.x, last_position.y, last_position.z);
        auto line_end = math::vector4(jump_position.x, jump_position.y, jump_position.z);
        auto ground_point = math::vector4(last_position.x, ground_y, last_position.z);
        math::plane ground = std::make_tuple(ground_point, math::vector4(0.0, 1.0, 0.0)/*平面の法線*/);
        math::collision hit = std::make_tuple(false, math::vector4());

        if (math::utility::collision_plane_line(ground, line_start, line_end, hit)) {
            auto ground_hit = std::get<1>(hit);

            position = VGet(ground_hit.get_x(), ground_hit.get_y(), ground_hit.get_z());
        }
    }

#endif
}
