#include "DxLib.h"
#include "gun.h"

namespace {
    constexpr auto DEGREE_TO_RADIAN = DX_PI_F / 180.0f;
}

namespace mv1 {

    gun::gun() : model_base() {
        offset_matrix = MGetIdent();
        target_frame = -1;
    }

    bool gun::initialize() {
        if (handle == -1) {
            return false;
        }

        // DxLibModelViewer_64bit.exe �Œ��ׂ��`��ɕK�v�Ȃ� Frame ������
        MV1SetFrameVisible(handle, 0, FALSE);
        MV1SetFrameVisible(handle, 1, FALSE);
        MV1SetFrameVisible(handle, 2, FALSE);
        MV1SetFrameVisible(handle, 5, FALSE);
        MV1SetFrameVisible(handle, 8, FALSE);
        MV1SetFrameVisible(handle, 9, FALSE);
        MV1SetFrameVisible(handle, 10, FALSE);
        MV1SetFrameVisible(handle, 11, FALSE);
        MV1SetFrameVisible(handle, 12, FALSE);
        MV1SetFrameVisible(handle, 13, FALSE);

        return true;
    }

    bool gun::setup_offset_matrix(std::shared_ptr<mv1::model_base>& target) {
        // DxLibModelViewer_64bit.exe �Œ��ׂ� �L�����N�^�[���f���� �e������������ Frame �̔ԍ�
        // Viewer �� Frame �ԍ������Ă���͂��Ȃ̂Œ��ڐ��l�������Ă��ǂ�
        target_frame = MV1SearchFrame(target->get_handle(), _T("SDChar_RightHand"));

        if (target_frame < 0) {
            return false;
        }

        // �L�����N�^�[ ���f���Ƃ̑傫�����X�P�[���ō��킹��
        MATRIX gun_scale = MGetScale(VGet(15.0f, 15.0f, 15.0f));

        // �U���A�j���[�V�����ō��킹�����Ȃ� Z ����]�����Ă����Ɨǂ�
        //MATRIX gun_rot_z = MGetRotZ(-20.0f * DEGREE_TO_RADIAN);
        MATRIX gun_rot_y = MGetRotY(180.0f * DEGREE_TO_RADIAN);
        MATRIX gun_rot_x = MGetRotX(-90.0f * DEGREE_TO_RADIAN);
        MATRIX gun_offset = MGetTranslate(VGet(-16.0f, -2.0f, 0.0f));

        // DxLibModelViewer_64bit.exe �Œ��ׂ� Frame(index 28) ��
        // XYZ (�ԗΐ�)�� �ɍ����悤�ɗ\�߃��f�����A�t�B���ϊ����Ă���
        //offset_matrix = MMult(MMult(MMult(MMult(gun_scale, gun_rot_z), gun_rot_y), gun_rot_x), gun_offset);
        offset_matrix = MMult(MMult(MMult(gun_scale, gun_rot_y), gun_rot_x), gun_offset);

        return true;
    }
}
