//!
//! @file world_logic.cpp
//!
//! @brief 3D main_world.cpp 用の 3D 世界(world_base class)の実装
//!
#include "DxLib.h"
#include "world_logic.h"
#include "camera_logic.h"
#include "world_base.h"
#include "camera_base.h"
#include "player.h"
#include "gun.h"
#include "primitive_plane.h"
#include "primitive_sphere.h"
#include "primitive_cube.h"
#include "dx_utility.h"

namespace {
    // ファイル
    constexpr auto MODEL_FILE = _T("model/character/SDChar.mv1");
    constexpr auto MODEL_GUN_FILE = _T("model/gun/Handgun_fbx_6.1_ASCII.mv1");
    constexpr auto TEXTURE_FILE_GROUND = _T("texture/Groundplants1_D.jpg");
    constexpr auto TEXTURE_FILE_SPHERE = _T("texture/earth.png");
    constexpr auto TEXTURE_FILE_STEPS = _T("texture/kime-yoko.jpg");
    constexpr auto TEXTURE_FILE_TREE = _T("texture/tree.png");

    // キャラクターパラメーター
    constexpr auto MODEL_MOVEMENT = 10.0;
    constexpr auto MODEL_COLLISION_SPHERE_RADIUS = 60.0;

    // 地面の正方形の数とサイズ
    constexpr auto PLANE_DIVISION_NUM = 150;
    constexpr auto PLANE_SIZE = 300.0 * static_cast<double>(PLANE_DIVISION_NUM);

    // 地球の情報
    constexpr auto SPHERE_RADIUS = 200.0f;
    constexpr auto SPHERE_DIVISION_NUM = 64;
    constexpr auto SPHERE_POSITION_OFFSET = 500.0;

    constexpr auto STEPS_CUBE_NUM = 4;
    std::vector<std::shared_ptr<primitive::cube>> cube_list;

    constexpr auto TREE_NUM = 4;
    constexpr auto TREE_DIVISION_NUM = 1;
    constexpr auto TREE_SIZE = 400;
    std::vector<std::shared_ptr<primitive::plane>> plane_list;

    auto sphere_angle = 0.0;

    std::shared_ptr<world::camera_base> camera = nullptr;

    bool player_initialize(std::shared_ptr<mv1::player>& player) {

        if (!player->load(MODEL_FILE)) {
            return false;
        }

        // 待機モーションをループ指定で
        player->set_attach(0, true);
        // 移動量を指定
        player->set_movement(MODEL_MOVEMENT);
        // コリジョン値を指定
        player->set_collision_sphere_radius(MODEL_COLLISION_SPHERE_RADIUS);

        return true;
    }

    bool gun_initialize(std::shared_ptr<mv1::gun>& gun, std::shared_ptr<mv1::player>& player) {
        auto player_base = std::dynamic_pointer_cast<mv1::model_base>(player);

        if (!gun->load(MODEL_GUN_FILE) || !gun->initialize() || !gun->setup_offset_matrix(player_base)) {
            return false;
        }

        // update 系メソッドは posture_base class しか渡ってこないので、必要な情報はラムダ式にキャプチャさせる
        MATRIX offset = gun->get_offset_matrix();
        auto frame_index = gun->get_target_frame();

        // shared_ptr などは参照でキャプチャしてはいけない(コピー使用にする)
        auto update_gun = [player, offset, frame_index](posture_base* base)-> void {
            auto hundle = player->get_handle();

            if (hundle == -1) {
                return;
            }

            MATRIX player_hand = MV1GetFrameLocalWorldMatrix(hundle, frame_index/*28*/);
            MATRIX posture = MMult(offset, player_hand);

#if defined(_AMG_MATH)
            base->set_posture_matrix(ToMath(posture));
#else
            base->set_posture_matrix(posture);
#endif
    };

        gun->set_update_after(update_gun);

        return true;
    }

