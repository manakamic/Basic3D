//!
//! @file camera_logic.cpp
//!
//! @brief 3D main_world.cpp 用のカメラ(camera_base class)処理の実装
//!           camera の移動とキャラクターとのコリジョンを処理
//!
#include "DxLib.h"
#include "camera_logic.h"
#include "camera_base.h"
#include "player.h"

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
    // カメラコリジョン情報
    constexpr auto CAMERA_MOVEMENT = 5.0;
    constexpr auto CAMERA_ROTATION = 2.0;
    constexpr auto CAMERA_SPHERE_RADIUS = 100.0;

#if defined(_AMG_MATH)
    void process_camera(world::camera_base* camera, const math::vector4& model_last_position, const math::vector4& model_position, const double model_radius) {
#else
    void process_camera(world::camera_base * camera, const VECTOR & model_last_position, const VECTOR & model_position, const double model_radius) {
#endif
        auto camera_position = camera->get_position();

        // カメラのターゲットとポジションを算出
#if defined(_AMG_MATH)
        auto model_moved = model_position - model_last_position;

        camera_position = camera_position + model_moved;

        auto camera_target = model_position;

        camera_target.add(0.0, model_radius, 0.0);
#else
        VECTOR model_moved = VSub(model_position, model_last_position);

        camera_position = VAdd(camera_position, model_moved);

        VECTOR camera_target = model_position;

        camera_target.y += model_radius;
#endif

        auto movement = 0.0;
        auto angle = 0.0;

        if (1 == CheckHitKey(KEY_INPUT_W)) {
            movement = CAMERA_MOVEMENT;
        } else if (1 == CheckHitKey(KEY_INPUT_S)) {
            movement = -CAMERA_MOVEMENT;
        } else if (1 == CheckHitKey(KEY_INPUT_D)) {
            angle = CAMERA_ROTATION;
        } else if (1 == CheckHitKey(KEY_INPUT_A)) {
            angle = -CAMERA_ROTATION;
        }

        if (movement != 0.0) {
#if defined(_AMG_MATH)
            // カメラ位置からカメラターゲットへのベクトルを作成
            auto camera_direction = camera_target - camera_position;

            // 単位化する
            camera_direction.normalized();

            // 移動量を掛け算して移動分のベクトルを作成して、カメラの位置ベクトルに足す
            auto camera_moved_position = camera_position + (camera_direction * movement);

            // モデルとカメラの球で当たり判定を行う

            // カメラとモデルを結ぶベクトルを作成
            auto model_to_camera = camera_moved_position - camera_target;

            // ベクトルの長さの Sqrt を行わない値
            auto x = model_to_camera.get_x();
            auto y = model_to_camera.get_y();
            auto z = model_to_camera.get_z();
            auto check_size = x * x + y * y + z * z;

            // ベクトルの長さが、当たり判定を取りたい球同士の半径より小さければ衝突している
            // (実際は Sqrt を行わない値でチェック)
            auto collision_rasius = model_radius + CAMERA_SPHERE_RADIUS;

            if (check_size < collision_rasius * collision_rasius) {
                // 衝突している場合は、正確な衝突位置へ座標を戻す
                camera_position = (camera_direction * -collision_rasius) + camera_target;
            } else {
                // 衝突していなければ移動させる
                camera_position = camera_moved_position;
            }
#else
            // カメラ位置からカメラターゲットへのベクトルを作成
            VECTOR camera_direction = VSub(camera_target, camera_position);

            // 単位化する
            VECTOR dir = VNorm(camera_direction);

            // 移動量を掛け算して移動分のベクトルを作成して、カメラの位置ベクトルに足す
            VECTOR camera_moved_position = VAdd(camera_position, VScale(dir, movement));

            // モデルとカメラの球で当たり判定を行う

            // カメラとモデルを結ぶベクトルを作成
            VECTOR model_to_camera = VSub(camera_moved_position, camera_target);

            // ベクトルの長さの Sqrt を行わない値
            auto x = model_to_camera.x;
            auto y = model_to_camera.y;
            auto z = model_to_camera.z;
            auto check_size = x * x + y * y + z * z;

            // ベクトルの長さが、当たり判定を取りたい球同士の半径より小さければ衝突している
            // (実際は Sqrt を行わない値でチェック)
            auto collision_rasius = model_radius + CAMERA_SPHERE_RADIUS;

            if (check_size < collision_rasius * collision_rasius) {
                // 衝突している場合は、正確な衝突位置へ座標を戻す
                camera_position = VAdd(VScale(dir, -collision_rasius), camera_target);
            } else {
                // 衝突していなければ移動させる
                camera_position = camera_moved_position;
            }
#endif
        }

        camera->set_position(camera_position);
        camera->set_target(camera_target);

        if (angle != 0.0) {
#if defined(_AMG_MATH)
            // Y軸の回転値を度(ディグリー)で指定して行列で取得する
            auto camera_y_matrix = math::matrix44();

            camera_y_matrix.rotate_y(angle, true);

            // モデルからカメラへのベクトル
            auto model_to_camera = camera_position - model_position;

            // モデルからカメラへのベクトルを回転させて、その後にモデル位置へ戻す
            camera_position = (model_to_camera * camera_y_matrix) + model_position;
#else
            // Y軸の回転値を度(ディグリー)で指定して行列で取得する
            MATRIX camera_y_matrix = MGetRotY(angle * DEGREE_TO_RADIAN);

            // モデルからカメラへのベクトル
            VECTOR model_to_camera = VSub(camera_position, model_position);

            // モデルからカメラへのベクトルを回転させて、その後にモデル位置へ戻す
            camera_position = VAdd(VTransform(model_to_camera, camera_y_matrix), model_position);
#endif

            camera->set_position(camera_position);
        }
    }

    bool camera_initialize(std::shared_ptr<world::camera_base>& camera, const std::shared_ptr<mv1::player>& player) {
#if defined(_AMG_MATH)
        auto camera_position = math::vector4(-50.0, 150.0, -300.0);
        auto camera_target = math::vector4(0.0, 50.0, 0.0);
#else
        VECTOR camera_position = VGet(-50.0f, 150.0f, -300.0f);
        VECTOR camera_target = VGet(0.0f, 50.0f, 0.0f);
#endif

        camera->set_position(camera_position);
        camera->set_target(camera_target);

        auto update_camera = [player](world::camera_base* base)-> void {
            process_camera(base, player->get_last_position(), player->get_position(), player->get_collision_sphere_radius());
        };

        camera->set_update(update_camera);

        return true;
    }
}

std::shared_ptr<world::camera_base> camera_initialize(const int screen_width, const int screen_height, const std::shared_ptr<mv1::player>& player) {
    // カメラ
    std::shared_ptr<world::camera_base> camera(new world::camera_base(screen_width, screen_height));

    if (!camera_initialize(camera, player)) {
        return nullptr;
    }

    return camera;
}
