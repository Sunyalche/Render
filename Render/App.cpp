#include "App.h"

namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	App* gApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gApp->MsgProc(hwnd, msg, wParam, lParam);
}

App::App(HINSTANCE hInstance)
	: mhAppInst(hInstance),
	mMainWndCaption(__TEXT("Application")),
	mhMainWnd(0),
	mClientWidth(800),
	mClientHeight(600),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false)
{
	// Get a pointer to the application object so we can forward 
	// Windows messages to the object's window procedure through
	// the global window procedure.
	gApp = this;
}

App::~App()
{
}

HINSTANCE App::AppInst()const
{
	return mhAppInst;
}

HWND App::MainWnd()const
{
	return mhMainWnd;
}

int App::Run()
{
	MSG msg = {0};

	mTimer.Reset();

	while (msg.message != WM_QUIT) {
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else {
			mTimer.Tick();

			if (!mAppPaused) {
				CalculateFrameStats();
				UpdateScene(mTimer.DeltaTime());
				void * lpBits = DrawScene();
				HDC hdc = GetDC(mhMainWnd);
				HDC memdc = CreateCompatibleDC(hdc);
				HBITMAP hbmp = CreateBitmap(mClientWidth, mClientHeight, 1, 32, lpBits);
				SelectObject(memdc, hbmp);
				BitBlt(hdc, 0, 0, mClientWidth, mClientHeight, memdc, 0, 0, SRCCOPY);
				DeleteObject(hbmp);
				DeleteObject(memdc);
				ReleaseDC(mhMainWnd, hdc);
			} else {
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

bool App::Init()
{
	if (!InitMainWindow())
		return false;
	return true;
}

void App::OnResize()
{

}

LRESULT App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE) {
			mAppPaused = true;
			mTimer.Stop();
		} else {
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (wParam == SIZE_MINIMIZED) {
			mAppPaused = true;
			mMinimized = true;
			mMaximized = false;
		} else if (wParam == SIZE_MAXIMIZED) {
			mAppPaused = false;
			mMinimized = false;
			mMaximized = true;
			OnResize();
		} else if (wParam == SIZE_RESTORED) {

			// Restoring from minimized state?
			if (mMinimized) {
				mAppPaused = false;
				mMinimized = false;
				OnResize();
			}

			// Restoring from maximized state?
			else if (mMaximized) {
				mAppPaused = false;
				mMaximized = false;
				OnResize();
			} else if (mResizing) {
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			} else { // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				OnResize();
			}
			
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool App::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = __TEXT("WndClassName");

	if (!RegisterClass(&wc)) {
		MessageBox(0, __TEXT("RegisterClass Failed."), 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = {0, 0, mClientWidth, mClientHeight};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(__TEXT("WndClassName"), mMainWndCaption.c_str(),
							 WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd) {
		MessageBox(0, __TEXT("CreateWindow Failed."), 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

void App::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f) {
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << __TEXT("    ")
			<< __TEXT("FPS: ") << fps << __TEXT("    ")
			<< __TEXT("Frame Time: ") << mspf << __TEXT(" (ms)");
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}


