#include "InputHandler.hpp"
#include "Errors.hpp"

namespace input
{
	InputHandler::InputHandler() :
		m_prevMousePos({-1, -1}),
		m_movementsTop(0)
	{
		for (uint32_t i = 0; i < NumKeyCodes; i++)
		{
			m_keyDown[i] = 0;
		}
	}

	void InputHandler::keyDown(uint8_t keyCode)
	{
		// If the previous key down messages were not handled already,
		// use negative numbers to indicate the number of key presses.
		if (m_keyDown[keyCode] <= 0)
		{
			m_keyDown[keyCode]--;
		}
		else
		{
			m_keyDown[keyCode]++;
		}
	}

	void InputHandler::keyUp(uint8_t keyCode)
	{
		// Reset the key press counter only if the messages were handled,
		// which is indicated by positive number. This way, key presses that
		// last less than a frame get properly handled.
		if (m_keyDown[keyCode] > 0)
		{
			m_keyDown[keyCode] = 0;
		}
	}

	void InputHandler::mouseButtonDown(MouseButton button)
	{
		if (button == MouseButton::Left)
		{
			m_actions.emplace_back(Action(ActionType::Select));
		}
	}

	void InputHandler::mouseButtonUp(MouseButton button)
	{	
	}

	void InputHandler::mouseMove(uint16_t x, uint16_t y, uint16_t flags)
	{
		if (!any(m_prevMousePos < 0))
		{
			if (flags & input::FlagLeftButton)
			{
				int2 dxdy = int2{ x, y } - m_prevMousePos;
				m_movementsStack[m_movementsTop] = dxdy;
				m_actions.emplace_back(Action(ActionType::Rotate, &m_movementsStack[m_movementsTop]));
				m_movementsTop++;
				SP_ASSERT(m_movementsTop < MovementBufferSize, "Mouse actions buffer full");
			}
		}

		m_prevMousePos = { x, y };
	}

	void InputHandler::outOfFocus()
	{
		m_prevMousePos = { -1, -1 };
	}

	void InputHandler::registerListener(std::shared_ptr<ActionListener> listener)
	{
		m_listeners.emplace_back(listener);
	}

	void InputHandler::tick()
	{
		// Generate keyboard actions once per frame for smoother control
		// To catch less than one frame long key presses, set a special value for the first key press
		for (uint32_t i = 0; i < NumKeyCodes; i++)
		{
			uint8_t keyCode = static_cast<uint8_t>(i);
			int numTimesPressed = m_keyDown[keyCode];
			if (numTimesPressed != 0)
			{
				switch (keyCode)
				{
				case 'S':
					m_actions.emplace_back(Action(ActionType::MoveBackward));
					break;
				case 'W':
					m_actions.emplace_back(Action(ActionType::MoveForward));
					break;
				case 'A':
					m_actions.emplace_back(Action(ActionType::StrafeLeft));
					break;
				case 'D':
					m_actions.emplace_back(Action(ActionType::StrafeRight));
					break;
				default:
					break;
				}

				// If we encountered unhandled key presses, which is indicated by a negative number,
				// turn it into a positive number to mark that the key down events are handled
				if (numTimesPressed < 0)
				{
					m_keyDown[keyCode] = -numTimesPressed;
				}
			}
		}

		// Handle other actions one by one
		for (const auto& a : m_actions)
		{
			for (auto l : m_listeners)
			{
				l->onAction(a);
			}
		}
		m_actions.clear();
		m_movementsTop = 0;
	}
}
