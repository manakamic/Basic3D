#pragma once
#include <memory>
#include "model_base.h"

struct tagMATRIX;

namespace mv1 {

    class gun : public model_base {
    public:
        // コンストラクタ
        gun();
        gun(const gun&) = default; // コピー
        gun(gun&&) = default; // ムーブ

        virtual bool initialize();
        virtual bool setup_offset_matrix(std::shared_ptr<mv1::model_base>& target);

         // デストラクタ
        virtual ~gun() = default;

        MATRIX get_offset_matrix() const { return offset_matrix; }
        int get_target_frame() const { return target_frame; }

    private:
        MATRIX offset_matrix;
        int target_frame;
    };
}
