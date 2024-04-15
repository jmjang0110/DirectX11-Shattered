#pragma once
#include "UI.h"

class CParticleSystem;
class CSound;

class ParticleTool : public UI
{
private:
	Ptr<CTexture> m_pParticleTarget;
	Ptr<CTexture> m_pEmissiveTarget;
	Ptr<CTexture> m_pDepthTarget;

	vector<CGameObject*> m_vecParticleSystem;
	CGameObject* m_pTargetParticle;
	vector<string>       m_strvecParticleName;

	Vec2 m_vecTargetResolution;

	int  m_iMaxCount;   // 파티클 최대 개수
	bool m_bPosInherit; // 오브젝트 좌표에 영향
	int  m_iAliveCount; // 이번 프레임에 생성(활성화) 될 파티클 수

	float m_fArrMinMaxTime[2];    // 파티클의 최소/최대 생명 시간
	float m_fArrMinMaxSpeed[2];   // 파티클의 최소/최대 속력
	Vec4  m_vArrStartEndColor[2]; // 파티클 초기/최종 색상
	Vec3  m_vArrStartEndScale[2]; // 파티클 초기/최종 크기
	float m_fArrDirection[2];     // 파티클 방향
	Vec4  m_vArrStartEndEmissive[2]; // 파티클 초기/최종 Emissive

	float m_fRange; // 파티클 생성 범위
	float m_fTerm;  // 파티클 생성 간격
	float m_fAngle;

	bool m_bShaderUseEmissive;
	bool m_bShaderUseSpeedDetail;
	bool m_bLinerParticle;
	bool m_bUseSoftParticle;

	int m_iParticleComboIDX;

	wstring m_strSoundName;
	CSound* m_pSound;

private:
	void SetData();
	void TextureSelect(void* _pTextureName);

public:
	void ParticleCreate();
	void ParticleErase();
	void ParticleSaveNLoad();
	void AddParticleCombo(CGameObject* _newparti);
	void MakeTargetParticleCombo();
	void ParticleOptionSetting();
	void SaveParticle();
	void LoadParticle(CGameObject* _load);
	void SetSound();
	void SoundLoad();

public:
	void update() override;
	void render_update() override; //


public:
	ParticleTool();
	virtual ~ParticleTool();
};
