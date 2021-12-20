#include "DxLib.h"
#include "dx_utility.h"
#include "vector4.h"
#include "matrix44.h"
#include "primitive_cube.h"

namespace {
    constexpr auto DEFAULT_SIZE = 100.0;
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
    constexpr COLOR_U8 DEFAULT_DIFFUSE = { 255, 255, 255, 255 };
    constexpr COLOR_U8 DEFAULT_SPECULAR = { 0, 0, 0, 0 };

    // 6 面分の法線
    math::vector4 normal_front  = {  0.0,  0.0, -1.0 };
    math::vector4 normal_right  = {  1.0,  0.0,  0.0 };
    math::vector4 normal_back   = {  0.0,  0.0,  1.0 };
    math::vector4 normal_left   = { -1.0,  0.0,  0.0 };
    math::vector4 normal_top    = {  0.0,  1.0,  0.0 };
    math::vector4 normal_bottom = {  0.0, -1.0,  0.0 };
    std::array<math::vector4, 6> normal_list = {
        normal_front, normal_right, normal_back, normal_left, normal_top, normal_bottom
    };
}

namespace primitive {

    cube::cube() : primitive_base() {
        size = DEFAULT_SIZE;
    }

    cube::cube(double size) : primitive_base() {
        this->size = size;
    }

    bool cube::initialize(double size) {
        this->size = size;

        return true;
    }

    bool cube::create() {
        // 基本の 8 頂点
        auto half_size = size * 0.5;
        auto position_0 = math::vector4(-half_size,  half_size, -half_size);
        auto position_1 = math::vector4(-half_size,  half_size,  half_size);
        auto position_2 = math::vector4( half_size,  half_size, -half_size);
        auto position_3 = math::vector4( half_size,  half_size,  half_size);
        auto position_4 = math::vector4(-half_size, -half_size, -half_size);
        auto position_5 = math::vector4(-half_size, -half_size,  half_size);
        auto position_6 = math::vector4( half_size, -half_size, -half_size);
        auto position_7 = math::vector4( half_size, -half_size,  half_size);
        std::array<math::vector4, 4> face_front = {
            position_4, position_0, position_6, position_2
        };
        std::array<math::vector4, 4> face_right = {
            position_6, position_2, position_7, position_3
        };
        std::array<math::vector4, 4> face_back = {
            position_7, position_3, position_5, position_1
        };
        std::array<math::vector4, 4> face_left = {
            position_5, position_1, position_4, position_0
        };
        std::array<math::vector4, 4> face_top = {
            position_0, position_1, position_2, position_3
        };
        std::array<math::vector4, 4> face_bottm = {
            position_4, position_5, position_6, position_7
        };

        face_list.emplace_back(face_front);
        face_list.emplace_back(face_right);
        face_list.emplace_back(face_back);
        face_list.emplace_back(face_left);
        face_list.emplace_back(face_top);
        face_list.emplace_back(face_bottm);

        // uv のテーブル
        constexpr std::array<float, 4> u_list = { 0.0f, 0.0f, 1.0f, 1.0f };
        constexpr std::array<float, 4> v_list = { 0.0f, 1.0f, 0.0f, 1.0f };

        unsigned short offset_index = 0;

        // cube の 6 面
        for (auto face = 0; face < 6; ++face) {
            // 正方形を形成する
            for (auto i = 0; i < 4; ++i) {
                VERTEX3D vertex;
                auto face_vertices = face_list[face];

                vertex.pos = ToDX(face_vertices[i]);
                vertex.norm = ToDX(normal_list[face]);
                vertex.dif = DEFAULT_DIFFUSE;
                vertex.spc = DEFAULT_SPECULAR;
                vertex.u = u_list[i];
                vertex.v = v_list[i];

                this->vertex->emplace_back(std::move(vertex));
            }

            index->push_back(offset_index);
            index->push_back(offset_index + 1);
            index->push_back(offset_index + 2);

            index->push_back(offset_index + 2);
            index->push_back(offset_index + 1);
            index->push_back(offset_index + 3);

            offset_index += 4;
        }

        return true;
    }

    const face cube::get_face(face_type type) const {
        auto index = static_cast<int>(type);
#if defined(_AMG_MATH)
        std::array<math::vector4, 4> face_vertices = {
            face_list[index][0] * posture,
            face_list[index][1] * posture,
            face_list[index][2] * posture,
            face_list[index][3] * posture
        };
        auto face_normal = normal_list[index] * posture;
#else
        MATRIX temp = posture;
        auto posture_matrix = ToMath(temp);
        std::array<math::vector4, 4> face_vertices = {
            face_list[index][0] * posture_matrix,
            face_list[index][1] * posture_matrix,
            face_list[index][2] * posture_matrix,
            face_list[index][3] * posture_matrix
        };
        auto face_normal = normal_list[index] * posture_matrix;
#endif

        return std::make_tuple(face_vertices, face_normal);
    }
}
