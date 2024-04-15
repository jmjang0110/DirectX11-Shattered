#pragma once
#include "CRenderComponent.h"

#include "CStructuredBuffer.h"
#include "CParticleUpdateShader.h"

class CSound;

class CParticleSystem : public CRenderComponent
{
private:
	Ptr<CParticleUpdateShader> m_CS; // 파티클 업데이트 쉐이더

	CStructuredBuffer* m_ParticleBuffer; // 파티클 구조화 버퍼
	CStructuredBuffer* m_DataBuffer;     // 추가 데이터 버퍼(파티클 AliveCount)
	UINT               m_iMaxCount;      // 파티클 최대 개수

	int m_bPosInherit; // 오브젝트 좌표에 영향
	int m_iAliveCount; // 이번 프레임에 생성(활성화) 될 파티클 수

	float m_fMinLifeTime; // 파티클의 최소 생명 시간
	float m_fMaxLifeTime; // 파티클의 최대 생명 시간

	float m_fStartSpeed; // 파티클의 최소 속력
	float m_fEndSpeed;   // 파티클의 최대 속력

	Vec4 m_vStartColor; // 파티클 초기 색상
	Vec4 m_vEndColor;   // 파티클 최종 색상

	Vec4 m_vStartEmissiveColor; // 파티클 초기 Emissive 색상
	Vec4 m_vEndEmissiveColor;   // 파티클 최종 Emissive 색상

	Vec3 m_vStartScale; // 파티클 초기 크기
	Vec3 m_vEndScale;   // 파티클 최종 크기


	float m_fParticleCreateDistance; // 파티클 생성 범위
	float m_fParticleCreateTerm;     // 파티클 생성 간격
	float m_fAccTime;

	float	m_fAngle;				// 파티클 사출 각도
	int m_iSpeedDetail_Func;

	bool	m_bLinearParicle;
	bool	m_bUseSoftParticle;
	bool	m_bUseEmissive;

	int m_iEmissive; // 파티클 발광여부
	int m_iLighting; // 광원 적용 여부

	Vec2 m_vDirection;

	float m_fLifeTime;

	wstring			m_strSoundName;
	CSound* m_pSound;

	bool m_bPlayOnce;

public:
	void                       SetEmissive(bool _bEmissive);
	void                       SetApplyLighting(bool _bLighting);
	void                       SetComputeShader(Ptr<CParticleUpdateShader> pCS) { m_CS = pCS; }
	Ptr<CParticleUpdateShader> GetComputeShader() const { return m_CS; }

	void SetMaxParticleCount(UINT count);
	UINT GetMaxParticleCount() const { return m_iMaxCount; }

	void SetPosInherit(bool _enable) { m_bPosInherit = _enable; }
	bool PosInherit() const { return m_bPosInherit; }

	void SetAliveCount(UINT _iCount);
	UINT GetAliveCount() const { return m_iAliveCount; }

	void SetMinMaxLifeTime(float fMin, float fMax);
	Vec2 GetMinMaxLifeTime() const { return Vec2{ m_fMinLifeTime, m_fMaxLifeTime }; }

	void SetMinMaxSpeed(float fMin, float fMax);
	Vec2 GetMinMaxSpeed() const { return Vec2{ m_fStartSpeed, m_fEndSpeed }; }

	void                  SetStartEndColor(const Vec4& vStartColor, const Vec4& vEndColor);
	std::pair<Vec4, Vec4> GetStartEndColor() const { return std::pair<Vec4, Vec4>{m_vStartColor, m_vEndColor}; }

	void                  SetStartEndEmissiveColor(const Vec4& vStartColor, const Vec4& vEndColor);
	std::pair<Vec4, Vec4> GetStartEndEmissiveColor() const { return std::pair<Vec4, Vec4>{m_vStartColor, m_vEndColor}; }

	void                  SetStartEndScale(const Vec3& vStartScale, const Vec3& vEndScale);
	std::pair<Vec3, Vec3> GetStartEndScale() const { return std::pair<Vec3, Vec3>{m_vStartScale, m_vEndScale}; }

	void  SetRange(float _fRange);
	float GetRange() const { return m_fParticleCreateDistance; }

	void  SetAngle(float _angle) { m_fAngle = _angle; }
	const float GetAngle() const { return m_fAngle; }

	void  SetTerm(float _fTerm);
	float GetTerm() const { return m_fParticleCreateTerm; }

	void SetDirection(Vec2 direction) { m_vDirection = direction; }
	Vec2 GetDirection() const { return m_vDirection; }

	void SetSpeedDetailFunc(int _funcNum) { m_iSpeedDetail_Func = _funcNum; }
	int GetSpeedDetailFunc() { return m_iSpeedDetail_Func; }

	void SetLinearParticle(bool _TorF) { m_bLinearParicle = _TorF; }
	bool GetLinearParticle() { return m_bLinearParicle; }

	void SetSoftParticle(bool _TorF) { m_bUseSoftParticle = _TorF; }
	bool GetSoftParticle() { return m_bUseSoftParticle; }

	void SetUseEmissive(bool _TorF) { m_bUseEmissive = _TorF; }
	bool GetUseEmissive() { return m_bUseEmissive; }

	void SetSoundName(wstring _name)
	{
		m_strSoundName = _name;
		SetSound(_name);
	}
	void SetSound(wstring _name);
	wstring GetSoundName() { return m_strSoundName; }

	void SetMaterial(wstring _mtrl);
	void SetParticlePlayOneTime() { m_bPlayOnce = true; }
	void SetLifeTime(float _time) { m_fLifeTime = _time; }

public:
	virtual void start() override;
	virtual void finalupdate() override;
	virtual void render() override;

public:
	void SaveParticle(FILE* _pFile);
	void LoadParticle(FILE* _pFile);


public:
	virtual void SaveToScene(FILE* _pFile) override;
	virtual void LoadFromScene(FILE* _pFile) override;
	CLONE(CParticleSystem);

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;
public:
	CParticleSystem();
	CParticleSystem(const CParticleSystem& _origin);
	virtual ~CParticleSystem();
};
