#ifndef SP_OCTAHEDRAL_NORMAL_H_HLSL
#define SP_OCTAHEDRAL_NORMAL_H_HLSL

float2 octaWrap(float2 v)
{
    return (1.f - abs(v.yx)) * (v.xy >= 0.f ? 1.f : -1.f);
}

float2 encodeOctahedral(float3 n)
{
    n /= (abs(n.x) + abs(n.y) + abs(n.z));
    n.xy = n.z >= 0.f ? n.xy : octaWrap(n.xy);
    n.xy = n.xy * 0.5f + 0.5f;
    return n.xy;
}

float3 decodeOctahedral(float2 f)
{
    f = f * 2.f - 1.f;
    float3 n = float3(f.x, f.y, 1.f - abs(f.x) - abs(f.y));
    float t = saturate(-n.z);
    n.xy += n.xy >= 0.f ? -t : t;
    return normalize(n);
}

#endif