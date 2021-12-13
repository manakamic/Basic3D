#include "DxLib.h"
#include "primitive_base.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#include "dx_utility.h"
#endif

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
    MATRIX identity = MGetIdent();
}

namespace primitive {

    primitive_base::primitive_base() {
        handle = -1;

        vertex.reset(new std::vector<VERTEX3D>());
        index.reset(new std::vector<unsigned short>());

#if defined(_AMG_MATH)
        scale = math::vector4(1.0, 1.0, 1.0);
#else
        position = VGet(0.0f, 0.0f, 0.0f);
        rotation = VGet(0.0f, 0.0f, 0.0f);
        scale = VGet(1.0f, 1.0f, 1.0f);
        posture = MGetIdent();
#endif
    }

    primitive_base::~primitive_base() {
        vertex->clear();
        index->clear();
    }

    bool primitive_base::load(const TCHAR* fileName) {
        unload();

        handle = LoadGraph(fileName);

        return (-1 != handle);
    }

    bool primitive_base::unload() {
        if (-1 == handle) {
            return false;
        }

        return (-1 != DeleteGraph(handle));
    }

    void primitive_base::process() {
        process_posture();
    }

    void primitive_base::process_posture() {
#if defined(_AMG_MATH)
        posture.scale(scale.get_x(), scale.get_y(), scale.get_z(), true);
        posture.rotate_z(rotation.get_z(), false);
        posture.rotate_x(rotation.get_x(), false);
        posture.rotate_y(rotation.get_y(), false);
        posture.transfer(position.get_x(), position.get_y(), position.get_z(), false);
#else
        auto radian_x = rotation.x * DEGREE_TO_RADIAN;
        auto radian_y = rotation.y * DEGREE_TO_RADIAN;
        auto radian_z = rotation.z * DEGREE_TO_RADIAN;
        auto rotate = MMult(MMult(MGetRotZ(radian_z), MGetRotX(radian_x)), MGetRotY(radian_y));

        posture = MMult(MMult(MGetScale(scale), rotate), MGetTranslate(position));
#endif
    }

    bool primitive_base::render() {
        auto vertex_num = static_cast<int>(vertex->size());
        auto index_num = static_cast<int>(index->size());

        // ƒ|ƒŠƒSƒ“‚ª 1 ‚Â‚à–³‚¯‚ê‚Î•`‰æ‚µ‚È‚¢
        if (3 > vertex_num || 3 > index_num) {
            return false;
        }

        auto polygon_num = index_num / 3;
        auto use_handle = (handle == -1) ? DX_NONE_GRAPH : handle;

        SetTransformToWorld(&posture);
        DrawPolygonIndexed3D(vertex->data(), vertex_num, index->data(), polygon_num, use_handle, FALSE);
        SetTransformToWorld(&identity);

        return true;
    }
}
