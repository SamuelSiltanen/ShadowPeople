#include "Lighting.if.h"
#include "OctahedralNormal.h.hlsl"
#include "Quaternion.h.hlsl"

static const float PI           = 3.141592653589793f;

static const float SunElevation = 80.f * PI / 180.f;
static const float SunAzimuth   = 44.f * PI / 180.f;

static const float3 Ambient     = float3(0.2f, 0.3f, 0.4);
static const float3 SunDir      = float3(sin(SunAzimuth), sin(SunElevation), cos(SunAzimuth) * cos(SunElevation));
static const float SunIntensity = 10.f;
static const float3 SunColor    = float3(0.9f, 0.8f, 0.7f);
static const float SunDiscCos   = 0.999989f;
static const float F0Dielectric = 0.04f;

float2 unpackUintToFloat2(uint i)
{
    return float2(i & 0xffff, i >> 16) / 65535.f;
}

float3 diffuseLight(float3 N, float3 L)
{
    return saturate(dot(N, L));
}

float G1V(float d, float k)
{
    return 1.f / (d * (1.f - k) + k);
}

float3 specularLight(float3 N, float3 L, float3 V, float alpha, float F0)
{
    float3 H        = normalize(V + L);
    float dotNL     = saturate(dot(N, L));
    float dotNV     = saturate(dot(N, V));
    float dotNH     = saturate(dot(N, H));
    float dotLH     = saturate(dot(L, H));
    
    float alphaSqr  = alpha * alpha;
    float denom     = dotNH * dotNH * (alphaSqr - 1.f) + 1.f;
    float D         = alphaSqr / (PI * denom * denom);

    float dotLH5    = pow(1.f - dotLH, 5.f);
    float F         = F0 + (1.f - F0) * dotLH5;

    float k         = alpha * 0.5f;
    float G         = G1V(dotNL, k) * G1V(dotNV, k);

    return dotNL * D * F * G;
}

float perezFormula(float dotVY, float dotVL, float A, float B, float C, float D, float E)
{
    float f1 = 1.f + A * exp(B / (dotVY + 0.01f));
    float f2 = 1.f + C * exp(D * acos(dotVL)) + E * dotVL * dotVL;
    return f1 * f2;
}

float AY(float T) { return  0.1787 * T - 1.4630; }
float BY(float T) { return -0.3554 * T + 0.4275; }
float CY(float T) { return -0.0227 * T + 5.3251; }
float DY(float T) { return  0.1206 * T - 2.5771; }
float EY(float T) { return -0.0670 * T + 0.3703; }

float Ax(float T) { return -0.0193 * T - 0.2592; }
float Bx(float T) { return -0.0665 * T + 0.0008; }
float Cx(float T) { return -0.0004 * T + 0.2125; }
float Dx(float T) { return -0.0641 * T - 0.8989; }
float Ex(float T) { return -0.0033 * T + 0.0452; }

float Ay(float T) { return -0.0167 * T - 0.2608; }
float By(float T) { return -0.0950 * T + 0.0092; }
float Cy(float T) { return -0.0079 * T + 0.2102; }
float Dy(float T) { return -0.0441 * T - 1.6537; }
float Ey(float T) { return -0.0109 * T + 0.0529; }

float3 CIEXYZ2sRGB(float3 XYZ)
{
    const float3x3 mat =
    {
        3.2406, -1.5372, -0.4986,
        -0.9689, 1.8758, 0.0415,
        0.0557, -0.2040, 1.0570
    };
    return mul(mat, XYZ);
}

float zenithY(float T, float theta)
{
    return (4.0453 * T - 4.9710) * tan((4.f / 9.f - T / 120.f) * (PI - 2.f * theta)) - 0.2155f * T + 2.4192f;
}

float zenithx(float T, float theta)
{
    float T2 = T * T;
    float theta2 = theta * theta;
    float theta3 = theta2 * theta;

    return 
        ( 0.00166 * theta3 - 0.00375 * theta2 + 0.00209 * theta + 0) * T2 +
        (-0.02903 * theta3 + 0.06377 * theta2 - 0.03202 * theta + 0.00394) * T +
        ( 0.11693 * theta3 - 0.21196 * theta2 + 0.06052 * theta + 0.25886);
}

float zenithy(float T, float theta)
{
    float T2 = T * T;
    float theta2 = theta * theta;
    float theta3 = theta2 * theta;

    return 
        ( 0.00275 * theta3 - 0.00610 * theta2 + 0.00317 * theta + 0) * T2 +
        (-0.04214 * theta3 + 0.08970 * theta2 - 0.04153 * theta + 0.00516) * T +
        ( 0.15346 * theta3 - 0.26756 * theta2 + 0.06670 * theta + 0.26688);
}

