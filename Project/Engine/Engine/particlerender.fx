#ifndef _PARTICLERENDER
#define _PARTICLERENDER

#include "value.fx"
#include "struct.fx"

// ====================
// ParticleRenderShader
// Mesh     : PointMesh
// Domain   : DOMAIN_EMISSIVE_PARTICLE
// Blend    : AlphaBlend
// DepthStencil : Less
// Rasterizer : CULL_NONE

// Parameter
#define POS_INHERIT     g_int_0
#define PARTICLE_INDEX  g_int_1
#define APPLY_LIGHTING  g_int_2

#define EMISSIVE_USE    g_int_3

#define LINEAR_PARTICLE      g_float_0
#define SOFT_PARTICLE      g_float_1

#define POSITION_TEXTURE   g_tex_1

#define EMISSIVE_COLOR g_vec4_0
// ====================

StructuredBuffer<tParticle> ParticleBuffer : register(t16);

struct VTX_IN
{       
    uint InstID : SV_InstanceID;
};

struct VTX_OUT
{ 
    uint InstID : SV_InstanceID;
};


VTX_OUT VS_ParticleRender(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;    
   
    output.InstID = _in.InstID;       
    
    return output;
}

struct GS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    uint InstID : SV_InstanceID;
};

// 1 개의 정점을 6개로 분할
[maxvertexcount(6)]
void GS_ParticleRender(point VTX_OUT _in[1], inout TriangleStream<GS_OUT> _output)
{
    uint ID = _in[0].InstID;
    
    // 해당 인스턴스가 비활성화면, 출력 정점이 없다 ==> 아무일도 없다
    if (0 == ParticleBuffer[ID].Alive)
        return;
    
     // 파티클 정점의 월드위치 구하기
    float3 vParticleWorldPos = (float3) 0.f;
    
    
    if (POS_INHERIT)
    {
        vParticleWorldPos = g_matWorld._41_42_43 + ParticleBuffer[ID].vPos;
    }
    else
    {
        vParticleWorldPos = ParticleBuffer[ID].vPos;
    }
    
    // World 좌표를 View, Proj 으로 변환
    float4 vViewPos = mul(float4(vParticleWorldPos, 1.f), g_matView);
    float3 vScale = ParticleBuffer[ID].vScale;
       
    
    // ViewSpace 에서 Point 를 Rect 로 분할
    // 0 --- 1
    // |  \  |
    // 3 --- 2
    float3 arrRect[4] =
    {
        float3(vViewPos.x - vScale.x / 2.f, vViewPos.y + vScale.y / 2.f, vViewPos.z),
        float3(vViewPos.x + vScale.x / 2.f, vViewPos.y + vScale.y / 2.f, vViewPos.z),
        float3(vViewPos.x + vScale.x / 2.f, vViewPos.y - vScale.y / 2.f, vViewPos.z),
        float3(vViewPos.x - vScale.x / 2.f, vViewPos.y - vScale.y / 2.f, vViewPos.z)
    };

    if (1.f == g_float_0)
    {
        // dir view로 변환
        float3 newDir = mul(float4(ParticleBuffer[ID].vDir, 0.f), g_matView).xyz;
        newDir = normalize(newDir);

        // 카메라와 newDir을 외적하여 화면상의 회전된 upvector를 구한다.
        // 카메라와 -> 파티클 벡터 구하기.
        float3 vZVec = vViewPos;
        float3 vFront = normalize(cross(newDir, float3(0.f, 1.f, 0.f)));
        float3 vUP = normalize(cross(vFront, newDir));

        float diagonal = sqrt((vScale.x / 2.f) * (vScale.x / 2.f) + (vScale.y / 2.f) * (vScale.y / 2.f));

        float3 arrCalcRect[4] =
        {
              vViewPos + (newDir * vScale.x * -0.5f) + vUP * vScale.y * 0.5f,
              vViewPos + (newDir * vScale.x * 0.5f) + vUP * vScale.y * 0.5f,
              vViewPos + (newDir * vScale.x * 0.5f) + vUP * vScale.y * -0.5f,
              vViewPos + (newDir * vScale.x * -0.5f) + vUP * vScale.y * -0.5f
        };

        arrRect = arrCalcRect;
    }
    
    GS_OUT output[4] = { (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f, (GS_OUT) 0.f };
    
    for (int i = 0; i < 4; ++i)
    {
        output[i].vPosition = mul(float4(arrRect[i], 1.f), g_matProj);
        output[i].InstID = _in[0].InstID;
        output[i].vViewPos = arrRect[i];
    }
    
    output[0].vUV = float2(0.f, 0.f);
    output[1].vUV = float2(1.f, 0.f);
    output[2].vUV = float2(1.f, 1.f);
    output[3].vUV = float2(0.f, 1.f);
        
            
    _output.Append(output[0]);
    _output.Append(output[2]);
    _output.Append(output[3]);
    _output.RestartStrip();
    
    _output.Append(output[2]);
    _output.Append(output[0]);
    _output.Append(output[1]);
    _output.RestartStrip();
}

struct PS_OUT
{
    float4 vParticle    : SV_Target0;
    float4 vEmissive    : SV_Target1;
};

PS_OUT PS_ParticleRender(GS_OUT _in)
{
    PS_OUT output = (PS_OUT)0.f;
    
    if(g_btex_0)
    {
        output.vParticle = g_tex_0.Sample(g_sam_0, _in.vUV) * ParticleBuffer[_in.InstID].vColor;
    }
    else
    {
        output.vParticle = ParticleBuffer[_in.InstID].vColor;
    }
        
     // Lighting 연산
    if (APPLY_LIGHTING && 0 < iLight3DCount)
    {
        tLightColor lightColor = (tLightColor) 0.f;
        
        for (int i = 0; i < iLight3DCount; ++i)
        {            
            CalculateLight3D_Particle(_in.vViewPos, i, lightColor);
        }
                    
        // Diffuse, Specular, Ambient 로 최종 색상을 구한다.
        output.vParticle.rgb = (output.vParticle.rgb * lightColor.vDiff.rgb)
                        + (output.vParticle.rgb * lightColor.vAmb.rgb);
    }    

    if (1.f == SOFT_PARTICLE)
    {

        float4 projPos = mul(float4(_in.vViewPos, 1.f), g_matProj);
        float2 vScreenUV;

        vScreenUV.x = (projPos.x / projPos.w) * 0.5f + 0.5f;
        vScreenUV.y = (projPos.y / projPos.w) * -0.5f + 0.5f;

        float2 vFullUV = float2(_in.vPosition.x / vResolution.x, _in.vPosition.y / vResolution.y);
        float3 vTargetViewPos = g_tex_1.Sample(g_sam_0, vFullUV).xyz;

        float diff = vTargetViewPos.z - _in.vViewPos.z;

        float fAlpha = diff / 100.f;

        fAlpha = clamp(fAlpha, 0.1f, 1.f);

        if (diff < 100.f && 0.f <= diff)
        {
            output.vParticle *= fAlpha;
        }
    }

    if (1 == EMISSIVE_USE)
    {
        output.vEmissive = EMISSIVE_COLOR;
    }
    
    return output;
}

#endif