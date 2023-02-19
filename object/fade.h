#pragma once

#include <memory>
#include "model_base.h"

namespace world {
    class fade_camera;
}

namespace mv1 {

    class fade : public model_base {
    public:
        // コンストラクタ
        fade();
        fade(const fade&) = default; // コピー
        fade(fade&&) = default; // ムーブ

        // デストラクタ
        virtual ~fade();

        bool initialize(int screen_width, int screen_height);

        bool set_fade_in(int millisecond);
        bool set_fade_out(int millisecond);

        void process() override;
        bool render() override;

    protected:
        bool initialize_3d();
        bool initialize_2d();
        void fade_process_3d(float fade_rate);
        void fade_process_2d(float fade_rate);
        bool render_2d();
        bool render_3d();
        void set_shader_fade(float alpha, float rgb_threshold);

        std::unique_ptr<world::fade_camera> camera;

        int screen_width;
        int screen_height;

        int texture_handle;
        int alpha_val;

        int vertex_shder_handle;
        int pixel_shader_handle;

        int vertex_shder_constant_buffer_handle;

        int fade_start_count;
        int fade_millisecond;

        bool fade_in;
        bool fade_out;
        bool technical_fade;
        bool fade_2d;
    };
}
