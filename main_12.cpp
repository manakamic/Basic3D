//!
//! @file main_12.cpp
//!
//! @brief 3D モデルを読み込み、最低限の表示を行うサンプル(+行列でのモデル回転とスケールと移動+カメラ移動)
//!        main_08.cpp の vector4 / matrix44 class 版
//!
#include "DxLib.h"
#include "vector4.h"
#include "matrix44.h"
#include "utility.h"
#include "dx_utility.h"
#include <cmath>
#include <utility>

namespace {
    constexpr auto WINDOW_TITLE = _T("Basic 3D");
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;
    constexpr auto MODEL_FILE = _T("model/Formula1.mv1");
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

    auto string_color = GetColor(255, 255, 255);
    auto string_camera_position = _T("Camera Position : X[%.1lf], Y[%.1lf], Z[%.1lf]");
    auto string_model_position = _T("Model Position : X[%.1lf], Y[%.1lf], Z[%.1lf]");
    auto model_y_angle = 0.0;
    auto model_y_matrix = math::matrix44();
    auto model_scale_matrix = math::matrix44();
    auto model_position_matrix = math::matrix44();

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // モデルを Y軸で回転させてみる
        model_y_angle += 0.5; // 0.5 度づつ加算

        // Y軸の回転値を度(ディグリー)で指定して行列で取得する
        model_y_matrix.rotate_y(model_y_angle, true);

        // 度 -> ラジアンに変換
        auto model_y_radian = math::utility::degree_to_radian(model_y_angle);

        // スケールも追加してみる(モデルの Y 軸角度を流用)
        auto model_scale = std::abs(std::cos(model_y_radian));

        // スケールの行列を取得
        model_scale_matrix.scale(model_scale, model_scale, model_scale, true);

        // Z方向の移動も追加してみる(モデルの Y軸角度を流用)
        auto model_z_position = 300.0 * std::sin(model_y_radian);

        // スケールの行列と Y軸回転の行列を合成する
        model_position_matrix.transfer(0.0, 0.0, model_z_position, true);

        auto model_matritx = model_y_matrix * model_scale_matrix * model_position_matrix;

        // モデルに対しての情報を行列でセットする
        MV1SetMatrix(handle, ToDX(model_matritx));

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
        //SetCameraPositionAndTargetAndUpVec(ToDX(camera_position), ToDX(camera_target), ToDX(camera_up));
        auto camera_matrix = GetCameraViewMatrix(camera_position, camera_target, camera_up);
        SetCameraViewMatrix(ToDX(camera_matrix));

        //SetCameraNearFar(static_cast<float>(camera_near), static_cast<float>(camera_far));
        //SetupCamera_Perspective(static_cast<float>(fov));
        auto projection_matrix = GetCameraProjectionMatrix(camera_near, camera_far, fov);
        SetupCamera_ProjectionMatrix(ToDX(projection_matrix));

        ClearDrawScreen();

        // 3D モデルの表示
        MV1DrawModel(handle);

        DrawFormatString(0, 0, string_color, string_camera_position, camera_position.get_x(), camera_position.get_y(), camera_position.get_z());
        DrawFormatString(0, 20, string_color, string_model_position, model_matritx.get_value(3, 0), model_matritx.get_value(3, 1), model_matritx.get_value(3, 2));

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
    auto aspect = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    math::matrix44 projection_matrix = math::matrix44();

    projection_matrix.perspective(fov, aspect, cameraNear, cameraFar);

    return projection_matrix;
}
