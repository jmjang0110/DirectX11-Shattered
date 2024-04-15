#pragma once
#include "CRenderComponent.h"

enum class DECAL_TYPE
{
	SPHERE,
	CUBE,
	END
};

class CDecal : public CRenderComponent
{
private:
	DECAL_TYPE    m_eType;
	bool          m_bUseDeferredLighting;
	Ptr<CTexture> m_pDecalTex;
	Vec4          m_vColor;	// 0 ~ 255

public:
	void SetDecalType(DECAL_TYPE _eType);
	void SetDeferredLighting(bool _bLighting);
	void SetDecalTexture(Ptr<CTexture> _pTex) { m_pDecalTex = _pTex; }
	void SetColor(Vec4 _vColor) { m_vColor = _vColor; }

	DECAL_TYPE    GetDecalType() const { return m_eType; }
	std::string   GetDecalTypeName(DECAL_TYPE _type) const;
	Ptr<CTexture> GetDecalTexture() { return m_pDecalTex; }
	Vec4          GetColor() const { return m_vColor; }
	bool          IsUsingDeferredLighting() const { return m_bUseDeferredLighting; }

public:
	void finalupdate() override;
	void finalupdate_debug() override;
	void UpdateData() override;
	void render() override;
	void render_debug() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CDecal);
	CDecal();
	virtual ~CDecal();
};
