
//--------------------------------------------------------------------------------------
// GBuffer Textures and Samplers
//--------------------------------------------------------------------------------------

Texture2D<float>  DepthTexture                  : register( t0 );
Texture2D<float4> ColorSpecularIntensityTexture : register( t1 );
Texture2D<float3> NormalTexture                 : register( t2 );
Texture2D<float4> SpecularExponentTexture       : register( t3 );
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
    float SpecularExponent;
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
    output.SpecularExponent = SpecularExponentTexture.Sample( PointSampler, UV.xy ).x;

    return output;
}
