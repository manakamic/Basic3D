//!
//! @file world_logic.cpp
//!
//! @brief 3D main_world.cpp �p�� 3D ���E(world_base class)�̎���
//!
#include "DxLib.h"
#include "world_logic.h"
#include "camera_logic.h"
#include "world_base.h"
#include "player.h"
#include "primitive_plane.h"
#include "primitive_sphere.h"
#include "primitive_cube.h"

namespace {
    // �t�@�C��
    constexpr auto MODEL_FILE = _T("model/character/SDChar.mv1");
    constexpr auto TEXTURE_FILE_GROUND = _T("texture/Groundplants1_D.jpg");
    constexpr auto TEXTURE_FILE_SPHERE = _T("texture/earth.png");
    constexpr auto TEXTURE_FILE_STEPS = _T("texture/kime-yoko.jpg");

    // �L�����N�^�[�p�����[�^�[
    constexpr auto MODEL_MOVEMENT = 10.0;
    constexpr auto MODEL_COLLISION_SPHERE_RADIUS = 60.0;

    // �n�ʂ̐����`�̐��ƃT�C�Y
    constexpr auto PLANE_DIVISION_NUM = 150;
    constexpr auto PLANE_SIZE = 300.0 * static_cast<double>(PLANE_DIVISION_NUM);

    // �n���̏��
    constexpr auto SPHERE_RADIUS = 200.0f;
    constexpr auto SPHERE_DIVISION_NUM = 64;
    constexpr auto SPHERE_POSITION_OFFSET = 500.0;

    constexpr auto STEPS_CUBE_NUM = 4;
    std::vector<std::shared_ptr<primitive::cube>> cube_list;

    auto sphere_angle = 0.0;

    bool player_initialize(std::shared_ptr<mv1::player>& player) {

        if (!player->load(MODEL_FILE)) {
            return false;
        }

        // �ҋ@���[�V���������[�v�w���
        player->set_attach(0, true);
        // �ړ��ʂ��w��
        player->set_movement(MODEL_MOVEMENT);
        // �R���W�����l���w��
        player->set_collision_sphere_radius(MODEL_COLLISION_SPHERE_RADIUS);

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

        for (auto i = 0; i < STEPS_CUBE_NUM; ++i) {
            std::shared_ptr<primitive::cube> cube(new primitive::cube(cube_size));

            if (!cube->load(TEXTURE_FILE_STEPS) || !cube->create()) {
                return false;
            }

            cube->set_scale(scale_list[i]);
            cube->set_position(position_list[i]);

            cube_list.emplace_back(std::move(cube));
        }

        return true;
    }
}

std::shared_ptr<world::world_base> world_initialize(const int screen_width, const int screen_height) {
    // �L�����N�^�[ ���f��
    std::shared_ptr<mv1::player> player(new mv1::player());

    if (!player_initialize(player)) {
        return nullptr;
    }

    // �n��
    std::shared_ptr<primitive::plane> plane(new primitive::plane(PLANE_SIZE, PLANE_DIVISION_NUM));

    if (!plane_initialize(plane)) {
        return nullptr;
    }

    // �n��
    std::shared_ptr<primitive::sphere> sphere(new primitive::sphere(SPHERE_RADIUS, SPHERE_DIVISION_NUM));

    if (!sphere_initialize(sphere)) {
        return nullptr;
    }

    // �K�i
    if (!create_steps()) {
        return nullptr;
    }

    // �J����
    auto camera = camera_initialize(screen_width, screen_height, player);

    if (camera == nullptr) {
        return nullptr;
    }

    // ���E
    std::shared_ptr<world::world_base> world(new world::world_base());

    // ���E�֓o�^�ƃL�����N�^�[�̃R���W�����o�^
    world->add_camera(camera);
    world->add_model(player);
    world->add_primitive(plane);
    world->add_primitive(sphere);

    for (auto&& cube : cube_list) {
        world->add_primitive(cube);
        player->set_collision_primitive(cube);
    }

    player->set_collision_primitive(plane);

    return world;
}
