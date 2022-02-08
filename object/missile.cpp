#include <cstring>
#include <cmath>
#include "DxLib.h"
#include "player.h"
#include "primitive_sphere.h"
#include "missile.h"
#include "dx_utility.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#endif

namespace {
    constexpr auto model_scale = 0.2; // モデルの描画スケール
    constexpr auto count_down_millisecond = 5000; // 発射までのカウントダウン
    constexpr auto launch_millisecond = 2000; // 発射後の直進時間
    constexpr auto homing_millisecond = 100; // ホーミング処理の間隔
    constexpr auto homing_end_y = 50.0; // 着弾判定の Y 値
    constexpr auto default_velocity = 20.0f; // 移動量
    constexpr auto default_homing_rate = 0.2f; // ホーミングする割合
    constexpr auto explode_add_angle = 15.0; // 爆発の回転
    constexpr auto explode_multi_scale = 1.075; // 爆発のスケール
    constexpr auto explode_end_scale = 20.0; // 爆発の最終スケール
    constexpr auto line_width = 10; // 別枠描画用のラインの太さ
    constexpr auto separate_distance = 3250.0f; // 別枠描画時のモデルとカメラの距離
    constexpr auto warning_text_offset_y = 130.0f; // 警告描画のオフセット値
    constexpr auto text_format = _T("%.2f"); // カウントダウン用
    constexpr auto warning_message = _T("W A R N I N G"); // プレイヤー用の警告
    const auto text_color = GetColor(255, 255, 0); // 文字描画用
    const auto line_color = GetColor(32, 32, 32); // 別枠描画用
    const auto back_color = GetColor(16, 64, 88); // 別枠描画用

    // 別枠描画用の各種 1 度だけ計算すれば良い値
    auto half_width = 0;
    auto half_height = 0;
    auto quarter_width = 0;
    auto quarter_height = 0;
    auto base_width = 0;
    auto line_width_pos = 0;
    auto line_height_pos = 0;

    auto explode_angle = 0.0;
    auto explode_scale = 1.0;
}

namespace mv1 {

    missile::missile(const int screen_width, const int screen_height) : model_base() {
        player = nullptr;
        explosion = nullptr;

        posture = MGetIdent();
        fire_point = VGet(0.0f, 0.0f, 0.0f);

        velocity = default_velocity;
        homing_rate = default_homing_rate;

        count_down_time = 0.0f;
        count_start_time = 0;

        this->screen_width = screen_width;
        this->screen_height = screen_height;

        state = state::none;

        is_explosion = false;
    }

    bool missile::initialize(const std::shared_ptr<mv1::player>& player, std::shared_ptr<primitive::sphere>& explosion) {
        if (player == nullptr || explosion == nullptr) {
            return false;
        }

        this->player = player;
        this->explosion = explosion;

        // モデルが大きいのでスケールをかける
#if defined(_AMG_MATH)
        set_scale(math::vector4(model_scale, model_scale, model_scale));
#else
        set_scale(VGet(model_scale, model_scale, model_scale));
#endif

        // DrawString 用に描画文字列の半分の横幅を計算する
        const auto count_down_text = _T("0.00");

        count_down.offset_x = GetDrawStringWidth(count_down_text, std::strlen(count_down_text)) / 2;
        player_warning.offset_x = GetDrawStringWidth(warning_message, std::strlen(warning_message)) / 2;

        // 別枠描画用の値を計算
        half_width = screen_width / 2;
        half_height = screen_height / 2;
        quarter_width = half_width / 2;
        quarter_height = half_height / 2;
        base_width = half_width + quarter_width;
        line_width_pos = base_width + line_width;
        line_height_pos = quarter_height - line_width;

        return true;
    }

    void missile::start_count() {
        count_start_time = GetNowCount();
    }

    int missile::get_count() const {
        return GetNowCount() - count_start_time;
    }

    VECTOR missile::get_posture_x() const {
        return VGet(posture.m[0][0], posture.m[0][1], posture.m[0][2]);
    }

    VECTOR missile::get_posture_y() const {
        return VGet(posture.m[1][0], posture.m[1][1], posture.m[1][2]);
    }

    VECTOR missile::get_posture_z() const {
        return VGet(posture.m[2][0], posture.m[2][1], posture.m[2][2]);
    }

