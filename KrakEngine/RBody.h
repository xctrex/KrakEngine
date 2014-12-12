#pragma once

#include "Precompiled.h"

namespace KrakEngine
{
    struct rBody{
        // Constants
        double mass;
        XMFLOAT3X3 iBody;
        XMFLOAT3X3 iBodyInverse;

        // State variables
        XMFLOAT3 x; //x(t)
        XMFLOAT3X3 R; //R(t)
        XMFLOAT3 P; //P(t)
        XMFLOAT3 L; //L(t)

        // Derived quantities
        XMFLOAT3X3 iInv;
        XMFLOAT3 v;
        XMFLOAT3 omega;

        // Computed quantitites
        XMFLOAT3 force;
        XMFLOAT3 torque;
    };
}
