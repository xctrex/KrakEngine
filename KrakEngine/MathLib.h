#pragma once
#include "Precompiled.h"
#define PI 3.14159265

namespace KrakEngine{
	typedef XMFLOAT2   Vector2;
	typedef XMFLOAT3   Vector3;
	typedef XMFLOAT4X4 Matrix4;

	inline Vector2 operator+(const Vector2 &pVec0, const Vector2 &pVec1)
	{
		Vector2 result;
		result.x = pVec0.x + pVec1.x;
		result.y = pVec0.y + pVec1.y;
		return result;
	}

	inline Vector2 operator-(const Vector2 &pVec0, const Vector2 &pVec1)
	{
		Vector2 result;
		result.x = pVec0.x - pVec1.x;
		result.y = pVec0.y - pVec1.y;
		return result;
	}

	inline Vector2 operator*(const Vector2 &pVec0, float c)
	{
		Vector2 result;
		result.x = pVec0.x * c;
		result.y = pVec0.y * c;
		return result;
	}
    
	inline float Dot(const Vector2& a, const Vector2& b)
	{
		return a.x * b.x + a.y * b.y;
	}

    inline float Mag(const Vector2& a)
    {
        return sqrt(a.x * a.x + a.y * a.y);
    }

    inline float AngleInRadians(const Vector2& a, const Vector2& b)
    {
        //return acosf(Dot(a, b) / (Mag(a) * Mag(b)));
        float result;
        float dot = Dot(a, b);
        float det = a.x * b.y - a.y * b.x;
        result = atan2(det, dot);
        return result;
    }

    inline float AngleInDegrees(const Vector2& a, const Vector2& b)
    {
        return AngleInRadians(a,b) * 180.f / PI;
    }

	inline Vector3 operator+(const Vector3 &pVec0, const float &pfloat)
	{
		Vector3 result;
		result.x = pVec0.x + pfloat;
		result.y = pVec0.y + pfloat;
		result.z = pVec0.z + pfloat;
		return result;
	}

	inline Vector3 operator+(const Vector3 &pVec0, const Vector3 &pVec1)
	{
		Vector3 result;
		result.x = pVec0.x + pVec1.x;
		result.y = pVec0.y + pVec1.y;
		result.z = pVec0.z + pVec1.z;
		return result;
	}

	inline Vector3 operator-(const Vector3 &pVec0, const Vector3 &pVec1)
	{
		Vector3 result;
		result.x = pVec0.x - pVec1.x;
		result.y = pVec0.y - pVec1.y;
		result.z = pVec0.z - pVec1.z;
		return result;
	}

	inline Vector3 operator*(const Vector3 &pVec0, float c)
	{
		Vector3 result;
		result.x = pVec0.x * c;
		result.y = pVec0.y * c;
		result.z = pVec0.z * c;
		return result;
	}
	
	inline Vector3 operator*(float c,const Vector3 &pVec0)
	{
		Vector3 result;
		result.x = pVec0.x * c;
		result.y = pVec0.y * c;
		result.z = pVec0.z * c;
		return result;
	}
	
    inline Vector3 operator/(const Vector3 &pVec0, float c)
	{
        float oneOverC = 1.0f / c; // Note: no check for divide by zero here
        return Vector3(pVec0.x * oneOverC, pVec0.y * oneOverC, pVec0.z * oneOverC);
	}
	
	inline float Dot(const Vector3& a, const Vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

    inline Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        Vector3 result;
        result.x = a.y * b.z - a.z * b.y;
        result.y = a.z * b.x - a.x * b.z;
        result.z = a.x * b.y - a.y * b.x;
        return result;
    }


    inline float Mag(const Vector3& a)
    {
        return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
    }

    inline float AngleInRadians(const Vector3& a, const Vector3& b)
    {
        return acosf(Dot(a, b) / (Mag(a) * Mag(b)));
    }

    inline float AngleInDegrees(const Vector3& a, const Vector3& b)
    {
        return AngleInRadians(a, b) * 180.f / PI;
    }

	inline float Length(const Vector3& a)
	{
		return sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z));
	}

	inline Vector3 Normalize(const Vector3& a)
	{
		float length = Length(a);
		Vector3 unit;
		if (length == 0)
			return Vector3(0,0,0);
		
		unit.x = a.x / length;
		unit.y = a.y / length;
		unit.z = a.z / length;
		return unit;
	}

    /*
    This function checks if the point Pos is colliding with the rectangle
    whose top left is Rect, width is "Width" and height is Height
    */
    inline bool StaticPointToStaticRect(XMFLOAT2 &Pos, XMFLOAT2 &TopLeft, float Width, float Height){
        if (Pos.x < TopLeft.x               //P.X < left
            || Pos.x > TopLeft.x + Width    //P.X > right
            || Pos.y < TopLeft.y            //P.Y < top
            || Pos.y > TopLeft.y + Height)  //P.Y > bottom
        {
            return false;
        }
        else return true;
    }
}