float3 skyColor(float3 V, float3 L)
{
    if (V.y < 0.f) return float3(0.5f, 0.5f, 0.5f);

    float dotVL = dot(V, L);

    if (dotVL >= SunDiscCos) return SunColor * SunIntensity;

    float T = 1.f;

    float Yref = perezFormula(1.f, L.y, AY(T), BY(T), CY(T), DY(T), EY(T));
    float xref = perezFormula(1.f, L.y, Ax(T), Bx(T), Cx(T), Dx(T), Ex(T));
    float yref = perezFormula(1.f, L.y, Ay(T), By(T), Cy(T), Dy(T), Ey(T));

    float theta = acos(L.y);

    float Yz = zenithY(T, theta); //Yz *= 1000.f; //??
    float xz = zenithx(T, theta);
    float yz = zenithy(T, theta);

    float Y = perezFormula(V.y, dotVL, AY(T), BY(T), CY(T), DY(T), EY(T)) / Yref * Yz;
    float x = perezFormula(V.y, dotVL, Ax(T), Bx(T), Cx(T), Dx(T), Ex(T)) / xref * xz;
    float y = perezFormula(V.y, dotVL, Ay(T), By(T), Cy(T), Dy(T), Ey(T)) / yref * yz;

    float X = x / y * Y;
    float Z = (1.f - x - y) / y * Y;

    return CIEXYZ2sRGB(float3(X, Y, Z));
}

float3 worldPos(uint2 coords, float depth)
{
    float2 screenUV = (coords + 0.5f) / float2(size);
    screenUV.y      = 1.f - screenUV.y;
    float3 ndcPos   = float3(2.f * screenUV - 1.f, depth);
    float4 viewPos  = mul(invProj, float4(ndcPos, 1.f));
    viewPos         /= viewPos.w;
    float4 worldPos = mul(invView, viewPos);
    return worldPos.xyz;
}

[numthreads(ThreadGroupsX, ThreadGroupsY, ThreadGroupsZ)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	if (any(DTid.xy >= size)) return;

    float3 color = 0.f;
    float depth = zBuffer[DTid.xy];	
    if (depth == 1.f) 
    {
        // Hit the sky
        float3 pixelWorldPos    = worldPos(DTid.xy, 1.f);
        float3 cameraWorldPos   = mul(invView, float4(0.f, 0.f, 0.f, 1.f)).xyz;
        float3 viewDir          = normalize(pixelWorldPos - cameraWorldPos);

        color = skyColor(viewDir, normalize(SunDir));
    }
    else
    {
        uint4 pixel     = gBuffer[DTid.xy];

        // Unpack g-buffer
        float2 uvFlt    = unpackUintToFloat2(pixel.x);
        float2 oxyFlt   = unpackUintToFloat2(pixel.y);
        float2 ozwFlt   = unpackUintToFloat2(pixel.z);
        float bitangentSign = asfloat(pixel.w);
        float4 orientation = float4(oxyFlt.x, oxyFlt.y, ozwFlt.x, ozwFlt.y) * 2.f - 1.f;

        // Read material
        float4 texel    = albedoRoughness.SampleLevel(bilinearSampler, uvFlt, 0);
        float2 texel2   = normal.SampleLevel(bilinearSampler, uvFlt, 0);

        // Interpret material data
        float3 n        = decodeOctahedral(texel2);
        n.y = -n.y;
        float3 detailNorm = qRot(orientation, n);

        float3 albedo   = texel.rgb;

        // Based on Phong default exponent 60 and https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
        float alpha     = rsqrt(max(30.f * texel.w - 1.f, 1e-3));

        // Find out pixel view position
        float3 pixelWorldPos    = worldPos(DTid.xy, depth);
        float3 cameraWorldPos   = mul(invView, float4(0.f, 0.f, 0.f, 1.f)).xyz;

        float3 viewDir  = normalize(cameraWorldPos - pixelWorldPos);
        
        float3 diff     = albedo * diffuseLight(detailNorm, SunDir);
        float3 spec     = specularLight(detailNorm, SunDir, viewDir, alpha, F0Dielectric);
        
        color    = albedo * Ambient + SunIntensity * SunColor * (diff + spec);
    }

    // Cheap tone mapping
    float lum = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
    color *= 1.f / (1.f + lum);
    color = pow(color, 1.f / 2.2f);

    litBuffer[DTid.xy] = float4(color, 1.f);
}
