/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: VQS.h
Purpose: Quaternion and VQS classes for interpolation and animation
Language: C++, MSC
Platform: Windows
Project: CS 560
Author: Tommy Walton
Creation date: 10/13/2014
- End Header -----------------------------------------------------*/
#pragma once
#include "MathLib.h"

namespace KrakEngine{

    class Quaternion{
    public:
        Quaternion();
        Quaternion(const Quaternion &q);
        Quaternion(const XMFLOAT4 &xm);
        ~Quaternion(){};

        Quaternion operator +(const Quaternion &q2) const;
        Quaternion operator *(const Quaternion &q2) const;
        Quaternion operator *(const float c) const;
        Quaternion& Quaternion::operator =(const Quaternion &q);
        Quaternion& Quaternion::operator =(const XMFLOAT4 &xm);
        Quaternion Inverse() const;

        // Returns the quaternion as an XMFLOAT4 - for compatability/interoperability with the original Chris Peters binary format code
        XMFLOAT4 GetXMFloat4() const;

        // Returns the matrix representation of the Quaternion
        XMFLOAT4X4 GetMatrix4x4() const;
        XMMATRIX GetMatrix() const;

        Vector3 v;
        float s;

    private:
    };

    // Quaternion dot product
    float Dot(const Quaternion &q1, const Quaternion &q2);
    // Quaternion magnitude
    float Mag(const Quaternion &q);
    // Spherical Linear Interpolation between two quaternions
    Quaternion Slerp(const Quaternion &q1, const Quaternion &q2, float t);

    class VQS{
    public:
        VQS();
        VQS(const VQS &vqs);
        VQS(XMFLOAT3, XMFLOAT4, float);
        ~VQS(){};

        VQS& VQS::operator =(const VQS &vqs);

        Vector3 v;
        Quaternion q;
        float s;

        // Returns the matrix representing the concatination of the translation, rotation and scale
        XMFLOAT4X4 GetMatrix() const;

    private:
    };

    VQS Interpolate(const VQS &vqs1, const VQS &vqs2, float t);
}