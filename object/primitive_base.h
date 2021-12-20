#pragma once
#include <tchar.h>
#include <memory>
#include <vector>
#include "posture_base.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#else
struct tagVECTOR;
struct tagMATRIX;
struct tagVERTEX3D;
#endif

namespace primitive {
    class primitive_base : public posture_base {
    public:
        // �R���X�g���N�^
        primitive_base();
        primitive_base(const primitive_base&) = default; // �R�s�[
        primitive_base(primitive_base&&) = default; // ���[�u

         // �f�X�g���N�^
        virtual ~primitive_base();

        virtual bool load(const TCHAR* fileName);
        virtual bool unload();

        virtual bool create() = 0;

        virtual void process();
        virtual bool render();

        const std::shared_ptr<std::vector<VERTEX3D>>& get_vertex() const { return vertex; }
        const std::shared_ptr<std::vector<unsigned short>>& get_index() const { return index; }

    protected:
        int handle;

        std::shared_ptr<std::vector<VERTEX3D>> vertex;
        std::shared_ptr<std::vector<unsigned short>> index;
    };
}
