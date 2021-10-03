//!
//! @file main_11.cpp
//!
//! @brief 3D モデルを読み込み、最低限の表示を行うサンプル(+カメラ移動+カメラ行列の計算+プロジェクション行列)
//!
#include "DxLib.h"
#include "vector4.h"
#include "matrix44.h"
#include "utility.h"
#include <cmath>

namespace {
    constexpr auto WINDOW_TITLE = "Basic 3D";
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;
    constexpr auto MODEL_FILE = _T("model/Formula1.mv1");

    VECTOR ToDX(math::vector4& vector) {
        return VGet(static_cast<float>(vector.get_x()), static_cast<float>(vector.get_y()), static_cast<float>(vector.get_z()));
    }

    math::vector4 ToMath(VECTOR& vector) {
        return math::vector4(static_cast<double>(vector.x), static_cast<double>(vector.y), static_cast<double>(vector.z));
    }

    MATRIX ToDX(math::matrix44 matrix) {
        MATRIX ret;

        for (auto i = 0; i < math::row_max; ++i) {
            for (auto j = 0; j < math::column_max; ++j) {
                ret.m[i][j] = static_cast<float>(matrix.get_value(i, j));
            }
        }

        return ret;
    }

    math::matrix44 ToMath(MATRIX& matrix) {
        math::matrix44 ret;

        for (auto i = 0; i < math::row_max; ++i) {
            for (auto j = 0; j < math::column_max; ++j) {
                ret.set_value(i, j, static_cast<double>(matrix.m[i][j]));
            }
        }

        return ret;
    }
}

math::matrix44 GetCameraViewMatrix(math::vector4& cameraPosition, math::vector4& cameraTarget, math::vector4& cameraUp);
math::matrix44 GetCameraProjectionMatrix(double cameraNear, double cameraFar, double fov);

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
    math::vector4 camera_position = math::vector4(-400.0, 250.0, -150.0);
    math::vector4 camera_target = math::vector4(0.0, 0.0, 0.0);
    math::vector4 camera_up = math::vector4(0.0, 1.0, 0.0);

    // パースペクティブ処理に必要な情報
    auto camera_near = 1.0;
    auto camera_far = 1000.;
    auto fov = math::utility::degree_to_radian(60.0);

    SetDrawScreen(DX_SCREEN_BACK);

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // カメラを雑に動かしてみる
        if (1 == CheckHitKey(KEY_INPUT_UP)) {
            camera_position.add(0.0, 1.0, 0.0);
        } else if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
            camera_position.add(0.0, -1.0, 0.0);
        } else if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            camera_position.add(-1.0, 0.0, 0.0);
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            camera_position.add(1.0, 0.0, 0.0);
        }

        // カメラ情報をセット
        //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
        math::matrix44 camera_matrix = GetCameraViewMatrix(camera_position, camera_target, camera_up);
        SetCameraViewMatrix(ToDX(camera_matrix));

        //SetCameraNearFar(camera_near, camera_far);
        //SetupCamera_Perspective(fov);
        math::matrix44 projection_matrix = GetCameraProjectionMatrix(camera_near, camera_far, fov);
        SetupCamera_ProjectionMatrix(ToDX(projection_matrix));

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

math::matrix44 GetCameraViewMatrix(math::vector4& cameraPosition, math::vector4& cameraTarget, math::vector4& cameraUp) {
    //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
    //MATRIX camera_matrix = GetCameraViewMatrix();
    // ↑ 上記と同じ MATRIX の内容を計算する
    math::matrix44 camera_matrix = math::matrix44();

    camera_matrix.look_at(cameraPosition, cameraTarget, cameraUp);

    return camera_matrix;
}

math::matrix44 GetCameraProjectionMatrix(double cameraNear, double cameraFar, double fov) {
    //SetCameraNearFar(camera_near, camera_far);
    //SetupCamera_Perspective(fov);
    //MATRIX projection_matrix = GetCameraProjectionMatrix();
    // ↑ 上記と同じ MATRIX の内容を計算する
    auto aspect = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(SCREEN_WIDTH);
    math::matrix44 projection_matrix = math::matrix44();

    projection_matrix.perspective(fov, aspect, cameraNear, cameraFar);

    return projection_matrix;
}
