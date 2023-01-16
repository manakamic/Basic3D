#include <array>
#include "DxLib.h"
#include "primitive_sphere.h"

namespace {
    constexpr auto DEFAULT_RADIUS = 10.0;
    constexpr auto DEFAULT_DIVISION_NUM = 16;
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
    constexpr COLOR_U8 DEFAULT_DIFFUSE = { 255, 255, 255, 255 };
    constexpr COLOR_U8 DEFAULT_SPECULAR = { 0, 0, 0, 0 };
    constexpr float PI2_ANGLE = 360.0f;
    constexpr float PI_ANGLE = 180.0f;
}

namespace primitive {

    sphere::sphere() : primitive_base() {
        radius = DEFAULT_RADIUS;
        division_num = DEFAULT_DIVISION_NUM;
    }

    sphere::sphere(double radius, int division_num) : primitive_base() {
        this->radius = radius;
        this->division_num = division_num;
    }

    bool sphere::initialize(double radius, int division_num) {
        this->radius = radius;
        this->division_num = division_num;

        return true;
    }

    bool sphere::create() {
        VECTOR start = VGet(0.0f, -radius, 0.0f);
        int division_half_num = division_num / 2;
        unsigned short offset_index = 0;
        float unit_angle_x = PI_ANGLE / static_cast<float>(division_half_num);
        float unit_angle_y = PI2_ANGLE / static_cast<float>(division_num);
        float angle_x = 0.0f;
        float angle_y = 0.0f;
        float angle_next_x = unit_angle_x;
        float angle_next_y = unit_angle_y;

        for (auto height = 0; height < division_half_num; ++height) {
            const std::array<float, 4> angle_x_list = { angle_x, angle_next_x, angle_x, angle_next_x };

            angle_y = 0.0f;
            angle_next_y = unit_angle_y;

            for (auto width = 0; width < division_num; ++width) {
                const std::array<float, 4> angle_y_list = { angle_y, angle_y, angle_next_y, angle_next_y };

                for (auto i = 0; i < 4; ++i) {
                    const MATRIX rot_x = MGetRotX(angle_x_list[i] * DEGREE_TO_RADIAN);
                    const MATRIX rot_y = MGetRotY(angle_y_list[i] * DEGREE_TO_RADIAN);
                    const VECTOR pos = VTransform(start, MMult(rot_x, rot_y));

                    VERTEX3D v;

                    v.pos = pos;
                    v.norm = VNorm(pos);
                    v.dif = DEFAULT_DIFFUSE;
                    v.spc = DEFAULT_SPECULAR;
                    v.u = 1.0f - angle_y_list[i] / PI2_ANGLE;
                    v.v = 1.0f - angle_x_list[i] / PI_ANGLE;

                    vertex->emplace_back(std::move(v));
                }

                index->push_back(offset_index);
                index->push_back(offset_index + 1);
                index->push_back(offset_index + 2);

                index->push_back(offset_index + 2);
                index->push_back(offset_index + 1);
                index->push_back(offset_index + 3);

                offset_index += 4;

                angle_y = angle_next_y;
                angle_next_y += unit_angle_y;
            }

            angle_x = angle_next_x;
            angle_next_x += unit_angle_x;
        }

        return true;
    }
}
