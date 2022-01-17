#include "DxLib.h"
#include "player.h"
#include "utility.h"
#include "vector4.h"
#include "matrix44.h"
#include "primitive_plane.h"
#include "primitive_sphere.h"
#include "primitive_cube.h"
#include "dx_utility.h"

namespace {
    constexpr auto DEFAULT_COLLISION_RADIUS = 55.0;
    constexpr auto GRAVITY = 9.8; // �d�͉����x
    constexpr auto DEFAULT_JUMP_POWER = 110.0;
    constexpr auto DEFAULT_JUMP_ANGLE = 85.0;
    constexpr auto DEFAULT_BACK_POWER = 60.0;
    constexpr auto DEFAULT_BACK_ANGLE = 97.5; // 90�x���傫���Ȃ��ƌ��ɂȂ�Ȃ�
#if !defined(_AMG_MATH)
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
#endif

    const std::array<primitive::cube::face_type, 4> cube_side = {
        primitive::cube::face_type::front,
        primitive::cube::face_type::right,
        primitive::cube::face_type::back,
        primitive::cube::face_type::left
    };

    const auto debug_sphere_diffuse = GetColor(255, 255, 255);
    const auto debug_sphere_specular = GetColor(255, 255, 255);
    const auto debug_direction_color = GetColor(255, 0, 0);
}

namespace mv1 {

    player::player() : model() {
        collision_ride_on = nullptr;
        collision_sphere_radius = DEFAULT_COLLISION_RADIUS;
        movement = 1.0;
        rotate = 1.0;
        jump_power = DEFAULT_JUMP_POWER;
        jump_angle = DEFAULT_JUMP_ANGLE;
        jump_timer = 0.0;
        is_jump = false;
        is_fall = false;
        is_back = false;
        is_forward = false;
        is_attack = false;
        is_debug = false;

#if defined(_AMG_MATH)
        // ���ɏ����l���K�v�Ȃ���΃R���X�g���N�^�ł̒l�ɂȂ�
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
        auto start_back = false;
        auto rotate_value = 0.0;

        if (1 == CheckHitKey(KEY_INPUT_SPACE)) {
            // jump �� attack �͔r������
            if (!is_attack && !is_jump) {
                is_jump = true;
                start_jump = true;
            }
        }

        if (1 == CheckHitKey(KEY_INPUT_V)) {
            // jump �� attack �͔r������
            if (!is_jump && !is_attack) {
                is_attack = true;
                start_attack = true;
            }
        }

        is_forward = (1 == CheckHitKey(KEY_INPUT_UP));

        if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
            if (!is_back) {
                is_back = true;
                is_jump = true;
                start_back = true;
            }
        }

