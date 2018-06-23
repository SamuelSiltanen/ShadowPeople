#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Graphics.hpp"
#include "Errors.hpp"
#include "SceneRenderer.hpp"
#include "GameLogic.hpp"
#include "InputHandler.hpp"
#include "ImGuiInputHandler.hpp"

#include <tchar.h>

static TCHAR szWindowClass[]	= _T("shadowpeople");
static TCHAR szTitle[]			= _T("Shadow People");

static BOOL exitApplication		= FALSE;
static BOOL mouseOutOfWindow	= FALSE;
static BOOL mouseCaptured		= FALSE;
static BOOL keyboardCaptured	= FALSE;

static std::unique_ptr<input::InputHandler>			gameInputHandler	= nullptr;
static std::unique_ptr<input::ImGuiInputHandler>	imGuiInputHandler	= nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND			 createWindow(HINSTANCE hInstance);
void			 trackMouseLeave(HWND hWnd);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = createWindow(hInstance);
	SP_EXPECT_NOT_NULL(hWnd, ERROR_CODE_WINDOW_CREATION_FAILED);
	
	RECT clientRect;
	BOOL isOK = GetClientRect(hWnd, &clientRect);
	SP_EXPECT_NOT_NULL(isOK, ERROR_CODE_GET_CLIENT_RECT_FAILED);

	int2 clientArea {clientRect.right - clientRect.left, clientRect.bottom - clientRect.top };

	graphics::Device device(hWnd, clientArea);

	rendering::SceneRenderer sceneRenderer(device);

	gameInputHandler = std::make_unique<input::InputHandler>();

	std::shared_ptr<game::GameLogic> gameLogic = std::make_shared<game::GameLogic>(clientArea);
	gameInputHandler->registerListener(gameLogic);

	imGuiInputHandler = std::make_unique<input::ImGuiInputHandler>(hWnd);

	ShowWindow(hWnd, nCmdShow);
	trackMouseLeave(hWnd);

	int counter = 0;

	MSG msg	= {};
	while (!exitApplication)
	{
		// Check if the GUI has captured input devices, so that they don't go to he game input handler
		mouseCaptured		= imGuiInputHandler->mouseCaptured();
		keyboardCaptured	= imGuiInputHandler->keyboardCaptured();

		// Read Windows messages and send them to event handler
		while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		std::string text("Frame ");
		text.append(std::to_string(counter));
		if (mouseCaptured) text.append(" mouse captured");
		text.append("\n");
		SP_DEBUG_OUTPUT(text.c_str());

		// Update input handlers
		gameInputHandler->tick();
		imGuiInputHandler->tick(hWnd);

		// Draw frame
		graphics::CommandBuffer gfx = device.createCommandBuffer();
		sceneRenderer.render(gfx, gameLogic->camera());
		device.submit(gfx);
		device.present(1);

		counter++;
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Mouse events
	if ((uMsg >= WM_MOUSEFIRST) && (uMsg <= WM_MOUSELAST))
	{
		if (!mouseCaptured)
		{
			// Game input handler
			switch (uMsg)
			{
				case WM_MOUSEMOVE:
				{
					uint16_t x = static_cast<uint16_t>(lParam & 0xffff);
					uint16_t y = static_cast<uint16_t>(lParam >> 16);
					uint16_t flags = 0;
					if (wParam & MK_LBUTTON) flags |= input::FlagLeftButton;
					if (wParam & MK_RBUTTON) flags |= input::FlagRightButton;
					if (wParam & MK_MBUTTON) flags |= input::FlagMiddleButton;
					if (wParam & MK_SHIFT)   flags |= input::FlagShift;
					if (wParam & MK_CONTROL) flags |= input::FlagControl;
					gameInputHandler->mouseMove(x, y, flags);
					if (mouseOutOfWindow)
					{
						trackMouseLeave(hWnd);
						mouseOutOfWindow = FALSE;
					}
					break;
				}
				case WM_LBUTTONDOWN:
					gameInputHandler->mouseButtonDown(input::MouseButton::Left);
					break;
				case WM_LBUTTONUP:
					gameInputHandler->mouseButtonUp(input::MouseButton::Left);
					break;
				case WM_MBUTTONDOWN:
					gameInputHandler->mouseButtonDown(input::MouseButton::Middle);
					break;
				case WM_MBUTTONUP:
					gameInputHandler->mouseButtonUp(input::MouseButton::Middle);
					break;
				case WM_RBUTTONDOWN:
					gameInputHandler->mouseButtonDown(input::MouseButton::Right);
					break;
				case WM_RBUTTONUP:
					gameInputHandler->mouseButtonUp(input::MouseButton::Right);
					break;
				default:
					break;
			}
		}
		else
		{
			// GUI input handler
			switch (uMsg)
			{
				case WM_LBUTTONDOWN:
				case WM_LBUTTONDBLCLK:
					imGuiInputHandler->mouseButtonDown(hWnd, input::ImGuiInputHandler::MouseButtonLeft);
					break;
				case WM_RBUTTONDOWN:
				case WM_RBUTTONDBLCLK:
					imGuiInputHandler->mouseButtonDown(hWnd, input::ImGuiInputHandler::MouseButtonRight);
					break;
				case WM_MBUTTONDOWN:
				case WM_MBUTTONDBLCLK:
					imGuiInputHandler->mouseButtonDown(hWnd, input::ImGuiInputHandler::MouseButtonMiddle);
					break;
				case WM_LBUTTONUP:
					imGuiInputHandler->mouseButtonUp(hWnd, input::ImGuiInputHandler::MouseButtonLeft);
					break;
				case WM_RBUTTONUP:
					imGuiInputHandler->mouseButtonUp(hWnd, input::ImGuiInputHandler::MouseButtonRight);
					break;
				case WM_MBUTTONUP:
					imGuiInputHandler->mouseButtonUp(hWnd, input::ImGuiInputHandler::MouseButtonMiddle);
					break;
				case WM_MOUSEWHEEL:
				{
					float change = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA);
					imGuiInputHandler->mouseWheel(change);
					break;
				}
				case WM_MOUSEHWHEEL:
				{
					float change = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA);
					imGuiInputHandler->mouseWheelH(change);
					break;
				}
				default:
					break;
			}

		}
	}

	if ((uMsg >= WM_KEYFIRST) && (uMsg <= WM_KEYLAST))
	{
		if (!keyboardCaptured)
		{
			switch (uMsg)
			{
				case WM_KEYDOWN:
					gameInputHandler->keyDown(static_cast<uint8_t>(wParam));
					break;
				case WM_KEYUP:
					gameInputHandler->keyUp(static_cast<uint8_t>(wParam));
					break;
				default:
					break;
			}
		}
		else
		{
			switch (uMsg)
			{
				case WM_KEYDOWN:
					imGuiInputHandler->keyDown(static_cast<uint8_t>(wParam));
					break;
				case WM_KEYUP:
					imGuiInputHandler->keyUp(static_cast<uint8_t>(wParam));
					break;
				default:
					break;
			}
		}
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		exitApplication = TRUE;
		return 0;	
	
	case WM_MOUSELEAVE:
		gameInputHandler->outOfFocus();
		mouseOutOfWindow = TRUE;
		break;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			if (imGuiInputHandler->updateMouseCursor()) return 1;			
		}
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

HWND createWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));	// TODO: Find a proper icon
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);		// TODO: Custom cursor
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	ATOM id = RegisterClassEx(&wcex);
	SP_EXPECT_NOT_NULL_RET(id, ERROR_CODE_WIN_CLASS_REG_FAILED, NULL);
	
	return CreateWindowEx(0, szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						  NULL, NULL, hInstance, NULL);
}

void trackMouseLeave(HWND hWnd)
{
	TRACKMOUSEEVENT mouseTracking;
	mouseTracking.cbSize		= sizeof(TRACKMOUSEEVENT);
	mouseTracking.dwFlags		= TME_LEAVE;
	mouseTracking.hwndTrack		= hWnd;
	mouseTracking.dwHoverTime	= HOVER_DEFAULT;
	TrackMouseEvent(&mouseTracking);
}