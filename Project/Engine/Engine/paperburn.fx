#ifndef _PAPERBURN
#define _PAPERBURN

#include "value.fx"


// ================
// PaperBurn Shader
// g_float_0 : Strength
#define InputTexture    g_tex_0
#define NoiseTexture    g_tex_1
#define ResultTexture   g_tex_2
#define BurnColor       g_vec4_0
#define BurnStrength    g_float_0
#define EnableAllBlack g_int_0
#define Ratio          g_float_0
// g_tex_0 : Output Texture
// Rasterizer : CULL_NONE
// BlendState : Default
// DepthStencilState : LESS
// DOMAIN : Masked
// =========================
struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition    : SV_Position;
    float2 vUV          : TEXCOORD;
};


VTX_OUT VS_PaperBurnFullScreen(uint vertexID : SV_VertexID)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    float2 grid = float2((vertexID << 1) & 2, vertexID & 2);
    float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    output.vUV = grid * float2(1.0f, 1.0f);
    output.vPosition = float4(xy, 1.0f, 1.0f);

    return output;
}

VTX_OUT VS_PaperBurn(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_PaperBurn(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;

    if(EnableAllBlack)
    {
        return float4(0.f, 0.f, 0.f, Ratio);
    }
    // Animation 정보가 있는 경우
    if (g_useAnim2D)
    {
        float2 vUV = _in.vUV * g_vBackgroundSize;
        vUV = vUV - (g_vBackgroundSize - g_vSlice) / 2.f + g_vLT - g_vOffset;
        
        if (vUV.x < g_vLT.x || g_vLT.x + g_vSlice.x < vUV.x
            || vUV.y < g_vLT.y || g_vLT.y + g_vSlice.y < vUV.y)
        {
            discard;
        }
                
        vOutColor = g_Atlas.Sample(g_sam_1, vUV);
    }
    else
    {        
        if (g_btex_0)
        {
            vOutColor = InputTexture.Sample(g_sam_0, _in.vUV);
        }
        else
        {
            vOutColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }
    
    float fNoise = NoiseTexture.Sample(g_sam_0, _in.vUV).g;
    vOutColor.a -= ((fNoise * BurnStrength) + BurnStrength);
        
    float fRatio = 0.f;
    
    if (0.2f <= vOutColor.a)
    {
        fRatio = 1.f - ((vOutColor.a - 0.2f) / 0.8f);
        vOutColor.rgb = BurnColor.rgb * (fRatio) + (vOutColor.rgb * (1.f - fRatio));
    }
    else if (0.05f < vOutColor.a)
    {
        fRatio = 1.f - ((vOutColor.a - 0.05f) / 0.15f);
        vOutColor.rgb = float3(1.f, 1.f, 1.f) * (fRatio) + (BurnColor.rgb * (1.f - fRatio));
    }
    else
    {
        fRatio = 1.f - ((vOutColor.a - 0.f) / 0.05f);
        vOutColor.rgb = float3(0.f, 0.f, 0.f) * (fRatio) + (float3(1.f, 1.f, 1.f) * (1.f - fRatio));
    }
      
    if (vOutColor.a <= 0.f)
    {       
        //discard;
        vOutColor = ResultTexture.Sample(g_sam_0, _in.vUV); // TEX COLOR 
        //vOutColor = float4(0.f, 0.f, 0.f, 1.f);           // BLACK
    }        
   
    return vOutColor;
}






#endif