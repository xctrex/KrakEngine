// Common

float2 get2dPoint(float3 point3D, float4x4 viewMatrix,
    float4x4 projectionMatrix, float width, float height) {

    float4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;
    //transform world to clipping coordinates
    point3D = mul(viewProjectionMatrix, point3D);
    float winX = ((point3D.x + 1.0f) / 2.0f) * width;
    //we calculate -point3D.getY() because the screen Y axis is
    //oriented top->down 
    int winY = ((1.0f - point3D.y) / 2.0) * height;
    return float2(winX, winY);
}

// Tranforms position from object to homogenous space
inline float4 UnityObjectToClipPos(in float3 pos, float4x4 MVP)
{
    return mul(MVP, float4(pos, 1.0));
}

float4 ComputeScreenPos(float4 pos) {
    float4 o = pos * 0.5f;
//#if defined(UNITY_HALF_TEXEL_OFFSET)
//    o.xy = float2(o.x, o.y*_ProjectionParams.x) + o.w * _ScreenParams.zw;

    o.xy = float2(o.x, o.y*1.0) + o.w;

    o.zw = pos.zw;
    return o;
}

// pos is the input position multiplied by the ModelViewProjection matrix
float4 ScreenSpaceInVertexShader(in float4 pos, in float2 ScreenSize)
{
    return float4(0.5 * (float2(pos.x + pos.w, pos.w - pos.y) + pos.w * ScreenSize.xy), pos.zw) * (1.0f / pos.w);
}