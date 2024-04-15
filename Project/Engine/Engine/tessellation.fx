#ifndef _TESS
#define _TESS

#include "value.fx"

// ========================
// Tessellation Test Shader
// DOMAIN   : Forward
// RS_TYPE  : Wire Frame
// DS_TYPE  : Default
// BS_TYPE  : Default
// ========================
struct VTX_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VTX_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

VTX_OUT VS_Tess(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPos = _in.vPos;
    output.vUV = _in.vUV;
    
    return output;
}


// 패치 상수 함수
// 패치 당 수행되는 함수
struct PatchTessFactor
{
    float EdgeFactor[3] : SV_TessFactor;
    float InsideFactor : SV_InsideTessFactor;
};

PatchTessFactor PatchConstFunc(InputPatch<VTX_OUT, 3> _in, uint _patchID : SV_PrimitiveID)
{
    PatchTessFactor factor = (PatchTessFactor) 0.f;
    
    factor.EdgeFactor[0] = 8.f;
    factor.EdgeFactor[1] = 8.f;
    factor.EdgeFactor[2] = 8.f;    
    factor.InsideFactor = 16.f;
    
    return factor;
}


// 정점 당 수행되는 함수
[domain("tri")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchConstFunc")]
[maxtessfactor(64.f)]
[partitioning("integer")] // [partitioning("fractional_odd")]
VTX_OUT HS_Tess(InputPatch<VTX_OUT, 3> _in
            , uint _VtxID : SV_OutputControlPointID
            , uint _patchID : SV_PrimitiveID)
{
    VTX_OUT output = (VTX_OUT) 0.f;

    output.vPos = _in[_VtxID].vPos;
    output.vUV = _in[_VtxID].vUV;
    
    return output;
}

// Tessellator
// HullShader 에서 전달한 정보를 토대로 정점을 생성 시키는 단계
// 생성된 정점을에 대해서 Domain Shader 를 호출 시킨다.

struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

[domain("tri")]
DS_OUT DS_Tess(OutputPatch<VTX_OUT, 3> _OriginPatch, float3 _Ratio : SV_DomainLocation, PatchTessFactor _factor)
{
    DS_OUT output = (DS_OUT) 0.f;

    float3 vLocalPos =    _OriginPatch[0].vPos * _Ratio[0] 
                        + _OriginPatch[1].vPos * _Ratio[1] 
                        + _OriginPatch[2].vPos * _Ratio[2];
    
    float2 vUV =  _OriginPatch[0].vUV * _Ratio[0]
                + _OriginPatch[1].vUV * _Ratio[1]
                + _OriginPatch[2].vUV * _Ratio[2];
    
    output.vPosition = mul(float4(vLocalPos, 1.f), g_matWVP);
    output.vUV = vUV;
    
    
    return output;
}




float4 PS_Tess(DS_OUT _in) : SV_Target
{
    float4 vOutColor = float4(1.f, 0.f, 1.f, 1.f);    
    return vOutColor;
}

#endif
