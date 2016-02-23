#include "RenderMath.h"
namespace RenderMath
{
	Vector4 Vector4::NewVector4(double v0, double v1, double v2, double v3)
	{
		Vector4 result;
		result.v0 = v0;
		result.v1 = v1;
		result.v2 = v2;
		result.v3 = v3;
		return result;
	}

	Vector4 Vector4::operator+(const Vector4 & vector4) const
	{
		Vector4 result;
		result.v0 = v0 + vector4.v0;
		result.v1 = v1 + vector4.v1;
		result.v2 = v2 + vector4.v2;
		result.v3 = v3 + vector4.v3;
		return result;
	}
	Vector4 Vector4::operator-(const Vector4 & vector4) const
	{
		Vector4 result;
		result.v0 = v0 - vector4.v0;
		result.v1 = v1 - vector4.v1;
		result.v2 = v2 - vector4.v2;
		result.v3 = v3 - vector4.v3;
		return result;
	}
	Vector4 & Vector4::operator+=(const Vector4 & vector4)
	{
		v0 += vector4.v0;
		v1 += vector4.v1;
		v2 += vector4.v2;
		v3 += vector4.v3;
		return *this;
	}
	Vector4 & Vector4::operator-=(const Vector4 & vector4)
	{
		v0 -= vector4.v0;
		v1 -= vector4.v1;
		v2 -= vector4.v2;
		v3 -= vector4.v3;
		return *this;
	}
	Vector4 operator*(const double k, const Vector4 vector4)
	{
		Vector4 result;
		result.v0 = k * vector4.v0;
		result.v1 = k * vector4.v1;
		result.v2 = k * vector4.v2;
		result.v3 = k * vector4.v3;
		return result;
	}
	Vector4 operator/(const double k, const Vector4 vector4)
	{
		Vector4 result;
		assert(k != 0);
		result.v0 = vector4.v0 / k;
		result.v1 = vector4.v1 / k;
		result.v2 = vector4.v2 / k;
		result.v3 = vector4.v3 / k;
		return result;
	}
	Vector4 & Vector4::operator*=(const double k)
	{
		v0 *= k;
		v1 *= k;
		v2 += k;
		v3 += k;
		return *this;
	}
	Vector4 & Vector4::operator/=(const double k)
	{
		v0 /= k;
		v1 /= k;
		v2 /= k;
		v3 /= k;
		return *this;
	}
	double Vector4::DotProduct(const Vector4 & vector4) const
	{
		return v0 * vector4.v0 + v1 * vector4.v1 
			+ v2 * vector4.v2 + v3 * vector4.v3;
	}
	double Vector4::GetLength() const
	{
		return sqrt(v0 * v0 + v1 * v1 + v2 * v2 + v3 * v3);
	}
	void Vector4::Normalize(double v0, double v1, double v2, double v3)
	{
		this->v0 = v0;
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		Normalize();
	}
	void Vector4::Normalize()
	{
		*this /= GetLength();
	}
	void Vector4::SetValueByInterpolation(const Vector4 & B, const double k)
	{
		v0 += k*(B.v0 - v0);
		v1 += k*(B.v1 - v1);
		v2 += k*(B.v2 - v2);
		v3 += k*(B.v3 - v3);
	}

	std::string Vector4::ToString() const
	{
		char buff[1024];
		sprintf_s(buff, "(%6.2f, %6.2f, %6.2f, %6.2f)", v0, v1, v2, v3);
		return std::string(buff);
	}
	std::ostream & operator<<(std::ostream & os, const Vector4 & vector4)
	{
		os << vector4.ToString().data() << std::endl;
		return os;
	}

