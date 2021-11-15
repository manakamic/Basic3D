//!
//! @file main_11.cpp
//!
//! @brief 3D ���f����ǂݍ��݁A�Œ���̕\�����s���T���v��(+�J�����ړ�+�J�����s��̌v�Z+�v���W�F�N�V�����s��)
//!        main_10.cpp �� vector4 / matrix44 class ��
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
    auto camera_position = math::vector4(-400.0, 250.0, -150.0);
    auto camera_target = math::vector4(0.0, 0.0, 0.0);
    auto camera_up = math::vector4(0.0, 1.0, 0.0);

    // �p�[�X�y�N�e�B�u�����ɕK�v�ȏ��
    auto camera_near = 1.0;
    auto camera_far = 1000.;
    auto fov = math::utility::degree_to_radian(60.0);

    SetDrawScreen(DX_SCREEN_BACK);

    auto string_color = GetColor(255, 255, 255);
    auto string_camera_position = _T("Camera Position : X[%.1lf], Y[%.1lf], Z[%.1lf]");

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // �J�������G�ɓ������Ă݂�
        if (1 == CheckHitKey(KEY_INPUT_UP)) {
            camera_position.add(0.0, 1.0, 0.0);
        } else if (1 == CheckHitKey(KEY_INPUT_DOWN)) {
            camera_position.add(0.0, -1.0, 0.0);
        } else if (1 == CheckHitKey(KEY_INPUT_LEFT)) {
            camera_position.add(-1.0, 0.0, 0.0);
        } else if (1 == CheckHitKey(KEY_INPUT_RIGHT)) {
            camera_position.add(1.0, 0.0, 0.0);
        }

        // �J���������Z�b�g
        //SetCameraPositionAndTargetAndUpVec(ToDX(camera_position), ToDX(camera_target), ToDX(camera_up));
        auto camera_matrix = GetCameraViewMatrix(camera_position, camera_target, camera_up);
        SetCameraViewMatrix(ToDX(camera_matrix));

        //SetCameraNearFar(static_cast<float>(camera_near), static_cast<float>(camera_far));
        //SetupCamera_Perspective(static_cast<float>(fov));
        auto projection_matrix = GetCameraProjectionMatrix(camera_near, camera_far, fov);
        SetupCamera_ProjectionMatrix(ToDX(projection_matrix));

        ClearDrawScreen();

        // 3D ���f���̕\��
        MV1DrawModel(handle);

        DrawFormatString(0, 0, string_color, string_camera_position, camera_position.get_x(), camera_position.get_y(), camera_position.get_z());

        ScreenFlip();
    }

    // �쐬���������̔j��
    DeleteLightHandle(light_handle);

    // �ǂݍ��� 3D ���f���̔j��
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}

math::matrix44 GetCameraViewMatrix(math::vector4& cameraPosition, math::vector4& cameraTarget, math::vector4& cameraUp) {
    //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
    //MATRIX camera_matrix = GetCameraViewMatrix();
    // �� ��L�Ɠ��� MATRIX �̓��e���v�Z����
    math::matrix44 camera_matrix = math::matrix44();

    camera_matrix.look_at(cameraPosition, cameraTarget, cameraUp);

    return camera_matrix;
}

math::matrix44 GetCameraProjectionMatrix(double cameraNear, double cameraFar, double fov) {
    //SetCameraNearFar(camera_near, camera_far);
    //SetupCamera_Perspective(fov);
    //MATRIX projection_matrix = GetCameraProjectionMatrix();
    // �� ��L�Ɠ��� MATRIX �̓��e���v�Z����
    auto aspect = static_cast<float>(SCREEN_HEIGHT) / static_cast<float>(SCREEN_WIDTH);
    math::matrix44 projection_matrix = math::matrix44();

    projection_matrix.perspective(fov, aspect, cameraNear, cameraFar);

    return projection_matrix;
}
