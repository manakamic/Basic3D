#pragma once

#include <tchar.h>
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#else
struct tagVECTOR;
struct tagMATRIX;
#endif

class posture_base {
public:
    // コンストラクタ
    posture_base();
    posture_base(const posture_base&) = default; // コピー
    posture_base(posture_base&&) = default; // ムーブ

     // デストラクタ
    virtual ~posture_base() = default;

    virtual void process_posture();

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