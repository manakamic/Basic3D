#pragma once
#include <tuple>
#include <array>
#include <tchar.h>
#include <memory>
#include <vector>
#include "posture_base.h"

struct tagVERTEX3D;

namespace math {
    class vector4;
}

namespace primitive {

    using face = std::tuple<std::array<math::vector4, 4>/*vertex*/, math::vector4/*normal*/>;

    class primitive_base : public posture_base {
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
        virtual bool render();

        bool set_handle(const int handle);
        int get_handle() const { return handle; }

        void set_lighting(const int lighting) { this->lighting = lighting; };
        const int get_lighting() const { return lighting; };

        void set_transparent(const int transparent) { this->transparent = transparent; };
        const int get_transparent() const { return transparent; };

        const std::shared_ptr<std::vector<VERTEX3D>>& get_vertex() const { return vertex; }
        const std::shared_ptr<std::vector<unsigned short>>& get_index() const { return index; }

        void set_invisible(const bool invisible) { this->invisible = invisible; };
        const bool get_invisible() const { return invisible; };

        void set_debug(const bool debug) { is_debug = debug; };
        const bool get_debug() const { return is_debug; };

    protected:
        int handle;
        int lighting;
        int transparent;

        std::shared_ptr<std::vector<VERTEX3D>> vertex;
        std::shared_ptr<std::vector<unsigned short>> index;

        bool invisible;
        bool is_debug;
    };
}
