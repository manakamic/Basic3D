#pragma once

#include "primitive_base.h"

namespace primitive {

    class plane : public primitive_base {
    public:
        // �R���X�g���N�^
        plane();
        plane(double size, int division_num);
        plane(const plane&) = default; // �R�s�[
        plane(plane&&) = default; // ���[�u

         // �f�X�g���N�^
        virtual ~plane() = default;

        bool initialize(double size, int division_num);
        bool create() override;

        const face get_info() const;

    protected:
        double size;
        int division_num;
    };
}