        if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            rotate_value = -rotate;
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            rotate_value = rotate;
        }

        // jump �� ����ȊO�͔r���������s��
        if (is_jump) {
            if (start_jump) {
                process_jump_initialize(jump_type::normal);
            }
            else if (start_back) {
                process_jump_initialize(jump_type::back);
            }

            process_jump();
        }
        else {
            // attack �� ����ȊO�͔r���������s��
            if (is_attack) {
                process_attack(start_attack);
            }
            else {
                process_rotate(rotate_value);
                process_forward();
            }
        }

        model::process();

        process_collision();
    }

    void player::set_collision_primitive(const std::shared_ptr<primitive::primitive_base>& primitive) {
        collision_list.emplace_back(primitive);
    }

    void player::process_collision() {
        for (auto&& primitive : collision_list) {
            auto ret = process_collision_plane(primitive);

            if (!ret) {
                ret = process_collision_sphere(primitive);
            }

            if (!ret) {
                ret = process_collision_cube(primitive);
            }
        }
    }

    bool player::process_collision_plane(std::shared_ptr<primitive::primitive_base>& primitive) {
        auto plane = std::dynamic_pointer_cast<primitive::plane>(primitive);

        if (plane == nullptr) {
            return false;
        }

        // �W�����v���Ȃ��ɏ�邩�`�F�b�N
        if (is_jump) {
            auto info = plane->get_info();

            if (process_jump_landing(position, std::get<0>(info))) {
                process_jump_finished(primitive);
            }
        }
        else {
            // ��ɏ���Ă����痎���`�F�b�N
            if (collision_ride_on != nullptr && collision_ride_on == primitive) {
                auto info = plane->get_info();

                check_fall(std::get<0>(info));
            }
        }

        return true;
    }

    bool player::process_collision_sphere(std::shared_ptr<primitive::primitive_base>& primitive) {
        auto sphere = std::dynamic_pointer_cast<primitive::sphere>(primitive);

        if (sphere == nullptr) {
            return false;
        }

        if (!check_sphere_distance(sphere)) {
            return true; // sphere �Ȃ̂� true
        }

        // TODO : ���̃R���W���������͖�����

        return true;
    }

    bool player::process_collision_cube(std::shared_ptr<primitive::primitive_base>& primitive) {
        auto cube = std::dynamic_pointer_cast<primitive::cube>(primitive);

        if (cube == nullptr) {
            return false;
        }

        // �͈͊O�Ȃ珈�����Ȃ�s
        if (!check_cube_distance(cube)) {
            return true; // cube �Ȃ̂� true
        }

        if (is_jump) {
            // �W�����v���Ȃ��ɏ�邩�`�F�b�N
            auto face_top = cube->get_face(primitive::cube::face_type::top);

            if (process_jump_landing(position, std::get<0>(face_top))) {
                process_jump_finished(primitive);
            }
        }
        else {
            // ��ɏ���Ă����痎���`�F�b�N
            if (collision_ride_on != nullptr && collision_ride_on == primitive) {
                auto face_top = cube->get_face(primitive::cube::face_type::top);

                if (check_fall(std::get<0>(face_top))) {
                    is_jump = true;
                    is_fall = true;
                    process_jump_initialize(jump_type::fall);
                }
            }
        }

        // �������łȂ����
        if (!is_fall) {
            // ���ʂƂ̃R���W��������
            auto collision_line = make_collision_line();

            for (auto iterator = cube_side.begin(); iterator != cube_side.end(); ++iterator) {
                auto face = cube->get_face(*iterator);
                auto normal = std::get<1>(face);

                // �ʂ̖@���ƌ����̃x�N�g���œ��ς��s���O�����̎���������
#if defined(_AMG_MATH)
                auto dot = normal.dot(direction);
#else
                auto dot = VDot(ToDX(normal), direction);
#endif
                // �o�b�N�W�����v���͏��O
                if (is_back || dot <= 0.0) {
                    auto vertices = std::get<0>(face);
                    auto collision = math::utility::collision_polygon_line(vertices[0], vertices[1], vertices[2], vertices[3], std::get<0>(collision_line), std::get<1>(collision_line));

                    if (std::get<0>(collision)) {
                        set_collision_to_position(std::get<1>(collision));

                        if (is_jump) {
                            jump_velocity_initialize_reflect();
                        }
                        else if (is_forward) {
                            auto normal = std::get<1>(face);

                            process_press_forward(normal);
                        }
                        // ���̑��ʂƂ̔���͕s�v
                        break;
                    }
                }
            }
        }

        return true;
    }

    void player::process_press_forward(const math::vector4& normal) {
#if defined(_AMG_MATH)
        auto player_dir = direction;
#else
        auto player_dir = ToMath(direction);
#endif
        auto up = math::vector4(0.0, 1.0, 0.0);
        auto xz_dir = math::vector4(normal.get_x(), 0.0, normal.get_z());

        xz_dir.normalized();

        auto check = xz_dir.cross(player_dir);
        auto forward = math::vector4();

        // �O�ς̌��ʂ� Y �̒l�̐����ō��E����
        if (check.get_y() < 0.0) {
            forward = xz_dir.cross(up);
        }
        else if (check.get_y() > 0.0) {
            forward = up.cross(xz_dir);
        }

        // �ʂɓ������Ă���̂œ��ς̒l�� -1 ~ 0
        auto power = movement * (1.0 + xz_dir.dot(player_dir));
        auto moved = forward * power;

#if defined(_AMG_MATH)
        position = position + moved;
#else
        position = VAdd(position, ToDX(moved));
#endif
    }

    // primitive::sphere �Ƃ̋������`�F�b�N����
    // (����������Ă���Ȃ�R���W�������������Ȃ�)
    bool player::check_sphere_distance(const std::shared_ptr<primitive::sphere>& sphere) const {
#if defined(_AMG_MATH)
        auto distance = sphere->get_position() - position;
        auto check = distance.get_x() * distance.get_x() + distance.get_y() * distance.get_y() + distance.get_z() * distance.get_z();
        auto radius = sphere->get_radius() * sphere->get_scale().get_x();
#else
        VECTOR distance = VSub(sphere->get_position(), position);
        auto check = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
        auto radius = sphere->get_radius() * sphere->get_scale().x; // �X�P�[���� x �l���g���Ă݂�
#endif
        auto player_range = (collision_sphere_radius + movement) * 2.0; // �M���M�����Ƃ��蔲�����N����\��������̂� 2 �{���Ă���
        auto range = radius + player_range;

        if (check > range * range) {
            return false;
        }

        return true;
    }

    // primitive::cube �Ƃ̋������`�F�b�N����
    // (����������Ă���Ȃ�R���W�������������Ȃ�)
    bool player::check_cube_distance(const std::shared_ptr<primitive::cube>& cube) const {
        // ����R���W��������������̂��������ׂȂ̂ł�����x��G�c�Ȕ���ŗǂ�
        // (�t�ɃM���M�����Ƃ��蔲�����N����\�������܂�)
#if defined(_AMG_MATH)
        auto distance = cube->get_position() - position;
        auto check = distance.get_x() * distance.get_x() + distance.get_y() * distance.get_y() + distance.get_z() * distance.get_z();
        auto cube_size = cube->get_size() * cube->get_scale().get_x();
#else
        VECTOR distance = VSub(cube->get_position(), position);
        auto check = distance.x * distance.x + distance.y * distance.y + distance.z * distance.z;
        auto cube_size = cube->get_size() * cube->get_scale().x; // �X�P�[���� x �l���g���Ă݂�
#endif
        auto player_range = (collision_sphere_radius + movement) * 2.0; // �M���M�����Ƃ��蔲�����N����\��������̂� 2 �{���Ă���
        auto range = cube_size + player_range; // �{���Ȃ� cube �̑Ίp���̒����̔������g�p���邪 ��������]�T���������� size �����̂܂܎g�p����

        if (check > range * range) {
            return false;
        }

        return true;
    }

    // ��`�̓��O����ŗ����̃`�F�b�N���s��
    bool player::check_fall(const std::array<math::vector4, 4>& vertices) const {
#if defined(_AMG_MATH)
        auto& check = position;
#else
        VECTOR temp = position;
        auto check = ToMath(temp);
#endif

        if (math::utility::inside_triangle_point(vertices[0], vertices[1], vertices[2], check)) {
            return false;
        }

        if (math::utility::inside_triangle_point(vertices[1], vertices[3], vertices[2], check)) {
            return false;
        }

        // ��`�͈̔͊O
        return true;
    }

    // �R���W�������̔��a�������������ݒn�̃x�N�g�����쐬
    math::vector4 player::make_collision_position() const {
#if defined(_AMG_MATH)
        auto radius_position = position + (direction * collision_sphere_radius);
#else
        VECTOR radius = VAdd(position, VScale(direction, collision_sphere_radius));
        auto radius_position = math::vector4(radius.x, radius.y, radius.z);
#endif

        return radius_position;
    }

    // �R���W�������̔��a�Ō��ݒn�Ƃ̐������쐬
    const std::tuple<math::vector4, math::vector4> player::make_collision_line() const {
        // �R���W�������̔��a���̌��ݒn�̃x�N�g�����쐬
#if defined(_AMG_MATH)
        auto start = position;
#else
        auto start = math::vector4(position.x, position.y, position.z);
#endif
        auto end = make_collision_position();

        // �R���W�������̔��a���̍���������
        start.add(0.0, collision_sphere_radius, 0.0);
        end.add(0.0, collision_sphere_radius, 0.0);

        return std::make_tuple(start, end);
    }

    // �R���W�����ʒu����R���W�������̑傫�����������Č��݈ʒu���Z�b�g����
    void player::set_collision_to_position(math::vector4& collision) {
        // make_collision_sphere �Ƌt�̏���������
        collision.add(0.0, -collision_sphere_radius, 0.0);

#if defined(_AMG_MATH)
        position = collision + (direction * -collision_sphere_radius);
#else
        VECTOR inverse_radius = VScale(direction, -collision_sphere_radius);

        position = VAdd(ToDX(collision), inverse_radius);
#endif
    }

    void player::process_forward() {
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
            // index 2 �̃A�j���[�V�������I�������s������
            std::function<void(void)> attack_end = [this](void) -> void {
                is_attack = false;
                set_blend(0, 5, true);
            };

            set_blend(2, 5, false, attack_end);
        }
    }

    void player::process_jump() {
        // ========================================================================================================
        // jump �̌v�Z���W�b�N
        position = process_jump_logic_physics();
        //position = process_jump_logic_vector();
        // ========================================================================================================
    }

    void player::process_jump_initialize(const jump_type type) {
        jump_timer = 0.0;
        jump_start_position = position;

        collision_ride_on.reset();

        set_blend(3, 10, false);

        switch (type) {
        case jump_type::normal:
        default:
            jump_velocity_initialize_normal();
            break;

        case jump_type::fall:
            jump_velocity_initialize_fall();
            break;

        case jump_type::back:
            jump_velocity_initialize_back();
            break;
        }
    }

    void player::jump_velocity_initialize_normal() {
        // jump �̃x�N�g�����쐬����
        // jump_power �̊�{�̃x�N�g��(����)���쐬���� jump_angle �̊p�x�ŌX����
        // �Ō�� player �̕����ɉ�]�����Č��������킹��
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

    void player::jump_velocity_initialize_fall() {
#if defined(_AMG_MATH)
        jump_start_position = make_collision_position();
        jump_velocity = math::vector4();
#else
        auto start_position = make_collision_position();
        jump_start_position = ToDX(start_position);
        jump_velocity = VGet(0.0f, 0.0f, 0.0f);
#endif
    }

    void player::jump_velocity_initialize_back() {
#if defined(_AMG_MATH)
        auto back_base = math::vector4(0.0, 0.0, -DEFAULT_BACK_POWER);
        auto back_rotate = math::matrix44();

        back_rotate.rotate_x(DEFAULT_BACK_ANGLE, true);
        back_rotate.rotate_y(rotation.get_y(), false);

        jump_velocity = back_base * back_rotate;
#else
        VECTOR back_base = VGet(0.0f, 0.0f, -DEFAULT_BACK_POWER);
        MATRIX back_rotate = MMult(MGetRotX(DEFAULT_BACK_ANGLE * DEGREE_TO_RADIAN), MGetRotY(rotation.y * DEGREE_TO_RADIAN));

        jump_velocity = VTransform(back_base, back_rotate);
#endif
    }

    void player::jump_velocity_initialize_reflect() {
#if defined(_AMG_MATH)
        auto reflect_matrix = math::matrix44();

        reflect_matrix.rotate_y(180.0, true);
        jump_velocity = jump_velocity * reflect_matrix;
#else
        jump_velocity = VTransform(jump_velocity, MGetRotY(180.0f * DEGREE_TO_RADIAN));
#endif
    }

    void player::process_jump_finished(std::shared_ptr<primitive::primitive_base>& primitive) {
        collision_ride_on.reset(); // ���L�������
        collision_ride_on = primitive;

        is_jump = false;
        is_fall = false;
        is_back = false;

        set_blend(0, 10, true);
    }

#if defined(_AMG_MATH)

    math::vector4 player::process_jump_logic_physics() {
        jump_timer += 1.0;

        // �������̎��Ōv�Z����
        // y = y0 + v0t - 1/2gt^2
        auto jump_position = jump_start_position + (jump_velocity * jump_timer);
        auto jump_y = (0.5 * GRAVITY * jump_timer * jump_timer);

        jump_position.add(0.0, -jump_y, 0.0);

        return jump_position;
    }

    math::vector4 player::process_jump_logic_vector() {
        // �x�N�g���ŏ�������
        auto jump_position = position + jump_velocity;

        jump_velocity.add(0.0, -GRAVITY, 0.0);

        return jump_position;
    }

    bool player::process_jump_landing(const math::vector4& jump_position, const std::array<math::vector4, 4>& vertices) {
        if (last_position.get_y() < jump_position.get_y()) {
            return false;
        }

        auto collision = math::utility::collision_polygon_line(vertices[0], vertices[1], vertices[2], vertices[3], last_position, jump_position);

        if (std::get<0>(collision)) {
            position = std::get<1>(collision);
            return true;
        }

        return false;
    }

#else

    VECTOR player::process_jump_logic_physics() {
        // �������̎��Ōv�Z����
        // y = y0 + v0t - 1/2gt^2
        VECTOR jump_position = VAdd(jump_start_position, VScale(jump_velocity, jump_timer));
        auto jump_y = (0.5 * GRAVITY * jump_timer * jump_timer);

        jump_position.y -= jump_y;
        jump_timer += 1.0;

        return jump_position;
    }

    VECTOR player::process_jump_logic_vector() {
        // �x�N�g���ŏ�������
        VECTOR jump_position = VAdd(position, jump_velocity);

        jump_velocity.y -= GRAVITY;

        return jump_position;
    }

    bool player::process_jump_landing(const VECTOR& jump_position, const std::array<math::vector4, 4>& vertices) {
        if (last_position.y < jump_position.y) {
            return false;
        }

        auto line_start = math::vector4(last_position.x, last_position.y, last_position.z);
        auto line_end = math::vector4(jump_position.x, jump_position.y, jump_position.z);
        auto collision = math::utility::collision_polygon_line(vertices[0], vertices[1], vertices[2], vertices[3], line_start, line_end);

        if (std::get<0>(collision)) {
            auto hit_position = std::get<1>(collision);

            position = VGet(hit_position.get_x(), hit_position.get_y(), hit_position.get_z());

            return true;
        }

        return false;
    }

#endif

    bool player::render() {
        auto ret = model::render();

        if (is_debug && ret) {
#if defined(_AMG_MATH)
            VECTOR center = ToDX(position);
            VECTOR dir = ToDX(direction);
#else
            VECTOR center = position;
            VECTOR dir = direction;
#endif
            center.y += collision_sphere_radius;

            DrawLine3D(center, VAdd(center, VScale(dir, 100.0f)), debug_direction_color);
            DrawSphere3D(center, collision_sphere_radius, 16, debug_sphere_diffuse, debug_sphere_specular, FALSE);
        }

        return ret;
    }
}
