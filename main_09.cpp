//!
//! @file main_09.cpp
//!
//! @brief 3D ���f����ǂݍ��݁A�Œ���̕\�����s���T���v��(+�J�����ړ�+�J�����s��̌v�Z)
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

MATRIX GetCameraViewMatrix(VECTOR cameraPosition, VECTOR cameraTarget, VECTOR cameraUp);

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

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // �J�������G�ɓ������Ă݂�
        if (1 == CheckHitKey(KEY_INPUT_UP)) {
            camera_position.y += 1.0f;
        } else if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
            camera_position.y -= 1.0f;
        } else if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            camera_position.x -= 1.0f;
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            camera_position.x += 1.0f;
        }

        // �J���������Z�b�g
        //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
        MATRIX camera_matrix = GetCameraViewMatrix(camera_position, camera_target, camera_up);
        SetCameraViewMatrix(camera_matrix);

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

MATRIX GetCameraViewMatrix(VECTOR cameraPosition, VECTOR cameraTarget, VECTOR cameraUp) {
    //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
    //MATRIX camera_matrix = GetCameraViewMatrix();
    // �� ��L�Ɠ��� MATRIX �̓��e���v�Z����

    // �J�����̎p���ł� XYZ ���쐬
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
    // DX ���C�u�����ɂ� VECTOR �Ƀ}�C�i�X���Z�q������@�\�͂Ȃ��̂� VScale �Ń}�C�i�X�ɂ���
    camera_matrix.m[3][0] = VDot(VScale(camera_x, -1.0f), cameraPosition);
    camera_matrix.m[3][1] = VDot(VScale(camera_y, -1.0f), cameraPosition);
    camera_matrix.m[3][2] = VDot(VScale(camera_z, -1.0f), cameraPosition);

    return camera_matrix;
}
