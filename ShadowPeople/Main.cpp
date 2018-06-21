#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Graphics.hpp"
#include "Errors.hpp"
#include "SceneRenderer.hpp"
#include "GameLogic.hpp"
#include "InputManager.hpp"

#include <tchar.h>

static TCHAR szWindowClass[]	= _T("shadowpeople");
static TCHAR szTitle[]			= _T("Shadow People");

static BOOL exitApplication		= FALSE;
static BOOL mouseOutOfWindow	= FALSE;

static std::unique_ptr<input::InputManager> inputManager = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND createWindow(HINSTANCE hInstance);
void trackMouseLeave(HWND hWnd);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = createWindow(hInstance);
	SP_EXPECT_NOT_NULL(hWnd, ERROR_CODE_WINDOW_CREATION_FAILED);
	
	RECT clientRect;
	BOOL isOK = GetClientRect(hWnd, &clientRect);
	SP_EXPECT_NOT_NULL(isOK, ERROR_CODE_GET_CLIENT_RECT_FAILED);

	int2 clientArea {clientRect.right - clientRect.left, clientRect.bottom - clientRect.top };

	graphics::Device device(hWnd, clientArea);

	inputManager = std::make_unique<input::InputManager>();

	rendering::SceneRenderer sceneRenderer(device);
	
	std::shared_ptr<game::GameLogic> gameLogic = std::make_shared<game::GameLogic>(clientArea);
	inputManager->registerListener(gameLogic);

	ShowWindow(hWnd, nCmdShow);
	trackMouseLeave(hWnd);

	MSG msg	= {};
	while (!exitApplication)
	{
		while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		inputManager->tick();

		graphics::CommandBuffer gfx = device.createCommandBuffer();

		sceneRenderer.render(gfx, gameLogic->camera());

		device.submit(gfx);
		device.present(1);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		exitApplication = TRUE;
		return 0;
	
	case WM_KEYDOWN:
		inputManager->keyDown(static_cast<uint8_t>(wParam));
		break;
	case WM_KEYUP:
		inputManager->keyUp(static_cast<uint8_t>(wParam));
		break;
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
		inputManager->mouseMove(x, y, flags);
		if (mouseOutOfWindow)
		{
			trackMouseLeave(hWnd);
			mouseOutOfWindow = FALSE;
		}
		break;
	}
	case WM_LBUTTONDOWN:
		inputManager->mouseButtonDown(input::MouseButton::Left);
		break;
	case WM_LBUTTONUP:
		inputManager->mouseButtonUp(input::MouseButton::Left);
		break;
	case WM_MBUTTONDOWN:
		inputManager->mouseButtonDown(input::MouseButton::Middle);
		break;
	case WM_MBUTTONUP:
		inputManager->mouseButtonUp(input::MouseButton::Middle);
		break;
	case WM_RBUTTONDOWN:
		inputManager->mouseButtonDown(input::MouseButton::Right);
		break;
	case WM_RBUTTONUP:
		inputManager->mouseButtonUp(input::MouseButton::Right);
		break;
	case WM_MOUSELEAVE:
		inputManager->outOfFocus();
		mouseOutOfWindow = TRUE;
		break;
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