	Matrix4X4::Matrix4X4(
		const double m00, const double m01, const double m02, const double m03,
		const double m10, const double m11, const double m12, const double m13,
		const double m20, const double m21, const double m22, const double m23,
		const double m30, const double m31, const double m32, const double m33
		)
	{
		this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
		this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
		this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
		this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
	}
	Matrix4X4 Matrix4X4::operator+(const Matrix4X4 & matrix4X4) const
	{
		Matrix4X4 result(
			r0 + matrix4X4.r0,
			r1 + matrix4X4.r1,
			r2 + matrix4X4.r2,
			r3 + matrix4X4.r3
			);
		return result;
	}
	Matrix4X4 Matrix4X4::operator-(const Matrix4X4 & matrix4X4) const
	{
		Matrix4X4 result(
			r0 - matrix4X4.r0,
			r1 - matrix4X4.r1,
			r2 - matrix4X4.r2,
			r3 - matrix4X4.r3
			);
		return result;
	}
	Matrix4X4 & Matrix4X4::operator+=(const Matrix4X4 & matrix4X4)
	{
		r0 += matrix4X4.r0;
		r1 += matrix4X4.r1;
		r2 += matrix4X4.r2;
		r3 += matrix4X4.r3;
		return *this;
	}
	Matrix4X4 & Matrix4X4::operator-=(const Matrix4X4 & matrix4X4)
	{
		r0 -= matrix4X4.r0;
		r1 -= matrix4X4.r1;
		r2 -= matrix4X4.r2;
		r3 -= matrix4X4.r3;
		return *this;
	}
	Matrix4X4 Matrix4X4::operator*(const Matrix4X4 & matrix4X4) const
	{
		const Matrix4X4 & a = *this;
		const Matrix4X4 & b = matrix4X4;
		Matrix4X4 result (
			a.m00*b.m00 + a.m01*b.m10 + a.m02*b.m20 + a.m03*b.m30,
			a.m00*b.m01 + a.m01*b.m11 + a.m02*b.m21 + a.m03*b.m31,
			a.m00*b.m02 + a.m01*b.m12 + a.m02*b.m22 + a.m03*b.m32,
			a.m00*b.m03 + a.m01*b.m13 + a.m02*b.m23 + a.m03*b.m33,
			a.m10*b.m00 + a.m11*b.m10 + a.m12*b.m20 + a.m13*b.m30,
			a.m10*b.m01 + a.m11*b.m11 + a.m12*b.m21 + a.m13*b.m31,
			a.m10*b.m02 + a.m11*b.m12 + a.m12*b.m22 + a.m13*b.m32,
			a.m10*b.m03 + a.m11*b.m13 + a.m12*b.m23 + a.m13*b.m33,
			a.m20*b.m00 + a.m21*b.m10 + a.m22*b.m20 + a.m23*b.m30,
			a.m20*b.m01 + a.m21*b.m11 + a.m22*b.m21 + a.m23*b.m31,
			a.m20*b.m02 + a.m21*b.m12 + a.m22*b.m22 + a.m23*b.m32,
			a.m20*b.m03 + a.m21*b.m13 + a.m22*b.m23 + a.m23*b.m33,
			a.m30*b.m00 + a.m31*b.m10 + a.m32*b.m20 + a.m33*b.m30,
			a.m30*b.m01 + a.m31*b.m11 + a.m32*b.m21 + a.m33*b.m31,
			a.m30*b.m02 + a.m31*b.m12 + a.m32*b.m22 + a.m33*b.m32,
			a.m30*b.m03 + a.m31*b.m13 + a.m32*b.m23 + a.m33*b.m33
			);
		return result;
	}
	Matrix4X4 & Matrix4X4::operator *= (const Matrix4X4 & matrix4X4)
	{
		Matrix4X4 result = *this * matrix4X4;
		return *this = result;
	}
	Matrix4X4 operator*(const double k, const Matrix4X4 & matrix4X4)
	{
		Matrix4X4 result(
			k * matrix4X4.r0,
			k * matrix4X4.r1,
			k * matrix4X4.r2,
			k * matrix4X4.r3
		);
		return result;
	}
	Vector4 operator*(const Vector4 & vector4, const Matrix4X4 & matrix4X4)
	{
		Vector4 result;
		const Vector4 & a = vector4;
		const Matrix4X4 & b = matrix4X4;
		result.v0 = a.v0*b.m00 + a.v1*b.m10 + a.v2*b.m20 + a.v3*b.m30;
		result.v1 = a.v0*b.m01 + a.v1*b.m11 + a.v2*b.m21 + a.v3*b.m31;
		result.v2 = a.v0*b.m02 + a.v1*b.m12 + a.v2*b.m22 + a.v3*b.m32;
		result.v3 = a.v0*b.m03 + a.v1*b.m13 + a.v2*b.m23 + a.v3*b.m33;
		return result;
	}
	Vector4 operator*(const Matrix4X4 & matrix4X4, const Vector4 & vector4)
	{
		Vector4 result;
		const Matrix4X4 & a = matrix4X4;
		const Vector4 & b = vector4;
		result.v0 = a.m00*b.v0 + a.m01*b.v1 + a.m02*b.v2 + a.m03*b.v3;
		result.v1 = a.m10*b.v0 + a.m11*b.v1 + a.m12*b.v2 + a.m13*b.v3;
		result.v2 = a.m20*b.v0 + a.m21*b.v1 + a.m22*b.v2 + a.m23*b.v3;
		result.v3 = a.m30*b.v0 + a.m31*b.v1 + a.m32*b.v2 + a.m33*b.v3;
		return result;
	}
	Vector4 operator*=(Vector4 & vector4, const Matrix4X4 & matrix4X4)
	{
		Vector4 result;
		const Vector4 & a = vector4;
		const Matrix4X4 & b = matrix4X4;
		result.v0 = a.v0*b.m00 + a.v1*b.m10 + a.v2*b.m20 + a.v3*b.m30;
		result.v1 = a.v0*b.m01 + a.v1*b.m11 + a.v2*b.m21 + a.v3*b.m31;
		result.v2 = a.v0*b.m02 + a.v1*b.m12 + a.v2*b.m22 + a.v3*b.m32;
		result.v3 = a.v0*b.m03 + a.v1*b.m13 + a.v2*b.m23 + a.v3*b.m33;
		vector4 = result;
		return vector4;
	}
	std::string Matrix4X4::ToString() const
	{
		std::string str("{ ");
		str += r0.ToString(); str += ",\n  ";
		str += r1.ToString(); str += ",\n  ";
		str += r2.ToString(); str += ",\n  ";
		str += r3.ToString(); str += " }\n";
		return str;
	}
	std::ostream & operator << (std::ostream & os, const Matrix4X4 & matrix4X4)
	{
		os << matrix4X4.ToString().data() << std::endl;
		return os;
	}
} // end of namespace RenderMath