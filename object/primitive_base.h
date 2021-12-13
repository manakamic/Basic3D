#pragma once
#include <tchar.h>
#include <memory>
#include <vector>

#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#else
struct tagVECTOR;
struct tagMATRIX;
struct tagVERTEX3D;
#endif

namespace primitive {
    class primitive_base {
    public:
        // コンストラクタ
        primitive_base();
        primitive_base(const primitive_base&) = default; // コピー
        primitive_base(primitive_base&&) = default; // ムーブ

         // デストラクタ
        virtual ~primitive_base();

        virtual bool load(const TCHAR* fileName);
        virtual bool unload();

        virtual bool create() = 0;

        virtual void process();
        virtual void process_posture();
        virtual bool render();

        const std::shared_ptr<std::vector<VERTEX3D>>& get_vertex() const { return vertex; }
        const std::shared_ptr<std::vector<unsigned short>>& get_index() const { return index; }

#if defined(_AMG_MATH)
        virtual void set_position(const math::vector4& position) { this->position = position; };
        virtual void set_rotation(const math::vector4& rotation) { this->rotation = rotation; };
        virtual void set_scale(const math::vector4& scale) { this->scale = scale; };
        virtual void set_posture(const math::matrix44& posture) { this->posture = posture; };

        virtual math::vector4 get_position() const { return position; };
        virtual math::vector4 get_rotation() const { return rotation; };
        virtual math::vector4 get_scale() const { return scale; };
        virtual math::matrix44 get_posture() const { return posture; };
#else
        virtual void set_position(const VECTOR position) { this->position = position; };
        virtual void set_rotation(const VECTOR rotation) { this->rotation = rotation; };
        virtual void set_scale(const VECTOR scale) { this->scale = scale; };
        virtual void set_posture(const MATRIX posture) { this->posture = posture; };

        virtual VECTOR get_position() const { return position; };
        virtual VECTOR get_rotation() const { return rotation; };
        virtual VECTOR get_scale() const { return scale; };
        virtual MATRIX get_posture() const { return posture; };
#endif

    protected:
        int handle;

        std::shared_ptr<std::vector<VERTEX3D>> vertex;
        std::shared_ptr<std::vector<unsigned short>> index;

#if defined(_AMG_MATH)
        math::vector4 position;
        math::vector4 rotation;
        math::vector4 scale;
        math::matrix44 posture;
#else
        VECTOR position;
        VECTOR rotation;
        VECTOR scale;
        MATRIX posture;
#endif
    };
}
