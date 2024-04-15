#ifndef _VALUE
#define _VALUE

#include "struct.fx"

cbuffer TRANSFORM : register(b0)
{
    // 행 우선으로 읽기
    row_major matrix g_matWorld;
    row_major matrix g_matView;
    row_major matrix g_matProj;
    
    row_major matrix g_matWV;
    row_major matrix g_matWVP;
    
    row_major matrix g_matWorldInv;
    row_major matrix g_matViewInv;
}

cbuffer SCALAR_PARAM : register(b1)
{
    float4 vDiff;
    float4 vSpec;
    float4 vAmb;
    float4 vEmv;
    
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    row_major matrix g_mat_0;
    row_major matrix g_mat_1;
    row_major matrix g_mat_2;
    row_major matrix g_mat_3;
        
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;   
    int g_btex_4;
    int g_btex_5;
    int g_btexcube_0;
    int g_btexcube_1;    
    int g_btexarr_0;
    int g_btexarr_1;
    
    // 3D Animation 정보
    int g_iAnim;
    int g_iBoneCount;
}


cbuffer ANIM2D : register(b2)
{
    float2  g_vLT;              // 아틀라스에서 좌상단 UV 좌표
    float2  g_vSlice;           // 아틀라스에서 출력을 위해 자를 사이즈
    float2  g_vBackgroundSize;  // 아틀라스에서 가져올 단위
    float2  g_vOffset;
    
    int     g_useAnim2D;
    float   g_Atlas_Width;    
    float   g_Atlas_Height;    
    float   g_Anim2D_Padding;
}

cbuffer GLOBAL : register(b3)
{
    float2  vResolution;
    float2  vNoiseResolution;
    float   fDT;
    float   fAccTime;
    int     iLight2DCount;
    int     iLight3DCount;
}

cbuffer SSAO_CHANGES_ON_RESIZE : register(b4)
{
    //
    // SSAO를 위해
    //
    matrix g_ViewToTexSpace; // Proj * Texture
    float4 g_FarPlanePoints[3]; // 원거리 삼각형(네 모서리를 덮음), 삼각형은 아래 참조
    float2 g_TexelSize; // (1.0f/W, 1.0f/H)
}

cbuffer SSAO_CHANGES_RARELY : register(b5)
{
    // 방향으로 균일한 분포를 갖지만 길이는 무작위인 14개의 벡터
    float4 g_OffsetVectors[14];

    // 보는 공간의 좌표
    float g_OcclusionRadius;
    float g_OcclusionFadeStart;
    float g_OcclusionFadeEnd;
    float g_SurfaceEpsilon;

    //
    // SSAO_Blur용
    //
    float4 g_BlurWeights[3];
    int g_BlurRadius;
    float3 g_Pad;
};



Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);

TextureCube g_texcube_0 : register(t6);
TextureCube g_texcube_1 : register(t7);

Texture2DArray g_texarr_0 : register(t8);
Texture2DArray g_texarr_1 : register(t9);

Texture2D g_Atlas : register(t10);

// Animation3D Bone Matrix Buffer
StructuredBuffer<matrix> g_arrBoneMat : register(t30);

// Light2DBuffer, // Light3DBuffer
StructuredBuffer<tLightInfo> g_Light2DBuffer : register(t60);
StructuredBuffer<tLightInfo> g_Light3DBuffer : register(t61);

// Global Noise Texture
Texture2D g_noise_01 : register(t70);
Texture2D g_noise_02 : register(t71);
Texture2D g_noise_cloud : register(t72);

// Sampler State Type 
SamplerState g_sam_0 : register(s0); // ANISOTROPIC_2X_WRAP 
SamplerState g_sam_1 : register(s1); // POINT_WRAP 

SamplerState g_sam_LinearClamp : register(s2); // LINEAR_CLAMP 
SamplerState g_sam_PointClamp : register(s3); // POINT_CLAMP 

SamplerState g_sam_Anisotropic_2xClamp : register(s4); // ANISOTROPIC_2X_CLAMP 
SamplerState g_sam_Anisotropic_4xClamp : register(s5); // ANISOTROPIC_4X_CLAMP 
SamplerState g_sam_Anisotropic_8xClamp : register(s6); // ANISOTROPIC_8X_CLAMP 
SamplerState g_sam_Anisotropic_16xClamp : register(s7); // ANISOTROPIC_16X_CLAMP 
SamplerState g_sam_Anisotropic_16xWrap : register(s8); // ANISOTROPIC_16X_WRAP 

SamplerState g_sam_LinearWrap : register(s9); // LINEAR_WRAP 
SamplerState g_sam_ShadowPCF : register(s10); // SHADOW_PCF 

SamplerState g_SamNormalDepth : register(s11); // ANISOTROPIC_8X_CLAMP 
SamplerState g_SamRandomVec : register(s12); // ANISOTROPIC_16X_CLAMP 
SamplerState g_SamBlur : register(s13); // ANISOTROPIC_16X_WRAP 




#include "func.fx"

#endif

