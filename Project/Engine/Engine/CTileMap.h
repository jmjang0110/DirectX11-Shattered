#pragma once
#include "CRenderComponent.h"

#include "CTexture.h"

class CTileMap : public CRenderComponent
{
private:
	Ptr<CTexture> m_pAtlasTex;
	Vec2          m_vSlicePixel;
	Vec2          m_vSliceUV;

	UINT m_iRowCount;
	UINT m_iColCount;

	UINT               m_iTileCountX;
	UINT               m_iTileCountY;
	vector<tTileData>  m_vecTileData;
	CStructuredBuffer* m_pBuffer;

	bool m_bBufferUpdated;

public:
	void SetAtlasTex(Ptr<CTexture> _pAtlasTex)
	{
		m_pAtlasTex = _pAtlasTex;
		m_vSliceUV  = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
	}

	void SetTileSize(Vec2 _vPixelSize)
	{
		m_vSlicePixel = _vPixelSize;
		if (nullptr != m_pAtlasTex)
			m_vSliceUV = m_vSlicePixel / Vec2(m_pAtlasTex->Width(), m_pAtlasTex->Height());
	}

	void SetTileMapCount(UINT _iCountX, UINT _iCountY);
	void SetTileData(int _iTileIdx, int _iImgIdx);
	void ClearTileData();

public:
	void finalupdate() override;
	void render() override;
	void UpdateData() override;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CTileMap);
	CTileMap();
	CTileMap(const CTileMap& _origin);
	virtual ~CTileMap();
};
