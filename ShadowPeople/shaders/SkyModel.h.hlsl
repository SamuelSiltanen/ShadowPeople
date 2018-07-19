#ifndef SP_SKY_MODEL_H_HLSL
#define SP_SKY_MODEL_H_HLSL

#include "../cpugpu/Constants.h"
#include "ColorSpaces.h.hlsl"

// Preetham sky model

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

    float T = 1.f;

    float Yref = perezFormula(1.f, L.y, AY(T), BY(T), CY(T), DY(T), EY(T));
    float xref = perezFormula(1.f, L.y, Ax(T), Bx(T), Cx(T), Dx(T), Ex(T));
    float yref = perezFormula(1.f, L.y, Ay(T), By(T), Cy(T), Dy(T), Ey(T));

    float theta = acos(L.y);

    float Yz = zenithY(T, theta); //Yz *= 1000.f; //??
    float xz = zenithx(T, theta);
    float yz = zenithy(T, theta);

    float dotVL = dot(V, L);

    float Y = perezFormula(V.y, dotVL, AY(T), BY(T), CY(T), DY(T), EY(T)) / Yref * Yz;
    float x = perezFormula(V.y, dotVL, Ax(T), Bx(T), Cx(T), Dx(T), Ex(T)) / xref * xz;
    float y = perezFormula(V.y, dotVL, Ay(T), By(T), Cy(T), Dy(T), Ey(T)) / yref * yz;

    float X = x / y * Y;
    float Z = (1.f - x - y) / y * Y;

    return CIEXYZ2sRGB(float3(X, Y, Z));
}

#endif
