#include "DxLib.h"
#include <cmath>

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

    auto string_color = GetColor(255, 255, 255);
    auto string_camera_position = _T("Camera Position : X[%.1f], Y[%.1f], Z[%.1f]");
    auto string_model_position = _T("Model Position : X[%.1f], Y[%.1f], Z[%.1f]");
    auto model_y_angle = 0.0f;

    while (ProcessMessage() != -1) {
        if (1 == CheckHitKey(KEY_INPUT_ESCAPE)) {
            break;
        }

        // ���f���� Y���ŉ�]�����Ă݂�
        model_y_angle += 0.5f; // 0.5 �x�Â��Z

        // �x -> ���W�A���ɕϊ�
        auto model_y_radian = model_y_angle * DX_PI_F / 180.0f;

        // Y���̉�]�l�����W�A���Ŏw�肵�čs��Ŏ擾����
        MATRIX model_y_matrix = MGetRotY(model_y_radian);

        // �X�P�[�����ǉ����Ă݂�(���f���� Y ���p�x�𗬗p)
        auto model_scale = std::abs(std::cos(model_y_radian));

        // XYZ �̊e���ɑ΂��ẴX�P�[���l���x�N�g���Ŏw��
        VECTOR model_scale_vector = VGet(model_scale, model_scale, model_scale);

        // �X�P�[���̍s����擾
        MATRIX model_scale_matrix = MGetScale(model_scale_vector);

        // Z�����̈ړ����ǉ����Ă݂�(���f���� Y���p�x�𗬗p)
        auto model_z_position = 300.0f * std::sin(model_y_radian);

        // ���f���� XYZ (�ʒu)�̃x�N�g��
        VECTOR model_position_vector = VGet(0.0f, 0.0f, model_z_position);

        // ���s�ړ��̍s����擾
        MATRIX model_position_matrix = MGetTranslate(model_position_vector);

        // �X�P�[���̍s��� Y����]�̍s�����������
        MATRIX model_matritx = MMult(model_scale_matrix, model_y_matrix);

        // �X�� Z�����̈ړ��̍s���ǉ�����
        model_matritx = MMult(model_matritx, model_position_matrix);

        // ���f���ɑ΂��Ă̏����s��ŃZ�b�g����
        MV1SetMatrix(handle, model_matritx);

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
        SetCameraPositionAndTargetAndUpVec(camera_position, camera_target, camera_up);

        ClearDrawScreen();

        // 3D ���f���̕\��
        MV1DrawModel(handle);

        DrawFormatString(0, 0, string_color, string_camera_position, camera_position.x, camera_position.y, camera_position.z);
        DrawFormatString(0, 20, string_color, string_model_position, model_matritx.m[3][0], model_matritx.m[3][1], model_matritx.m[3][2]);

        ScreenFlip();
    }

    // �쐬���������̔j��
    DeleteLightHandle(light_handle);

    // �ǂݍ��� 3D ���f���̔j��
    MV1DeleteModel(handle);

    DxLib_End();

    return 0;
}
