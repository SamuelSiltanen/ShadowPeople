#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Graphics.hpp"
#include "Errors.hpp"
#include "SceneRenderer.hpp"

#include <tchar.h>

static TCHAR szWindowClass[]	= _T("shadowpeople");
static TCHAR szTitle[]			= _T("Shadow People");

static BOOL exitApplication		= FALSE;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND createWindow(HINSTANCE hInstance);

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HWND hWnd = createWindow(hInstance);
	SP_EXPECT_NOT_NULL(hWnd, ERROR_CODE_WINDOW_CREATION_FAILED);
	
	RECT clientRect;
	BOOL isOK = GetClientRect(hWnd, &clientRect);
	SP_EXPECT_NOT_NULL(isOK, ERROR_CODE_GET_CLIENT_RECT_FAILED);

	unsigned clientAreaWidth	= clientRect.right - clientRect.left;
	unsigned clientAreaHeight	= clientRect.bottom - clientRect.top;

	graphics::Device device(hWnd, clientAreaWidth, clientAreaHeight);

	rendering::SceneRenderer sceneRenderer(device);

	ShowWindow(hWnd, nCmdShow);

	MSG msg	= {};
	while (!exitApplication)
	{
		if (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		graphics::CommandBuffer gfx = device.createCommandBuffer();

		sceneRenderer.render(gfx);

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
		// TODO: Handle key down
		break;
	case WM_KEYUP:
		// TODO: Handle key up
		break;
	case WM_MOUSEMOVE:
		// TODO: Handle mouse movement
		break;
	case WM_LBUTTONDOWN:
		// TODO: Handle left mouse button down
		break;
	case WM_LBUTTONUP:
		// TODO: Handle left mouse button up
		break;
	case WM_RBUTTONDOWN:
		// TODO: Handle right mouse button down
		break;
	case WM_RBUTTONUP:
		// TODO: Handle right mouse button up
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
