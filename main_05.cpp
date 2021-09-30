//!
//! @file main_05.cpp
//!
//! @brief 3D モデルを読み込み、最低限の表示を行うサンプル(+行列でのモデル回転とスケール)
//!
#include "DxLib.h"
#include <cmath>

namespace {
    constexpr auto WINDOW_TITLE = "Basic 3D";
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;
    constexpr auto MODEL_FILE = _T("model/Formula1.mv1");
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    auto window_mode = FALSE;

#ifdef _DEBUG
    window_mode = TRUE;
#endif

    SetMainWindowText(WINDOW_TITLE);

    ChangeWindowMode(window_mode);

    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);

    SetBackgroundColor(16, 64, 88);

    if (DxLib_Init() == -1) {
        return -1;
    }

    // 3D モデルの読み込み
    auto handle = MV1LoadModel(MODEL_FILE);

    if (handle == -1) {
        DxLib_End();
        return -1;
    }

    // 並行光源を 1 つ追加する
    VECTOR light_dir = VGet(1.0f, 0.0f, -0.1f);
    auto light_handle = CreateDirLightHandle(light_dir);

    // カメラ情報
    VECTOR camera_position = VGet(-400.0f, 250.0f, -150.0f);
    VECTOR camera_target = VGet(0.0f, 0.0f, 0.0f);
    VECTOR camera_up = VGet(0.0f, 1.0f, 0.0f);

    SetDrawScreen(DX_SCREEN_BACK);

    auto model_y_angle = 0.0f;

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // モデルを Y軸で回転させてみる
        model_y_angle += 0.5f; // 0.5 度づつ加算

        // 度 -> ラジアンに変換
        auto model_y_radian = model_y_angle * DEGREE_TO_RADIAN;

        // Y軸の回転値をラジアンで指定して行列で取得する
        MATRIX model_y_matrix = MGetRotY(model_y_radian);

        // スケールも追加してみる(モデルの Y 軸角度を流用)
        auto model_scale = std::abs(std::cos(model_y_radian));

        // =====================================================================
        // XYZ の各軸に対してのスケール値をベクトルで指定
        VECTOR model_scale_vector = VGet(model_scale, model_scale, model_scale);

        // スケールの行列を取得
        MATRIX model_scale_matrix = MGetScale(model_scale_vector);

        // スケールの行列と Y軸回転の行列を合成する
        MATRIX model_matritx = MMult(model_scale_matrix, model_y_matrix);
        // =====================================================================

        // モデルに対しての情報を行列でセットする
        MV1SetMatrix(handle, model_matritx);

        // カメラ情報をセット
        SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);

        ClearDrawScreen();

        // 3D モデルの表示
        MV1DrawModel(handle);

        ScreenFlip();
    }

    // 作成した光源の破棄
    DeleteLightHandle(light_handle);

    // 読み込んだ 3D モデルの破棄
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}
