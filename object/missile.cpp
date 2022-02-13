//!
//! @file missile.h
//!
//! @brief ミサイルのモデルを扱うクラス
//!
//! @details
//! ■ ミサイル動作仕様
//! 発射までは特定時間でカウントダウンを行い、秒の文字列描画も行う
//! 発射後はミサイル処理が終わるまで player class の座標に 警告の文字描画を行う
//! 発射後は特定時間まで真上に直進する
//! 特定時間後は player class の座標にホーミングする
//! ホーミング処理は 特定の間隔で特定の割合で行う
//! ホーミング中は特定の高さを下回ると処理を終了する
//!
//! ■ ミサイルの姿勢制御
//! メンバ変数の posture(MATRIX型)の各行の値を
//! 姿勢制御用のそれぞれの x / y/ z 軸(ベクトル)の値として使用する
//! (正規直交基底 相当として回転行列として扱う)
//!
//! ■ 別描画について
//! 画面の右上 1/4 の領域に別描画処理を行い、発射中のミサイルの姿勢を見える様にする
//!
#include <cstring>
#include <cmath>
#include "DxLib.h"
#include "world_base.h"
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
        world = nullptr;
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

        camera_index = -1;

        state = state::none;
    }

    bool missile::initialize(const std::shared_ptr<world::world_base>& world,
                             const std::shared_ptr<mv1::player>& player,
                             const std::shared_ptr<primitive::sphere>& explosion) {
        if (world == nullptr || player == nullptr || explosion == nullptr) {
            return false;
        }

        this->world = world;
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
        // 画面の右上で、大きさは画面の 1/4 
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
        auto x = static_cast<int>(screeen.x);
        auto y = static_cast<int>(screeen.y);
        auto in_x = (x >= 0) && (x < screen_width);
        auto in_y = (y >= 0) && (x < screen_height);
        auto in_z = (screeen.z > 0.0f) && (screeen.z < 1.0f); // ConvWorldPosToScreenPos の仕様
        auto in = in_x && in_y && in_z;

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

            // ホーミング中に指定の Y 値を下回ったら処理終了の仕様とする
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

                // 各種初期化(次の発射用)
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

        // ホーミング処理が終わったミサイルの XZ 座標で処理する
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

        // 回転しながら拡大させる
#if defined(_AMG_MATH)
        explosion->set_rotation(math::vector4(0.0, explode_angle, 0.0));
        explosion->set_scale(math::vector4(explode_scale, explode_scale, explode_scale));
#else
        explosion->set_rotation(VGet(0.0f, explode_angle, 0.0f));
        explosion->set_scale(VGet(explode_scale, explode_scale, explode_scale));
#endif

        explode_angle += explode_add_angle;
        explode_scale *= explode_multi_scale;

        // 指定のスケール値を超えたら爆発終了の仕様とする
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

        // 
        return VScale(get_posture_y(), velocity);
    }

    // 発射して最初の直進(上昇)処理
    void missile::process_moving_launch() {
        // 基本的に真上に直進なので何も処理はしない
        // (現在の姿勢の Y 方向へ velocity 分動くだけなので)
        // よって launch 終了判定 および homing 処理のセットを行うだけ
        if (get_count() > launch_millisecond) {
            state = state::homing;
            start_count();

            // コールバックでホーミング中の姿勢制御だけを行う
            // ホーミング処理は process_moving_homing の箇所
            auto update_after = [this](posture_base* base) -> void {
                // posture_base のマトリクス処理はスケールも含まれるので
                // initialize でセットした scale 値の計算も行わなければならない
                // 現在の姿勢を表すメンバ変数の posture は、そのまま回転行列として使える
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
            // 自身(ミサイル)から player への単位ベクトルを作成
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
            // ここがホーミング処理相当
            // (ミサイルは直進するので姿勢を制御するのがホーミングになる)
            // 100% の player へのベクトルを適応してしまうと 必ず当たってしまうので
            // ミサイルの向き( Y 軸)から、player への(向きの)単位ベクトルを結ぶベクトル作成して
            // そのベクトルを割合(これがホーミングの割合)で小さくして使用する
            VECTOR homing = VScale(VSub(to_player, y), homing_rate);

            // 互いに直交状態の x / y / z 軸を同じ分だけ処理する
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
        // player モデルの頭上あたりになるようにオフセットをかける
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
        if (world  == nullptr || -1 == camera_index || is_stand_by() || is_explode()) {
            return;
        }

        // 画面の右上(全体の1/4サイズ)を描画対象にする設定
        SetDrawArea(base_width, 0, screen_width, quarter_height);
        SetCameraScreenCenter(base_width + (quarter_width / 2), quarter_height / 2);

        // カメラを切り替える
        auto now_camera_index = world->get_camera_index();

        world->set_camera_index(camera_index);
        world->process_camera();
        world->set_camera_index(now_camera_index);

        // Z バッファをクリアして再描画
        ClearDrawScreenZBuffer();

        world->render_primitive();
        world->render_model();

        // 枠描画
        DrawBox(base_width, 0, line_width_pos, quarter_height, line_color, TRUE);
        DrawBox(line_width_pos, line_height_pos, screen_width, quarter_height, line_color, TRUE);

        // カメラの設定も戻す
        SetCameraScreenCenter(screen_width * 0.5f, screen_height * 0.5f);
        SetDrawAreaFull();
    }
}
