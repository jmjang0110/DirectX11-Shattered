#pragma once
#include "UI.h"

class CGameObject;

class FogTool :
    public UI
{
private:
    CGameObject* m_pFog;
    bool m_bFogOn;
    Vec4 m_vFogColor;

    CGameObject* m_pParticleEmissive;
    bool m_bParticleEmissiveOn;

public:
    void SetData();

public:
    virtual void update() override;
    virtual void render_update() override;


public:
    FogTool();
    ~FogTool();
};

