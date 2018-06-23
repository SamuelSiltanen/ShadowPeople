#include "ImGuiInputHandler.hpp"
#include "imgui\imgui.h"

#include "Errors.hpp"

namespace input
{
	ImGuiInputHandler::ImGuiInputHandler(HWND hWnd) :
		m_lastMouseCursor(ImGuiMouseCursor_COUNT)
	{
		SP_ASSERT(QueryPerformanceFrequency((LARGE_INTEGER *)&m_ticksPerSecond), "Failed to query performance counter");
		SP_ASSERT(QueryPerformanceCounter((LARGE_INTEGER *)&m_time), "Failes to query performace counter");

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ImeWindowHandle = hWnd;

		RECT rect;
		GetClientRect(hWnd, &rect);
		io.DisplaySize = ImVec2(static_cast<float>(rect.right - rect.left),
								static_cast<float>(rect.bottom - rect.top));

		setKeyboardMappings();
	}

	void ImGuiInputHandler::setKeyboardMappings()	
	{
		ImGuiIO& io = ImGui::GetIO();

		io.KeyMap[ImGuiKey_Tab]			= VK_TAB;
		io.KeyMap[ImGuiKey_LeftArrow]	= VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow]	= VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow]		= VK_UP;
		io.KeyMap[ImGuiKey_DownArrow]	= VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp]		= VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown]	= VK_NEXT;
		io.KeyMap[ImGuiKey_Home]		= VK_HOME;
		io.KeyMap[ImGuiKey_End]			= VK_END;
		io.KeyMap[ImGuiKey_Insert]		= VK_INSERT;
		io.KeyMap[ImGuiKey_Delete]		= VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace]	= VK_BACK;
		io.KeyMap[ImGuiKey_Space]		= VK_SPACE;
		io.KeyMap[ImGuiKey_Enter]		= VK_RETURN;
		io.KeyMap[ImGuiKey_Escape]		= VK_ESCAPE;
		io.KeyMap[ImGuiKey_A]			= 'A';
		io.KeyMap[ImGuiKey_C]			= 'C';
		io.KeyMap[ImGuiKey_V]			= 'V';
		io.KeyMap[ImGuiKey_X]			= 'X';
		io.KeyMap[ImGuiKey_Y]			= 'Y';
		io.KeyMap[ImGuiKey_Z]			= 'Z';
	}

	bool ImGuiInputHandler::updateMouseCursor()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) return false;

		ImGuiMouseCursor imGuiCursor = ImGui::GetMouseCursor();
		if (imGuiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			SetCursor(NULL);
		}
		else
		{
			LPTSTR win32Cursor = IDC_ARROW;
			switch (imGuiCursor)
			{
			case ImGuiMouseCursor_Arrow:
				win32Cursor = IDC_ARROW;
				break;
			case ImGuiMouseCursor_TextInput:
				win32Cursor = IDC_IBEAM;
				break;
			case ImGuiMouseCursor_ResizeAll:
				win32Cursor = IDC_SIZEALL;
				break;
			case ImGuiMouseCursor_ResizeEW:
				win32Cursor = IDC_SIZEWE;
				break;
			case ImGuiMouseCursor_ResizeNS:
				win32Cursor = IDC_SIZENS;
				break;
			case ImGuiMouseCursor_ResizeNESW:
				win32Cursor = IDC_SIZENESW;
				break;
			case ImGuiMouseCursor_ResizeNWSE:
				win32Cursor = IDC_SIZENWSE;
				break;
			default:
				break;
			}
			SetCursor(LoadCursor(NULL, win32Cursor));
		}

		return true;
	}

	void ImGuiInputHandler::updateMousePosition(HWND hWnd)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (io.WantSetMousePos)
		{
			POINT pos = { static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y) };
			ClientToScreen(hWnd, &pos);
			SetCursorPos(pos.x, pos.y);
		}
		
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

		if (GetActiveWindow() != hWnd) return;

		POINT pos;
		if (!GetCursorPos(&pos)) return;
		if (!ScreenToClient(hWnd, &pos)) return;

		io.MousePos = ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y));
	}

	void ImGuiInputHandler::tick(HWND hWnd)
	{
		ImGuiIO& io = ImGui::GetIO();

		int64_t currentTime;
		QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
		io.DeltaTime = static_cast<float>(currentTime - m_time) / m_ticksPerSecond;
		m_time = currentTime;

		io.KeyCtrl	= (GetKeyState(VK_CONTROL) & 0x8000) != 0;
		io.KeyShift	= (GetKeyState(VK_SHIFT) & 0x8000) != 0;
		io.KeyAlt	= (GetKeyState(VK_MENU) & 0x8000) != 0;
		io.KeySuper = false;

		updateMousePosition(hWnd);

		ImGuiMouseCursor mouseCursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
		if (m_lastMouseCursor != mouseCursor)
		{
			m_lastMouseCursor = mouseCursor;
			updateMouseCursor();
		}
	}

	void ImGuiInputHandler::mouseButtonDown(HWND hWnd, int button)
	{
		ImGuiIO& io = ImGui::GetIO();

		if (!ImGui::IsAnyMouseDown() && GetCapture() == NULL)
		{
			SetCapture(hWnd);
		}

		io.MouseDown[button] = true;
	}

	void ImGuiInputHandler::mouseButtonUp(HWND hWnd, int button)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.MouseDown[button] = false;

		if (!ImGui::IsAnyMouseDown() && GetCapture() == hWnd)
		{
			ReleaseCapture();
		}
	}

	void ImGuiInputHandler::mouseWheel(float change)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += change;
	}

	void ImGuiInputHandler::mouseWheelH(float change)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += change;
	}

	void ImGuiInputHandler::keyDown(uint8_t keyCode)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[keyCode] = 1;
	}
	
	void ImGuiInputHandler::keyUp(uint8_t keyCode)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[keyCode] = 0;
	}

	bool ImGuiInputHandler::mouseCaptured()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureMouse;
	}

	bool ImGuiInputHandler::keyboardCaptured()
	{
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureKeyboard;
	}
}
