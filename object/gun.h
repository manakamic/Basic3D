#pragma once
#include <memory>
#include "model_base.h"

struct tagMATRIX;

namespace mv1 {

    class gun : public model_base {
    public:
        // �R���X�g���N�^
        gun();
        gun(const gun&) = default; // �R�s�[
        gun(gun&&) = default; // ���[�u

        virtual bool initialize();
        virtual bool setup_offset_matrix(std::shared_ptr<mv1::model_base>& target);

         // �f�X�g���N�^
        virtual ~gun() = default;

        MATRIX get_offset_matrix() const { return offset_matrix; }
        int get_target_frame() const { return target_frame; }

    private:
        MATRIX offset_matrix;
        int target_frame;
    };
}
