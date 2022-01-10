#include "DxLib.h"
#include "model_base.h"
#if defined(_AMG_MATH)
#include "vector4.h"
#include "matrix44.h"
#include "dx_utility.h"
#endif

namespace mv1 {

    model_base::model_base() : posture_base(){
        handle = -1;
    }

    model_base::~model_base() {
        unload();
    }

    bool model_base::load(const TCHAR* fileName) {
        unload();

        handle = MV1LoadModel(fileName);

        return (-1 != handle);
    }

    bool model_base::unload() {
        if (-1 == handle) {
            return false;
        }

        return (-1 != MV1DeleteModel(handle));
    }

    void model_base::process() {
        process_posture();
    }

    void model_base::process_posture() {
        posture_base::process_posture();

#if defined(_AMG_MATH)
        MV1SetMatrix(handle, ToDX(posture_matrix));
#else
        MV1SetMatrix(handle, posture_matrix);
#endif
    }

    bool model_base::render() {
        if (handle == -1) {
            return false;
        }

        // Z バッファを有効化
        MV1SetUseZBuffer(handle, TRUE);
        MV1SetWriteZBuffer(handle, TRUE);

        return (-1 != MV1DrawModel(handle));
    }
}
