#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <array>

#include "Action.hpp"

#include "../Types.hpp"

namespace input
{
	enum class MouseButton
	{
		Left,
		Right,
		Middle
	};

	// Flags to mark which buttons are down during mouse events
	constexpr uint16_t FlagLeftButton	= 1;
	constexpr uint16_t FlagRightButton	= 2;
	constexpr uint16_t FlagShift		= 4;
	constexpr uint16_t FlagControl		= 8;
	constexpr uint16_t FlagMiddleButton = 16;

	// Maps input events into game actions
	class InputHandler
	{
	public:
		InputHandler();

		void keyDown(uint8_t keyCode);
		void keyUp(uint8_t keyCode);
		void mouseButtonDown(MouseButton button);
		void mouseButtonUp(MouseButton button);

		void mouseMove(uint16_t x, uint16_t y, uint16_t flags);
		void outOfFocus();

		void registerListener(std::shared_ptr<ActionListener> listener);
		void tick();
	private:
		std::vector<std::shared_ptr<ActionListener>>	m_listeners;
		std::vector<Action>								m_actions;		

		static const uint32_t NumKeyCodes = 256;

		std::array<int, NumKeyCodes>						m_keyDown;

		static const uint32_t MovementBufferSize = 1024;

		std::array<int2, MovementBufferSize>			m_movementsStack;
		int												m_movementsTop;
		int2											m_prevMousePos;
	};
}
