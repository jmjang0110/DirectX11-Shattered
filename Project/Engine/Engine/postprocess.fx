#ifndef _POSTPROCESS
#define _POSTPROCESS

#include "value.fx"
#include "func.fx"


struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};


struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float3 vWorldPos : POSITION;
    float3 vViewPos : POSITION1;
    float2 vUV : TEXCOORD;
};

// =====================================
// PostProcess Shader
// Domain       : Post Process
// Mesh         : RectMesh
// Blend        : Default
// DepthStencil : NoTest NoWrite
#define PostProcessTarget   g_tex_0
#define IsBind              g_btex_0
// =====================================
VTX_OUT VS_PostProcess(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_PostProcess(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    // _in.vPosition; ÇÈ¼¿ ÁÂÇ¥
    float2 vScreenUV = _in.vPosition.xy / vResolution;
    
    if (IsBind)
    {
        //vScreenUV.y += sin(vScreenUV.x * 3.141592f * 20.f + (fAccTime * 4.f)) * 0.01;
                
        _in.vUV.x += fAccTime * 0.1f;
        vScreenUV += (g_noise_cloud.Sample(g_sam_0, _in.vUV).rg - 0.5f) * 0.03f;
        vScreenUV = saturate(vScreenUV);
                
        vOutColor = PostProcessTarget.Sample(g_sam_1, vScreenUV);
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vOutColor;
}


// =====================================
// FlamePostProcess Shader
// Domain       : Post Process
// Mesh         : RectMesh
// Blend        : Default
// DepthStencil : NoTest NoWrite
// =====================================
#define SPEED g_float_0

VTX_OUT VS_FlamePostProcess(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_FlamePostProcess(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
    
    if (IsBind)
    {
        // _in.vPosition; ÇÈ¼¿ ÁÂÇ¥
        float2 vScreenUV = _in.vPosition.xy / vResolution;
        float4 vMaskColor = g_noise_cloud.Sample(g_sam_0, vScreenUV);
        
        if (vMaskColor.r == 0.0f)
        {
            float X = vScreenUV.x + SPEED;
            float Y = vScreenUV.y + SPEED;
            vScreenUV.x += sin(X - Y) * 0.01 * sin(Y);
            vScreenUV.y += cos(X + Y) * 0.01 * cos(Y);
        }
        
        vOutColor = PostProcessTarget.Sample(g_sam_1, vScreenUV);
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vOutColor;
}


// =====================================
// PostProcess Depth of Feild Shader
// Domain       : Post Process
// Mesh         : RectMesh
// Blend        : Default
// DepthStencil : NoTest NoWrite
#define PostProcessTarget   g_tex_0
#define PositionTarget      g_tex_1
#define FogColor            g_vec4_0
// =====================================
VTX_OUT VS_FogPostProcess(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    //output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

float4 PS_FogPostProcess(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;

    // _in.vPosition; ÇÈ¼¿ ÁÂÇ¥
    float2 vScreenUV = _in.vPosition.xy / vResolution;
    vOutColor = PostProcessTarget.Sample(g_sam_1, vScreenUV);
    float4 fcolor = FogColor;

    float3 vTargetViewPos = PositionTarget.Sample(g_sam_0, vScreenUV).xyz;
    
    if (vTargetViewPos.z < 1000.f)
        return vOutColor;

    float fAlpha = (vTargetViewPos.z - 1000.f) / 10000.f;
    fAlpha = clamp(fAlpha, 0.001f, 0.5f);

    fAlpha = 1.f - fAlpha;

    vOutColor = (lerp(fcolor, vOutColor, fAlpha));

    return vOutColor;
}



// =====================================
// Emissive PostProcess Shader
// Domain       : Post Process
// Mesh         : RectMesh
// Blend        : Default
// DepthStencil : NoTest NoWrite
#define PostProcessTarget   g_tex_0
#define EmissiveTarget      g_tex_1
// =====================================
VTX_OUT VS_EmissivePostProcess(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;

    return output;
}

static const float Weight4[25] = // 12 - 1 - 12
{
    0.0050, 0.0162, 0.0382, 0.0561, 0.1353, 0.1953, 0.278, 0.4868, 0.6018, 0.7261, 0.8131, 0.9231,
    1, 0.9231, 0.8131, 0.7261, 0.6018, 0.4868, 0.278, 0.1953, 0.1353, 0.0561, 0.0382, 0.0162, 0.0050
};

float4 PS_EmissivePostProcess(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;

    // _in.vPosition; ÇÈ¼¿ ÁÂÇ¥
    float2 vScreenUV = _in.vPosition.xy / vResolution;
    vOutColor = PostProcessTarget.Sample(g_sam_1, vScreenUV);

    // bloom Àû¿ë
    float fWeightOnePixel = 1.f / vResolution.x;
    float fHeightOnePixel = 1.f / vResolution.y;
    
    float2 t = vScreenUV;
    float2 uv = (float2) 0.f;
    float4 vBloomColor = (float4) 0.f;
    
    for (int i = -12; i < 12; ++i)
    {
        uv = t + float2(fWeightOnePixel * i, 0.f);
        vBloomColor += Weight4[12 + i] * EmissiveTarget.Sample(g_sam_0, uv);
        
        if (vBloomColor.x <= 0.1f && vBloomColor.y <= 0.1f && vBloomColor.z <= 0.1f)
        {
            vBloomColor = (float4) 0.f;
        }
    
    }

    for (int i = -12; i < 12; ++i)
    {
        uv = t + float2(0, fHeightOnePixel * i);
        vBloomColor += Weight4[12 + i] * EmissiveTarget.Sample(g_sam_0, uv);
        
        if (vBloomColor.x <= 0.1f && vBloomColor.y <= 0.1f && vBloomColor.z <= 0.1f)
        {
            vBloomColor = (float4) 0.f;
        }
    }
    
    if (vBloomColor.x <= 0.5f && vBloomColor.y <= 0.5f && vBloomColor.z <= 0.5f)
    {
        return vOutColor;
    }
    
    // bloom ÃßÃâ
    vBloomColor /= 12.f;
    float vBrightness = dot(vBloomColor.rgb, float3(0.2126f, 0.7152f, 0.0722f));
    float4 calcColor = float4(vOutColor.xyx + (vBloomColor.xyz * 0.5f), 1.f);
    
    return calcColor;
}

float4 PS_SinePostProcess(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    // _in.vPosition; ÇÈ¼¿ ÁÂÇ¥
    if(g_int_0 == 0)
    {
        return float4(0.f,0.f,0.f,1.f);
    }

    if (IsBind)
    {
        const float timeScale = 4.f;
        const float speed = 20.f;
        const float PI = 3.141592f;
        const float distortionRange = 0.01f;
        
        float2 vScreenUV = _in.vPosition.xy / vResolution;
        vScreenUV.x = _in.vUV.x;
        vScreenUV.y += sin((vScreenUV.x * PI * speed + fAccTime * timeScale)) * distortionRange;
        vScreenUV = saturate(vScreenUV);
                
        vOutColor = PostProcessTarget.Sample(g_sam_0, vScreenUV);
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }
    
    return vOutColor;
}

float2 Rotate(float2 xy, float theta)
{
    float cosX = xy.x;
    float sinY = xy.y;

    return -float2(cosX * cos(theta) - sinY * sin(theta), cosX * sin(theta) + sinY * cos(theta));
}

float Calc(float a, float b)
{
    if (a < 0.f)
    {
        return b - fmod(abs(a), b);
    }
    return fmod(a, b);
}

float atan3(float2 inputUV)
{
    return Calc(atan2(inputUV.y, inputUV.x), 2.f * 3.141592f);
}

float4 PS_AbsorbPostProcess(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
       
    if (IsBind)
    {
        const float PI = 3.141592f;
                
        float factor = sin(g_float_0 / 2.f);
        float2 size;
        g_tex_0.GetDimensions(size.x, size.y);
        float2 sizeUV = size / vResolution;
        float2 UV = _in.vUV - sizeUV*float2(0.8f,0.5f);

        UV = Rotate(UV, fAccTime + pow(length(UV), 0.4) * factor * 5.5);
    
        float angle = atan3(UV);
        float magnitude = max(0.0, length(UV) + abs(factor));
        UV = float2(cos(angle), sin(angle)) * magnitude;

        vOutColor = lerp(PostProcessTarget.Sample(g_sam_0, UV),(float4)0.f, max(0.0, pow(magnitude - abs(factor) * 0.1f, 3.f    )));

       
    }
    else
    {
        vOutColor = float4(1.f, 0.f, 1.f, 1.f);
    }

    if (g_float_0 > 3.5f)
    {
        vOutColor = float4(0.f, 0.f, 0.f, 1.f);
    }

    return vOutColor;
}

#endif
