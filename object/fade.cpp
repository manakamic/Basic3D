#include <cmath>
#include "DxLib.h"
#include "fade_camera.h"
#include "fade.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#endif

namespace {
    constexpr auto MODEL_FILE = _T("model/fade/transition.mv1");
    constexpr auto TEXTURE_FILE = _T("model/fade/transition.png");
    constexpr auto VERTEX_SHADER_FILE = _T("shader/vertex_shader_transition.vso");
    constexpr auto PIXEL_SHADER_FILE = _T("shader/pixel_shader_transition.pso");
    constexpr auto HALF_PI = DX_PI_F / 2.0f;
}

namespace mv1 {

    fade::fade() : model_base() {
        camera.reset();
        screen_width = 0;
        screen_height = 0;
        texture_handle = -1;
        alpha_val = 0;
        vertex_shder_handle = -1;
        pixel_shader_handle = -1;
        vertex_shder_constant_buffer_handle = 0;
        fade_start_count = 0;
        fade_millisecond = 0;
        fade_in = false;
        fade_out = false;
        technical_fade = false;
        fade_2d = false;
    }

    fade::~fade() {
        if (-1 != pixel_shader_handle) {
            DeleteShader(pixel_shader_handle);
        }

        if (-1 != vertex_shder_handle) {
            DeleteShader(vertex_shder_handle);
        }

        if (0 != vertex_shder_constant_buffer_handle) {
            DeleteShaderConstantBuffer(vertex_shder_constant_buffer_handle);
        }

        if (-1 != texture_handle) {
            DeleteGraph(texture_handle);
        }
    }

    bool fade::initialize(int screen_width, int screen_height) {
        auto ret = false;

        this->screen_width = screen_width;
        this->screen_height = screen_height;

        if (fade_2d) {
            ret = initialize_2d();
        }
        else {
            ret = initialize_3d();
        }

        return ret;
    }

    bool fade::initialize_2d() {
        texture_handle = LoadGraph(TEXTURE_FILE);

        return (-1 != texture_handle);
    }

    bool fade::initialize_3d() {
        if (!model_base::load(MODEL_FILE)) {
            return false;
        }

        // transition.mv1 は 100 x 100 の頂点モデルなので、画面の比率 1280 x 720 に合わせてスケーリングする
#if defined(_AMG_MATH)
        set_scale(math::vector4(1.78, 1.0, 1.0));
#else
        set_scale(VGet(1.78f, 1.0f, 1.0f));
#endif

        vertex_shder_handle = LoadVertexShader(VERTEX_SHADER_FILE);
        pixel_shader_handle = LoadPixelShader(PIXEL_SHADER_FILE);

        vertex_shder_constant_buffer_handle = CreateShaderConstantBuffer(sizeof(float) * 4);

        camera.reset(new world::fade_camera(screen_width, screen_height));
        camera->set_ortho(true);

        return (-1 != vertex_shder_handle && -1 != pixel_shader_handle);
    }

    void fade::process() {
        if (!fade_in && !fade_out) {
            return;
        }

        auto time_count = GetNowCount();
        auto fade_count = time_count - fade_start_count;
        auto fade_rate = static_cast<float>(fade_count) / static_cast<float>(fade_millisecond);

        if (fade_rate >= 1.0f) {
            fade_rate = 1.0f;

            if (fade_out) {
                fade_in = false;
                fade_out = false;
                fade_millisecond = 0;
            }
        }

        if (fade_2d) {
            fade_process_2d(fade_rate);
        }
        else {
            model_base::process();
            fade_process_3d(fade_rate);
        }
    }

    bool fade::set_fade_in(int millisecond) {
        if (fade_in || millisecond <= 0) {
            return false;
        }

        fade_in = true;
        fade_out = false;

        fade_millisecond = millisecond;
        fade_start_count = GetNowCount();

        return true;
    }

    bool fade::set_fade_out(int millisecond) {
        if (!fade_in || fade_out || millisecond <= 0) {
            return false;
        }

        fade_in = false;
        fade_out = true;

        fade_millisecond = millisecond;
        fade_start_count = GetNowCount();

        return true;
    }

    void fade::fade_process_2d(float fade_rate) {
        auto theta = HALF_PI * fade_rate;

        if(fade_in && !fade_out) {
            alpha_val = static_cast<int>(255.0f * std::sin(theta));
        }
        else if (fade_out) {
            alpha_val = static_cast<int>(255.0f * std::cos(theta));
        }
    }

    void fade::fade_process_3d(float fade_rate) {
        if (fade_in && !fade_out) {
            if (technical_fade) {
                set_shader_fade(1.0f, 1.0f - fade_rate);
            }
            else {
                set_shader_fade(fade_rate, 0.0f);
            }
        }
        else if (fade_out) {
            if (technical_fade) {
                set_shader_fade(1.0f, fade_rate);
            }
            else {
                set_shader_fade(1.0f - fade_rate, 0.0f);
            }
        }
    }

    void fade::set_shader_fade(float alpha, float rgb_threshold) {
        auto fade_color = reinterpret_cast<COLOR_F*>(GetBufferShaderConstantBuffer(vertex_shder_constant_buffer_handle));

        fade_color->r = rgb_threshold;
        fade_color->g = 1.0f;
        fade_color->b = 1.0f;
        fade_color->a = alpha;

        UpdateShaderConstantBuffer(vertex_shder_constant_buffer_handle);

        SetShaderConstantBuffer(vertex_shder_constant_buffer_handle, DX_SHADERTYPE_VERTEX, 4);
    }

    bool fade::render() {
        if (!fade_in && !fade_out) {
            return false;
        }

        auto ret = false;

        if (fade_2d) {
            ret = render_2d();
        }
        else {
            camera->process();
            ret = render_3d();
        }

        return ret;
    }

    bool fade::render_2d() {
        if (-1 == texture_handle) {
            return false;
        }

        SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha_val);

        auto ret = DrawExtendGraph(0, 0, screen_width, screen_height, texture_handle, FALSE);

        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

        return (-1 != ret);
    }

    bool fade::render_3d() {
        if (-1 == handle || invisible) {
            return false;
        }

        MV1SetUseOrigShader(TRUE);

        SetUseVertexShader(vertex_shder_handle);
        SetUsePixelShader(pixel_shader_handle);

        if (!camera->is_ortho()) {
            MV1SetUseZBuffer(handle, FALSE);
            MV1SetWriteZBuffer(handle, FALSE);
        }

        auto ret = MV1DrawModel(handle);

        MV1SetUseOrigShader(FALSE);

        return (-1 != ret);
    }
}
