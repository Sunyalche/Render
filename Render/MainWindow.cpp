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

bool MainWindow::Init()
{
	if (!App::Init())
		return false;
	static const Vertex cubeVertices[ ] =
	{
		Vertex(Vector4::NewVector4(-0.5f, -0.5f, -0.5f), Vector4::NewVector4(0.0f, 0.0f, 0.0f)),
		Vertex(Vector4::NewVector4(-0.5f, -0.5f, 0.5f), Vector4::NewVector4(0.0f, 0.0f, 1.0f)),
		Vertex(Vector4::NewVector4(-0.5f, 0.5f, -0.5f), Vector4::NewVector4(0.0f, 1.0f, 0.0f)),
		Vertex(Vector4::NewVector4(-0.5f, 0.5f, 0.5f), Vector4::NewVector4(0.0f, 1.0f, 1.0f)),
		Vertex(Vector4::NewVector4(0.5f, -0.5f, -0.5f), Vector4::NewVector4(1.0f, 0.0f, 0.0f)),
		Vertex(Vector4::NewVector4(0.5f, -0.5f, 0.5f), Vector4::NewVector4(1.0f, 0.0f, 1.0f)),
		Vertex(Vector4::NewVector4(0.5f, 0.5f, -0.5f), Vector4::NewVector4(1.0f, 1.0f, 0.0f)),
		Vertex(Vector4::NewVector4(0.5f, 0.5f, 0.5f), Vector4::NewVector4(1.0f, 1.0f, 1.0f)),
	};
	RenderManager::GetInstance()->SetInVertexBuffer(cubeVertices, 8);
	std::vector<int> cubeIndices =
	{
		0, 2, 1, // -x
		1, 2, 3,

		4, 5, 6, // +x
		5, 7, 6,

		0, 1, 5, // -y
		0, 5, 4,

		2, 6, 7, // +y
		2, 7, 3,

		0, 4, 6, // -z
		0, 6, 2,

		1, 3, 7, // +z
		1, 7, 5,
	};
	
	RenderManager::GetInstance()->SetIndexBuffer(cubeIndices.data(), cubeIndices.size());
	RenderManager::GetInstance()->SetVertexShader([](const Vertex * pInVertex, Vertex * pOutVertex) {
		memcpy(pOutVertex, pInVertex, sizeof(Vertex));
		Matrix4X4 view(1.0f, 0.0f, 0.0f, 0.0f, 
					   0.0f, 1.0f, 0.0f, 0.0f, 
					   0.0f, 0.0f, 1.0f, 0.0f, 
					   1.0f, 1.0f, 3.0f, 1.0f);
		pOutVertex->position *= view;
	}, sizeof(Vertex));

	return true;
}

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