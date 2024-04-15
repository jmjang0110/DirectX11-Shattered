#include "pch.h"
#include "CatmullRomSpline.h"
#include <Engine/CtimeMgr.h>


CatmullRomSpline::CatmullRomSpline()
{
}

CatmullRomSpline::~CatmullRomSpline()
{
}


void CatmullRomSpline::CreateRoute(Vec3 _p0, Vec3 _p1, Vec3 _p2, Vec3 _p3)
{

    SetP0(_p0);
    SetP1(_p1);
    SetP2(_p2);
    SetP3(_p3);

}

float CatmullRomSpline::GetAngleRadian(Vec3 _v1, Vec3 _v2)
{
    Vec3 vFrom = _v1;
    vFrom.Normalize();

    Vec3 vTo = _v2;
    vTo.Normalize();

    float fCos = vFrom.Dot(vTo);
    if (fCos >= 1.f)  fCos = 1.f;
    if (fCos <= -1.f) fCos = -1.f;

    float fRad = acosf(fCos);					// 사이각 ( Radian )
    float fAng = fRad * (180.f / XM_PI);		// 사이각 ( Angle )	}

    return fRad;
}


Vec3 CatmullRomSpline::CalculatePosByCatmullRom()
{
    Vec3 ResultPos = Vec3(0.f, 0.f, 0.f);
    Vec3 P0 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P0];	// P0 위치 - P0 --> P2 
    Vec3 P1 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1];	// P1 위치 - Start ( 시작 위치 ) t = 0.f
    Vec3 P2 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2];	// P2 위치 - End   ( 끝 위치   ) t = 1.f
    Vec3 P3 = m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P3];	// P3 위치 - P1 --> P3 
    float t = m_fInterPolation_Control_Coefficient;			// t  보간 제어 계수
    ResultPos = XMVectorCatmullRom(P0, P1, P2, P3, t); // Dx11 DirectX Math 캣멀롬 스플라인 결과값

    return ResultPos;
}

void CatmullRomSpline::Update()
{
    if (!m_bStart)
        return;
    // End 지점 도착 
    if (m_fInterPolation_Control_Coefficient >= 1.f)
    {
        m_fInterPolation_Control_Coefficient = 1.f;
        m_bIsEnd = true;
    }
    // Start 지점 
    else if (m_fInterPolation_Control_Coefficient <= 0.f)
    {
        m_fInterPolation_Control_Coefficient = 0.f;
    }

    // P1 -------> P2
    // t : ( Start ) 0.f ------ 1.f ( End )
    m_fInterPolation_Control_Coefficient += DT * m_fSpeed;

}

