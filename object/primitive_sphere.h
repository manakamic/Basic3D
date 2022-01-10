#pragma once

#include "primitive_base.h"

namespace primitive {

    class sphere : public primitive_base{
    public:
        // �R���X�g���N�^
        sphere();
        sphere(double radius, int division_num);
        sphere(const sphere&) = default; // �R�s�[
        sphere(sphere&&) = default; // ���[�u

         // �f�X�g���N�^
        virtual ~sphere() = default;

        bool initialize(double radius, int division_num);
        bool create() override;

        double get_radius() const { return radius; }

    protected:
        double radius;
        int division_num;
    };
}
