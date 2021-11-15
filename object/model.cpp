#include "DxLib.h"
#include "model.h"

namespace mv1 {

    model::model() {
        anim_num = 0;
        blend_frame = 0;
        blend_count = 0;
        main.reset();
        blend.reset();
    }

    bool model::load(const TCHAR* fileName) {
        if (!model_base::load(fileName)) {
            return false;
        }

        anim_num = MV1GetAnimNum(handle);

        return (-1 != anim_num);
    }

    void model::process() {
        model_base::process();

        set_anim_time(main);
        set_anim_time(blend);
        process_blend();
    }

    // アニメーション ブレンド処理
    void model::process_blend() {
        if (-1 == main .index || -1 == blend.index) {
            return;
        }

        auto base_frame = static_cast<float>(blend_frame);
        auto rate = static_cast<float>(blend_count) / base_frame;

        MV1SetAttachAnimBlendRate(handle, main.attach, 1.0f - rate);
        MV1SetAttachAnimBlendRate(handle, blend.attach, rate);

        blend_count++;

        // 指定のブレンドフレームが過ぎたら
        if (blend_count > blend_frame) {
            detach_anim_info(main); // main の方は破棄

            main = blend;           // blend のアニメーションを main にする
            main.play = base_frame; // ブレンド フレーム分進める

            blend.reset();          // main にコピーしたのでリセット
        }
    }

    bool model::set_attach(int index, bool loop, float speed) {
        if (index >= anim_num) {
            return false;
        }

        return attach_anim_info(main, index, loop, speed);
    }

    bool model::set_blend(int index, int frame, bool loop, float speed) {
        if (index >= anim_num) {
            return false;
        }

        auto ret = attach_anim_info(blend, index, loop, speed);

        if (ret) {
            blend_frame = frame;
            blend_count = 0;
        }

        return ret;
    }

    bool model::attach_anim_info(anim_info& info, int index, bool loop, float speed) {
        detach_anim_info(info);

        info.attach = MV1AttachAnim(handle, index);

        auto ret = (-1 != info.attach);

        if (ret) {
            info.index = index;
            info.total = MV1GetAttachAnimTotalTime(handle, info.attach);
            info.speed = speed;
            info.loop = loop;
        }

        return ret;
    }

    bool model::detach_anim_info(anim_info& info) {
        if (-1 == info.index) {
            return false;
        }

        auto ret = MV1DetachAnim(handle, info.attach);

        info.reset();

        return (0 == ret);
    }

    void model::set_anim_time(anim_info& info) {
        if (-1 == info.index) {
            return;
        }

        MV1SetAttachAnimTime(handle, info.attach, info.play);

        info.play += info.speed;

        if (info.play > info.total) {
            info.play = info.loop ? 0.0f : info.total;
        }
    }
}
