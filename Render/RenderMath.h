#pragma once
#include "Util.h"
namespace RenderMath
{
	static const double FINITE = 1e100;

	typedef struct Vector4 Color;
	struct Vector4
	{
		static Vector4 NewVector4(double v0, double v1, double v2, double v3 = 1.0f);
		static Vector4 ZeroVector() { return NewVector4(0.0f, 0.0f, 0.0f, 0.0f); }

		Vector4 operator+(const Vector4 & vector4) const;
		Vector4 operator-(const Vector4 & vector4) const;
		Vector4 & operator+=(const Vector4 & vector4);
		Vector4 & operator-=(const Vector4 & vector4);

		friend Vector4 operator*(const double k, const Vector4 vector4);
		friend Vector4 operator/(const double k, const Vector4 vector4);
		Vector4 & operator*=(const double k);
		Vector4 & operator/=(const double k);

		double DotProduct(const Vector4 & vector4) const;
		double GetLength() const;
		void Normalize(double v0, double v1, double v2, double v3);
		void Normalize();
		void SetValueByInterpolation(const Vector4 & B, const double k);

		std::string ToString() const;
		friend std::ostream & operator<<(std::ostream & os, const Vector4 & vector4);


		union
		{
			double v[4];
			struct
			{
				double v0, v1, v2, v3;
			};
			struct
			{
				double x, y, z, w;
			};
			struct
			{
				double r, g, b, a;
			};
		};
	}; // end of struct Vector4

	struct Matrix4X4
	{
		Matrix4X4() { r0 = r1 = r2 = r3 = Vector4::ZeroVector(); }
		Matrix4X4(const Vector4 & r0, const Vector4 & r1, const Vector4 & r2, const Vector4 & r3)
		{ this->r0 = r0; this->r1 = r1; this->r2 = r2, this->r3 = r3; }
		Matrix4X4(
			const double m00, const double m01, const double m02, const double m03,
			const double m10, const double m11, const double m12, const double m13,
			const double m20, const double m21, const double m22, const double m23,
			const double m30, const double m31, const double m32, const double m33
			);
		Matrix4X4(const Matrix4X4 & matrix4X4)
			{ memcpy(m, matrix4X4.m, sizeof(m)); }

		Matrix4X4 & operator=(const Matrix4X4 & matrix4X4)
			{ memcpy(m, matrix4X4.m, sizeof(m)); return *this;}
		Matrix4X4 operator+(const Matrix4X4 & matrix4X4) const;
		Matrix4X4 operator-(const Matrix4X4 & matrix4X4) const;
		Matrix4X4 & operator+=(const Matrix4X4 & matrix4X4);
		Matrix4X4 & operator-=(const Matrix4X4 & matrix4X4);
		Matrix4X4 operator*(const Matrix4X4 & matrix4X4) const;
		Matrix4X4 & operator*=(const Matrix4X4 & matrix4X4);

		friend Matrix4X4 operator*(const double k, const Matrix4X4 & matrix4X4);
		friend Vector4 operator*(const Vector4 & vector4, const Matrix4X4 & matrix4X4);
		friend Vector4 operator*(const Matrix4X4 & matrix4X4, const Vector4 & vector4);
		friend Vector4 operator*=(Vector4 & vector4, const Matrix4X4 & matrix4X4);

		std::string ToString() const;
		friend std::ostream & operator<<(std::ostream & os, const Matrix4X4 & matrix4X4);


		union
		{
			Vector4 r[4];
			struct
			{
				Vector4 r0, r1, r2, r3;
			};
			double m[4][4];
			struct
			{
				double m00, m01, m02, m03;
				double m10, m11, m12, m13;
				double m20, m21, m22, m23;
				double m30, m31, m32, m33;
			};
		};
	}; // end of struct Matrix4X4

	inline double LinearInterpolation(const double A, const double B, const double k)
	{
		return A + (B - A) * k;
	}

	inline int GetApproximateInt(const double b)
	{
		int floorInt = static_cast<int>(b);
		double fractional = b - floorInt;
		if (fractional >= 0.5f) {
			return floorInt + 1;
		} else {
			return floorInt;
		}
	}
	inline double GetSlopeXY(const double Ax, const double Ay, const double Bx, const double By)
	{
		return (Ax - Bx == 0.0f) ? FINITE : (Ay - By) / (Ax - Bx);
	}
	inline double GetSlopeXY(const Vector4 & positionA, const Vector4 & positionB)
	{
		return GetSlopeXY(positionA.x, positionA.y, positionB.x, positionB.y);
	}
	inline double GetPixelCenterX(const Vector4 & position)
	{
		return 0.5f + static_cast<int>(position.x);
	}
	inline double GetPixelCenterY(const Vector4 & position)
	{
		return 0.5f + static_cast<int>(position.y);
	}
	inline double GetK(const double begin, const double mid, const double end)
	{
		return (end - begin == 0.0f)? FINITE : (mid - begin) / (end - begin);
	}
} // end of namespace RenderMath