/*
    Copyright 2018 Samuel Siltanen
    GameLogic.cpp
*/

#include "GameLogic.hpp"
#include "../Errors.hpp"

#include <string>

namespace game
{
    const float GameLogic::CameraMoveSpeed      = 0.1f;
    const float GameLogic::CameraRotateSpeed    = 3.0f;

    GameLogic::GameLogic(int2 screenSize) :
        m_screenSize(screenSize),
        m_camera(rendering::DefaultFov,
                 static_cast<float>(screenSize[0]) / static_cast<float>(screenSize[1]),
                 float4({0.f, 1.5f, -10.f, 1.f}))
    {
    }

    void GameLogic::onAction(const input::Action& action)
    {
        float4 position = m_camera.position();
        float yaw       = m_camera.yaw();
        float pitch     = m_camera.pitch();

        switch (action.type)
        {
        case input::ActionType::MoveForward:
            position += m_camera.front() * CameraMoveSpeed;
            break;
        case input::ActionType::MoveBackward:
            position -= m_camera.front() * CameraMoveSpeed;
            break;
        case input::ActionType::StrafeRight:
            position += m_camera.right() * CameraMoveSpeed;
            break;
        case input::ActionType::StrafeLeft:
            position -= m_camera.right() * CameraMoveSpeed;
            break;
        case input::ActionType::Rotate:
        {
            int2 dxdy = *static_cast<int2 *>(action.data);
            float2 fxfy { static_cast<float>(dxdy[0]) / static_cast<float>(m_screenSize[0]),
                          static_cast<float>(dxdy[1]) / static_cast<float>(m_screenSize[1]) };
            yaw     += fxfy[0] * CameraRotateSpeed;
            pitch   -= fxfy[1] * CameraRotateSpeed;
        }
        default:
            break;
        }

        m_camera.setTransform(position, yaw, pitch);
    }
}
