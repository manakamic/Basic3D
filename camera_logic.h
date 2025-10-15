#pragma once

#include <memory>
#include <optional>

namespace world {
    class camera_base;
}

namespace mv1 {
    class player;
}

// C++17 std::optional
std::optional<std::shared_ptr<world::camera_base>> camera_initialize(const int screen_width, const int screen_height, const std::shared_ptr<mv1::player>& player);
