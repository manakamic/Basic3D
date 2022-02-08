#pragma once
#include <vector>
#include <memory>
#include <functional>

namespace mv1 {
    class model_base;
}

namespace primitive {
    class primitive_base;
}

namespace world {
    class camera_base;

    class world_base {
    public:
        // コンストラクタ
        world_base();
        world_base(const world_base&) = default; // コピー
        world_base(world_base&&) = default; // ムーブ

         // デストラクタ
        virtual ~world_base() = default;

        virtual void process();
        virtual bool render();

        void add_model(const std::shared_ptr<mv1::model_base>& model) { model_list.emplace_back(model); }
        void add_primitive(const std::shared_ptr<primitive::primitive_base>& primitive) { primitive_list.emplace_back(primitive); }

        void add_camera(const std::shared_ptr<camera_base>& camera) {
            camera_list.emplace_back(camera);

            if (camera_index < 0) {
                camera_index = 0;
            }
        }

        void set_camera_index(const int index) { camera_index = index; }
        int get_camera_index() const { return camera_index; }

        void set_pre_render(const std::function<void(void)>& render) {  pre_render = render; }
        void set_post_render(const std::function<void(void)>& render) { post_render = render; }

    protected:
        std::vector<std::shared_ptr<mv1::model_base>> model_list;
        std::vector<std::shared_ptr<primitive::primitive_base>> primitive_list;

        std::vector<std::shared_ptr<camera_base>> camera_list;
        int camera_index;

        std::function<void(void)> pre_render;
        std::function<void(void)> post_render;
    };

}
