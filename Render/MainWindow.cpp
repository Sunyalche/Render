#include "MainWindow.h"

using namespace RenderMath;

MainWindow::MainWindow(HINSTANCE hInstance)
	: App(hInstance)
{
	mMainWndCaption = __TEXT("Cube Demo");

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;
}

MainWindow::~MainWindow()
{ }


void MainWindow::OnResize()
{
	App::OnResize();
	RenderManager::GetInstance()->ResizeClient(mClientWidth, mClientHeight);
}

void MainWindow::UpdateScene(float dt)
{
	
}
void * MainWindow::DrawScene()
{
	RenderManager::GetInstance()->BeginRender();
	return RenderManager::GetInstance()->screenBuffer.data.pData;
}

void MainWindow::OnMouseDown(WPARAM btnState, int x, int y)
{ }

void MainWindow::OnMouseUp(WPARAM btnState, int x, int y)
{ }

void MainWindow::OnMouseMove(WPARAM btnState, int x, int y)
{ }