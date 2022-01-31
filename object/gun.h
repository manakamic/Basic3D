#pragma once
#include "model_base.h"

struct tagMATRIX;

namespace mv1 {

    class gun : public model_base {
    public:
        // コンストラクタ
        gun() = default;
        gun(const gun&) = default; // コピー
        gun(gun&&) = default; // ムーブ

        bool initialize();

         // デストラクタ
        virtual ~gun() = default;

        MATRIX get_offset_matrix() const { return offset_matrix; }

    private:
        MATRIX offset_matrix;
    };
}
