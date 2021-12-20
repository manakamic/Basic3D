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

    primitive_base::primitive_base() : posture_base() {
        handle = -1;

        vertex.reset(new std::vector<VERTEX3D>());
        index.reset(new std::vector<unsigned short>());
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

    bool primitive_base::render() {
        auto vertex_num = static_cast<int>(vertex->size());
        auto index_num = static_cast<int>(index->size());

        // ƒ|ƒŠƒSƒ“‚ª 1 ‚Â‚à–³‚¯‚ê‚Î•`‰æ‚µ‚È‚¢
        if (3 > vertex_num || 3 > index_num) {
            return false;
        }

        auto polygon_num = index_num / 3;
        auto use_handle = (handle == -1) ? DX_NONE_GRAPH : handle;

        SetUseZBuffer3D(TRUE);
        SetWriteZBuffer3D(TRUE);

#if defined(_AMG_MATH)
        auto posture_dx = ToDX(posture);
        SetTransformToWorld(&posture_dx);
#else
        SetTransformToWorld(&posture);
#endif
        DrawPolygonIndexed3D(vertex->data(), vertex_num, index->data(), polygon_num, use_handle, FALSE);
        SetTransformToWorld(&identity);

        return true;
    }
}