    bool plane_initialize(std::shared_ptr<primitive::plane>& plane) {
        if (!plane->load(TEXTURE_FILE_GROUND) || !plane->create()) {
            return false;
        }

        return true;
    }

    bool sphere_initialize(std::shared_ptr<primitive::sphere>& sphere) {
        if (!sphere->load(TEXTURE_FILE_SPHERE) || !sphere->create()) {
            return false;
        }

#if defined(_AMG_MATH)
        sphere->set_position(math::vector4(SPHERE_POSITION_OFFSET, SPHERE_POSITION_OFFSET, SPHERE_POSITION_OFFSET));
#else
        sphere->set_position(VGet(SPHERE_POSITION_OFFSET, SPHERE_POSITION_OFFSET, SPHERE_POSITION_OFFSET));
#endif

        auto update_sphere = [](posture_base* base)-> void {
#if defined(_AMG_MATH)
            base->set_rotation(math::vector4(0.0, sphere_angle, 0.0));
#else
            base->set_rotation(VGet(0.0f, sphere_angle, 0.0f));
#endif
            sphere_angle += 0.5;
        };

        sphere->set_update(update_sphere);

        return true;
    }

    bool create_steps() {
        auto cube_size = 200.0;
        auto half_cube_size = cube_size * 0.5;
#if defined(_AMG_MATH)
        std::array<math::vector4, STEPS_CUBE_NUM> scale_list = {
            math::vector4(1.0, 1.0, 1.0),
            math::vector4(2.0, 2.0, 2.0),
            math::vector4(3.0, 3.0, 3.0),
            math::vector4(4.0, 4.0, 4.0)
        };
        std::array<math::vector4, STEPS_CUBE_NUM> position_list = {
            math::vector4(-500.0, half_cube_size,       500.0),
            math::vector4(-500.0, half_cube_size * 2.0, 500.0 + 300.0),
            math::vector4(-500.0, half_cube_size * 3.0, 500.0 + 300.0 + 500.0),
            math::vector4(-500.0, half_cube_size * 4.0, 500.0 + 300.0 + 500.0 + 700.0),
        };
#else
        std::array<VECTOR, STEPS_CUBE_NUM> scale_list = {
            VGet(1.0f, 1.0f, 1.0f),
            VGet(2.0f, 2.0f, 2.0f),
            VGet(3.0f, 3.0f, 3.0f),
            VGet(4.0f, 4.0f, 4.0f)
        };
        std::array<VECTOR, STEPS_CUBE_NUM> position_list = {
            VGet(-500.0f, half_cube_size,        500.0f),
            VGet(-500.0f, half_cube_size * 2.0f, 500.0f + 300.0f),
            VGet(-500.0f, half_cube_size * 3.0f, 500.0f + 300.0f + 500.0f),
            VGet(-500.0f, half_cube_size * 4.0f, 500.0f + 300.0f + 500.0f + 700.0f),
        };
#endif

        auto handle = -1;

        for (auto i = 0; i < STEPS_CUBE_NUM; ++i) {
            std::shared_ptr<primitive::cube> cube(new primitive::cube(cube_size));

            if (handle == -1) {
                if (!cube->load(TEXTURE_FILE_STEPS)) {
                    return false;
                }

                handle = cube->get_handle();
            }
            else {
                cube->set_handle(handle);
            }

            if (!cube->create()) {
                return false;
            }

            cube->set_scale(scale_list[i]);
            cube->set_position(position_list[i]);

            cube_list.emplace_back(std::move(cube));
        }

        return true;
    }

