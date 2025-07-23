//!
//! @file main_world.cpp
//!
//! @brief 3D モデルを読み込み、プリミティブの作成と描画、各種の当たり判定を行うサンプル
//!
#include "DxLib.h"
#include "world_logic.h"
#include "world_base.h"
#include "fade.h"

namespace {
    constexpr auto WINDOW_TITLE = _T("Basic 3D");
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
    auto window_mode = FALSE;

#ifdef _DEBUG
    window_mode = TRUE;
#endif

    SetMainWindowText(WINDOW_TITLE);

    ChangeWindowMode(window_mode);

    SetGraphMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH);

    SetBackgroundColor(0, 192, 255);

    if (DxLib_Init() == -1) {
        return -1;
    }

    SetFontSize(32);

    auto world = world_initialize(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (world == nullptr) {
        DxLib_End();
        return -1;
    }

    // Shader を使用したフェード
    std::unique_ptr<mv1::fade> fade(new mv1::fade());

    if (!fade->initialize(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        DxLib_End();
        return -1;
    }

    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);

    // Fog
    SetFogEnable(TRUE);
    SetFogColor(0, 100, 100);
    SetFogStartEnd(2000.0f, 8000.0f);

    SetDrawScreen(DX_SCREEN_BACK);

    VECTOR light_dir = VGet(-1.0f, -1.0f, -1.0f);
    auto light_handle = CreateDirLightHandle(light_dir);

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        if (1 == CheckHitKey(KEY_INPUT_I)) {
            fade->set_fade_in(2000);
        }

        if (1 == CheckHitKey(KEY_INPUT_O)) {
            fade->set_fade_out(2000);
        }

        world->process();
        fade->process();

        ClearDrawScreen();

        world->render();
        fade->render();

        ScreenFlip();
    }

    DeleteLightHandle(light_handle);

    DxLib_End();

    return 0;
}
