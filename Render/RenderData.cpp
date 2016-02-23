#include "RenderData.h"

using namespace std;

RenderData::~RenderData()
{ }
void RenderData::setValueByLI(const RenderData * B, const double k)
{
	this->position.SetValueByInterpolation(B->position, k);
	this->color.SetValueByInterpolation(B->color, k);
}

Buffer::Buffer(int inNumber, size_t inStride)
{
	resize(inNumber, inStride);
}

Buffer::~Buffer()
{
	clear();
}

void Buffer::clear()
{
	number = stride = 0;
	pMemAddr = nullptr;
	charVec.clear();
}

void Buffer::resize(int newNumber)
{
	max_number = number = newNumber;
	charVec.resize(number * stride);
	pMemAddr = charVec.data();
}
void Buffer::resize(int newNumber, int newStride)
{
	stride = newStride;
	resize(newNumber);
}

void Buffer::push_back(const void * pObj)
{
	if (number == max_number) {
		vector<char> newCharVec;
		if (max_number == 0) {
			max_number = 1;
			newCharVec.resize(stride);
		} else {
			max_number *= 2;
			newCharVec.resize(max_number * stride);
			memcpy(newCharVec.data(), charVec.data(), number * stride);
		}
		memcpy(newCharVec.data() + number * stride, pObj, stride);
		charVec = newCharVec;
		pMemAddr = charVec.data();
	} else {
		memcpy(pMemAddr + number * stride, pObj, stride);
	}
	number += 1;
}

void Buffer::pop_back(int popNum)
{
	number -= popNum;
}

RenderData * Buffer::back()
{
	return reinterpret_cast<RenderData *>(pMemAddr + (number-1) * stride);
}

RenderData * Buffer::operator[](const int sub)
{
	return reinterpret_cast<RenderData *>(pMemAddr + sub * stride);
}
const RenderData * Buffer::operator[](const int sub) const
{
	return reinterpret_cast<const RenderData *>(pMemAddr + sub * stride);
}

void Buffer::SetValue(const RenderData * pSrc, const int sub)
{
	memcpy(pMemAddr + sub * stride, pSrc, stride);
}