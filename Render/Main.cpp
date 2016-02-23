#include "Util.h"
#include "RenderMath.h"
#include "RenderManager.h"
#include "MainWindow.h"

using namespace std;
using namespace RenderMath;


void raster_unit()
{
	vector<Vertex> vertexs;
	Vertex newVertex;

	newVertex.position = Vector4::NewVector4(40, 20, 60, 1);
	vertexs.push_back(newVertex);
	newVertex.position = Vector4::NewVector4(20, 10, 60, 1);
	vertexs.push_back(newVertex);
	newVertex.position = Vector4::NewVector4(30, 30, 60, 1);
	vertexs.push_back(newVertex);

	for (int i = 0; i < 1; ++i) {
		RenderManager::GetInstance()->clippedIndexBuffer.push_back(0);
		RenderManager::GetInstance()->clippedIndexBuffer.push_back(1);
		RenderManager::GetInstance()->clippedIndexBuffer.push_back(2);
	}
	RenderManager::GetInstance()->SetInVertexBuffer(vertexs.data(), vertexs.size());
	RenderManager::GetInstance()->DoVertexShader();
	RenderManager::GetInstance()->DoRasterization();

	int pixelNum = RenderManager::GetInstance()->PixelBuffer.size();
	for (int i = 0; i < pixelNum; ++i) {
		cout << RenderManager::GetInstance()->PixelBuffer[i]->position;
	}
}

void clip_unit()
{
	vector<Vertex> vertexs;
	Vertex newVertex;

	newVertex.position = Vector4::NewVector4(10, 0, 60, 1);
	vertexs.push_back(newVertex);
	newVertex.position = Vector4::NewVector4(0, 10, 60, 1);
	vertexs.push_back(newVertex);
	newVertex.position = Vector4::NewVector4(-10, 0, 60, 1);
	vertexs.push_back(newVertex);

	vector<int> ints{0, 1, 2};
	for (int i = 0; i < 100; ++i) {
		ints.push_back(0);
		ints.push_back(1);
		ints.push_back(2);
	}

	RenderManager::GetInstance()->SetInVertexBuffer(vertexs.data(), vertexs.size());
	RenderManager::GetInstance()->SetIndexBuffer(ints.data(),ints.size());
	RenderManager::GetInstance()->BeginRender();

	int clippedIndexNum = RenderManager::GetInstance()->clippedIndexBuffer.size();
	for (int i = 0; i < clippedIndexNum; ++i) {
		int verIndex = RenderManager::GetInstance()->clippedIndexBuffer[i];
		Vertex * pOutVertex = reinterpret_cast<Vertex *>(
			RenderManager::GetInstance()->outVertexBuffer[verIndex]);
		Vector4 pos = pOutVertex->position;
		pos.y *= 3.0f/4.0f;
		//cout << verIndex << endl;
		//cout << pos << endl;
	}
}

void input_buffer_unit()
{
	vector<Vertex> vertexs;
	int vertexNum = 6;
	for (int i = 0; i < vertexNum; ++i) {
		Vertex newVertex;
		newVertex.position = Vector4::NewVector4(i, i, i, i);
		vertexs.push_back(newVertex);
	}
	RenderManager::GetInstance()->SetInVertexBuffer(vertexs.data(), vertexs.size());
	RenderManager::GetInstance()->BeginRender();

	for (int i = 0; i < 30; ++i) {
		Vertex newVertex;
		newVertex.position = Vector4::NewVector4(i, i, i, i);
		RenderManager::GetInstance()->outVertexBuffer.push_back(&newVertex);
	}

	for (int i = 0; i < RenderManager::GetInstance()->outVertexBuffer.size(); ++i) {
		Vertex * pOutVertex = reinterpret_cast<Vertex *>(
			RenderManager::GetInstance()->outVertexBuffer[i]);
		cout << pOutVertex->position << endl;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	MainWindow mainWindow(hInstance);
	if (!mainWindow.Init())
		return 0;

	return mainWindow.Run();
}