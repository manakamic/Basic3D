//!
//! @file missile.h
//!
//! @brief ミサイルのモデルを扱うクラス
//!        詳細は missile.cpp 側
//!
#pragma once
#include <memory>
#include <tuple>
#include "model_base.h"

struct tagVECTOR;

namespace world {
    class world_base;
}

namespace primitive {
    class sphere;
}

namespace mv1 {
    class player;

    class missile : public model_base {
    public:
        // コンストラクタ
        missile(const int screen_width, const int screen_height);
        missile(const missile&) = default; // コピー
        missile(missile&&) = default; // ムーブ

        // デストラクタ
        virtual ~missile();

        void process() override;
        bool render() override;

        bool initialize(const std::shared_ptr<world::world_base>& world,
                        const std::shared_ptr<mv1::player>& player,
                        const std::shared_ptr<primitive::sphere>& explosion);

        void set_fire(const VECTOR position);

        void set_camera_index(const int camera_index) { this->camera_index = camera_index; }

        void set_use_render_texture(const int use_render_texture) { this->use_render_texture = use_render_texture; }

    private:
        enum class state {
            none, wait, launch, homing, explode
        };

        // このクラスだけで使用する文字列描画用の構造体
        // 同じ処理で 違う描画を複数行うのでまとめる
        struct draw_text {
            draw_text() {
                offset_x = 0; x = 0; y = 0; is_valid = false; is_draw = false;
            }
            void reset() {
                is_valid = false; is_draw = false;
            }
            int offset_x;
            int x;
            int y;
            bool is_valid;
            bool is_draw;
        };

        bool initialize_click_screen();
        bool initialize_camera();

        void start_count();
        int get_count() const;

        std::tuple<bool, int, int> check_in_screen(const VECTOR& position) const;

        void set_explosion();

        void process_explosion();
        void process_count_down();
        void process_player_warning();
        void process_fire();
        void process_moving_launch();
        void process_moving_homing();
        VECTOR process_moving();

        VECTOR get_posture_x() const;
        VECTOR get_posture_y() const;
        VECTOR get_posture_z() const;

        void set_posture(const VECTOR& x, const VECTOR& y, const VECTOR& z);

        void world_process_and_render() const;

        void render_screen();
        void render_texture();
        void render_separate();

        bool is_stand_by() const { return (state == state::none); }
        bool is_explode() const { return (state == state::explode); }

        std::shared_ptr<world::world_base> world;
        std::shared_ptr<mv1::player> player;
        std::shared_ptr<primitive::sphere> explosion;

        MATRIX posture;
        VECTOR fire_point;

        float velocity;
        float homing_rate;

        float count_down_time;
        int count_start_time;

        int screen_width;
        int screen_height;

        int camera_index;
        int render_texture_handle;

        draw_text count_down;
        draw_text player_warning;

        state state;

        bool use_render_texture;
    };
}
