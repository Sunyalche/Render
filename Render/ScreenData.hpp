#pragma once
#include "Util.h"
#include "RenderMath.h"

template<class _ElemType>
struct Data
{
	Data(const unsigned int & inSize) : _size(inSize)
	{
		pData = new _ElemType[_size];
	};
	Data(const unsigned int & inSize, const _ElemType & data) : _size(inSize)
	{
		pData = new _ElemType[_size];
		for (unsigned int i = 0; i < _size; ++i) {
			pData[i] = data;
		}
	};
	Data(const std::vector<_ElemType> & dataVec) : _size(data.size())
	{
		pData = new _ElemType[_size];
		memcpy(pData, dataVec.data(), _size * sizeof(pData[0]));
	}
	Data(const unsigned int inSize, const std::vector<_ElemType> & dataVec) : _size(inSize)
	{
		pData = new _ElemType[_size];
		INT32 delta = _size - dataVec.size();
		if (delta <= 0) {
			memcpy(pData, dataVec.data(), _size * sizeof(pData[0]));
		} else {
			memcpy(pData, dataVec.data(), dataVec.size() * sizeof(pData[0]));
			memset(pData + dataVec.size(), 0, delta * sizeof(pData[0]));
		}
	}
	Data(const Data & B)
	{
		_size = B._size;
		pData = new _ElemType[_size];
		memcpy(pData, B.pData, _size * sizeof(pData[0]));
	}

	~Data()
	{
		delete[ ] pData;
	}

	inline _ElemType & operator[](const int & i) { return pData[i]; }
	inline const _ElemType & operator[](const int & i) const { return pData[i]; }

	void resize(const unsigned int & newSize) { _size = newSize; delete[ ] pData; pData = new _ElemType[_size]; }
	const unsigned int & size() const { return _size; }
	void InilByValue(const _ElemType & inilValue)
	{
		for (unsigned int i = 0; i < _size; ++i) {
			pData[i] = inilValue;
		}
	}
	Data & operator=(const Data & B)
	{
		_size = B._size;
		pData = new _ElemType[_size];
		memcpy(pData, B.pData, _size * sizeof(pData[0]));
		return *this;
	}

	_ElemType * pData;
	unsigned int _size;
};

template<class _ElemType>
class Matrix
{
public:
	Matrix(const unsigned int & inRow = 0, const unsigned int & inCol = 0)
		: row(inRow), col(inCol), data(inRow * inCol) { }
	Matrix(const unsigned int & inRow, const unsigned int & inCol, const std::vector<_ElemType> dataVec)
		: row(inRow), col(inCol), data(inRow * inCol, dataVec) { }
	Matrix(const Matrix & B) : row(B.row), col(B.col), data(B.data) { }
	Matrix(const unsigned int & inRow, const unsigned int & inCol, const _ElemType & inilValue)
		: row(inRow), col(inCol), data(inRow * inCol, inilValue) { }


	inline _ElemType & GetElem(const unsigned int & inRow, const unsigned int & inCol)
	{
		//assert(inRow < row && inCol < col);
		return  *(data.pData + inRow * col + inCol);
	}
	inline const _ElemType & GetElem(const unsigned int & inRow, const unsigned int & inCol) const
	{
		//assert(inRow < row && inCol < col);
		return  *(data.pData + inRow * col + inCol);
	}
	
	void resize(const unsigned int inRow, const unsigned int inCol)
	{
		row = inRow;
		col = inCol;
		data.resize(row * col);
	}
	void InilByValue(const _ElemType & inilValue)
	{
		data.InilByValue(inilValue);
	}
	void Print() const
	{
		for (unsigned int r = 0; r < row; ++r) {
			for (unsigned int c = 0; c < col; ++c)
				std::cout << GetElem(r, c) << "\t";
			std::cout << "\n";
		}
		std::cout << std::endl;
	}

	unsigned int row;
	unsigned int col;
	Data<_ElemType> data;
	//std::vector<_ElemType> data;
};

struct Pixel
{
	Pixel(COLORREF inColor = 0)
		: red(GetRValue(inColor)), green(GetGValue(inColor)), blue(GetBValue(inColor)), alpha(255) { }
	Pixel(const Pixel & B) : red(B.red), green(B.green), blue(B.blue), alpha(B.alpha) { }
	
	Pixel & operator=(const Pixel & B)
	{
		red = B.red;
		green = B.green;
		blue = B.blue;
		alpha = B.alpha;
		return *this;
	}
	inline void SetByColor(const RenderMath::Color & B)
	{
		red = static_cast<byte>(max(0.0f, min(1.0f,B.r)) * 255.0f);
		green = static_cast<byte>(max(0.0f, min(1.0f, B.g)) * 255.0f);
		blue = static_cast<byte>(max(0.0f, min(1.0f, B.b)) * 255.0f);
		alpha = static_cast<byte>(max(0.0f, min(1.0f, B.a)) * 255.0f);
	}

	COLORREF GetColor() { return RGB(red, green, blue); }

	byte blue;
	byte green;
	byte red;
	byte alpha;
};