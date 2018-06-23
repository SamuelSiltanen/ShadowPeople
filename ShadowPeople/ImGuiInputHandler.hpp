#pragma once

// Unfortunately, this is windows-specific
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Types.hpp"

namespace input
{
	class ImGuiInputHandler
	{
	public:
		ImGuiInputHandler(HWND hWnd);

		void tick(HWND hWnd);

		void mouseButtonDown(HWND hWnd, int button);
		void mouseButtonUp(HWND hWnd, int button);
		void mouseWheel(float change);
		void mouseWheelH(float change);
		void keyDown(uint8_t keyCode);
		void keyUp(uint8_t keyCode);

		bool updateMouseCursor();

		bool mouseCaptured();
		bool keyboardCaptured();

		static const int MouseButtonLeft = 0;
		static const int MouseButtonRight = 0;
		static const int MouseButtonMiddle = 0;
	private:
		void setKeyboardMappings();		
		void updateMousePosition(HWND hWnd);

		int64_t	m_ticksPerSecond;
		int64_t	m_time;

		int		m_lastMouseCursor;
	};
}