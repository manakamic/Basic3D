//!
//! @file main_11.cpp
//!
//! @brief 3D ���f����ǂݍ��݁A�Œ���̕\�����s���T���v��(+�J�����ړ�+�J�����s��̌v�Z+�v���W�F�N�V�����s��)
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
    math::vector4 camera_position = math::vector4(-400.0, 250.0, -150.0);
    math::vector4 camera_target = math::vector4(0.0, 0.0, 0.0);
    math::vector4 camera_up = math::vector4(0.0, 1.0, 0.0);

    // �p�[�X�y�N�e�B�u�����ɕK�v�ȏ��
    auto camera_near = 1.0;
    auto camera_far = 1000.;
    auto fov = math::utility::degree_to_radian(60.0);

    SetDrawScreen(DX_SCREEN_BACK);

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
        //SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);
        math::matrix44 camera_matrix = GetCameraViewMatrix(camera_position, camera_target, camera_up);
        SetCameraViewMatrix(ToDX(camera_matrix));

        //SetCameraNearFar(camera_near, camera_far);
        //SetupCamera_Perspective(fov);
        math::matrix44 projection_matrix = GetCameraProjectionMatrix(camera_near, camera_far, fov);
        SetupCamera_ProjectionMatrix(ToDX(projection_matrix));

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
