#pragma once

#include "../Types.hpp"
#include "../Math.hpp"

namespace rendering
{
    constexpr float DefaultNear         = 1.f / 128.f;
    constexpr float DefaultFar          = 16384.f;
    constexpr float DefaultFov          = math::Pi * 0.25f; // Vertical field of view = 45 degrees
    constexpr float DefaultAspectRatio  = 1.f;

    static const Rect<int, 2> DefaultOrthoDimensions = Rect<int, 2>(int2{1024, 1024});

    class Camera
    {
    public:
        enum class Projection
        {
            Orthogonal,
            Perspective
        };

        Camera(
            float fov           = DefaultFov,
            float aspectRatio   = DefaultAspectRatio,
            float4 position     = float4({0.f, 0.f, 0.f, 1.f}),
            float yaw           = 0.f,
            float pitch         = 0.f,
            float nearZ         = DefaultNear,
            float farZ          = DefaultFar);
        Camera(
            Rect<int, 2> orthoDimensions = DefaultOrthoDimensions,
            float4 position     = float4({0.f, 0.f, 0.f, 1.f}),
            float yaw           = 0.f,
            float pitch         = 0.f,
            float nearZ         = DefaultNear,
            float farZ          = DefaultFar);

        Matrix4x4 viewMatrix() const;
        Matrix4x4 projectionMatrix() const;

        float4 position() const;
        float yaw() const;
        float pitch() const;

        void setTransform(float4 position, float yaw, float pitch);

        float4 front() const;
        float4 up() const;
        float4 right() const;
    private:
        float4          m_position;
        float           m_yaw;
        float           m_pitch;
        float           m_near;
        float           m_far;
        float           m_fov;
        float           m_aspectRatio;
        Projection      m_projection;
        Rect<int, 2>    m_orthoDimensions;
    };
}
