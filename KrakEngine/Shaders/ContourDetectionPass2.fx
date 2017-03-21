
//--------------------------------------------------------------------------------------
// Input Texture and Point Sampler
//--------------------------------------------------------------------------------------
Texture2D<float4> ContourPass1Texture : register( t0 );
SamplerState      PointSampler : register( s0 );

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

static const float2 BasePos[6] = {
    float2(-1.0f,  1.0f),
    float2( 1.0f,  1.0f),
    float2( 1.0f, -1.0f),
    float2( 1.0f, -1.0f),
    float2(-1.0f, -1.0f),
    float2(-1.0f,  1.0f),
};

static const float2 UV[6] = {
    float2(0.0f, 0.0f), // Top Left
    float2(1.0f, 0.0f), // Top Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(1.0f, 1.0f), // Bottom Right
    float2(0.0f, 1.0f), // Bottom Left
    float2(0.0f, 0.0f), // Top Left
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


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // SilhouetteThreshold
    float Kp = 0.3;
    // Edge Threshold
    float Ka = 0.4;

    // Get the silhouette gradient value at x and the surrounding 8 pixels
    //|A||B||C|
    //|D||x||E|
    //|F||G||H|    
    float left, right, top, bottom;
    left    = clamp(input.TextureUV.x - (1.0f / ScreenSize.x), 0.0f, 1.0f);
    right   = clamp(input.TextureUV.x + (1.0f / ScreenSize.x), 0.0f, 1.0f);
    top     = clamp(input.TextureUV.y + (1.0f / ScreenSize.y), 0.0f, 1.0f);
    bottom  = clamp(input.TextureUV.y - (1.0f / ScreenSize.y), 0.0f, 1.0f);
    
    // Bail early for pixels at the edge of the screen
    if(left < 0.0001f || right > 0.9999f || top < 0.0001f || bottom > 0.9999f) return float4(1.0f, 1.0f, 1.0f, 0.0f);// Not an edge

    float x, A, B, C, D, E, F, G, H, gMax, gMin;
    
    gMax = gMin = x = ContourPass1Texture.Sample(PointSampler, input.TextureUV.xy).w;
    
    A = ContourPass1Texture.Sample(PointSampler, float2(left, top)).w;
    B = ContourPass1Texture.Sample(PointSampler, float2(input.TextureUV.x, top)).w;
    C = ContourPass1Texture.Sample(PointSampler, float2(right, top)).w;
    
    D = ContourPass1Texture.Sample(PointSampler, float2(left, input.TextureUV.y)).w;
    E = ContourPass1Texture.Sample(PointSampler, float2(right, input.TextureUV.y)).w;
    
    F = ContourPass1Texture.Sample(PointSampler, float2(left, bottom)).w;
    G = ContourPass1Texture.Sample(PointSampler, float2(input.TextureUV.x, bottom)).w;
    H = ContourPass1Texture.Sample(PointSampler, float2(right, bottom)).w;
    
    gMax = (A > gMax) ? A : gMax;
    gMin = (A < gMin) ? A : gMin;
    
    gMax = (B > gMax) ? B : gMax;
    gMin = (B < gMin) ? B : gMin;
    
    gMax = (C > gMax) ? C : gMax;
    gMin = (C < gMin) ? C : gMin;
    
    gMax = (D > gMax) ? D : gMax;
    gMin = (D < gMin) ? D : gMin;
    
    gMax = (E > gMax) ? E : gMax;
    gMin = (E < gMin) ? E : gMin;
    
    gMax = (F > gMax) ? F : gMax;
    gMin = (F < gMin) ? F : gMin;
    
    gMax = (G > gMax) ? G : gMax;
    gMin = (G < gMin) ? G : gMin;
    
    gMax = (H > gMax) ? H : gMax;
    gMin = (H < gMin) ? H : gMin;
    
    gMin = max(gMin, 0);
    
    // Calculate the gradient value at x
    float p = min(pow((gMax - gMin)/Kp, 2), 1.0);
    
    
    // Get the edge gradient value at x and the surrounding 8 pixels
    float xN, AN, BN, CN, DN, EN, FN, GN, HN, gNMax, gNMin;
    
    gNMax = gNMin = xN = ContourPass1Texture.Sample(PointSampler, input.TextureUV.xy).z;
    
    AN = ContourPass1Texture.Sample(PointSampler, float2(left, top)).z;
    BN = ContourPass1Texture.Sample(PointSampler, float2(input.TextureUV.x, top)).z;
    CN = ContourPass1Texture.Sample(PointSampler, float2(right, top)).z;
    
    DN = ContourPass1Texture.Sample(PointSampler, float2(left, input.TextureUV.y)).z;
    EN = ContourPass1Texture.Sample(PointSampler, float2(right, input.TextureUV.y)).z;
    
    FN = ContourPass1Texture.Sample(PointSampler, float2(left, bottom)).z;
    GN = ContourPass1Texture.Sample(PointSampler, float2(input.TextureUV.x, bottom)).z;
    HN = ContourPass1Texture.Sample(PointSampler, float2(right, bottom)).z;
    
    gNMax = (AN > gNMax) ? AN : gNMax;
    gNMin = (AN < gNMin) ? AN : gNMin;
    
    gNMax = (BN > gNMax) ? BN : gNMax;
    gNMin = (BN < gNMin) ? BN : gNMin;
    
    gNMax = (CN > gNMax) ? CN : gNMax;
    gNMin = (CN < gNMin) ? CN : gNMin;
    
    gNMax = (DN > gNMax) ? DN : gNMax;
    gNMin = (DN < gNMin) ? DN : gNMin;
    
    gNMax = (EN > gNMax) ? EN : gNMax;
    gNMin = (EN < gNMin) ? EN : gNMin;
    
    gNMax = (FN > gNMax) ? FN : gNMax;
    gNMin = (FN < gNMin) ? FN : gNMin;
    
    gNMax = (GN > gNMax) ? GN : gNMax;
    gNMin = (GN < gNMin) ? GN : gNMin;
    
    gNMax = (HN > gNMax) ? HN : gNMax;
    gNMin = (HN < gNMin) ? HN : gNMin;
    
    gNMin = max(gNMin, 0);
    
    // Calculate the gradient value at x
    float pN = min(pow((gNMax - gNMin)/Ka, 2), 1.0);
    
    // 0.0 if edge, 1.0 if not
    float edge = 1.0 - max(p, pN);
    //p = (p > 1) ? 0 : 1;
    // Opaque black if this pixel is an edge, transparent white if not
    return float4(edge, edge, edge, 1-edge);
}
