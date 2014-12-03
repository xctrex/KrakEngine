/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: VQS.cpp
Purpose: Quaternion and VQS classes for interpolation and animation
Language: C++, MSC
Platform: Windows
Project: CS 560
Author: Tommy Walton
Creation date: 10/13/2014
- End Header -----------------------------------------------------*/
#pragma once

#include "VQS.h"

namespace KrakEngine{

    Quaternion::Quaternion() :
        v(0.0f, 0.0f, 0.0f),
        s(1.0f) {}

    Quaternion::Quaternion(const Quaternion &q) :
        v(q.v),
        s(q.s) {}

    Quaternion::Quaternion(const XMFLOAT4 &xm) :
        v(xm.x, xm.y, xm.z),
        s(xm.w) {}

    Quaternion Quaternion::operator +(const Quaternion &q2) const{
        Quaternion result;
        result.s = s + q2.s;
        result.v = v + q2.v;
        return result;
    }

    Quaternion Quaternion::operator *(const Quaternion &q2) const{
        Quaternion result;
        result.s = s * q2.s - Dot(v, q2.v);
        result.v = s * q2.v + q2.s * v + Cross(v, q2.v);
        return result;
    }

    Quaternion Quaternion::operator *(const float c) const{
        Quaternion result;
        result.s = c * s;
        result.v = c * v;
        return result;
    }

    Quaternion operator *(const float c, const Quaternion& q1){
        return q1 * c;
    }

    Quaternion& Quaternion::operator =(const Quaternion &q){
        this->s = q.s;
        this->v = q.v;
        return *this;
    }

    Quaternion& Quaternion::operator =(const XMFLOAT4 &xm){
        this->v.x = xm.x;
        this->v.y = xm.y;
        this->v.z = xm.z;
        this->s = xm.w;
        return *this;
    }

    Quaternion Quaternion::Inverse() const{
        Quaternion result;
        result.s = s / (s * s + v.x * v.x + v.y * v.y + v.z * v.z);
        result.v = v / (s * s + v.x * v.x + v.y * v.y + v.z * v.z);
        return result;
    }

    // Returns the quaternion as an XMFLOAT4 - for compatability/interoperability with the original Chris Peters binary format code
    XMFLOAT4 Quaternion::GetXMFloat4() const{
        XMFLOAT4 result;
        result.x = v.x;
        result.y = v.y;
        result.z = v.z;
        result.w = s;
        return result;
    }

    // Returns the Matrix representation of the Quaternion
    XMFLOAT4X4 Quaternion::GetMatrix4x4() const{
        return XMFLOAT4X4(
            1.0f - 2.0f * (v.y * v.y + v.z * v.z), 2.0f * (v.x * v.y - s * v.z), 2.0f * (v.x * v.z + s * v.y), 0.0f,
            2.0f * (v.x * v.y + s * v.z), 1.0f - 2.0f * (v.x * v.x + v.z * v.z), 2.0f * (v.y * v.z - s * v.x), 0.0f,
            2.0f * (v.x * v.z - s * v.y), 2.0f * (v.y * v.z + s * v.x), 1.0f - 2.0f * (v.x * v.x + v.y * v.y), 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
            );
    }

    // Returns the Matrix representation of the Quaternion
    XMMATRIX Quaternion::GetMatrix() const{
        XMFLOAT4 xm(v.x, v.y, v.z, s);        
        return XMMatrixRotationQuaternion(XMLoadFloat4(&xm));
    }

    // Quaternion dot product
    float Dot(const Quaternion &q1, const Quaternion &q2){
        return q1.s * q2.s + Dot(q1.v, q2.v);
    }

    // Quaternion magnitude
    float Mag(const Quaternion &q){
        return sqrt(q.s * q.s + q.v.x * q.v.x + q.v.y * q.v.y + q.v.z * q.v.z);
    }

    // Spherical Linear Interpolation between two quaternions
    Quaternion Slerp(const Quaternion &q1, const Quaternion &q2, float t){
        if (t < 0.0f) return q1;
        if (t > 1.0f) return q2;

        Quaternion q1Adjusted = q1;
        float dot = Dot(q1, q2);
        if (dot < 0){
            dot = -dot;
            q1Adjusted.s = q1Adjusted.s * -1.0f;
            q1Adjusted.v = q1Adjusted.v * -1.0f;
        }

        // We should have two unit quaternions, so dot should be <= 1.0f
        ThrowErrorIf(dot > 1.1f, "Slerp being called on non-unit quaternions");

        float k1, k2;
        // Use slerp for most circumstances
        if (dot < 0.999f){
            // Compute the sin of the angle using the trig identity
            float alpha = acos(dot);
            float sinAlpha = sin(alpha);
            k1 = sin(alpha - t * alpha) / sinAlpha;
            k2 = sin(t * alpha) / sinAlpha;
        }
        // if dot is close to 1, use linear interpolation to avoid a divide by 0
        else{
            k1 = 1.0f - t;
            k2 = t;
        }

        return k1 * q1Adjusted + k2 * q2;
    }

    VQS::VQS() :
        v(0.0f, 0.0f, 0.0f),
        s(1.0f){}

    VQS::VQS(const VQS &vqs) :
        v(vqs.v),
        q(vqs.q),
        s(vqs.s){}

    VQS::VQS(XMFLOAT3 translate, XMFLOAT4 rotate, float scale) :
        v(translate),
        q(rotate),
        s(scale){}


    VQS& VQS::operator =(const VQS &vqs){
        this->v = vqs.v;
        this->q = vqs.q;
        this->s = vqs.s;
        return *this;
    }

    // Returns the matrix representing the concatination of the translation, rotation and scale
    XMFLOAT4X4 VQS::GetMatrix() const{
        XMFLOAT4X4 result;
        XMMATRIX m2 = XMMatrixTranslation(v.x, v.y, v.z);
        XMStoreFloat4x4(&result, XMMatrixMultiply(q.GetMatrix(), m2));
        return result;
    }

    VQS Interpolate(const VQS &vqs1, const VQS &vqs2, float t){
        VQS result;

        // Lerp for tranlsation
        XMStoreFloat3(&result.v,
            XMVectorAdd(
                XMVectorScale(XMLoadFloat3(&vqs1.v), (1.0f - t)),
                XMVectorScale(XMLoadFloat3(&vqs2.v), t)));
        
        // Slerp for rotation
        result.q = Slerp(vqs1.q, vqs2.q, t);

        // Lerp for scale
        // result.s = (1.0f - t) * vqs1.s + t * vqs2.s;

        // Elerp for scale
        result.s = pow(vqs2.s / vqs1.s, t) * vqs1.s;;

        return result;
    }
}