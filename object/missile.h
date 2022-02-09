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
        virtual ~missile() = default;

        void process() override;
        bool render() override;

        void separate_render();

        bool initialize(const std::shared_ptr<mv1::player>& player, std::shared_ptr<primitive::sphere>& explosion);
        void set_fire(const VECTOR position);

        bool is_stand_by() const { return (state == state::none); }
        bool is_explode() const { return (state == state::explode); }

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

        draw_text count_down;
        draw_text player_warning;

        state state;
    };
}
