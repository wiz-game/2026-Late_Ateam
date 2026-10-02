#include "stdafx.h"
#include "EditorCamera.h"
#include "Project.h"

namespace basecross {

    void EditorCamera::OnCreate()
    {
        // 初期位置をセット
        SetEye(m_eye);
        SetAt(m_at);

        // 遠近法カメラとして設定
        SetPers(true);
        SetFovY(XM_PIDIV4);
        SetNear(0.1f);
        SetFar(1000.0f);
    }

    void EditorCamera::OnUpdate()
    {
        auto& app = App::GetApp();
        auto device = app->GetInputDevice();
        auto key = device.GetKeyState();
        float delta = app->GetElapsedTime();

        // マウス右ドラッグで回転
        if (key.m_bPushKeyTbl[VK_RBUTTON])
        {
            auto p = key.m_MouseClientPoint;

            m_yaw -= (p.x - m_at.x) * m_rotateSpeed;
            m_pitch -= (p.y - m_at.y) * m_rotateSpeed;

            // ピッチ制限
            m_pitch = std::clamp(m_pitch, -XM_PIDIV2 + 0.1f, XM_PIDIV2 - 0.1f);
        }

        // カメラ方向ベクトル
        Vec3 forward(
            cosf(m_yaw) * cosf(m_pitch),
            sinf(m_pitch),
            sinf(m_yaw) * cosf(m_pitch)
        );

        Vec3 right(
            sinf(m_yaw - XM_PIDIV2),
            0,
            cosf(m_yaw - XM_PIDIV2)
        );

        // WASD移動
        if (key.m_bPushKeyTbl['W']) m_eye += forward * m_moveSpeed * delta;
        if (key.m_bPushKeyTbl['S']) m_eye -= forward * m_moveSpeed * delta;
        if (key.m_bPushKeyTbl['A']) m_eye -= right * m_moveSpeed * delta;
        if (key.m_bPushKeyTbl['D']) m_eye += right * m_moveSpeed * delta;

        // 上下移動
        if (key.m_bPushKeyTbl[VK_SPACE]) m_eye.y += m_moveSpeed * delta;
        if (key.m_bPushKeyTbl[VK_SHIFT]) m_eye.y -= m_moveSpeed * delta;

        // 注視点は前方方向
        m_at = m_eye + forward;

        SetEye(m_eye);
        SetAt(m_at);
    }

    void EditorCamera::SetPosition(const Vec3& eye, const Vec3& at)
    {
        m_eye = eye;
        m_at = at;
        SetEye(m_eye);
        SetAt(m_at);

        Vec3 dir = Vec3(m_at - m_eye).normalize();

        m_pitch = asinf(dir.y);
        m_yaw = atan2(dir.z, dir.x);
    }

}