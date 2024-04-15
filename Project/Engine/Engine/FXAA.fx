//**************************************************************************
// Modified by X_Jun(MKXJun)
// Source at https://github.com/NVIDIAGameWorks/Falcor/blob/master/Source/RenderPasses/Antialiasing/FXAA/FXAA.slang
// Licensed under the MIT License.
//
/***************************************************************************
 # Copyright (c) 2015-21, NVIDIA CORPORATION. All rights reserved.
 #
 # Redistribution and use in source and binary forms, with or without
 # modification, are permitted provided that the following conditions
 # are met:
 #  * Redistributions of source code must retain the above copyright
 #    notice, this list of conditions and the following disclaimer.
 #  * Redistributions in binary form must reproduce the above copyright
 #    notice, this list of conditions and the following disclaimer in the
 #    documentation and/or other materials provided with the distribution.
 #  * Neither the name of NVIDIA CORPORATION nor the names of its
 #    contributors may be used to endorse or promote products derived
 #    from this software without specific prior written permission.
 #
 # THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND ANY
 # EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 # IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 # PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 # CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 # EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 # PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 # PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 # OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 # (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 # OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/

#ifndef FXAA_HLSL
#define FXAA_HLSL


#define DEBUG_OUTPUT
#include "FullScreenTriangle.fx"
#include "value.fx"

#ifndef FXAA_QUALITY__PRESET
#define FXAA_QUALITY__PRESET 39
#endif

 //   FXAA 고중량 - 저중량, 중속
#if (FXAA_QUALITY__PRESET == 10)
#define FXAA_QUALITY__PS 3 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.5, 3.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 11)
#define FXAA_QUALITY__PS 4 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 3.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 12)
#define FXAA_QUALITY__PS 5 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 4.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 13)
#define FXAA_QUALITY__PS 6 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 4.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 14)
#define FXAA_QUALITY__PS 7 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 4.0, 12.0 };
#endif

#if (FXAA_QUALITY__PRESET == 15)
#define FXAA_QUALITY__PS 8 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 12.0 };
#endif

//   FXAA 품질 - 중간, 덜 떨림
#if (FXAA_QUALITY__PRESET == 20)
#define FXAA_QUALITY__PS 3 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.5, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 21)
#define FXAA_QUALITY__PS 4 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 22)
#define FXAA_QUALITY__PS 5 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 23)
#define FXAA_QUALITY__PS 6 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 24)
#define FXAA_QUALITY__PS 7 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 3.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 25)
#define FXAA_QUALITY__PS 8 
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 26)
#define FXAA_QUALITY__PS 9
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 27)
#define FXAA_QUALITY__PS 10
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 28)
#define FXAA_QUALITY__PS 11
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

#if (FXAA_QUALITY__PRESET == 29)
#define FXAA_QUALITY__PS 12
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif

//   FXAA 고품질
#if (FXAA_QUALITY__PRESET == 39)
#define FXAA_QUALITY__PS 12
static const float s_SampleDistances[FXAA_QUALITY__PS] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };
#endif


//
// 밝기 계산
//


float LinearRGBToLuminance(float3 LinearRGB)
{
    //return dot(LinearRGB, float3(0.299f, 0.587f, 0.114f));
    //return dot(LinearRGB, float3(0.212671f, 0.715160, 0.072169));
    //return sqrt(dot(LinearRGB, float3(0.212671f, 0.715160, 0.072169)));
    //return dot(LinearRGB, half3(0.2126729f, 0.7151522f, 0.0721750f));
    return log2(1 + dot(LinearRGB, float3(0.212671, 0.715160, 0.072169)) * 50) / 4;
}



/*--------------------------------------------------------------------------*/
/*
* - QualitySubPix
    선명도에 영향 
    1.00 - 부드러운
    0.75 - 기본 필터 값
    0.50 - 더 선명하고 하위 픽셀 앨리어싱을 덜 제거합니다.
    0.25 - 살짝 꺼짐
    0.00 - 완전히 꺼짐

* - QualityEdgeThreshold
    원하는 로컬 콘트라스트의 임계값 제어
    0.333 - 매우 낮음(더 빠름)
    0.250 - 저품질
    0.166 - 기본
    0.125 - 고품질
    0.063 - 매우 높음(느림)
    * - QualityEdfeThresholdMin

    어두운 영역을 처리하지 않는 임계값
    0.0833 - 기본
    0.0625 - 약간 더 빠르게
    0.0312 - 느리게
*/
/*--------------------------------------------------------------------------*/
#define PostProcessTarget           g_tex_0

#define DebugMode                   g_int_0
#define QualitySubPix               g_float_0
#define QualityEdgeThreshold        g_float_1
#define QualityEdfeThresholdMin     g_float_2

#define TexelSize                   g_vec2_0
/*--------------------------------------------------------------------------*/

struct PS_IN
{
    float4 posH     : SV_Position;
    float2 texcoord : TEXCOORD;
};

