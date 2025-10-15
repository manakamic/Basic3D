#pragma once

#include <tchar.h>
#include "posture_base.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#else
struct tagVECTOR;
struct tagMATRIX;
#endif

namespace mv1 {

    class model_base : public posture_base {
    public:
        // �R���X�g���N�^
        model_base();
        model_base(const model_base&) = default; // �R�s�[
        model_base(model_base&&) = default; // ���[�u

         // �f�X�g���N�^
        virtual ~model_base();

        // C++17 �߂�l�𖳎������ꍇ�Ɍx�����o��
        [[nodiscard]] virtual bool load(const TCHAR* fileName);
        virtual bool unload();

        virtual void process();
        virtual bool render();

        virtual int get_handle() { return handle; }

        void process_posture() override;

        void set_invisible(const bool invisible) { this->invisible = invisible; };
        const bool get_invisible() const { return invisible; };

    protected:
        int handle;
        bool invisible;
    };
}
