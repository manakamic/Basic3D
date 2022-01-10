#pragma once

#include "primitive_base.h"

namespace primitive {

    class plane : public primitive_base {
    public:
        // コンストラクタ
        plane();
        plane(double size, int division_num);
        plane(const plane&) = default; // コピー
        plane(plane&&) = default; // ムーブ

         // デストラクタ
        virtual ~plane() = default;

        bool initialize(double size, int division_num);
        bool create() override;

        const face get_info() const;

    protected:
        double size;
        int division_num;
    };
}
