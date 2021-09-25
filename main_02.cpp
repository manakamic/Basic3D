#include "DxLib.h"

namespace {
    constexpr auto WINDOW_TITLE = "Basic 3D";
    constexpr auto SCREEN_WIDTH = 1280;
    constexpr auto SCREEN_HEIGHT = 720;
    constexpr auto SCREEN_DEPTH = 32;
    constexpr auto MODEL_FILE = _T("model/Formula1.mv1");
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

    // 3D ���f���̓ǂݍ���
    auto handle = MV1LoadModel(MODEL_FILE);

    if (handle == -1) {
        DxLib_End();
        return -1;
    }

    // ���s������ 1 �ǉ�����
    VECTOR light_dir = VGet(1.0f, 0.0f, -0.1f);
    auto light_handle = CreateDirLightHandle(light_dir);

    // �J�������
    VECTOR camera_position = VGet(-400.0f, 250.0f, -150.0f);
    VECTOR camera_target = VGet(0.0f, 0.0f, 0.0f);
    VECTOR camera_up = VGet(0.0f, 1.0f, 0.0f);

    SetDrawScreen(DX_SCREEN_BACK);

    auto model_y_angle = 0.0f;

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // ========================================================
        // ���f���� Y���ŉ�]�����Ă݂�
        model_y_angle += 0.5f; // 0.5 �x�Â��Z

        // �x -> ���W�A���ɕϊ�
        auto model_y_radian = model_y_angle * DX_PI_F / 180.0f;

        // XYZ ���ꂼ��̎��̉�]�l�����W�A���Ŏw��
        VECTOR model_xyz_rotate = VGet(0.0f, model_y_radian, 0.0f);

        // ���f���ɑ΂��Ẳ�]�� XYZ �̊e���ŃZ�b�g����
        MV1SetRotationXYZ(handle, model_xyz_rotate);
        // ========================================================

        // �J���������Z�b�g
        SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);

        ClearDrawScreen();

        // 3D ���f���̕\��
        MV1DrawModel(handle);

        ScreenFlip();
    }

    // �쐬���������̔j��
    DeleteLightHandle(light_handle);

    // �ǂݍ��� 3D ���f���̔j��
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}
