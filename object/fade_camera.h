#pragma once

#include "camera_base.h"

namespace world {

    class fade_camera : public camera_base {
    public:
        fade_camera(int screen_width, int screen_height);
        fade_camera(const fade_camera&) = default;
        fade_camera(fade_camera&&) = default;

        virtual ~fade_camera() = default;

        void process() override;

        void set_ortho(bool ortho) { this->ortho = ortho; }
        bool is_ortho() { return ortho; }

    protected:
        bool ortho;
    };
}
