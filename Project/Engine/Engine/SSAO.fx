#ifndef SSAO_HLSL
#define SSAO_HLSL

#include "FullScreenTriangle.fx"
#include "value.fx"

#define DiffuseTex      g_tex_0
#define NormalDepthTex  g_tex_1
#define RandomVecTex    g_tex_2
#define InputImageTex   g_tex_3



//
// Pass1 기하학 버퍼 생성
//

struct VertexPosNormalTex
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
    float2 tex : TEXCOORD;
};

struct VertexPosHVNormalVTex
{
    float4 posH : SV_POSITION;
    float3 posV : POSITION;
    float3 NormalV : NORMAL;
    float2 tex : TEXCOORD0;
};
// RTT : Render To Texture
// 
// 뷰 공간 법선 벡터 및 깊이 값의 RTT를 생성하는 버텍스 셰이더
VertexPosHVNormalVTex SSAO_GeometryVS(VertexPosNormalTex vIn)
{
    VertexPosHVNormalVTex vOut;

    // View Space 변환 
    vOut.posV = mul(float4(vIn.posL, 1.0f), g_matWV).xyz;
    vOut.NormalV = mul(vIn.normalL, (float3x3) g_matWorldInv);
    // 클립 공간으로 변환
    vOut.posH = mul(float4(vIn.posL, 1.0f), g_matWVP);
    vOut.tex = vIn.tex;

    return vOut;
}

// 뷰 공간 법선 벡터 및 깊이 값 생성  RTT 픽셀 셰이더
float4 SSAO_GeometryPS(VertexPosHVNormalVTex pIn, uniform bool alphaClip) : SV_TARGET
{
    // 법선 벡터를 다음으로 정규화합니다.
    pIn.NormalV = normalize(pIn.NormalV);

    if (alphaClip)
    {
        float4 TexColor = DiffuseTex.Sample(g_sam_LinearWrap, pIn.tex);

        clip(TexColor.a - 0.1f);
    }

    // 뷰 공간의 법선 벡터와 깊이 값을 반환합니다.
    return float4(pIn.NormalV, pIn.posV.z);
}

//
// Pass2 계산하다 AO
//

// 포인트 r과 p 사이의 깊이 차이가 주어지면 샘플링 포인트 q에 의한 포인트 p의 폐색 정도를 계산합니다.
float OcclusionFunction(float distZ)
{
    //
    // 깊이(q)가 깊이(p) 뒤에 있으면(반구 외부) 점 q는 점 p를 덮을 수 없습니다. 또한 깊이(q)와 깊이(p)가 너무 가까우면
    //  우리는 또한 깊이(p)-깊이(r)가 점 q가 점 p를 덮을 수 있다고 생각하기 위해 사용자가 가정한 엡실론 값을 초과해야 하기 때문에 점 q가 점 p를 덮을 수 없다고 믿습니다.
    //
     // 다음 함수를 사용하여 폐색 정도를 결정합니다.
    //
    //    /|\ Occlusion
    // 1.0 |      ---------------\
    //     |      |             |  \
    //     |                         \
    //     |      |             |      \
    //     |                             \
    //     |      |             |          \
    //     |                                 \
    // ----|------|-------------|-------------|-------> zv
    //     0     Eps          zStart         zEnd
    float occlusion = 0.0f;
    if (distZ > g_SurfaceEpsilon)
    {
        float fadeLength = g_OcclusionFadeEnd - g_OcclusionFadeStart;
        // distZ가 g_OcclusionFadeStart에서 g_OcclusionFadeEnd로 점차 기울어지면 마스킹 값이 1에서 0으로 선형적으로 감소합니다.
        occlusion = saturate((g_OcclusionFadeEnd - distZ) / fadeLength);
    }
    return occlusion;
}

struct SSAO_VS_OUT
{
    float4 posH             : SV_Position;
    float3 farPlanePoint    : POSITION;
    float2 texcoord         : TEXCOORD;
};

struct SSAO_PS_OUT
{
    float4 vAO      : SV_Target0;
    float4 vAOTemp  : SV_Target1;
    float4 vRandVec : SV_Target2;
};

#define sampleCount g_int_0

