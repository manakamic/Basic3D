//!
//! @file main_10.cpp
//!
//! @brief 3D モデルを読み込み、最低限の表示を行うサンプル(+カメラ移動+カメラ行列の計算+プロジェクション行列)
//!
#include "DxLib.h"
#include <cmath>

namespace {
    constexpr auto WINDOW_TITLE = _T("Basic 3D");
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;
    constexpr auto MODEL_FILE = _T("model/Formula1.mv1");
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

MATRIX GetCameraViewMatrix(VECTOR& cameraPosition, VECTOR& cameraTarget, VECTOR& cameraUp);
MATRIX GetCameraProjectionMatrix(float cameraNear, float cameraFar, float fov);

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

    // パースペクティブ処理に必要な情報
    auto camera_near = 1.0f;
    auto camera_far = 1000.f;
    auto fov = 60.0f * DEGREE_TO_RADIAN;

    SetDrawScreen(DX_SCREEN_BACK);

    auto string_color = GetColor(255, 255, 255);
    auto string_camera_position = _T("Camera Position : X[%.1f], Y[%.1f], Z[%.1f]");

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // カメラを雑に動かしてみる
        if (1 == CheckHitKey(KEY_INPUT_UP)) {
            camera_position.y += 1.0f;
        } else if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
            camera_position.y -= 1.0f;
        } else if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            camera_position.x -= 1.0f;
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            camera_position.x += 1.0f;
        }

        // カメラ情報をセット
        //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
        MATRIX camera_matrix = GetCameraViewMatrix(camera_position, camera_target, camera_up);
        SetCameraViewMatrix(camera_matrix);

        //SetCameraNearFar(camera_near, camera_far);
        //SetupCamera_Perspective(fov);
        MATRIX projection_matrix = GetCameraProjectionMatrix(camera_near, camera_far, fov);
        SetupCamera_ProjectionMatrix(projection_matrix);

        ClearDrawScreen();

        // 3D モデルの表示
        MV1DrawModel(handle);

        DrawFormatString(0, 0, string_color, string_camera_position, camera_position.x, camera_position.y, camera_position.z);

        ScreenFlip();
    }

    // 作成した光源の破棄
    DeleteLightHandle(light_handle);

    // 読み込んだ 3D モデルの破棄
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}

MATRIX GetCameraViewMatrix(VECTOR& cameraPosition, VECTOR& cameraTarget, VECTOR& cameraUp) {
    //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
    //MATRIX camera_matrix = GetCameraViewMatrix();
    // ↑ 上記と同じ MATRIX の内容を計算する

    // カメラの姿勢での XYZ を作成
    VECTOR camera_z = VNorm(VSub(cameraTarget, cameraPosition));
    VECTOR camera_x = VNorm(VCross(cameraUp, camera_z));
    VECTOR camera_y = VCross(camera_z, camera_x);

    MATRIX camera_matrix = MGetIdent();

    camera_matrix.m[0][0] = camera_x.x;
    camera_matrix.m[0][1] = camera_y.x;
    camera_matrix.m[0][2] = camera_z.x;

    camera_matrix.m[1][0] = camera_x.y;
    camera_matrix.m[1][1] = camera_y.y;
    camera_matrix.m[1][2] = camera_z.y;

    camera_matrix.m[2][0] = camera_x.z;
    camera_matrix.m[2][1] = camera_y.z;
    camera_matrix.m[2][2] = camera_z.z;
    // DX ライブラリには VECTOR にマイナス演算子をつける機能はないので VScale でマイナスにする
    camera_matrix.m[3][0] = VDot(VScale(camera_x, -1.0f), cameraPosition);
    camera_matrix.m[3][1] = VDot(VScale(camera_y, -1.0f), cameraPosition);
    camera_matrix.m[3][2] = VDot(VScale(camera_z, -1.0f), cameraPosition);

    return camera_matrix;
}

MATRIX GetCameraProjectionMatrix(float cameraNear, float cameraFar, float fov) {
    //SetCameraNearFar(camera_near, camera_far);
    //SetupCamera_Perspective(fov);
    //MATRIX projection_matrix = GetCameraProjectionMatrix();
    // ↑ 上記と同じ MATRIX の内容を計算する
    auto aspect = static_cast<float>(SCREEN_WIDTH)/ static_cast<float>(SCREEN_HEIGHT);
    auto range = cameraFar - cameraNear;
    auto temp = cameraFar / range;
    auto cot = 1.0f / std::tan(fov * 0.5f);

    // 0 行列の取得がないので 0 スケールで対応する
    MATRIX projection_matrix = MScale(MGetIdent(), 0.0f);

    projection_matrix.m[0][0] = cot / aspect;
    projection_matrix.m[1][1] = cot;
    projection_matrix.m[2][2] = temp;
    projection_matrix.m[2][3] = 1.0f;
    projection_matrix.m[3][2] = -cameraNear * temp;

    return projection_matrix;
}
