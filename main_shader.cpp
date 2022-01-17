//!
//! @file main_shader.cpp
//!
//! @brief 3D モデルを読み込み、Shader での表示を行うサンプル(モデル回転あり)
//!        モデルのテクスチャは、デフィーズ / ノーマルマップ / スぺキュラマップ
//!        Shader は ノーマルマップシェーダー
//!
#include "DxLib.h"

namespace {
    constexpr auto WINDOW_TITLE = _T("Basic 3D");
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;

    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;

    constexpr auto MODEL_FILE = _T("model_original/Formula1_Original.mv1");
    constexpr auto VERTEX_SHADER_FILE = _T("shader/vertex_shader.vso");
    constexpr auto PIXEL_SHADER_FILE = _T("shader/pixel_shader.pso");

    auto string_color = GetColor(255, 255, 255);
    auto string_shader_version = _T("Shader Version : [%.1f]");
    auto string_model_texture_num = _T("Model Texture Num : [%d]");

    // カメラ情報
    VECTOR camera_position = VGet(-400.0f, 250.0f, -150.0f);
    VECTOR camera_target = VGet(0.0f, 0.0f, 0.0f);
    VECTOR camera_up = VGet(0.0f, 1.0f, 0.0f);
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

    auto handle = MV1LoadModel(MODEL_FILE);
    // ShaderCompiler.exe でコンパイル済みの頂点シェーダーとピクセルシェーダーをロード
    auto vertex_shder_handle = LoadVertexShader(VERTEX_SHADER_FILE);
    auto pixel_shader_handle = LoadPixelShader(PIXEL_SHADER_FILE);
    // 使用可能なシェーダーバージョン
    auto shader_version = GetValidShaderVersion();

    if (handle == -1 || vertex_shder_handle == -1 || pixel_shader_handle == -1 || shader_version == 0) {
        DxLib_End();
        return -1;
    }

    // シェーダーを有効にしたモデル描画
    MV1SetUseOrigShader(TRUE);
    // シェーダーを適応
    SetUseVertexShader(vertex_shder_handle);
    SetUsePixelShader(pixel_shader_handle);

    SetDrawScreen(DX_SCREEN_BACK);

    auto model_y_angle = 0.0f;
    auto version = shader_version / 100.0f;
    auto texture_num = MV1GetTextureNum(handle); // デフィーズ / ノーマルマップ / スぺキュラマップの 3 枚

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        model_y_angle += 0.5f;

        // シェーダーの具合が分かり易い様にモデルを回転させる
        MV1SetMatrix(handle, MGetRotY(model_y_angle * DEGREE_TO_RADIAN));
        SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);

        ClearDrawScreen();

        MV1DrawModel(handle);

        DrawFormatString(0, 0, string_color, string_shader_version, version);
        DrawFormatString(0, 20, string_color, string_model_texture_num, texture_num);

        ScreenFlip();
    }

    DeleteShader(pixel_shader_handle);
    DeleteShader(vertex_shder_handle);
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}
