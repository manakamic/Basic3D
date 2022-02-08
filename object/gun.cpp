#include "DxLib.h"
#include "gun.h"

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

namespace mv1 {

    gun::gun() : model_base() {
        offset_matrix = MGetIdent();
        target_frame = -1;
    }

    bool gun::initialize() {
        if (handle == -1) {
            return false;
        }

        // DxLibModelViewer_64bit.exe で調べた描画に必要ない Frame を消す
        MV1SetFrameVisible(handle, 0, FALSE);
        MV1SetFrameVisible(handle, 1, FALSE);
        MV1SetFrameVisible(handle, 2, FALSE);
        MV1SetFrameVisible(handle, 5, FALSE);
        MV1SetFrameVisible(handle, 8, FALSE);
        MV1SetFrameVisible(handle, 9, FALSE);
        MV1SetFrameVisible(handle, 10, FALSE);
        MV1SetFrameVisible(handle, 11, FALSE);
        MV1SetFrameVisible(handle, 12, FALSE);
        MV1SetFrameVisible(handle, 13, FALSE);

        return true;
    }

    bool gun::setup_offset_matrix(std::shared_ptr<mv1::model_base>& target) {
        // DxLibModelViewer_64bit.exe で調べた キャラクターモデルの 銃を持たせたい Frame の番号
        // Viewer で Frame 番号を見ているはずなので直接数値を扱っても良い
        target_frame = MV1SearchFrame(target->get_handle(), _T("SDChar_RightHand"));

        if (target_frame < 0) {
            return false;
        }

        // キャラクター モデルとの大きさをスケールで合わせる
        MATRIX gun_scale = MGetScale(VGet(15.0f, 15.0f, 15.0f));

        // 攻撃アニメーションで合わせたいなら Z 軸回転をしておくと良い
        //MATRIX gun_rot_z = MGetRotZ(-20.0f * DEGREE_TO_RADIAN);
        MATRIX gun_rot_y = MGetRotY(180.0f * DEGREE_TO_RADIAN);
        MATRIX gun_rot_x = MGetRotX(-90.0f * DEGREE_TO_RADIAN);
        MATRIX gun_offset = MGetTranslate(VGet(-16.0f, -2.0f, 0.0f));

        // DxLibModelViewer_64bit.exe で調べた Frame(index 28) の
        // XYZ (赤緑青)軸 に合うように予めモデルをアフィン変換しておく
        //offset_matrix = MMult(MMult(MMult(MMult(gun_scale, gun_rot_z), gun_rot_y), gun_rot_x), gun_offset);
        offset_matrix = MMult(MMult(MMult(gun_scale, gun_rot_y), gun_rot_x), gun_offset);

        return true;
    }
}