    void missile::set_posture(const VECTOR& x, const VECTOR& y, const VECTOR& z) {
        posture.m[0][0] = x.x; posture.m[0][1] = x.y; posture.m[0][2] = x.z;
        posture.m[1][0] = y.x; posture.m[1][1] = y.y; posture.m[1][2] = y.z;
        posture.m[2][0] = z.x; posture.m[2][1] = z.y; posture.m[2][2] = z.z;
    }

    // 3D 座標をスクリーン座標に変換
    std::tuple<bool, int, int> missile::check_in_screen(const VECTOR& position) const {
        VECTOR screeen = ConvWorldPosToScreenPos(position);
        auto in = (screeen.z > 0.0f) && (screeen.z < 1.0f);
        auto x = static_cast<int>(screeen.x);
        auto y = static_cast<int>(screeen.y);

        return std::make_tuple(in, x, y);
    }

    // 発射座標をセットする
    void missile::set_fire(const VECTOR position) {
        state = state::wait;

#if defined(_AMG_MATH)
        VECTOR pos = position;
        set_position(ToMath(pos));
#else
        set_position(position);
#endif
        fire_point = position;

        count_down_time = count_down_millisecond;

        count_down.is_valid = true;

        start_count();
    }

    void missile::process() {
        model_base::process();

        process_fire();
        process_explosion();

        if (count_down.is_valid) {
            process_count_down();
        }
        else if (player_warning.is_valid) {
            process_player_warning();
        }
    }

    // 発射処理
    void missile::process_fire() {
        if (state == state::launch || state == state::homing) {
            VECTOR move = process_moving();

#if defined(_AMG_MATH)
            auto position = get_position() + ToMath(move);
            auto homing_end = (state == state::homing) && (position.get_y() < homing_end_y);

            set_position(position);
#else
            VECTOR position = VAdd(get_position(), move);
            auto homing_end = (state == state::homing) && (position.y < homing_end_y);

            set_position(position);
#endif
            if (homing_end) {
                state = state::explode;
                set_explosion();

                player_warning.reset();
                posture = MGetIdent();
#if defined(_AMG_MATH)
                set_rotation(math::vector4());
#else
                set_rotation(VGet(0.0f, 0.0f, 0.0f));
#endif
                set_update_posture_matrix(true);
                set_update_after(nullptr);
            }
        }
    }

    void missile::set_explosion() {
        if (explosion == nullptr) {
            return;
        }

        explode_angle = 0.0;
        explode_scale = 1.0;

#if defined(_AMG_MATH)
        auto position = get_position();

        explosion->set_position(math::vector4(position.get_x(), 0.0, position.get_z()));
#else
        VECTOR position = get_position();

        position.y = 0.0f;

        explosion->set_position(position);
#endif

        explosion->set_invisible(false);
    }

    void missile::process_explosion() {
        if (explosion == nullptr || state != state::explode) {
            return;
        }

#if defined(_AMG_MATH)
        explosion->set_rotation(math::vector4(0.0, explode_angle, 0.0));
        explosion->set_scale(math::vector4(explode_scale, explode_scale, explode_scale));
#else
        explosion->set_rotation(VGet(0.0f, explode_angle, 0.0f));
        explosion->set_scale(VGet(explode_scale, explode_scale, explode_scale));
#endif

        explode_angle += explode_add_angle;
        explode_scale *= explode_multi_scale;

        if (explode_scale > explode_end_scale) {
            state = state::none;
            explosion->set_invisible(true);
        }
    }

    // 移動処理
    VECTOR missile::process_moving() {
        if (state == state::launch) {
            process_moving_launch();
        }
        else if (state == state::homing) {
            process_moving_homing();
        }

        return VScale(get_posture_y(), velocity);
    }

    // 発射して最初の直進(上昇)処理
    void missile::process_moving_launch() {
        if (get_count() > launch_millisecond) {
            state = state::homing;
            start_count();

            auto update_after = [this](posture_base* base) -> void {
#if defined(_AMG_MATH)
                auto scale = math::matrix44();
                auto trans = math::matrix44();
                auto position = get_position();
                auto rotate = ToMath(posture);

                scale.scale(model_scale, model_scale, model_scale, true);
                trans.transfer(position.get_x(), position.get_y(), position.get_z(), true);

                set_posture_matrix(scale * rotate * trans);
#else
                MATRIX scale = MGetScale(VGet(model_scale, model_scale, model_scale));
                MATRIX trans = MGetTranslate(get_position());

                set_posture_matrix(MMult(MMult(scale, posture), trans));
#endif
            };

            set_update_posture_matrix(false);
            set_update_after(update_after);
        }
    }

