#pragma once

#include <memory>

namespace world {
    class world_base;
}

std::shared_ptr<world::world_base> world_initialize(const int screen_width, const int screen_height);
