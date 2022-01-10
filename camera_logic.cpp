//!
//! @file camera_logic.cpp
//!
//! @brief 3D main_world.cpp �p�̃J����(camera_base class)�����̎���
//!           camera �̈ړ��ƃL�����N�^�[�Ƃ̃R���W����������
//!
#include "DxLib.h"
#include "camera_logic.h"
#include "camera_base.h"
#include "player.h"

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
    // �J�����R���W�������
    constexpr auto CAMERA_MOVEMENT = 5.0;
    constexpr auto CAMERA_ROTATION = 2.0;
    constexpr auto CAMERA_SPHERE_RADIUS = 100.0;

#if defined(_AMG_MATH)
    void process_camera(world::camera_base* camera, const math::vector4& model_last_position, const math::vector4& model_position, const double model_radius) {
#else
    void process_camera(world::camera_base * camera, const VECTOR & model_last_position, const VECTOR & model_position, const double model_radius) {
#endif
        auto camera_position = camera->get_position();

        // �J�����̃^�[�Q�b�g�ƃ|�W�V�������Z�o
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
            // �J�����ʒu����J�����^�[�Q�b�g�ւ̃x�N�g�����쐬
            auto camera_direction = camera_target - camera_position;

            // �P�ʉ�����
            camera_direction.normalized();

            // �ړ��ʂ��|���Z���Ĉړ����̃x�N�g�����쐬���āA�J�����̈ʒu�x�N�g���ɑ���
            auto camera_moved_position = camera_position + (camera_direction * movement);

            // ���f���ƃJ�����̋��œ����蔻����s��

            // �J�����ƃ��f�������ԃx�N�g�����쐬
            auto model_to_camera = camera_moved_position - camera_target;

            // �x�N�g���̒����� Sqrt ���s��Ȃ��l
            auto x = model_to_camera.get_x();
            auto y = model_to_camera.get_y();
            auto z = model_to_camera.get_z();
            auto check_size = x * x + y * y + z * z;

            // �x�N�g���̒������A�����蔻�����肽�������m�̔��a��菬������ΏՓ˂��Ă���
            // (���ۂ� Sqrt ���s��Ȃ��l�Ń`�F�b�N)
            auto collision_rasius = model_radius + CAMERA_SPHERE_RADIUS;

            if (check_size < collision_rasius * collision_rasius) {
                // �Փ˂��Ă���ꍇ�́A���m�ȏՓˈʒu�֍��W��߂�
                camera_position = (camera_direction * -collision_rasius) + camera_target;
            } else {
                // �Փ˂��Ă��Ȃ���Έړ�������
                camera_position = camera_moved_position;
            }
#else
            // �J�����ʒu����J�����^�[�Q�b�g�ւ̃x�N�g�����쐬
            VECTOR camera_direction = VSub(camera_target, camera_position);

            // �P�ʉ�����
            VECTOR dir = VNorm(camera_direction);

            // �ړ��ʂ��|���Z���Ĉړ����̃x�N�g�����쐬���āA�J�����̈ʒu�x�N�g���ɑ���
            VECTOR camera_moved_position = VAdd(camera_position, VScale(dir, movement));

            // ���f���ƃJ�����̋��œ����蔻����s��

            // �J�����ƃ��f�������ԃx�N�g�����쐬
            VECTOR model_to_camera = VSub(camera_moved_position, camera_target);

            // �x�N�g���̒����� Sqrt ���s��Ȃ��l
            auto x = model_to_camera.x;
            auto y = model_to_camera.y;
            auto z = model_to_camera.z;
            auto check_size = x * x + y * y + z * z;

            // �x�N�g���̒������A�����蔻�����肽�������m�̔��a��菬������ΏՓ˂��Ă���
            // (���ۂ� Sqrt ���s��Ȃ��l�Ń`�F�b�N)
            auto collision_rasius = model_radius + CAMERA_SPHERE_RADIUS;

            if (check_size < collision_rasius * collision_rasius) {
                // �Փ˂��Ă���ꍇ�́A���m�ȏՓˈʒu�֍��W��߂�
                camera_position = VAdd(VScale(dir, -collision_rasius), camera_target);
            } else {
                // �Փ˂��Ă��Ȃ���Έړ�������
                camera_position = camera_moved_position;
            }
#endif
        }

        camera->set_position(camera_position);
        camera->set_target(camera_target);

        if (angle != 0.0) {
#if defined(_AMG_MATH)
            // Y���̉�]�l��x(�f�B�O���[)�Ŏw�肵�čs��Ŏ擾����
            auto camera_y_matrix = math::matrix44();

            camera_y_matrix.rotate_y(angle, true);

            // ���f������J�����ւ̃x�N�g��
            auto model_to_camera = camera_position - model_position;

            // ���f������J�����ւ̃x�N�g������]�����āA���̌�Ƀ��f���ʒu�֖߂�
            camera_position = (model_to_camera * camera_y_matrix) + model_position;
#else
            // Y���̉�]�l��x(�f�B�O���[)�Ŏw�肵�čs��Ŏ擾����
            MATRIX camera_y_matrix = MGetRotY(angle * DEGREE_TO_RADIAN);

            // ���f������J�����ւ̃x�N�g��
            VECTOR model_to_camera = VSub(camera_position, model_position);

            // ���f������J�����ւ̃x�N�g������]�����āA���̌�Ƀ��f���ʒu�֖߂�
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
    // �J����
    std::shared_ptr<world::camera_base> camera(new world::camera_base(screen_width, screen_height));

    if (!camera_initialize(camera, player)) {
        return nullptr;
    }

    return camera;
}