    // ホーミング処理
    void missile::process_moving_homing() {
        if (get_count() > homing_millisecond) {
#if defined(_AMG_MATH)
            auto player_position = player->get_position();
            auto position = get_position();
            auto to_player_math = player_position - position;

            to_player_math.normalized();

            VECTOR to_player = ToDX(to_player_math);
#else
            VECTOR player_position = player->get_position();
            VECTOR position = get_position();
            VECTOR to_player = VNorm(VSub(player_position, position));
#endif
            VECTOR x = get_posture_x();
            VECTOR y = get_posture_y();
            VECTOR z = get_posture_z();
            VECTOR homing = VScale(VSub(to_player, y), homing_rate);

            x = VNorm(VAdd(x, homing));
            y = VNorm(VAdd(y, homing));
            z = VNorm(VAdd(z, homing));

            set_posture(x, y, z);
            start_count();
        }
    }

    void missile::process_count_down() {
        auto check = check_in_screen(fire_point);
        count_down.is_draw = std::get<0>(check);

        if (count_down.is_draw) {
            count_down.x = std::get<1>(check);
            count_down.y = std::get<2>(check);

            auto count_down = count_down_millisecond - get_count();
            count_down_time = count_down / 1000.0f; // 秒へ変換

            if (count_down < 0) {
                count_down_time = 0.0f;

                this->count_down.reset();

                player_warning.is_valid = true;

                state = state::launch;

                start_count();
            }
        }
    }

    void missile::process_player_warning() {
#if defined(_AMG_MATH)
        auto position = player->get_position();
        VECTOR player_position = ToDX(position);
#else
        VECTOR player_position = player->get_position();
#endif
        VECTOR player_point = VAdd(player_position, VGet(0.0f, warning_text_offset_y, 0.0f));
        auto check = check_in_screen(player_point);

        player_warning.is_draw = std::get<0>(check);

        if (player_warning.is_draw) {
            player_warning.x = std::get<1>(check);
            player_warning.y = std::get<2>(check);
        }
    }

    bool missile::render() {
        if (is_stand_by() || is_explode()) {
            return false;
        }

        auto ret = model_base::render();

        if (ret) {
            if (count_down.is_draw) {
                auto x = count_down.x - count_down.offset_x;
                DrawFormatString(x, count_down.y, text_color, text_format, count_down_time);
            }

            if (player_warning.is_draw) {
                auto x = player_warning.x - player_warning.offset_x;
                DrawString(x, player_warning.y, warning_message, text_color);
            }
        }

        return ret;
    }

    // 別枠描画
    void missile::separate_render() {
        if (is_stand_by() || is_explode()) {
            return;
        }

        auto handle = get_handle();

        if (handle == -1) {
            return;
        }

        // 画面の右上(全体の1/4サイズ)を描画対象にする設定
        SetDrawArea(base_width, 0, screen_width, quarter_height);
        SetCameraScreenCenter(base_width + (quarter_width / 2), quarter_height / 2);

        // 背景とする
        DrawBox(base_width, 0, screen_width, quarter_height, back_color, TRUE);
        DrawBox(base_width, 0, line_width_pos, quarter_height, line_color, TRUE);
        DrawBox(line_width_pos, line_height_pos, screen_width, quarter_height, line_color, TRUE);

        // モデルを特定の位置から見えカメラをセット
        // (モデルは原寸大描画での距離を指定)
#if defined(_AMG_MATH)
        auto pos = get_position();
        VECTOR camera_target = ToDX(pos);
#else
        VECTOR camera_target = get_position();
#endif
        VECTOR camera_positon = VAdd(camera_target, VGet(0.0f, separate_distance, -separate_distance));

        SetCameraPositionAndTargetAndUpVec(camera_positon, camera_target, VGet(0.0f, 1.0f, 0.0f));

        // ZBuffer を OFF にして強制描画
        MV1SetUseZBuffer(handle, FALSE);
        MV1SetWriteZBuffer(handle, FALSE);

        SetFogEnable(FALSE);

        MV1DrawModel(handle);

        // 設定を戻す
        SetFogEnable(TRUE);

        MV1SetWriteZBuffer(handle, TRUE);
        MV1SetUseZBuffer(handle, TRUE);

        SetCameraScreenCenter(screen_width * 0.5f, screen_height * 0.5f);
        SetDrawAreaFull();
    }
}
