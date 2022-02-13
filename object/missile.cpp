//!
//! @file missile.h
//!
//! @brief �~�T�C���̃��f���������N���X
//!
//! @details
//! �� �~�T�C������d�l
//! ���˂܂ł͓��莞�ԂŃJ�E���g�_�E�����s���A�b�̕�����`����s��
//! ���ˌ�̓~�T�C���������I���܂� player class �̍��W�� �x���̕����`����s��
//! ���ˌ�͓��莞�Ԃ܂Ő^��ɒ��i����
//! ���莞�Ԍ�� player class �̍��W�Ƀz�[�~���O����
//! �z�[�~���O������ ����̊Ԋu�œ���̊����ōs��
//! �z�[�~���O���͓���̍����������Ə������I������
//!
//! �� �~�T�C���̎p������
//! �����o�ϐ��� posture(MATRIX�^)�̊e�s�̒l��
//! �p������p�̂��ꂼ��� x / y/ z ��(�x�N�g��)�̒l�Ƃ��Ďg�p����
//! (���K������� �����Ƃ��ĉ�]�s��Ƃ��Ĉ���)
//!
//! �� �ʕ`��ɂ���
//! ��ʂ̉E�� 1/4 �̗̈�ɕʕ`�揈�����s���A���˒��̃~�T�C���̎p����������l�ɂ���
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
    constexpr auto model_scale = 0.2; // ���f���̕`��X�P�[��
    constexpr auto count_down_millisecond = 5000; // ���˂܂ł̃J�E���g�_�E��
    constexpr auto launch_millisecond = 2000; // ���ˌ�̒��i����
    constexpr auto homing_millisecond = 100; // �z�[�~���O�����̊Ԋu
    constexpr auto homing_end_y = 50.0; // ���e����� Y �l
    constexpr auto default_velocity = 20.0f; // �ړ���
    constexpr auto default_homing_rate = 0.2f; // �z�[�~���O���銄��
    constexpr auto explode_add_angle = 15.0; // �����̉�]
    constexpr auto explode_multi_scale = 1.075; // �����̃X�P�[��
    constexpr auto explode_end_scale = 20.0; // �����̍ŏI�X�P�[��
    constexpr auto line_width = 10; // �ʘg�`��p�̃��C���̑���
    constexpr auto separate_distance = 3250.0f; // �ʘg�`�掞�̃��f���ƃJ�����̋���
    constexpr auto warning_text_offset_y = 130.0f; // �x���`��̃I�t�Z�b�g�l
    constexpr auto text_format = _T("%.2f"); // �J�E���g�_�E���p
    constexpr auto warning_message = _T("W A R N I N G"); // �v���C���[�p�̌x��
    const auto text_color = GetColor(255, 255, 0); // �����`��p
    const auto line_color = GetColor(32, 32, 32); // �ʘg�`��p

    // �ʘg�`��p�̊e�� 1 �x�����v�Z����Ηǂ��l
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

        // ���f�����傫���̂ŃX�P�[����������
#if defined(_AMG_MATH)
        set_scale(math::vector4(model_scale, model_scale, model_scale));
#else
        set_scale(VGet(model_scale, model_scale, model_scale));
