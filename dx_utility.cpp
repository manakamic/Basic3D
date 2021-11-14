#include "DxLib.h"
#include "dx_utility.h"
#include "vector4.h"
#include "matrix44.h"
#include <utility>

VECTOR ToDX(math::vector4& vector) {
    return VGet(static_cast<float>(vector.get_x()), static_cast<float>(vector.get_y()), static_cast<float>(vector.get_z()));
}

math::vector4 ToMath(VECTOR& vector) {
    return math::vector4(static_cast<double>(vector.x), static_cast<double>(vector.y), static_cast<double>(vector.z));
}

MATRIX ToDX(math::matrix44& matrix) {
    MATRIX ret;

    for (auto i = 0; i < math::row_max; ++i) {
        for (auto j = 0; j < math::column_max; ++j) {
            ret.m[i][j] = static_cast<float>(matrix.get_value(i, j));
        }
    }

    return ret;
}

math::matrix44 ToMath(MATRIX& matrix) {
    math::matrix44 ret;

    for (auto i = 0; i < math::row_max; ++i) {
        for (auto j = 0; j < math::column_max; ++j) {
            ret.set_value(i, j, static_cast<double>(matrix.m[i][j]));
        }
    }

    return ret;
}