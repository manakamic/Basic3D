#pragma once

#include <functional>
#include "model_base.h"

namespace mv1 {

    class model : public model_base {
    public:
        struct anim_info {
            void reset() {
                index = -1; attach = -1; total = 0.0f; play = 0.0f; speed = 1.0f;  loop = false; loop_end = nullptr;
            }

            int index;
            int attach;
            float total;
            float play;
            float speed;
            bool loop;
            std::function<void(void)> loop_end;
        };

        // コンストラクタ
        model();
        model(const model&) = default; // コピー
        model(model&&) = default; // ムーブ

         // デストラクタ
        virtual ~model() = default;

        bool load(const TCHAR* fileName) override;
        void process() override;

        bool set_attach(int index, bool loop = false, std::function<void(void)> loop_end = nullptr, float speed = 1.0f);
        bool set_blend(int index, int frame, bool loop = false, std::function<void(void)> loop_end = nullptr, float speed = 1.0f);

        int get_anim_num() const { return anim_num; }

        bool is_blend() const { return (-1 != blend.index); }

        int get_main_index() const { return main.index; }
        int get_blend_index() const { return blend.index; }

    private:
        void process_blend();
        bool attach_anim_info(anim_info& info, int index, bool loop, std::function<void(void)> loop_end, float speed);
        bool detach_anim_info(anim_info& info);
        void set_anim_time(anim_info& info);

        int anim_num;

        int blend_frame;
        int blend_count;

        anim_info main;
        anim_info blend;
    };
}
