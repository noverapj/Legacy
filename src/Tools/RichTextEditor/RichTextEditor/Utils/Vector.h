//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2012.5.1
// 이름		: Vector
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "math.h"

//-------------------------------------------------------------
/*
2개의 멤버를 가진 Vector
*/
template < typename T >
class Vector2
{
public:
	
	//-------------------------------------------------------------
	/*
	생성,파괴
	*/
	Vector2() : x( 0 ), y( 0 ) {}
	Vector2(T iV) : x(iV), y(iV) {}
	Vector2( T iX, T iY ) : x( iX ), y( iY ) {}
	Vector2( const Vector2<T>& r ) : x( r.x ), y( r.y ) {}
	~Vector2() {}

	//-------------------------------------------------------------
	/*
	연사자들
	*/
	const Vector2<T>&				operator=(const Vector2<T>& r) { x = r.x; y = r.y; return *this; }

	bool							operator==(const Vector2<T>& r) const { if ((x != r.x) || (y != r.y)) return false; return true; }
	bool							operator!=(const Vector2<T>& r) const { if ((x == r.x) && (y == r.y)) return true; return false; }
	
	Vector2<T>						operator+(const Vector2<T>& r) const { return Vector2<T>(x + r.x, y + r.y); }
	Vector2<T>						operator-(const Vector2<T>& r) const { return Vector2<T>(x - r.x, y - r.y); }		
	Vector2<T>						operator*(T i) const { return Vector2<T>(x * i, y * i); }
	Vector2<T>						operator/(T i) const { if (i == 0) return Vector2<T>(); return Vector2<T>(x / i, y / i); }

	void							operator+=(const Vector2<T>& r) { x += r.x; y += r.y; }
	void							operator-=(const Vector2<T>& r) { x -= r.x; y -= r.y; }
	void							operator*=(T i) { x *= i; y *= i; }
	void							operator/=(T i) { if (i == 0) { x = 0; y = 0; return; } x /= i; y /= i; }

	//-------------------------------------------------------------
	/*
	기타 제공 함수들
	*/
	void							Normalize() { *this /= Length(); }
	Vector2<T>						GetUnitVec() const { return *this / Length(); }
	float							Length() const { return sqrtf(LengthSq()); }
	float							LengthSq() const { return x * x + y * y; }
	double							LengthSqD() const { return (double)x * x + (double)y * y; }		
	float							Dot(const Vector2<T>& r) const { return (x * r.x + y * r.y); }
	float							Dot2(const Vector2<T>& r) const { return Dot(r)/Length(); }
	float							Radian() const { return atan2( y, x ); } // -PI ~ PI 까지
	float							Radian2() const { float ret = Radian(); if ( ret < 0.0f ) { return 2.0f * 3.141593f + ret; } return ret; } // 0 ~ ( 2 * PI )
	void							Rotate( float radian ) { float len = Length(); x = len * cosf(radian); y = len * sinf(radian); }
	void							Rotate2( float radian ) { radian += Radian2(); float len = Length(); x = len * cosf(radian); y = len * sinf(radian); }

	//-------------------------------------------------------------
	/*
	편의 정의
	*/
	static const Vector2<T>&		Zero() { static Vector2<T> zero( 0, 0 ); return zero; }

public:

	//-------------------------------------------------------------
	/*
	멤버
	*/
	T	x, y;
};

//-------------------------------------------------------------
/*
3개의 멤버를 가진 Vector
*/
template <typename T>
class Vector3
{
public:

	//-------------------------------------------------------------
	/*
	생성,파괴
	*/
	Vector3() : x(0), y(0), z(0) {}
	Vector3(T iV) : x(iV), y(iV), z(iV) {}
	Vector3(T iX, T iY, T iZ) : x(iX), y(iY), z(iZ) {}
	Vector3(const Vector3<T>& r) : x(r.x), y(r.y), z(r.z) {}
	~Vector3() {}

	//-------------------------------------------------------------
	/*
	연사자들
	*/
	const Vector3<T>&				operator=(const Vector3<T>& r) { x = r.x; y = r.y; z = r.z; return *this; }

	bool							operator==(const Vector3<T>& r) const { if ((x != r.x) || (y != r.y) || (z != r.z)) return false; return true; }
	bool							operator!=(const Vector3<T>& r) const { if ((x == r.x) && (y == r.y) && (z == r.z)) return true; return false; }
		
	Vector3<T>						operator+(const Vector3<T>& r) const { return Vector3<T>(x + r.x, y + r.y, z + r.z); }
	Vector3<T>						operator-(const Vector3<T>& r) const { return Vector3<T>(x - r.x, y - r.y, z - r.z); }		
	Vector3<T>						operator*(T i) const { return Vector3<T>(x * i, y * i, z * i); }
	Vector3<T>						operator/(T i) const { if (i == 0) return Vector3<T>(); return Vector3<T>(x / i, y / i, z / i); }

	void							operator+=(const Vector3<T>& r) { x += r.x; y += r.y; z += r.z; }
	void							operator-=(const Vector3<T>& r) { x -= r.x; y -= r.y; z -= r.z; }
	void							operator*=(T i) { x *= i; y *= i; z *= i; }
	void							operator/=(T i) { if (i == 0) { x = 0; y = 0; z = 0; return; } x /= i; y /= i; z /= i; }

	//-------------------------------------------------------------
	/*
	기타 제공 함수들
	*/
	void							Normalize() { *this /= Length(); }
	Vector3<T>						GetUnitVec() const { return *this / Length(); }
	float							Length() const { return sqrtf(LengthSq()); }
	float							LengthSq() const { return x * x + y * y + z * z; }
	double							LengthSqD() const { return (double)x * x + (double)y * y + (double)z * z; }
	float							Dot(const Vector3<T>& r) const { return (x * r.x + y * r.y + z * r.z); }
	float							Dot2(const Vector3<T>& r) const { return Dot(r)/Length(); }
	Vector3<T>						Cross(const Vector3<T>& r) const { return Vector3<T>(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x); }

	//-------------------------------------------------------------
	/*
	편의 정의
	*/
	static const Vector3<T>&		Zero() { static Vector3<T> zero( 0, 0, 0 ); return zero; }

public:

	//-------------------------------------------------------------
	/*
	멤버
	*/
	T	x, y, z;
};

//-------------------------------------------------------------
/*
편의를 위한 Type 정의
*/
typedef Vector3<float> Vector;
typedef Vector2<float> Vector2D;	

//-------------------------------------------------------------
/*
편의를 위한 define
*/
#define vec2_zero Vector2D::Zero()
#define vec3_zero Vector::Zero()


#endif //__VECTOR_H__
