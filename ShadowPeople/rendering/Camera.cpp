#include "Camera.hpp"
#include "../Errors.hpp"
#include "../Math.hpp"

namespace rendering
{
	static const float Epsilon = 1e-6f;

	Camera::Camera(float fov, float aspectRatio, float4 position, float yaw, float pitch,
				   float nearZ, float farZ) :
		m_position(position),
		m_yaw(yaw),
		m_pitch(pitch),
		m_near(nearZ),
		m_far(farZ),
		m_fov(fov),
		m_aspectRatio(aspectRatio),
		m_projection(Projection::Perspective)
	{}

	Camera::Camera(Rect<int, 2> orthoDimensions, float4 position, float yaw, float pitch,
				   float nearZ, float farZ) :
		m_position(position),
		m_yaw(yaw),
		m_pitch(pitch),
		m_near(nearZ),
		m_far(farZ),
		m_orthoDimensions(orthoDimensions),
		m_projection(Projection::Orthogonal)
	{}

	Matrix4x4 Camera::viewMatrix() const
	{
		Matrix4x4 rot = math::rotationMatrix(m_yaw, m_pitch);

		Matrix4x4 mov;
		mov(0, 3) = -m_position[0];
		mov(1, 3) = -m_position[1];
		mov(2, 3) = -m_position[2];

		Matrix4x4 view = rot * mov;

		return view;
	}

	Matrix4x4 Camera::projectionMatrix() const
	{
		Matrix4x4 mat;

		if (m_projection == Projection::Perspective)
		{
			float r = 1.f / (m_far - m_near);
			float f = tanf(0.5f * m_fov);
			mat(0, 0) = f / m_aspectRatio;
			mat(1, 1) = f;
			mat(2, 2) = (m_far + m_near) * r;
			mat(2, 3) = -2.f * m_far * m_near * r;
			mat(3, 2) = 1.f;
			mat(3, 3) = 0.f;
		}
		else
		{
			float r = 1.f / (m_far - m_near);
			int2 s = m_orthoDimensions.size();
			mat(0, 0) = 0.5f * s[0];
			mat(1, 1) = 0.5f * s[1];
			mat(2, 2) = 2.f * r;
			mat(2, 3) = -(m_far + m_near) * r;
		}

		return mat;
	}

	float4 Camera::position() const
	{
		return m_position;
	}

	float Camera::yaw() const
	{
		return m_yaw;
	}

	float Camera::pitch() const
	{
		return m_pitch;
	}

	void Camera::setTransform(float4 position, float yaw, float pitch)
	{
		m_position	= position;
		m_yaw		= yaw;
		m_pitch		= pitch;
	}

	float4 Camera::front() const
	{
		Matrix4x4 mat = math::rotationMatrix(m_yaw, m_pitch); 
		float4 front = { mat(2, 0), mat(2, 1), mat(2, 2), 0.f };
		return normalize(front);
	}

	float4 Camera::up() const
	{
		Matrix4x4 mat = math::rotationMatrix(m_yaw, m_pitch); 
		float4 up = { mat(1, 0), mat(1, 1), mat(1, 2), 0.f };
		return normalize(up);
	}

	float4 Camera::right() const
	{
		Matrix4x4 mat = math::rotationMatrix(m_yaw, m_pitch); 
		float4 right = { mat(0, 0), mat(0, 1), mat(0, 2), 0.f };
		return normalize(right);
	}
}
