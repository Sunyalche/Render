#pragma once
#include "Util.h"
#include "Timer.h"
class App
{
public:
	App(HINSTANCE hInstance);
	virtual ~App();

	HINSTANCE AppInst()const;
	HWND      MainWnd()const;

	int Run();

	virtual void OnResize();
	virtual void UpdateScene(float dt) = 0;
	virtual void * DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
	virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

protected:
	void CalculateFrameStats();

private:
	bool InitMainWindow();

protected:
	int mClientWidth;
	int mClientHeight;


	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;
	Timer mTimer;
	// Derived class should set these in derived constructor to customize starting values.
	std::string mMainWndCaption;
};