// 삼각형을 사용하여 NDC 공간을 덮습니다.
// (-1, 1)________ (3, 1)
//        |   |  /
// (-1,-1)|___|/ (1, -1)   
//        |  /
// (-1,-3)|/    
SSAO_VS_OUT SSAO_VS(uint vertexID : SV_VertexID)
{
    SSAO_VS_OUT output;

    float2 grid = float2((vertexID << 1) & 2, vertexID & 2);
    float2 xy = grid * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    output.texcoord      = grid * float2(1.0f, 1.0f);
    output.posH          = float4(xy, 1.0f, 1.0f);
    output.farPlanePoint = g_FarPlanePoints[vertexID].xyz;

    return output;

}

// SSAO 픽셀 셰이더
SSAO_PS_OUT SSAO_PS(SSAO_VS_OUT _VSin)
{
    SSAO_PS_OUT output;
    // p - 계산하려는 주변 폐색 대상 지점
    // n -- 점 p의 법선 벡터
    // q - 지점 p에서 반구의 임의 지점
    // r - 점 p를 폐색할 수 있는 점

    // 뷰 공간의 법선 벡터와 현재 픽셀을 가져옵니다.z 좌표
    float4 normalDepth = NormalDepthTex.SampleLevel(g_sam_LinearWrap, _VSin.texcoord, 0.0f);

    float3 n = normalDepth.xyz;
    float pz = normalDepth.w;

    //
     // 뷰 공간 좌표 재구성(x, y, z)
     // p = t * pIn.ToFarPlane이 되는 t 찾기
   // p.z = t * pIn.ToFarPlane.z
   // t = p.z / pIn.ToFarPlane.z
   //
   float3 p = (pz / _VSin.farPlanePoint.z) * _VSin.farPlanePoint;

   //랜덤 벡터를 가져와 [0, 1]^3에서 [-1, 1]^3으로 매핑
   float3 randVec = RandomVecTex.SampleLevel(g_sam_LinearWrap, 4.0f * _VSin.texcoord, 0.0f).xyz;
   randVec = 2.0f * randVec - 1.0f;

   float occlusionSum = 0.0f;

   // p를 중심으로 하는 반구에서 샘플은 법선 n에 따라 p 주위를 점
   for (int i = 0; i < sampleCount; ++i)
   {
        //  오프셋 벡터는 모두 고정되어 있고 균일하게 분포되어 있습니다(따라서 우리가 사용하는 오프셋 벡터는 같은 방향으로 함께 클러스터링되지 않습니다).
        //   반사를 위해 이러한 오프셋 벡터를 임의 벡터와 연관시키면 균일하게 분포된 집합을 얻어야 합니다.
        //   랜덤 오프셋 벡터
       float3 offset = reflect(g_OffsetVectors[i].xyz, randVec);

       // 오프셋 벡터가 (p, n)으로 정의된 평면 뒤에 있는 경우 뒤집기
       float flip = sign(dot(offset, n));

       // 포인트 p가 폐색 반경 내에 있는 반구 내 샘플링
       float3 q = p + flip * g_OcclusionRadius * offset;

       // 투영된 텍스처 좌표를 얻기 위한 프로젝트 q
       float4 projQ = mul(float4(q, 1.0f), g_ViewToTexSpace);
       projQ /= projQ.w;

       // 점 q의 방향에서 눈으로 관측할 수 있는 가장 가까운 점 r의 깊이 값을 구한다(점 r이 존재하지 않을 가능성이 있으며 이때 관측된
       // 먼 평면의 한 점입니다). 이렇게 하려면 깊이 맵에서 이 지점의 깊이 값을 살펴봐야 합니다.
       float rz = NormalDepthTex.SampleLevel(g_sam_LinearWrap, projQ.xy, 0.0f).w;

       // 관측 공간 r에서 재구성 지점 r의 좌표 = (rx, ry, rz)
    // 우리는 점 r이 눈에서 점 q까지의 광선에 위치한다는 것을 알고 있으므로 r = t * q
       // r.z = t * q.z ==> t = t.z / q.z
       float3 r = (rz / q.z) * q;

       // 점 r이 p를 음영 처리하는지 테스트
        // - 내적 dot(n, normalize(r - p))는 음영 처리된 점 r에서 평면(p, n)의 전면까지의 거리를 측정합니다. 에 가까운
        // 이 평면의 전면에는 더 큰 마스킹 웨이트를 설정합니다. 동시에 이것은 또한 다음을 방지합니다.
        // (p, n)에 있는 점 r의 자체 그림자는 잘못된 폐색 값을 생성합니다(g_SurfaceEpsilon 설정).
        //관찰자 입장에서 깊이 값이 다르기 때문인데, 사실은 경사면에서
        // (p, n) 위의 점 r은 목표점 p를 막지 않습니다.
        // - 오클루전 가중치의 크기는 오클루전 지점과 대상 지점 사이의 거리에 따라 다릅니다. 음영 처리된 점 r이 목표점 p에서 떨어져 있는 경우
        // 멀리, 그러면 포인트 r이 포인트 p를 차단하지 않는 것으로 간주됩니다.

       float distZ = p.z - r.z;
       float dp = max(dot(n, normalize(r - p)), 0.0f);
       float occlusion = dp * OcclusionFunction(distZ);

       occlusionSum += occlusion;
   }

   occlusionSum /= sampleCount;

   float access = 1.0f - occlusionSum;

   // SSAO 이미지의 대비를 강화하면 SSAO 이미지의 효과가 더 분명해집니다. 
   output.vAO       = saturate(pow(access, 8.f));
   output.vAOTemp   = float4(0.f, 1.f, 0.f, 1.f);
   output.vRandVec  = float4(0.f, 0.f, 1.f, 1.f);

   return output;

}