#endif

        // DrawString �p�ɕ`�敶����̔����̉������v�Z����
        const auto count_down_text = _T("0.00");

        count_down.offset_x = GetDrawStringWidth(count_down_text, std::strlen(count_down_text)) / 2;
        player_warning.offset_x = GetDrawStringWidth(warning_message, std::strlen(warning_message)) / 2;

        // �ʘg�`��p�̒l���v�Z
        // ��ʂ̉E��ŁA�傫���͉�ʂ� 1/4 
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

    // 3D ���W���X�N���[�����W�ɕϊ�
    std::tuple<bool, int, int> missile::check_in_screen(const VECTOR& position) const {
        VECTOR screeen = ConvWorldPosToScreenPos(position);
        auto x = static_cast<int>(screeen.x);
        auto y = static_cast<int>(screeen.y);
        auto in_x = (x >= 0) && (x < screen_width);
        auto in_y = (y >= 0) && (x < screen_height);
        auto in_z = (screeen.z > 0.0f) && (screeen.z < 1.0f); // ConvWorldPosToScreenPos �̎d�l
        auto in = in_x && in_y && in_z;

        return std::make_tuple(in, x, y);
    }

    // ���ˍ��W���Z�b�g����
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

    // ���ˏ���
    void missile::process_fire() {
        if (state == state::launch || state == state::homing) {
            VECTOR move = process_moving();

            // �z�[�~���O���Ɏw��� Y �l����������珈���I���̎d�l�Ƃ���
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

                // �e�평����(���̔��˗p)
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

        // �z�[�~���O�������I������~�T�C���� XZ ���W�ŏ�������
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

        // ��]���Ȃ���g�傳����
#if defined(_AMG_MATH)
        explosion->set_rotation(math::vector4(0.0, explode_angle, 0.0));
        explosion->set_scale(math::vector4(explode_scale, explode_scale, explode_scale));
#else
        explosion->set_rotation(VGet(0.0f, explode_angle, 0.0f));
        explosion->set_scale(VGet(explode_scale, explode_scale, explode_scale));
#endif

        explode_angle += explode_add_angle;
        explode_scale *= explode_multi_scale;

        // �w��̃X�P�[���l�𒴂����甚���I���̎d�l�Ƃ���
        if (explode_scale > explode_end_scale) {
            state = state::none;
            explosion->set_invisible(true);
        }
    }

    // �ړ�����
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

    // ���˂��čŏ��̒��i(�㏸)����
    void missile::process_moving_launch() {
        // ��{�I�ɐ^��ɒ��i�Ȃ̂ŉ��������͂��Ȃ�
        // (���݂̎p���� Y ������ velocity �����������Ȃ̂�)
        // ����� launch �I������ ����� homing �����̃Z�b�g���s������
        if (get_count() > launch_millisecond) {
            state = state::homing;
            start_count();

            // �R�[���o�b�N�Ńz�[�~���O���̎p�����䂾�����s��
            // �z�[�~���O������ process_moving_homing �̉ӏ�
            auto update_after = [this](posture_base* base) -> void {
                // posture_base �̃}�g���N�X�����̓X�P�[�����܂܂��̂�
                // initialize �ŃZ�b�g���� scale �l�̌v�Z���s��Ȃ���΂Ȃ�Ȃ�
                // ���݂̎p����\�������o�ϐ��� posture �́A���̂܂܉�]�s��Ƃ��Ďg����
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

    // �z�[�~���O����
    void missile::process_moving_homing() {
        if (get_count() > homing_millisecond) {
            // ���g(�~�T�C��)���� player �ւ̒P�ʃx�N�g�����쐬
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
            // �������z�[�~���O��������
            // (�~�T�C���͒��i����̂Ŏp���𐧌䂷��̂��z�[�~���O�ɂȂ�)
            // 100% �� player �ւ̃x�N�g����K�����Ă��܂��� �K���������Ă��܂��̂�
            // �~�T�C���̌���( Y ��)����Aplayer �ւ�(������)�P�ʃx�N�g�������ԃx�N�g���쐬����
            // ���̃x�N�g��������(���ꂪ�z�[�~���O�̊���)�ŏ��������Ďg�p����
            VECTOR homing = VScale(VSub(to_player, y), homing_rate);

            // �݂��ɒ�����Ԃ� x / y / z ���𓯂���������������
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
            count_down_time = count_down / 1000.0f; // �b�֕ϊ�

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
        // player ���f���̓��゠����ɂȂ�悤�ɃI�t�Z�b�g��������
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

    // �ʘg�`��
    void missile::separate_render() {
        if (world  == nullptr || -1 == camera_index || is_stand_by() || is_explode()) {
            return;
        }

        // ��ʂ̉E��(�S�̂�1/4�T�C�Y)��`��Ώۂɂ���ݒ�
        SetDrawArea(base_width, 0, screen_width, quarter_height);
        SetCameraScreenCenter(base_width + (quarter_width / 2), quarter_height / 2);

        // �J������؂�ւ���
        auto now_camera_index = world->get_camera_index();

        world->set_camera_index(camera_index);
        world->process_camera();
        world->set_camera_index(now_camera_index);

        // Z �o�b�t�@���N���A���čĕ`��
        ClearDrawScreenZBuffer();

        world->render_primitive();
        world->render_model();

        // �g�`��
        DrawBox(base_width, 0, line_width_pos, quarter_height, line_color, TRUE);
        DrawBox(line_width_pos, line_height_pos, screen_width, quarter_height, line_color, TRUE);

        // �J�����̐ݒ���߂�
        SetCameraScreenCenter(screen_width * 0.5f, screen_height * 0.5f);
        SetDrawAreaFull();
    }
}
