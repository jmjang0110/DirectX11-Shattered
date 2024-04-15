#pragma once

enum class CATMULL_ROM_SPLINE
{
    P0,
    P1,
    P2,
    P3,
    END,

};
/*

*                       →→→
*                   ↗       ↘
*       t = 0.f                      t = 1.f
         (start)  [P1]          [P2] ( End )
                     ↘      ↗
                        ↘ ↗
                        ↗ ↘
                      ↗     ↘
                    ↗         ↘
                  ↗             ↘
             [P0]                  [P3]
     t = 0.f                           t = 1.f
*/


class CatmullRomSpline
{
public:
    Vec3  m_SplinePos[(UINT)CATMULL_ROM_SPLINE::END];       // 정점 위치 저장 P0, P1, P2, P3
    float m_fInterPolation_Control_Coefficient = 0.f;       // 보간 제어 계수 
    float m_fSpeed = 0.2f;
    bool  m_bIsEnd = false;
    bool  m_bStart = false;


public:
    // [ SET ]
    void SetP0(Vec3 _p0) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P0] = _p0; }
    void SetP1(Vec3 _p1) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1] = _p1; }
    void SetP2(Vec3 _p2) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2] = _p2; }
    void SetP3(Vec3 _p3) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P3] = _p3; }
    void SetStartPos(Vec3 _vStart) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1] = _vStart; }
    void SetEndPos(Vec3 _vEnd) { m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2] = _vEnd; }
    void SetSpeed(float _fSpeed) { m_fSpeed = _fSpeed; }

    bool IsEnd() { return m_bIsEnd; }
    bool IsStart() { return m_bStart; }

    // [ GET ]
    Vec3 GetP0() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P0]; }
    Vec3 GetP1() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P1]; }
    Vec3 GetP2() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P2]; }
    Vec3 GetP3() { return m_SplinePos[(UINT)CATMULL_ROM_SPLINE::P3]; }

    // [ CREATE ]
    // 경로 지정 생성 
    void CreateRoute(Vec3 _p0, Vec3 _p1, Vec3 _p2, Vec3 _p3);



    // 캣멀롬 스플라인 결과값 계산  
    Vec3 CalculatePosByCatmullRom();
    // 보간 제어 계수값 업데이트 
    void Update();

    float GetAngleRadian(Vec3 _v1, Vec3 _v2);


public:
    CatmullRomSpline();
    ~CatmullRomSpline();
};