    bool create_trees() {
        if (camera == nullptr) {
            return false;
        }

        auto half_size = TREE_SIZE * 0.5;
#if defined(_AMG_MATH)
        auto rotation = math::vector4(90.0, 0.0, 0.0);
        std::array<math::vector4, TREE_NUM> position_list = {
            math::vector4( 1500.0, half_size, -500.0),
            math::vector4(  500.0, half_size, -500.0),
            math::vector4( -500.0, half_size, -500.0),
            math::vector4(-1500.0, half_size, -500.0)
        };
#else
        VECTOR rotation = VGet(90.0f, 0.0f, 0.0f);
        std::array<VECTOR, TREE_NUM> position_list = {
            VGet( 1500.0f, half_size, -500.0f),
            VGet(  500.0f, half_size, -500.0f),
            VGet( -500.0f, half_size, -500.0f),
            VGet(-1500.0f, half_size, -500.0f)
        };
#endif

        auto handle = -1;

        for (auto i = 0; i < TREE_NUM; ++i) {
            std::shared_ptr<primitive::plane> plane(new primitive::plane(TREE_SIZE, TREE_DIVISION_NUM));

            if (handle == -1) {
                if (!plane->load(TEXTURE_FILE_TREE)) {
                    return false;
                }

                handle = plane->get_handle();
            } else {
                plane->set_handle(handle);
            }

            if (!plane->create()) {
                return false;
            }

            plane->set_rotation(rotation);
            plane->set_position(position_list[i]);
            plane->set_lighting(FALSE);
            plane->set_transparent(TRUE);

            // ビルボード処理
#if false
            plane->set_update_posture_matrix(false);

            auto update_after_plale = [](posture_base* base)-> void {
#if defined(_AMG_MATH)
                auto billboard = camera->get_billboard_matrix();
                auto scale = base->get_scale_matrix();
                auto rotate = base->get_rotate_matrix();
                auto transfer = base->get_transfer_matrix();
                // plane は XZ 方向に頂点が作成されるので最初に回転処理を行う
                auto posture = scale * rotate * billboard * transfer;

                base->set_posture_matrix(posture);
#else
                MATRIX billboard = camera->get_billboard_matrix();
                MATRIX scale = base->get_scale_matrix();
                MATRIX rotate = base->get_rotate_matrix();
                MATRIX transfer = base->get_transfer_matrix();
                MATRIX posture = MMult(MMult(MMult(scale, rotate), billboard), transfer);

                base->set_posture_matrix(posture);
#endif
            };

            plane->set_update_after(update_after_plale);
#endif

            plane_list.emplace_back(std::move(plane));
        }

        return true;
    }
}

std::shared_ptr<world::world_base> world_initialize(const int screen_width, const int screen_height) {
    // キャラクター モデル
    std::shared_ptr<mv1::player> player(new mv1::player());

    if (!player_initialize(player)) {
        return nullptr;
    }

    // 地面
    std::shared_ptr<primitive::plane> plane(new primitive::plane(PLANE_SIZE, PLANE_DIVISION_NUM));

    if (!plane_initialize(plane)) {
        return nullptr;
    }

    // 地球
    std::shared_ptr<primitive::sphere> sphere(new primitive::sphere(SPHERE_RADIUS, SPHERE_DIVISION_NUM));

    if (!sphere_initialize(sphere)) {
        return nullptr;
    }

    // 階段
    if (!create_steps()) {
        return nullptr;
    }

    // カメラ
    camera = camera_initialize(screen_width, screen_height, player);

    if (camera == nullptr) {
        return nullptr;
    }

    // 木
    if (!create_trees()) {
        return nullptr;
    }

    // 世界
    std::shared_ptr<world::world_base> world(new world::world_base());

    // 世界へ登録とキャラクターのコリジョン登録
    world->add_camera(camera);
    world->add_model(player);
    world->add_primitive(plane);
    world->add_primitive(sphere);

    for (auto&& cube : cube_list) {
        world->add_primitive(cube);
        player->set_collision_primitive(cube);
    }

    for (auto&& plane : plane_list) {
        world->add_primitive(plane);
    }

    player->set_collision_primitive(plane);

    // 銃 モデルをキャラクターモデルに持たせる
#if false
    std::shared_ptr<mv1::gun> gun(new mv1::gun());

    if (gun_initialize(gun, player)) {
        world->add_model(gun);
    }
#endif

    return world;
}
