//!
//! @file main_shader_skinmesh.cpp
//!
//! @brief 3D モデルを読み込み、Shader でのスキニング処理をしたアニメーション表示を行うサンプル
//!        Shader は スキニング処理無しの通常の頂点変換の頂点シェーダー(vertex_shader_skinmesh_dummy
//!        も用意して切り替え可能(スキニングが必要な事を理解する)
//!
#include "DxLib.h"

namespace {
    constexpr auto WINDOW_TITLE = _T("Basic 3D");
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;

    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;

    constexpr auto MODEL_FILE = _T("model/character/SDChar.mv1");
    constexpr auto VERTEX_SHADER_FILE = _T("shader/vertex_shader_skinmesh.vso");
    constexpr auto VERTEX_SHADER_DUMMY_FILE = _T("shader/vertex_shader_skinmesh_dummy.vso");
    constexpr auto PIXEL_SHADER_FILE = _T("shader/pixel_shader_skinmesh.pso");

    auto string_color = GetColor(255, 255, 255);
    auto string_shader_version = _T("Shader Version : [%.1f]");
    auto string_shader_dummy = _T("No Skinning(Change Space Key) : [%d]");

    // カメラ情報
    VECTOR camera_position = VGet(0.0f, 100.0f, -225.0f);
    VECTOR camera_target = VGet(0.0f, 50.0f, 0.0f);
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
    auto vertex_shder_dummy_handle = LoadVertexShader(VERTEX_SHADER_DUMMY_FILE);
    auto pixel_shader_handle = LoadPixelShader(PIXEL_SHADER_FILE);
    // 使用可能なシェーダーバージョン
    auto shader_version = GetValidShaderVersion();

    if (handle == -1 || vertex_shder_handle == -1 ||
        vertex_shder_dummy_handle == -1 || pixel_shader_handle == -1 || shader_version == 0) {
        DxLib_End();
        return -1;
    }

    // シェーダーを有効にしたモデル描画
    MV1SetUseOrigShader(TRUE);
    // シェーダーを適応
    SetUseVertexShader(vertex_shder_handle);
    SetUsePixelShader(pixel_shader_handle);

    SetDrawScreen(DX_SCREEN_BACK);

    auto version = shader_version / 100.0f;
    auto attach_index = MV1AttachAnim(handle, 0, -1, FALSE);
    auto anim_time = MV1GetAttachAnimTotalTime(handle, attach_index);
    auto time = 0.0f;
    auto last_input_space = 0;
    auto dummy = false;

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // スペースキーで 頂点シェーダーを切り替え
        auto input_space = CheckHitKey(KEY_INPUT_SPACE);

        if (1 == input_space && 0 == last_input_space) {
            dummy = !dummy;
            SetUseVertexShader(dummy ? vertex_shder_dummy_handle : vertex_shder_handle);
        }

        SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
        MV1SetAttachAnimTime(handle, attach_index, time);

        time += 1.0f;

        if (time >= anim_time) {
            time = 0.0f;
        }

        ClearDrawScreen();

        MV1DrawModel(handle);

        DrawFormatString(0, 0, string_color, string_shader_version, version);
        DrawFormatString(0, 20, string_color, string_shader_dummy, dummy);

        ScreenFlip();

        last_input_space = input_space;
    }

    DeleteShader(pixel_shader_handle);
    DeleteShader(vertex_shder_dummy_handle);
    DeleteShader(vertex_shder_handle);
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}
