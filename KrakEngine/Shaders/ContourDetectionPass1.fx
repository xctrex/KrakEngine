
//--------------------------------------------------------------------------------------
// GBuffer Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D<float>  DepthTexture                  : register( t0 );
Texture2D<float4> ColorSpecularIntensityTexture : register( t1 );
Texture2D<float3> NormalTexture                 : register( t2 );
//Texture2D<float4> SpecularExponentTexture       : register( t3 );
SamplerState      PointSampler                  : register( s0 );

cbuffer ConstantBufferGBufferUnpack : register( b2 )
{
    float4 PerspectiveValues : packoffset( c0 );
    float4x4 ViewInverse     : packoffset( c1 );
};


struct SurfaceData
{
    float LinearDepth;
    float3 Color;
    float3 Normal;
    float SpecularIntensity;
//    float SpecularExponent;
};

float3 DecodeNormal(float2 EncodedNormal)
{
    float4 DecodedNormal = EncodedNormal.xyyy * float4(2.0f, 2.0f, 0.0f, 0.0f) + float4(-1.0f, -1.0f, 1.0f, -1.0f);
    DecodedNormal.z = dot(DecodedNormal.xyz, -DecodedNormal.xyw);
    DecodedNormal.xy *= sqrt(DecodedNormal.z);
    return DecodedNormal.xyz * 2.0f + float3(0.0f, 0.0f, -1.0f);
}

float ConvertZToLinearDepth(float depth)
{
    return PerspectiveValues.z / (depth + PerspectiveValues.w);
}

SurfaceData UnpackGBuffer(float2 UV)
{
    SurfaceData output;

    // Get the depth from the GBuffer and convert it to linear space
    float depth = DepthTexture.Sample( PointSampler, UV.xy ).x;
    output.LinearDepth = ConvertZToLinearDepth(depth);

    // Get the Color and specular intensity from the GBuffer
    float4 ColorSpecularIntensity = ColorSpecularIntensityTexture.Sample( PointSampler, UV.xy );
    output.Color = ColorSpecularIntensity.xyz;
    output.SpecularIntensity = ColorSpecularIntensity.w;

    // Get the normal and unpack it
    output.Normal = NormalTexture.Sample( PointSampler, UV.xy ).xyz;
    output.Normal = normalize(output.Normal * 2.0f - 1.0f);
    
    // Get the specular exponent and scale it back to the original range
//    output.SpecularExponent = SpecularExponentTexture.Sample( PointSampler, UV.xy ).x;

    return output;
}

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
    // Upack the GBuffer of this pixel (x) and the surrounding 8 pixels (A-H)
    //|A||B||C|
    //|D||x||E|
    //|F||G||H|
    float left, right, top, bottom;
    left    = clamp(input.TextureUV.x - (1.0f / ScreenSize.x), 0.0f, 1.0f);
    right   = clamp(input.TextureUV.x + (1.0f / ScreenSize.x), 0.0f, 1.0f);
    top     = clamp(input.TextureUV.y - (1.0f / ScreenSize.y), 0.0f, 1.0f);
    bottom  = clamp(input.TextureUV.y + (1.0f / ScreenSize.y), 0.0f, 1.0f);

    // Bail early for pixels at the edge of the screen
    if(left < 0.0001f || right > 0.9999f || top < 0.0001f || bottom > 0.9999f) return float4(0.0f, 0.0f, 0.0f, 0.0f);

    SurfaceData GBufferDataX = UnpackGBuffer(input.TextureUV.xy);

    SurfaceData GBufferDataA = UnpackGBuffer(float2(left, top));
    SurfaceData GBufferDataB = UnpackGBuffer(float2(input.TextureUV.x, top));
    SurfaceData GBufferDataC = UnpackGBuffer(float2(right, top));

    SurfaceData GBufferDataD = UnpackGBuffer(float2(left, input.TextureUV.y));
    SurfaceData GBufferDataE = UnpackGBuffer(float2(right, input.TextureUV.y));

    SurfaceData GBufferDataF = UnpackGBuffer(float2(left, bottom));
    SurfaceData GBufferDataG = UnpackGBuffer(float2(input.TextureUV.x, bottom));
    SurfaceData GBufferDataH = UnpackGBuffer(float2(right, bottom));

    // Get the depth of the pixel x and the surrounding 8 pixels
    float x, A, B, C, D, E, F, G, H;

    x = GBufferDataX.LinearDepth;
    A = GBufferDataA.LinearDepth;
    B = GBufferDataB.LinearDepth;
    C = GBufferDataC.LinearDepth;
    D = GBufferDataD.LinearDepth;
    E = GBufferDataE.LinearDepth;
    F = GBufferDataF.LinearDepth;
    G = GBufferDataG.LinearDepth;
    H = GBufferDataH.LinearDepth;    
    
    // Calculate the gradient value at x
    float g = (abs(A-x) + 2.0 * abs(B-x) + abs(C-x) + 2.0 * abs(D-x) + 2.0 * abs(E-x) + abs(F-x) + 2.0 * abs(G - x) + abs(H - x)) / 8.0;
    
    // Get the normal of the pixel x and the surrounding 8 pixels
    float3 xN, AN, BN, CN, DN, EN, FN, GN, HN;
    xN = GBufferDataX.Normal;
    AN = GBufferDataA.Normal;
    BN = GBufferDataB.Normal;
    CN = GBufferDataC.Normal;
    DN = GBufferDataD.Normal;
    EN = GBufferDataE.Normal;
    FN = GBufferDataF.Normal;
    GN = GBufferDataG.Normal;
    HN = GBufferDataH.Normal;
    
    
    // Calculate the gradient of the normals at x
    float gN = ( (abs(AN.x - xN.x) + abs(AN.y - xN.y) + abs(AN.z - xN.z)) +
                (abs(BN.x - xN.x) + abs(BN.y - xN.y) + abs(BN.z - xN.z)) * 2.0 +
                (abs(CN.x - xN.x) + abs(CN.y - xN.y) + abs(CN.z - xN.z)) +
                (abs(DN.x - xN.x) + abs(DN.y - xN.y) + abs(DN.z - xN.z)) * 2.0 +
                (abs(EN.x - xN.x) + abs(EN.y - xN.y) + abs(EN.z - xN.z)) * 2.0 +
                (abs(FN.x - xN.x) + abs(FN.y - xN.y) + abs(FN.z - xN.z)) +
                (abs(GN.x - xN.x) + abs(GN.y - xN.y) + abs(GN.z - xN.z)) * 2.0 +
                (abs(HN.x - xN.x) + abs(HN.y - xN.y) + abs(GN.z - xN.z))) / 8.0;
                
    
    // Store the gradient value in the contourDetectionPass1Buffer
    return float4(g,g,gN,g);
}
