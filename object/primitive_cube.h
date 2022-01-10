#pragma once
#include <array>
#include <vector>
#include "primitive_base.h"

namespace math {
    class vector4;
}

namespace primitive {

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

        double get_size() const { return size; }

    protected:
        double size;
        std::vector<std::array<math::vector4, 4>> face_list;
    };
}
