#pragma once
#include "CEntity.h"

#define GET_COMPONENT(type, TYPE)  class C##type* type() { return (C##type*)m_arrCom[(UINT)COMPONENT_TYPE::TYPE]; }

class CUIBase;
class CComponent;
class CTransform;
class CCollider2D;
class CAnimator2D;
class CAnimator3D;
class CCamera;
class CLight2D;
class CLight3D;
class CMeshRender;
class CTimeMap;
class CParticleSystem;
class CSkyBox;
class CDecal;
class CScript;
class CRenderComponent;
class CLandScape;
class CCollider3D;
class CBoundingBox;
class CFSM;
class CRigiBody;
class CNaviMap;
class CNaviAgent;
class CUIImage;
class CUIText;
class CUIPanel;
class CUIProgressBar;
class CUIButton;

class CGameObject : public CEntity
{
private:
	vector<CGameObject*> m_vecChild;
	vector<CScript*>     m_vecScript;

	CComponent*       m_arrCom[static_cast<UINT>(COMPONENT_TYPE::END)];
	CRenderComponent* m_pRenderComponent;
	CUIBase*          m_pUIBase;

	CGameObject* m_pParent;
	int          m_iLayerIdx; // 게임 오브젝트 소속 레이어 인덱스   
	bool         m_bDead;

	bool m_bActive;
	bool m_bDynamicShadow;  // 동적 그림자 생성
	bool m_bFrustumCulling; // 절두체 컬링 사용 유무

public:
	void start();
	void update();
	void lateupdate();
	void finalupdate();
	void finalupdate_module();
	void render();

private:
	void active() const;
	void deactive() const;

	/**
	 * \brief 계층관계가 깊게 형성된 게임오브젝트의 레이어를 모두 반영하기 위한 함수
	 * \param _pInnerChild 자식의 자식
	 */
	void CheckLayerRecursive(const CGameObject* _pInnerChild);

public:
	int  GetLayerIndex() const { return m_iLayerIdx; }
	void RenewLayerIndex(int _NewLayerIdx);

	// Deregister ==> 등록 취소(등록->미등록)
	// Unregister ==> 등록 안됨(등록 x == 등록->미등록, 애초에 등록된적 없음)
	void Deregister();
	void DisconnectBetweenParent();
	void Activate();
	void Deactivate();

	bool IsDead() const { return m_bDead; }
	bool IsActive() const { return m_bActive; }

public:
	void                        AddChild(CGameObject* _pChild);
	CGameObject*                GetParent() const { return m_pParent; }
	const vector<CGameObject*>& GetChild() const { return m_vecChild; }
	CGameObject*                GetChild(const std::wstring& childName) const;
	CGameObject*                FindChild(wstring _name) const;
	void                        SortChild(std::function<bool(CGameObject*, CGameObject*)> func);
	bool                        IsAncestor(const CGameObject* _pObj) const;

public:
	GET_COMPONENT(Transform, TRANSFORM)
	GET_COMPONENT(MeshRender, MESHRENDER)
	GET_COMPONENT(Camera, CAMERA)
	GET_COMPONENT(Collider2D, COLLIDER2D)
	GET_COMPONENT(Animator2D, ANIMATOR2D)
	GET_COMPONENT(Animator3D, ANIMATOR3D)
	GET_COMPONENT(TileMap, TILEMAP)
	GET_COMPONENT(ParticleSystem, PARTICLESYSTEM)
	GET_COMPONENT(SkyBox, SKYBOX)
	GET_COMPONENT(Light2D, LIGHT2D)
	GET_COMPONENT(Light3D, LIGHT3D)
	GET_COMPONENT(Decal, DECAL)
	GET_COMPONENT(LandScape, LANDSCAPE)
	GET_COMPONENT(Collider3D, COLLIDER3D);
	GET_COMPONENT(BoundingBox, BOUNDINGBOX);
	GET_COMPONENT(FSM, FSM);
	GET_COMPONENT(RigidBody, RIGIDBODY);
	GET_COMPONENT(NaviMap, NAVIMAP);
	GET_COMPONENT(NaviAgent, NAVIAGENT);
	GET_COMPONENT(UIImage, UIIMAGE);
	GET_COMPONENT(UIButton, UIBUTTON);
	GET_COMPONENT(UIPanel, UIPANEL);
	GET_COMPONENT(UIProgressBar, UIPROGRESSBAR);
	GET_COMPONENT(UIText, UITEXT);

	void              AddComponent(CComponent* _component);
	CComponent*       GetComponent(COMPONENT_TYPE _eType) const { return m_arrCom[static_cast<UINT>(_eType)]; }
	CRenderComponent* GetRenderComponent() const { return m_pRenderComponent; }
	CUIBase*          GetUIBaseComponenent() const { return m_pUIBase; }

	void DeleteComponent(COMPONENT_TYPE _eType);
	void DeleteScript(UINT _ScriptID);

	void Destroy();

	template <typename T>
	T* GetScript();

	const vector<CScript*>& GetScripts() { return m_vecScript; }
	CScript*                GetScript(UINT _iIdx);
	CScript*                GetScriptByName(const wstring& _strName) const;

public:
	void SaveToScene(FILE* _pFile) override;
	void LoadFromScene(FILE* _pFile) override;

public:
	void Serialize(YAML::Emitter& emitter) override;
	void Deserialize(const YAML::Node& node) override;

public:
	CLONE(CGameObject);
	CGameObject();
	CGameObject(const CGameObject& _origin);
	virtual ~CGameObject();

	friend class CEventMgr;
	friend class CScene;
	friend class CLayer;
	friend class CCollider3D;
};

template <typename T>
T* CGameObject::GetScript()
{
	for (size_t i = 0; i < m_vecScript.size(); ++i)
	{
		T* pScript = dynamic_cast<T*>(m_vecScript[i]);
		if (nullptr != pScript)
			return pScript;
	}

	return nullptr;
}
