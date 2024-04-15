#ifndef _STD3D
#define _STD3D

#include "value.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    
    float3 vViewPos : POSITION;
    
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;
    float3 vViewBinormal : BINORMAL;
};


// =========================
// Std3D Shader
// g_int_0 : Shader Type
// g_tex_0 : Output Texture
// g_tex_1 : Normal Map
//
// DOMAIN : Masked
// Rasterizer : CULL_BACK
// DepthStencilState : LESS
// BlendState : DEFAULT
// =========================
VTX_OUT VS_Std3D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
    
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV)).xyz;
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV)).xyz;
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV)).xyz;
    
    return output;
}

float4 PS_Std3D(VTX_OUT _in) : SV_Target
{
    float4 vObjectColor = (float4) float4(1.f, 0.f, 0.1f, 1.f);
    float4 vOutColor = (float4) float4(0.f, 0.f, 0.f, 1.f);
             
    float3 vViewNormal = _in.vViewNormal;
    
    // 오브젝트 색상 추출
    if (g_btex_0)
    {
        vObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
    
    // 노말맵핑
    if (g_btex_1)
    {
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).rgb;
        vNormal = vNormal * 2.f - 1.f; // 0~1 값을 -1 ~ 1 로 확장       
        
        float3x3 matRot =
        {
            _in.vViewTangent
            , _in.vViewBinormal
            , _in.vViewNormal
        };
        
        vViewNormal = normalize(mul(vNormal, matRot));
    }
          
    
    // Lighting 연산
    if (0 < iLight3DCount)
    {
        tLightColor lightColor = (tLightColor) 0.f;
        
        for (int i = 0; i < iLight3DCount; ++i)
        {
            CalculateLight3D(_in.vViewPos, vViewNormal, i, lightColor);
        }
                    
        // Diffuse, Specular, Ambient 로 최종 색상을 구한다.
        vOutColor.rgb = (vObjectColor.rgb * lightColor.vDiff.rgb)
                        + (lightColor.vSpec.rgb)
                        + (vObjectColor.rgb * lightColor.vAmb.rgb);
    }
    
    return vOutColor;
}


// ========================
// SkyBox Shader
// g_int_0 : SkyBox Type( 0 == Sphere, 1 == Cube )
// g_tex_0 : Output Texture
// g_texcube_0 : Cube Texture
// ========================
struct VTX_SKY_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_SKY_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float3 vDirUV : TEXCOORD1;
};

VTX_SKY_OUT VS_SkyBox(VTX_SKY_IN _in)
{
    VTX_SKY_OUT output = (VTX_SKY_OUT) 0.f;
            
    // 반지름이 0.5 인 원을 반지름 1로 변경    
    float3 vViewPos = _in.vPos * 2.f;
    
    // View 의 회전만 적용시켜서, 카메라 앞에 원하는 부위를 배치시킴
    vViewPos = mul(float4(vViewPos, 0.f), g_matView).xyz;
    
    // 투영
    float4 vProjPos = mul(float4(vViewPos, 1.f), g_matProj);
    vProjPos.z = vProjPos.w;
    
    output.vPosition = vProjPos;
    output.vUV = _in.vUV;
    output.vDirUV = normalize(_in.vPos);
    
    return output;
}

float4 PS_SkyBox(VTX_SKY_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) float4(0.3f, 0.3f, 0.3f, 1.f);

    // Sphere Type
    if (0 == g_int_0)
    {
        if (g_btex_0)
        {
            vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
        }
    }
    
    // Cube Type
    else if (1 == g_int_0)
    {
        if (g_btexcube_0)
        {
            vOutColor = g_texcube_0.Sample(g_sam_0, _in.vDirUV);
        }
    }
        
    return vOutColor;
}



// =========================
// Std3D Wire Shader
# define color g_vec4_0

// DOMAIN               : Forward
// Rasterizer           : NO_TEST_NO_WRITE
// DepthStencilState    : LESS
// BlendState           : DEFAULT
// =========================
VTX_OUT VS_Std3DWire(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT)0.f;

    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);

    return output;
}

float4 PS_Std3DWire(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = color;
    return vOutColor;
}

// ========================
// Std3DAlphaBlend
// BlendState           : Alpha Blend
// DepthStencilState    : No_Write
// DOMAIN               : OPAQUE
//=========================
VTX_OUT VS_Std3DAlpha(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Std3DAlpha(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;

    if (g_btex_0)
		vOutColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    else
        vOutColor = color;
       
    return vOutColor;
}



#endif