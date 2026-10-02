#pragma once
#include "stdafx.h"

namespace basecross {

    class EditorCamera : public Camera
    {
        Vec3 m_eye;
        Vec3 m_at;
        POINT m_prevMousePoint{ 0, 0 };

        float m_yaw;
        float m_pitch;
        float m_moveSpeed;
        float m_rotateSpeed;

    public:
        EditorCamera()
            : Camera(),
            m_eye(0.0f, 10.0f, -15.0f),
            m_at(0.0f, 3.0f, 0.0f),
            m_yaw(0.0f),
            m_pitch(0.0f),
            m_moveSpeed(20.0f),
            m_rotateSpeed(0.005f)
        {}

        virtual void OnCreate() override;
        virtual void OnUpdate() override;

        void SetPosition(const Vec3& eye, const Vec3& at);
    };

}