//
// Pass3 희미한AO
//

// 양방향 필터링



float4 BilateralPS(float4 posH : SV_position,
    float2 texcoord : TEXCOORD) : SV_Target
{
    float s_BlurWeights[12] = (float[12]) g_BlurWeights;

    // 항상 중앙 값을 계산에 추가합니다.
    float4 color = s_BlurWeights[g_BlurRadius] * InputImageTex.SampleLevel(g_SamBlur, texcoord, 0.0f);
    float totalWeight = s_BlurWeights[g_BlurRadius];

    float4 centerNormalDepth = NormalDepthTex.SampleLevel(g_SamBlur, texcoord, 0.0f);
    // 관찰 공간의 법선 벡터와 깊이를 분할합니다.
    float3 centerNormal = centerNormalDepth.xyz;
    float centerDepth = centerNormalDepth.w;

    for (int i = -g_BlurRadius; i <= g_BlurRadius; ++i)
    {
        // 가운데 값이 앞에 추가되었습니다.

        if (i == 0)
            continue;
#if defined BLUR_HORZ
        float2 offset = float2(g_TexelSize.x * i, 0.0f);
        float4 neighborNormalDepth = NormalDepthTex.SampleLevel(g_SamBlur, texcoord + offset, 0.0f);
#else
        float2 offset = float2(0.0f, g_TexelSize.y * i);
        float4 neighborNormalDepth = NormalDepthTex.SampleLevel(g_SamBlur, texcoord + offset, 0.0f);
#endif

        // 법선 벡터와 깊이 분할
        float3 neighborNormal = neighborNormalDepth.xyz;
        float neighborDepth = neighborNormalDepth.w;

        //
                // 중앙 값과 인접 값의 깊이 또는 법선 벡터 차이가 너무 크면 현재 샘플링 지점이 가장자리 영역에 있는 것으로 간주하고,
                // 따라서 현재 인접 값을 추가하는 것을 고려하지 마십시오.
                // 중앙 값이 직접 추가됩니다.
                //

        if (dot(neighborNormal, centerNormal) >= 0.8f && abs(neighborDepth - centerDepth) <= 0.2f)
        {
            float weight = s_BlurWeights[i + g_BlurRadius];

            // 인접 픽셀을 블러에 추가
#if defined BLUR_HORZ
            color += weight * InputImageTex.SampleLevel(g_SamBlur, texcoord + offset, 0.0f);
#else
            color += weight * InputImageTex.SampleLevel(g_SamBlur, texcoord + offset, 0.0f);
#endif
            totalWeight += weight;
        }

    }

    // 전체 가중치를 1로 만들어 드롭된 샘플 픽셀을 보상합니다.
    return color / totalWeight;
}


float4 DebugAO_PS(float4 posH : SV_position,
    float2 texCoord : TEXCOORD) : SV_Target
{
    float depth = DiffuseTex.Sample(g_sam_LinearWrap, texCoord).r;
    return float4(depth.rrr, 1.0f);
}


#endif
