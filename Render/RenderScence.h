#pragma once
#include "Util.h"
#include "RenderManager.h"

class RenderScence
{
public:
	RenderScence();
	~RenderScence();

private:
	void GenerateObject();
	void SetConstBuffer();
	void SetVertexShader();

	ConstBuffer constBuffer;
};

