#pragma once
#include <functional>
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

    void set_update(const std::function<void(posture_base*)>& update) { this->update = update; }

#if defined(_AMG_MATH)
    virtual void set_position(const math::vector4& position) { this->position = position; };
    virtual void set_rotation(const math::vector4& rotation) { this->rotation = rotation; };
    virtual void set_scale(const math::vector4& scale) { this->scale = scale; };

    virtual math::vector4 get_position() const { return position; };
    virtual math::vector4 get_rotation() const { return rotation; };
    virtual math::vector4 get_scale() const { return scale; };
    virtual math::matrix44 get_scale_matrix() const { return scale_matrix; };
    virtual math::matrix44 get_rotate_matrix() const { return rotate_matrix; };
    virtual math::matrix44 get_transfer_matrix() const { return transfer_matrix; };
    virtual math::matrix44 get_posture_matrix() const { return posture_matrix; };
#else
    virtual void set_position(const VECTOR position) { this->position = position; };
    virtual void set_rotation(const VECTOR rotation) { this->rotation = rotation; };
    virtual void set_scale(const VECTOR scale) { this->scale = scale; };

    virtual VECTOR get_position() const { return position; };
    virtual VECTOR get_rotation() const { return rotation; };
    virtual VECTOR get_scale() const { return scale; };
    virtual MATRIX get_scale_matrix() const { return scale_matrix; };
    virtual MATRIX get_rotate_matrix() const { return rotate_matrix; };
    virtual MATRIX get_transfer_matrix() const { return transfer_matrix; };
    virtual MATRIX get_posture_matrix() const { return posture_matrix; };
#endif

protected:
    std::function<void(posture_base*)> update;

#if defined(_AMG_MATH)
    math::vector4 position;
    math::vector4 rotation;
    math::vector4 scale;
    math::matrix44 scale_matrix;
    math::matrix44 rotate_matrix;
    math::matrix44 transfer_matrix;
    math::matrix44 posture_matrix;
#else
    VECTOR position;
    VECTOR rotation;
    VECTOR scale;
    MATRIX scale_matrix;
    MATRIX rotate_matrix;
    MATRIX transfer_matrix;
    MATRIX posture_matrix;
#endif
};