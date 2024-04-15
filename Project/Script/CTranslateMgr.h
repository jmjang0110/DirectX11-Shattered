#pragma once
#include <Engine/CEntity.h>
#include "CObjKeyMgr.h"

/*
    Player의 이동을 위해 생성된 클래스 입니다.
    외부에서 Init을 한 후에 동작해야 합니다.

    키를 받아 이동하는 모션이 MoveState 이외( ex) JumpState,,)에도 쓰여서 
    TranslateMgr 을 따로 만들었습니다. 

    현재 이 클래스는 플레이어 전용으로 만들었습니다. 
*/


// player 위치 관련 함수 정의 
class CTransform;

struct tAxis
{
    /*
    * ( x / z )
    * [QuadNum] / Direction / Angle

     [4]        Front 0       [1]
                  ↑
       -90 Left ←   → Right 90
                  ↓
     [3]        Back 180      [2]

    */
    Vec3    vFrontAxis;
    Vec3    vRightAxis;
    Vec3    vLeftAxis;
    Vec3    vBackAxis;
};

class CTranslateMgr :
    public CEntity
{
private:
	CGameObject* m_pOwner;               // Player 저장 
	CGameObject* m_pCamera;              // Cmaera 저장
	CGameObject* m_pCurTri;              // 현재 PolyGon
	CScript*     m_pScript;              // PlayerScript 를 저장한다. 
	CTransform*  m_pTransform;           // Player 의 Transform Component Pointer


    tAxis               m_vCamAxis;             // 카메라       모든 방향 벡터  
    Vec3                m_vObjForwardAxis;      // 플레이어     앞   방향 벡터

    tKey_Zip            m_tCurKeyInfo;          // 눌린 키 정보
    bool                m_bKeyUpdateFinish;     // 키 처리가 중복 되지 않기 위해 키 처리가 끝남을 알린다. 

private:
    // [ 플레이어 회전 관련 정보 ]
    float               m_fRotSpeed;            // 플레이어의 회전 속도 
    float               m_fRotRadian;           // 현재 카메라와 플레이어 사이 각 (Radian)
    float               m_fRotAngle;            // 현재 카메라와 플레이어 사이 각 (Angle)

    float               m_fMoveAccRot;          // 목표 각도까지 움직여야할 남은 각도 
    float               m_fDefaultAngle;        // 정면이 등을 보이게 하는데 필요한 초기 각도

    bool                m_bEqualizePlayerAxis_CamForward;       // Player 를 무조건 Camera가 바라보는 방향으로 회전 시킨다

    Vec3                m_vPrevPos;
    Vec3                m_vCurPos;


public:
    void    Init(CGameObject* _pOwner, CGameObject* _pCamera, CScript* _pPlayerScript);
    void	Update();
    void	LateUpdate();

    // APPLY TO PLAYER OBJ
    void Translate(const Vec3& direction, float magnitude);  //특정 방향으로 이동
    void TranslateByRigidBody(const Vec3& direction, float magnitude);


    void Strafe(float distance);                            //  [ Strafing ] 현재 앞 방향 기준 좌/우 이동
    Vec3 Walk(float distance);                              //  수평 지면에서 전방/후방 이동(1인칭 시점)

    void RotateBody();
    void RotatePlayerFront(Vec3 vDir);                      // Player 가 매개변수로 들어온 방향을 앞을 바라보게 회전한다. 
    bool CheckRotateFinish(float _fRot_DT = 0.f);           // 플레이어의 회전이 목표 회전각에 도달했는지 확인한다. 


    void UpdateMovePlayer();
    void UpdateCameraAxis();                                // 카메라의 축벡터를 갱신한다. 
    void UpdateCameraPlayerAngle();                         // 카메라기준으로 플레이어 사이각을 갱신한다. 
    int  GetQuadrantNum(float _fRad);                       // 현재 각도에 따른 면 번호를 반환한다. - 위 tAxis 그림 참고 


    float GetAngleBetweenVector(Vec3 _vStandard, Vec3 _vTarget);     // 두 벡터 사이각을 구한다. ( Angle )
    float GetRadianBetweenVector(Vec3 _vStandard, Vec3 _vTarget);    // 두 벡터 사이각을 구한다. ( Radian )
    Vec3  GetDiagnosisBetweenVector(Vec3 _vA, Vec3 _vB);             // 두 벡터 사이 중간 벡터 ( 대각선 벡터 )를 구한다.


    // 각 키에 따라 회전한다.
    void CheckForwardKey();
    void CheckBackwardKey();
    void CheckRightKey();
    void CheckLeftKey();

    void SetEqualize_Player_and_Camera(bool _b) { m_bEqualizePlayerAxis_CamForward = _b; }

    Vec3 GetMoveDir();


    // CALCULATE POSITION
public:
    // [ SET PART ]
    //  [ Scale ]
    void SetScale(const Vec3& scale);                       //객체 스케일 설정
    void SetScale(float x, float y, float z);               //객체 스케일 설정

    // [ Rotation ]
    void SetRotation(const Vec3& eulerAnglesInRadian);      //오브젝트 오일러 각도 설정(라디안).객체는 Z-X-Y 순서로 회전합니다.
    void SetRotation(float x, float y, float z);

    void Rotate(const Vec3& eulerAnglesInRadian);                                //오일러 각도 회전 객체 지정
    void RotateAxis(const Vec3& axis, float radian);                             //축을 중심으로 회전할 중심점을 원점으로 지정
    void RotateAround(const Vec3& point, const Vec3& axis, float radian);        //축을 중심으로 회전할 점 지정

    // [ Position ]
    void SetPosition(const Vec3& position);                                     //객체 위치 설정
    void SetPosition(float x, float y, float z);

    void SetCurKeyInfo(tKey_Zip _keyInfo);
    void SetDefaultAngle(float _fAngle) { m_fDefaultAngle = _fAngle; }

	// [ POLYGON]
	void SetCurTri(CGameObject* _pObj) { m_pCurTri = _pObj; }
    
    // [ GET PART ]
public:
    // [ Scale ]
    Vec3 GetScale() const;           //객체의 크기를 가져옵니다.

    // [ Rotation ]
    Vec3 GetRotation() const;        //객체의 오일러 각도를 가져옵니다(라디안).객체는 Z-X-Y 순서로 회전됩니다.
    Vec3 GetPosition() const;        //객체의 위치를 ​​얻는다

    // [ Axis ]
    Vec3 GetRightAxis() const;       // 오른쪽 축 
    Vec3 GetUpAxis() const;          // 위쪽 축 
    Vec3 GetForwardAxis(Vec3 vOffSetRot = Vec3(0.f, 0.f, 0.f)) const;     // 앞 축 

    // [ Matrix ]
    Matrix GetLocalToWorldMatrix() const;    // 세계 변환 행렬을 가져옵니다.
    Matrix GetWorldToLocalMatrix() const;    // 역 세계 변환 행렬을 가져옵니다.

    float GetDefaultAngle() { return m_fDefaultAngle; }


    // 돌아야할 각이 음수 양수 인지 구한다. 
    int GetRotateDirection(int _iQuadNum, float fAngle_Final);


private:
    Vec3 GetEulerAnglesFromRotationMatrix(const Matrix& rotationMatrix);    //회전 행렬에서 회전 오일러 각도를 가져옵니다.


    CLONE(CTranslateMgr)
public:
    CTranslateMgr();
    CTranslateMgr(const CTranslateMgr& _origin);
    virtual ~CTranslateMgr();

};

