#pragma once
#include "model_base.h"

struct tagMATRIX;

namespace mv1 {

    class gun : public model_base {
    public:
        // �R���X�g���N�^
        gun() = default;
        gun(const gun&) = default; // �R�s�[
        gun(gun&&) = default; // ���[�u

        bool initialize();

         // �f�X�g���N�^
        virtual ~gun() = default;

        MATRIX get_offset_matrix() const { return offset_matrix; }

    private:
        MATRIX offset_matrix;
    };
}