float4 PS_FXAA(PS_IN Input) : SV_Target
{
                
    float2 posM = Input.texcoord;
    float4 color = PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0);

    //   N
    // W M E
    //   S
    float lumaM = LinearRGBToLuminance(color.rgb);
    float lumaS = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(0, 1)).rgb);
    float lumaE = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(1, 0)).rgb);
    float lumaN = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(0, -1)).rgb);
    float lumaW = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(-1, 0)).rgb);

    //
    // 대비를 계산하고 앤티앨리어싱 적용 여부를 결정합니다.

    //

    // 대비를 얻기 위해 5개의 픽셀 중 최대/최소 상대 밝기를 찾습니다.
    float lumaRangeMax = max(lumaM, max(max(lumaW, lumaE), max(lumaN, lumaS)));
    float lumaRangeMin = min(lumaM, min(min(lumaW, lumaE), min(lumaN, lumaS)));
    float lumaRange = lumaRangeMax - lumaRangeMin;
    // 밝기 변화가 최대 밝기와 양의 상관관계가 있는 임계값 이하이거나 
    // 절대 임계값 이하이면 에지 영역에 있지 않음을 의미하며 
    // 안티 에일리어싱을 수행하지 않습니다.
    bool earlyExit = lumaRange < max(QualityEdfeThresholdMin, lumaRangeMax* QualityEdgeThreshold);

    // 임계값에 도달하지 않으면 조기 종료
    if (earlyExit)
        return color;

    //
    // 경계가 로컬로 수평인지 수직인지 결정
    //

    //           
    //  NW N NE          
    //  W  M  E
    //  WS S SE     
    //  edgeHorz = |(NW - W) - (W - WS)| + 2|(N - M) - (M - S)| + |(NE - E) - (E - SE)|
    //  edgeVert = |(NE - N) - (N - NW)| + 2|(E - M) - (M - W)| + |(SE - S) - (S - WS)|
    float lumaNW = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(-1, -1)).rgb);
    float lumaSE = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(1, 1)).rgb);
    float lumaNE = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(1, -1)).rgb);
    float lumaSW = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0, int2(-1, 1)).rgb);

    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;

    // 수평 및 수직 대비 계산
    float edgeHorz = abs(lumaNWSW - 2.0 * lumaW) + abs(lumaNS - 2.0 * lumaM) * 2.0 + abs(lumaNESE - 2.0 * lumaE);
    float edgeVert = abs(lumaSWSE - 2.0 * lumaS) + abs(lumaWE - 2.0 * lumaM) * 2.0 + abs(lumaNWNE - 2.0 * lumaN);

    // 지역 수평 경계인지 지역 수직 경계인지 판단
    bool horzSpan = edgeHorz >= edgeVert;

    //
    //기울기를 계산하고 경계의 방향을 결정합니다.
    //
    float luma1 = horzSpan ? lumaN : lumaW;
    float luma2 = horzSpan ? lumaS : lumaE;

    float gradient1 = luma1 - lumaM;
    float gradient2 = luma2 - lumaM;
    // 해당 방향의 기울기를 찾은 다음 후속 비교를 위해 크기를 조정합니다.
    float gradientScaled = max(abs(gradient1), abs(gradient2)) * 0.25f;
    // 어느 방향이 가장 가파른지
    bool is1Steepest = abs(gradient1) >= abs(gradient2);

    //
    // 현재 픽셀을 그라데이션 방향으로 절반 이동texel
    //
    float lengthSign = horzSpan ? TexelSize.y : TexelSize.x;
    lengthSign = is1Steepest ? -lengthSign : lengthSign;

    float2 posB = posM.xy;
    // 절반 텍셀 오프셋
    if (!horzSpan)
        posB.x += lengthSign * 0.5;
    if (horzSpan)
        posB.y += lengthSign * 0.5;

    //
    // posB에 인접한 두 픽셀의 휘도 평균을 계산합니다.
    //
    float luma3 = luma1 + lumaM;
    float luma4 = luma2 + lumaM;
    float lumaLocalAvg = luma3;
    if (!is1Steepest)
        lumaLocalAvg = luma4;
    lumaLocalAvg *= 0.5f;



    //
    //순회가 끝날 때까지 또는 에지가 아닌 지점에 도달할 때까지 양쪽을 순회합니다
    //

    // 테두리를 따라 양쪽으로 오프셋
    // 0    0    0
    // <-  posB ->
    // 1    1    1
    float2 offset;
    offset.x = (!horzSpan) ? 0.0 : TexelSize.x;
    offset.y = (horzSpan) ? 0.0 : TexelSize.y;
    // 음수 오프셋
    float2 posN = posB - offset * s_SampleDistances[0];
    // 포지티브 오프셋
    float2 posP = posB + offset * s_SampleDistances[0];

    //오프셋 지점에 대한 휘도 값을 구한 다음 중간 지점 휘도와의 차이를 계산합니다.
    float lumaEndN = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posN, 0).rgb);
    float lumaEndP = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posP, 0).rgb);
    lumaEndN -= lumaLocalAvg;
    lumaEndP -= lumaLocalAvg;

    // 끝점에서 루마의 차이가 로컬 그래디언트보다 크면 가장자리의 한쪽에 도달합니다.
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    bool doneNP = doneN && doneP;

    // 가장자리가 아닌 지점에 도달하지 않으면 해당 방향으로 계속 확장합니다.
    if (!doneN)
        posN -= offset * s_SampleDistances[1];
    if (!doneP)
        posP += offset * s_SampleDistances[1];

    // 양쪽이 가장자리의 한쪽 또는 반복 횟수에 도달할 때까지 계속 반복합니다.
    if (!doneNP)
    {
        [unroll] // [특성] : 실행이 중지될 때까지 루프를 풉니다. 선택적으로 루프가 실행되는 최대 횟수를 지정할 수 있습니다. [loop] 속성 과 호환되지 않습니다 .
        for (int i = 2; i < FXAA_QUALITY__PS; ++i)
        {
            if (!doneN)
                lumaEndN = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posN.xy, 0).rgb) - lumaLocalAvg;
            if (!doneP)
                lumaEndP = LinearRGBToLuminance(PostProcessTarget.SampleLevel(g_sam_LinearClamp, posP.xy, 0).rgb) - lumaLocalAvg;

            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            doneNP = doneN && doneP;

            if (!doneN)
                posN -= offset * s_SampleDistances[i];
            if (!doneP)
                posP += offset * s_SampleDistances[i];
            // 양쪽이 가장자리의 한쪽에 도달하면 중지
            if (doneNP)
                break;
        }
    }

    // 두 끝점까지의 거리를 개별적으로 계산
    float distN = horzSpan ? (posM.x - posN.x) : (posM.y - posN.y);
    float distP = horzSpan ? (posP.x - posM.x) : (posP.y - posM.y);

    // 어느 끝점이 현재 지점에서 더 가까운지 확인하고 거리를 측정합니다.
    bool directionN = distN < distP;
    float dist = min(distN, distP);

    // 두 끝 사이의 거리
    float spanLength = (distP + distN);

    // 가장 가까운 끝점을 향해 이동할 픽셀 오프셋
    float pixelOffset = -dist / spanLength + 0.5f;
    /*--------------------------------------------------------------------------*/

        // 현재 픽셀의 휘도가 posB의 인접한 두 픽셀의 휘도 평균값보다 작은지 여부
        bool isLumaMSmaller = lumaM < lumaLocalAvg;

        // 이것이 좋은 경계인지 결정
        bool goodSpanN = (lumaEndN < 0.0) != isLumaMSmaller;
        bool goodSpanP = (lumaEndP < 0.0) != isLumaMSmaller;
        bool goodSpan = directionN ? goodSpanN : goodSpanP;

        //그렇지 않은 경우 오프셋 없음
        float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
        /*--------------------------------------------------------------------------*/

            // 3x3 범위에서 픽셀의 밝기 변화 찾기
            //      [1  2  1]
            // 1/12 [2 -12 2]
            //      [1  2  1]
            float subpixNSWE = lumaNS + lumaWE;
            float subpixNWSWNESE = lumaNWSW + lumaNESE;
            float subpixA = (2.0 * subpixNSWE + subpixNWSWNESE) * (1.0 / 12.0) - lumaM;
            // 이 밝기 변화를 기반으로 서브픽셀 오프셋을 계산합니다.
            float subpixB = saturate(abs(subpixA) * (1.0 / lumaRange));
            float subpixC = (-2.0 * subpixB + 3.0) * subpixB * subpixB;
            float subpix = subpixC * subpixC * QualitySubPix;

            // 가장 큰 오프셋 선택
            float pixelOffsetSubpix = max(pixelOffsetGood, subpix);
            /*--------------------------------------------------------------------------*/

                if (!horzSpan)
                    posM.x += pixelOffsetSubpix * lengthSign;
                if (horzSpan)
                    posM.y += pixelOffsetSubpix * lengthSign;


                if (DebugMode)
                {
                    float3 Color = float3(1.0f - 2.0f * pixelOffsetSubpix, 2.0f * pixelOffsetSubpix, 0.0f);
                    return float4(Color, 1.0f);
                }
                else
                {
                    lumaM += 0.9f; // LumaM(휘도) 값이 높아질 수록 보간된 색이 짙어집니다. 
                    float3 Color = PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0).xyz;
                    return float4(Color, lumaM);
                }

          
    #ifdef DEBUG_OUTPUT
        float3 Color = float3(1.0f - 2.0f * pixelOffsetSubpix, 2.0f * pixelOffsetSubpix, 0.0f);
        return float4(Color, 1.0f);
    #else
        float3 Color = PostProcessTarget.SampleLevel(g_sam_LinearClamp, posM, 0).xyz;
        return float4(Color, lumaM);
    #endif
}


float4 PassThroughPS(float4 posH : SV_Position, float2 texCoord : TEXCOORD) : SV_Target
{
    uint2 pos = posH.xy;
    return PostProcessTarget[pos];
}

#endif
