#ifndef SP_QUATERNION_H_HLSL
#define SP_QUATERNION_H_HLSL

float4 qMul(float4 q, float4 r)
{
    return float4(q.w * r.xyz + r.w * q.xyz + cross(q.xyz, r.xyz),
                  q.w * r.w - dot(q.xyz, r.xyz));
}

float3 qRot(float4 q, float3 p)
{
    return qMul(q, qMul(float4(p, 0), float4(-q.xyz, q.w))).xyz;
}

#endif
