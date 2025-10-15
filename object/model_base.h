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
        // コンストラクタ
        model_base();
        model_base(const model_base&) = default; // コピー
        model_base(model_base&&) = default; // ムーブ

         // デストラクタ
        virtual ~model_base();

        // C++17 戻り値を無視した場合に警告を出す
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
