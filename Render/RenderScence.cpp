#include "RenderScence.h"

using namespace RenderMath;

RenderScence::RenderScence()
{ 
	GenerateObject();
	SetConstBuffer();
	SetVertexShader();
}


RenderScence::~RenderScence()
{ }
void RenderScence::GenerateObject()
{
	const Vertex cubeVertices[ ] =
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
	RenderManager::GetInstance()->SetInVertexBuffer(
		cubeVertices, sizeof(cubeVertices) / sizeof(Vertex));
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
}

void RenderScence::SetConstBuffer()
{
	Vector4 cameraPos = Vector4::NewVector4(1.0f, 1.0f, 2.0f);
	Vector4 targetPos = Vector4::NewVector4(0.0f, 0.0f, 0.0f);
	Vector4 upDir = Vector4::NewVector4(0.0f, 1.0f, 0.0f, 0.0f);
	Matrix4X4 cameraMatrix = RenderMath::GetCameraMatrix(cameraPos, targetPos, upDir);
	Matrix4X4 clipMatrix = RenderManager::GetInstance()->GetClipMatrix();
	constBuffer.modelToClip = cameraMatrix * clipMatrix;
	RenderManager::GetInstance()->SetConstBuffer(&constBuffer);
}
void RenderScence::SetVertexShader()
{
	auto lambda = [](const Vertex * inVertex, Vertex * outVertex) {
		memcpy(outVertex, inVertex, sizeof(Vertex));
		outVertex->position *= RenderManager::GetInstance()->pConstBuffer->modelToClip;
	};
	RenderManager::GetInstance()->SetVertexShader(lambda, sizeof(Vertex));
}