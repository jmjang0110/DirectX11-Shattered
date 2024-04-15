#ifndef _UI_
#define _UI_

#include "value.fx"
#include "struct.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};


// =========================
// UIShader
// g_tex_0 : UI Texture
// g_int_0 : ShowAlpha
// g_int_1 : UseInfo
// g_int_2 : UseProgressBar
// g_int_3 : ProgressBarDirection
// g_float_0 : Percentage
// g_vec2_0 : TextureSize
// g_vec4_0 : DrawInfo
// Rasterizer : CULL_BACK
// BlendState : Default
// DepthStencilState : LESS
// DOMAIN : FORWARD
// =========================

#define UITexture g_tex_0
#define ShowAlpha g_int_0
#define UseInfo g_int_1
#define TextureSize g_vec2_0
#define DrawInfo g_vec4_0
#define UseProgressBar g_int_2
#define ProgressBarDirection g_int_3
#define Percentage g_float_0
#define Anisotropy g_sam_0

#define	RIGHT_TO_LEFT	0
#define	LEFT_TO_RIGHT	1
#define	TOP_TO_BOTTOM	2
#define	BOTTOM_TO_TOP	3

float2 ComputeUV(float2 inputUV, float2 pos, float2 size)
{
    float2 result = inputUV;
    //좌상단
    if (inputUV.x == 0.f && inputUV.y == 0.f)
    {
        result.x = saturate(pos.x);
        result.y = saturate(pos.y);
    }
    //우상단
    else if (inputUV.x == 1.f && inputUV.y == 0.f)
    {
        result.x = saturate(pos.x + size.x);
        result.y = saturate(pos.y);
    }
    //좌하단
    else if (inputUV.x == 0.f && inputUV.y == 1.f)
    {
        result.x = saturate(pos.x);
        result.y = saturate(pos.y + size.y);
    }
    else if (inputUV.x == 1.f && inputUV.y == 1.f)
    {
        result.x = saturate(pos.x + size.x);
        result.y = saturate(pos.y + size.y);
    }
    return result;
}

VTX_OUT VS_UI(VTX_IN input)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(input.vPos, 1.f), g_matWVP);
    output.vUV = input.vUV;

    if (UseInfo)
    {
        float2 spritePosition;
        spritePosition.x = saturate(DrawInfo.x / TextureSize.x);
        spritePosition.y = saturate(DrawInfo.y / TextureSize.y);

        float2 imageSizeUV;
        imageSizeUV.x = saturate(DrawInfo.z / TextureSize.x);
        imageSizeUV.y = saturate(DrawInfo.w / TextureSize.y);

        output.vUV = ComputeUV(input.vUV, spritePosition, imageSizeUV);
    }

    return output;
}

float4 PS_UI(VTX_OUT input) : SV_Target
{
    float4 outputColor = (float4) 0.f;
    if (g_btex_0)
    {
        if (UseProgressBar)
        {
        	float2 imageSizeUV = DrawInfo.zw / TextureSize;
            // 오른쪽 -> 왼쪽
            if (ProgressBarDirection == RIGHT_TO_LEFT)
            {
                float2 leftPos;
                leftPos.x = DrawInfo.x / TextureSize.x;

                if(UseInfo)
                {
                    //currentWidth
                    float width = saturate(input.vUV.x - leftPos.x);
                    if(Percentage *imageSizeUV.x< width)
                    {
                        discard;
                    }
                }
                else
                {
                    if (Percentage < input.vUV.x)
                    {
                        discard;
                    }
                }
            }
			// 왼쪽 -> 오른쪽
            else if (ProgressBarDirection == LEFT_TO_RIGHT)
            {
                float2 leftPos;
                leftPos.x = saturate(DrawInfo.x / TextureSize.x);
                if (UseInfo)
                {
                    //currentWidth
                    float width = saturate(input.vUV.x - leftPos.x);
                    if (width < (1.f - Percentage) * imageSizeUV.x)
                    {
                        discard;
                    }
                }
                else
                {
                    if (input.vUV.x <= 1.f - Percentage)
                    {
                        discard;
                    }
                }
                
            }
			// 위 -> 아래
            else if (ProgressBarDirection == TOP_TO_BOTTOM)
            {
                float2 topPos;
                topPos.y = saturate(DrawInfo.y / TextureSize.y);
                if (UseInfo)
                {
                    float height = saturate(input.vUV.y - topPos.y);
                    if (height <= (1.f - Percentage) * imageSizeUV.y)
                    {
                        discard;
                    }

                }
                else
                {
                    if (input.vUV.y <= 1.f - Percentage)
                    {
                        discard;
                    }
                }
                
            }
			// 아래 -> 위
            else if (ProgressBarDirection == BOTTOM_TO_TOP)
            {
                float2 topPos;
                topPos.y = saturate(DrawInfo.y / TextureSize.y);
                if (UseInfo)
                {
                    float height = saturate(input.vUV.y - topPos.y);
                    if (height >= Percentage*imageSizeUV.y)
                    {
                        discard;
                    }

                }
                else
                {
                    if (input.vUV.y >= Percentage)
                    {
                        discard;
                    }
                }
            }
            outputColor = UITexture.Sample(Anisotropy, input.vUV);
        }
        else
        {
            outputColor = UITexture.Sample(Anisotropy, input.vUV);
        }
    }
    else
    {
        if (false == ShowAlpha)
        {
            discard;
        }
        else
        {
            outputColor = float4(1.f, 0.f, 1.f, 1.f);
        }
    }
    
    if (outputColor.a <= 0.f)
    {
        if (ShowAlpha)
            outputColor = float4(1.f, 0.f, 1.f, 1.f);
        else
            discard;
    }
    
    return outputColor;
}
#endif