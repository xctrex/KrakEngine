#include "Common.fx"
#include "CommonGBufferInput.fx"

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBufferPerFrame : register( b0 )
{
    matrix View;
    matrix Projection;
    float4 lightPosition;
    float2 ScreenSize;
};

cbuffer ConstantBufferPerObject : register( b1 )
{
    matrix World;
    float4 Animation;
};

static const float2 BasePos[6] = {
    float2(-1.0f,  1.0f),
    float2( 1.0f,  1.0f),
    float2( 1.0f, -1.0f),
    float2( 1.0f, -1.0f),
    float2(-1.0f, -1.0f),
    float2(-1.0f,  1.0f),
};

static float2 Offsets[4] = {
    float2(-0.5f, 0.5f),
    float2( 0.5f, 0.5f),
    float2(-0.5f, -0.5f),
    float2( 0.5f, -0.5f),
};

static const float2 UV[6] = {
    float2(0.0f, 0.0f), // Top Left
    float2(1.0f, 0.0f), // Top Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(0.0f, 1.0f), // Bottom Left
    float2(0.0f, 0.0f), // Top Left
};

static const float4 Mask[4] = {
    float4(1.0f, 0.0f, 0.0f, 0.0f),
    float4(0.0f, 1.0f, 0.0f, 0.0f),
    float4(0.0f, 0.0f, 1.0f, 0.0f),
    float4(0.0f, 0.0f, 0.0f, 1.0f),
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 Pos : POSITION;
    float2 TextureUV : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos        : SV_POSITION;
    float2 TextureUV  : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{    
    VS_OUTPUT output;

    output.Pos = input.Pos;
    output.TextureUV = input.TextureUV;

    return output;
};

// returns distance t along the ray where intersection occurs
float3 intersectPlane(const float3 planePoint, const float3 planeNormal, const float3 rayOrigin, const float3 rayDirection)
{
    float t = 0.0f;
    // assuming vectors are all normalized
    float denominator = dot(planeNormal, rayDirection);
    if (denominator > FLT_EPSILON) {
        float3 planePointToRayOrigin = planePoint - rayOrigin;
        t = dot(planePointToRayOrigin, planeNormal) / denominator;
    }

    return rayOrigin + rayDirection * t;
}

float triangle2xAreaSquared(const float3 p0, const float3 p1, const float3 p2)
{
    float3 a = p1 - p0;
    float3 b = p2 - p0;
    float3 axb = cross(a, b);
    return length(axb);
}

float clamp(float minValue, float maxValue, float value)
{
    return max(min(maxValue, value), minValue);
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    SurfaceData GBufferData = UnpackGBuffer(input.TextureUV.xy);
    // Sample the surrounding two pixels
    float offsetX = 1.0f / ScreenSize.x;
    SurfaceData GBufferData1 = UnpackGBuffer(float2(input.TextureUV.x - offsetX, input.TextureUV.y));
    SurfaceData GBufferData2 = UnpackGBuffer(float2(input.TextureUV.x + offsetX, input.TextureUV.y));

    // do plane intersection to get s, t, u, and v
    // s, t, 1 = > intersection with z = 1 plane
    // u, v, 0 = > intersection with z - 0 plane
    // Point1
    float3 intersectionPoint1 = intersectPlane(GBufferData.Position, GBufferData.Normal, GBufferData1.Position, GBufferData1.Normal);
    float3 intersectionPoint2 = intersectPlane(GBufferData.Position + GBufferData.Normal, GBufferData.Normal, GBufferData1.Position, GBufferData1.Normal);
    float s1 = intersectionPoint1.x;
    float t1 = intersectionPoint1.y;
    float u1 = intersectionPoint2.x;
    float v1 = intersectionPoint2.y;

    // Point2
    intersectionPoint1 = intersectPlane(GBufferData.Position, GBufferData.Normal, GBufferData2.Position, GBufferData2.Normal);
    intersectionPoint2 = intersectPlane(GBufferData.Position + GBufferData.Normal, GBufferData.Normal, GBufferData2.Position, GBufferData2.Normal);
    float s2 = intersectionPoint1.x;
    float t2 = intersectionPoint1.y;
    float u2 = intersectionPoint2.x;
    float v2 = intersectionPoint2.y;

    // sigma = s - u
    // tau = t - v
    float sigma1 = s1 - u1;
    float tau1 = t1 - v1;
    float sigma2 = s2 - u2;
    float tau2 = t2 - v2;

    //  plane z = lambda, calculate all intersection points, look for triangle area of 0
    float bestLambda = 1.0f;
    float bestArea = 20000000000000.0f;
    for (int i = 2; i < 10; ++i)
    {
        float3 p0 = GBufferData.Position + GBufferData.Normal * (float)i;
        float3 p1 = intersectPlane(p0, GBufferData.Normal, GBufferData1.Position, GBufferData1.Normal);
        float3 p2 = intersectPlane(p0, GBufferData.Normal, GBufferData2.Position, GBufferData2.Normal);
        float area = triangle2xAreaSquared(p0, p1, p2);

        //optimization? bestLambda = lerp((float)i, bestLambda, clamp(0.0f, 1.0f, area - bestArea)
        if (area < bestArea)
        {
            bestArea = area;
            bestLambda = (float)i;
        }
    }
    //     Eq. (1)
    //     looking for A*lambdaSquared + Blambda + C = 0
    //     A = sigma1 * tau2 - sigma2 * tau1
    //     B = sigma1 * v2 + tau2*u1 - sigma2 * v1 - tau1*u2
    //     C = u1 * v2 - u2 * v1

    //     find lambda1 and lambda2, the two solutions to Eq. (1)
    //     two slit directions are
    //     [u1 + sigma1 dot lambda1, v1 + tau1 dot lambda1, 0]
    // [u1 + sigma1 dot lambda2, v1 + tau1 dot lambda2, 0]

    // substitute kappa = 1 / lambda and solve for kappa insteqad due to numerical instability
    //     alternate slit directions :
    // [u1 dot kappa1 + sigma1, v1 dot kappa1 + tau1, 0]
    // [u1 dot kappa2 + sigma1, v1 dot kappa2 + tau1, 0]


    float4 finalColor = float4(1.0, 1.0, 1.0, 1.0);
    // If the pixel is in the rect for displaying depth (as determined in the vertex shader), SampleMask.x will equal 1 here and SampleMask.yzw will all equal 0.
    // Thus, only the depth will be added to finalColor, and all other values (color, normal, specular values) will be zero after being multiplied by the mask.
    // If the pixel is in the rect for displaying color, only SampleMask.y will equal 1 and thus only color data will be added to final color.
    // The same logic goes for displaying normal and specular intensity data.
    //finalColor += float4(1.0f - saturate(GBufferData.LinearDepth / 75.0f), 1.0f - saturate(GBufferData.LinearDepth / 125.0f), 1.0f - saturate(GBufferData.LinearDepth / 200.0f), 0.0f);// * input.SampleMask.xxxx;
    //finalColor += float4(GBufferData.Color.xyz, 0.0f);// * input.SampleMask.yyyy;
    //finalColor += float4(GBufferData.Normal.xyz * 0.5f + 0.5f, 0.0f);// * input.SampleMask.zzzz;
    //finalColor += float4(GBufferData.SpecularIntensity, GBufferData.SpecularExponent, 0.0f, 0.0f);// * input.SampleMask.wwww;
    float2 start = get2dPoint(GBufferData.Position, View, Projection, ScreenSize.x, ScreenSize.y);
    float2 end = get2dPoint(GBufferData.Position + GBufferData.Normal, View, Projection, ScreenSize.x, ScreenSize.y);
    float2 xyprojection = end - start;
    xyprojection /= sqrt(xyprojection.x * xyprojection.x + xyprojection.y * xyprojection.y);
    float direction = GetRotationFromDirection(xyprojection);
    finalColor.rg = xyprojection;
    //finalColor.rgb = GBufferData.Normal.xyz;
    return finalColor;
}