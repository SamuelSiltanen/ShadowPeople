#pragma once

#include "../input/Action.hpp"

#include "../rendering/Camera.hpp"

namespace game
{
	class GameLogic : public input::ActionListener
	{
	public:
		GameLogic(int2 screenSize);

		const rendering::Camera& camera() { return m_camera; }

		void onAction(const input::Action& action) final override;
	private:
		static const float CameraMoveSpeed;
		static const float CameraRotateSpeed;

		rendering::Camera	m_camera;
		int2				m_screenSize;
	};
}
