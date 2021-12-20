#pragma once
#include <array>
#include <vector>
#include <tuple>
#include "primitive_base.h"

namespace math {
    class vector4;
}

namespace primitive {

    using face = std::tuple<std::array<math::vector4, 4>/*vertex*/, math::vector4/*normal*/>;

    class cube : public primitive_base {
    public:
        enum class face_type {
            front, right, back, left, top, bottonm
        };

        // コンストラクタ
        cube();
        cube(double size);
        cube(const cube&) = default; // コピー
        cube(cube&&) = default; // ムーブ

         // デストラクタ
        virtual ~cube() = default;

        bool initialize(double size);
        bool create() override;

        const face get_face(face_type type) const;

    protected:
        double size;
        std::vector<std::array<math::vector4, 4>> face_list;
    };
}
