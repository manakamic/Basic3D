#include <array>
#include "DxLib.h"
#include "primitive_plane.h"

namespace {
    constexpr auto DEFAULT_SIZE = 100.0;
    constexpr auto DEFAULT_DIVISION_NUM = 10;
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
    constexpr COLOR_U8 DEFAULT_DIFFUSE = { 255, 255, 255, 255 };
    constexpr COLOR_U8 DEFAULT_SPECULAR = { 0, 0, 0, 0 };
    constexpr VECTOR DEFAULT_NORMAL = { 0.0f, 1.0f, 0.0f };
}

namespace primitive {

    plane::plane() : primitive_base() {
        size = DEFAULT_SIZE;
        division_num = DEFAULT_DIVISION_NUM;
    }

    plane::plane(double size, int division_num) : primitive_base() {
        this->size = size;
        this->division_num = division_num;
    }

    bool plane::initialize(double size, int division_num) {
        this->size = size;
        this->division_num = division_num;

        return true;
    }

    bool plane::create() {
        auto division_num_d = static_cast<double>(division_num);
        auto polygon_size = size / division_num_d;
        // 地面全体の中心を原点にするオフセット
        auto half_polygon_size = polygon_size * 0.5f;
        auto start_x = -half_polygon_size * division_num_d;
        auto start_z = -half_polygon_size * division_num_d;
        // 地面の基本の正方形用テーブル
        VECTOR position_0 = { 0.0f, 0.0f, 0.0f };
        VECTOR position_1 = { 0.0f, 0.0f, polygon_size };
        VECTOR position_2 = { polygon_size, 0.0f, 0.0f };
        VECTOR position_3 = { polygon_size, 0.0f, polygon_size };
        std::array<VECTOR, 4> position_list = {
            position_0, position_1, position_2, position_3
        };
        // uv のテーブル
        constexpr std::array<float, 4> u_list = { 0.0f, 0.0f, 1.0f, 1.0f };
        constexpr std::array<float, 4> v_list = { 0.0f, 1.0f, 0.0f, 1.0f };

        auto offset_x = start_x;
        auto offset_z = start_z;
        unsigned short offset_index = 0;

        for (auto z = 0; z < division_num; ++z) {
            offset_x = start_x;

            for (auto x = 0; x < division_num; ++x) {
                // 正方形を形成する
                for (auto i = 0; i < 4; ++i) {
                    VERTEX3D vertex;

                    VECTOR base = position_list[i];

                    base.x += offset_x;
                    base.z += offset_z;

                    vertex.pos = base;
                    vertex.norm = DEFAULT_NORMAL;
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

                offset_x += polygon_size;
            }

            offset_z += polygon_size;
        }

        return true;
    }
}
