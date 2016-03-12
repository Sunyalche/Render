#pragma once
#include "Util.h"
#include "App.h"
#include "RenderManager.h"
#include "RenderScence.h"

class MainWindow : public App
{
public:
	MainWindow(HINSTANCE hInstance);
	~MainWindow();

	void OnResize();
	void UpdateScene(float dt);
	void * DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	RenderScence mRenderScence;
	POINT mLastMousePos;
};