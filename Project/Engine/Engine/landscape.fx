#ifndef _LANDSCAPE
#define _LANDSCAPE

#include "value.fx"
#include "func.fx"

// ================
// Landscape Shader
#define FaceXCount      g_int_0
#define FaceZCount      g_int_1  
// ================
struct VTX_IN
{
    float3 vPos : POSITION;    
    float2 vUV : TEXCOORD;    
};

struct VS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
    float2 vFullUV : TEXCOORD1;
    float3 vViewPos : POSITION1;        
};


VS_OUT VS_LandScape(VTX_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
       
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    output.vFullUV = _in.vUV / float2(FaceXCount, FaceZCount);
    
    output.vViewPos = mul(float4(output.vPos, 1.f), g_matWV).xyz;
        
    return output;
}


// Hull Shader (덮개 쉐이더)
struct PatchTessFactor // 패치 당 분할 레벨 값
{
    float EdgeParam[3] : SV_TessFactor;
    float InsideParam : SV_InsideTessFactor;
};

// 패치 상수 함수(Patch Constant Function) - 패치당 한번씩 실행되는 함수, 
// 패치를 어떻게 분할 할 것인지를 반환해 줘야 한다.
PatchTessFactor HS_PatchConstant(InputPatch<VS_OUT, 3> _Patch, uint _PatchIdx : SV_PrimitiveID)
{
    PatchTessFactor param = (PatchTessFactor) 0.f;
    
    float3 vViewSidePos = (_Patch[0].vViewPos + _Patch[2].vViewPos) / 2.f;
    float3 vViewUpDownPos = (_Patch[1].vViewPos + _Patch[2].vViewPos) / 2.f;
    float3 vViewSlidePos = (_Patch[0].vViewPos + _Patch[1].vViewPos) / 2.f;
            
    param.EdgeParam[0] = GetTessFactor(vViewUpDownPos, 1.f, 16.f, 1000.f, 5000.f);
    param.EdgeParam[1] = GetTessFactor(vViewSidePos, 1.f, 16.f, 1000.f, 5000.f);
    param.EdgeParam[2] = GetTessFactor(vViewSlidePos, 1.f, 16.f, 1000.f, 5000.f);
    param.InsideParam = param.EdgeParam[2];     // 빗변의 레벨을 따른다.
    
    return param;
}



[domain("tri")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HS_PatchConstant")]
[maxtessfactor(64.0)]
//[partitioning("integer")] 
[partitioning("fractional_odd")]
VS_OUT HS_LandScape(InputPatch<VS_OUT, 3> _Patch, uint _Idx : SV_OutputControlPointID, uint _PatchIdx : SV_PrimitiveID)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPos = _Patch[_Idx].vPos;
    output.vUV = _Patch[_Idx].vUV;    
    output.vViewPos = _Patch[_Idx].vViewPos;
    output.vFullUV = _Patch[_Idx].vFullUV;
    return output;
}


// -----> Tessellator



// Domain Shader
struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD; 
    float2 vFullUV : TEXCOORD1;
    float3 vViewPos : POSITION; 
    
    float3 vViewTangent : TANGENT;
    float3 vViewBinormal : BINORMAL;
    float3 vViewNormal : NORMAL;
};


[domain("tri")]
DS_OUT DS_LandScape(float3 _vLocation : SV_DomainLocation, const OutputPatch<VS_OUT, 3> _Patch, PatchTessFactor _param)
{
    DS_OUT output = (DS_OUT) 0.f;

    // 기존 정점들의 위치와 비율을 이용해 생성된 정점들의 LocalPos, UV를 계산한다.
    float3 vLocalPos = _Patch[0].vPos * _vLocation[0] + _Patch[1].vPos * _vLocation[1] + _Patch[2].vPos * _vLocation[2];
    output.vUV = _Patch[0].vUV * _vLocation[0] + _Patch[1].vUV * _vLocation[1] + _Patch[2].vUV * _vLocation[2];      
    output.vFullUV = _Patch[0].vFullUV * _vLocation[0] + _Patch[1].vFullUV * _vLocation[1] + _Patch[2].vFullUV * _vLocation[2];
    
    // 투영좌표계까지 연산
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);    
    
    return output;
}


struct PS_OUT
{
    float4 vColor       : SV_Target0;
    float4 vViewNormal  : SV_Target1;
    float4 vViewPos     : SV_Target2;
    float4 vData        : SV_Target3;
};

PS_OUT PS_LandScape(DS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    output.vColor = float4(0.8f, 0.8f, 0.8f, 1.f);        
    
    float3 vViewNormal = _in.vViewNormal;    
 
    output.vViewPos = float4(_in.vViewPos, 1.f);    
    output.vViewNormal = float4(vViewNormal, 1.f);        
    
    return output;
}




#endif