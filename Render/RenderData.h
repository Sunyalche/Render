#pragma once
#include "Util.h"
#include "RenderMath.h"
typedef struct RenderData Vertex;
typedef struct RenderData PixelData;
struct RenderData
{
	RenderData() : position(RenderMath::Vector4::ZeroVector()),
			   color(RenderMath::Vector4::ZeroVector()) { }
	RenderData(RenderMath::Vector4 inPosition, RenderMath::Color inColor) 
		: position(inPosition), color(inColor) { }
	~RenderData();

	void setValueByLI(const RenderData * B, const double k);

	RenderMath::Vector4 position;
	RenderMath::Color color;
};

struct Plane
{
	Plane() : normal(RenderMath::Vector4::ZeroVector()), d(0.0f) { }
	RenderMath::Vector4 normal;
	double d;
};

struct Buffer
{
	Buffer() : max_number(0), number(0), stride(0), pMemAddr(nullptr) { };
	Buffer(int inNumber, size_t inStride);
	~Buffer();

	void clear();
	void resize(int newNumber);
	void resize(int newNumber, int newStride);
	void push_back(const void * pObj);
	void pop_back(int popNum = 1);
	char * data() { return pMemAddr; };
	const char * data() const { return pMemAddr; }
	int size() const { return number; };
	int capacity() const { return max_number; };
	RenderData * back() ;

	size_t GetStride() const { return stride; };
	void SetValue(const RenderData * pSrc, const int sub);

	RenderData * operator[](const int sub);
	const RenderData * operator[](const int sub) const;

	int max_number;
	int number;
	size_t stride;
	std::vector<char> charVec;
	char * pMemAddr;
};


