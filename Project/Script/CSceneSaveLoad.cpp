#include "pch.h"
#include "CSceneSaveLoad.h"

#include <Engine/CScene.h>
#include <Engine/CLayer.h>
#include <Engine/CGameObject.h>
#include <Engine/CPrefab.h>

#include <Engine/CScript.h>
#include "CScriptMgr.h"

#include <fstream>
#include <Engine/CSerializer.h>

bool CSceneSaveLoad::s_isBinary = false;

bool CSceneSaveLoad::IsBinary()
{
	return s_isBinary;
}

void CSceneSaveLoad::SaveScene(CScene* _pScene, const wstring& _strSceneFilePath, bool binary)
{
	// 리소스 변경상태 저장
	CResMgr::GetInst()->SaveChangedRes();

	s_isBinary = binary;

	if (s_isBinary)
	{
		SaveFILE(_pScene, _strSceneFilePath);
	}
	else
	{
		SaveYAML(_pScene, _strSceneFilePath);
	}
}

CScene* CSceneSaveLoad::LoadScene(const wstring& _strSceneFilePath, bool binary)
{
	s_isBinary = binary;

	if (s_isBinary)
	{
		return LoadFILE(_strSceneFilePath);
	}
	return LoadYAML(_strSceneFilePath);
}


// ======
// Prefab
// ======
void CSceneSaveLoad::SavePrefab(CPrefab* _Prefab, const wstring& _strFilePath, bool binary)
{
	s_isBinary = binary;

	if (s_isBinary)
	{
		FILE* pFile = nullptr;
		_wfopen_s(&pFile, _strFilePath.c_str(), L"wb");

		SaveGameObject(_Prefab->GetProto(), pFile);

		fclose(pFile);
	}
	else
	{
		YAML::Emitter emitter{};
		SaveGameObject(_Prefab->GetProto(), emitter);
		std::ofstream out{_strFilePath};
		out << emitter.c_str();
	}
}

int CSceneSaveLoad::LoadPrefab(CPrefab* _Prefab, const wstring& _strFilePath, bool binary)
{
	s_isBinary = binary;

	if (binary)
	{
		FILE* pFile = nullptr;
		_wfopen_s(&pFile, _strFilePath.c_str(), L"rb");
		if (nullptr == pFile) return E_FAIL;

		if (nullptr == _Prefab->GetProto())
		{
			CGameObject* pProto = LoadGameObject(pFile);
			_Prefab->SetProto(pProto);
		}
		fclose(pFile);

		return S_OK;
	}
	else
	{
		YAML::Node root = YAML::LoadFile(ToString(_strFilePath));

		CGameObject* pProto = LoadGameObject(root);
		_Prefab->SetProto(pProto);

		return S_OK;
	}
}

void CSceneSaveLoad::SaveYAML(CScene* _pScene, const wstring& _strSceneFilePath)
{
	YAML::Emitter emitter{};
	emitter << YAML::BeginMap;
	emitter << YAML::Key << "SCENE NAME";
	emitter << YAML::Value << ToString(_pScene->GetName());

	emitter << YAML::Key << "LAYERS";
	emitter << YAML::Value << YAML::BeginSeq; //
	for (int i = 0; i < MAX_LAYER; ++i)
	{
		emitter << YAML::BeginMap;
		// Layer 이름 저장
		CLayer* pLayer = _pScene->GetLayer(i);
		emitter << YAML::Key << "LAYER NAME";
		emitter << YAML::Value << ToString(pLayer->GetName());

		emitter << YAML::Key << "GAME OBJECTS";
		emitter << YAML::Value << YAML::BeginSeq;
		for (auto& pGameObject : pLayer->GetRootObjects())
		{
			SaveGameObject(pGameObject, emitter);
		}
		emitter << YAML::EndSeq; //GAME OBJECTS

		emitter << YAML::EndMap; //LAYER INFO
	}
	emitter << YAML::EndSeq; //LAYERS

	emitter << YAML::EndMap; //SCENE

	std::ofstream out{_strSceneFilePath, std::ios::binary};
	out << emitter.c_str();
}

CScene* CSceneSaveLoad::LoadYAML(const std::wstring& _strSceneFilePath)
{
	YAML::Node sceneNode = YAML::LoadFile(ToString(_strSceneFilePath));
	CScene*    pScene    = new CScene{};

	std::string sceneName = sceneNode["SCENE NAME"].as<std::string>();

	YAML::Node layersNode = sceneNode["LAYERS"];
	for (int i = 0; i < layersNode.size(); ++i)
	{
		std::string layerName = layersNode[i]["LAYER NAME"].as<std::string>();
		CLayer*     pLayer    = pScene->GetLayer(i);
		pLayer->SetName(ToWString(layerName));

		YAML::Node gameObjectsNode = layersNode[i]["GAME OBJECTS"];
		for (auto gameObjectNode : gameObjectsNode)
		{
			CGameObject* pGameObject = LoadGameObject(gameObjectNode);
			pScene->AddObject(pGameObject, i);
		}
	}
	return pScene;
}

void CSceneSaveLoad::SaveGameObject(CGameObject* pGameObject, YAML::Emitter& emitter)
{
	if (pGameObject->GetName() == L"ManagerScript")
	{
		return;
	}
	emitter << YAML::BeginMap;
	//GameObject
	pGameObject->Serialize(emitter);

	emitter << YAML::Key << "SCRIPTS";
	emitter << YAML::Value << YAML::BeginSeq;
	const std::vector<CScript*> vecScript = pGameObject->GetScripts();
	for (int i = 0; i < vecScript.size(); ++i)
	{
		emitter << YAML::BeginMap;
		//Script
		emitter << YAML::Key << ToString(CScriptMgr::GetScriptName(vecScript[i]));
		emitter << YAML::Value << YAML::BeginMap;
		vecScript[i]->Serialize(emitter);
		emitter << YAML::EndMap;
		emitter << YAML::EndMap;
	}
	emitter << YAML::EndSeq;

	emitter << YAML::Key << "CHILDREN";
	emitter << YAML::Value << YAML::BeginSeq;
	const std::vector<CGameObject*>& vecChild = pGameObject->GetChild();
	for (auto& pChild : vecChild)
	{
		SaveGameObject(pChild, emitter);
	}
	emitter << YAML::EndSeq;

	emitter << YAML::EndMap; //GAME OBJECT INFO
}

CGameObject* CSceneSaveLoad::LoadGameObject(const YAML::Node& node)
{
	CGameObject* pGameObject = new CGameObject{};
	pGameObject->Deserialize(node);

	const YAML::Node& scripts = node["SCRIPTS"];
	for (int i = 0; i < scripts.size(); ++i)
	{
		std::string scriptName = scripts[i].begin()->first.as<std::string>();
		CScript*    pScript    = CScriptMgr::GetScript(ToWString(scriptName));
		pGameObject->AddComponent(pScript);
		pScript->Deserialize(scripts[i].begin()->second);
	}

	YAML::Node childrenNode = node["CHILDREN"];
	for (int i = 0; i < childrenNode.size(); ++i)
	{
		CGameObject* pChild = LoadGameObject(childrenNode[i]);
		pGameObject->AddChild(pChild);
	}

	return pGameObject;
}

void CSceneSaveLoad::SaveFILE(CScene* _pScene, const wstring& _strSceneFilePath)
{
	// Scene 저장
	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strSceneFilePath.c_str(), L"wb");

	assert(pFile);
	if (nullptr == pFile) return;

	// Scene 이름 저장
	_pScene->SaveToScene(pFile);

	// Scene 의 Layer 정보 저장
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// Layer 이름 저장
		CLayer* pLayer = _pScene->GetLayer(i);
		pLayer->SaveToScene(pFile);

		// Layer 가 보유중인 Root 오브젝트 개수 저장
		vector<CGameObject*>& vecRootObj = pLayer->GetRootObjects();
		size_t                iObjCount  = vecRootObj.size();
		fwrite(&iObjCount, sizeof(size_t), 1, pFile);

		// 각 루트오브젝트 저장
		for (const auto& pRootObject : vecRootObj)
		{
			SaveGameObject(pRootObject, pFile);
		}
	}
	fclose(pFile);
}

CScene* CSceneSaveLoad::LoadFILE(const std::wstring& _strSceneFilePath)
{
	// 최종 경로에서 상대경로만 추출
	const std::wstring keyAsRelPath = CPathMgr::GetInst()->GetRelativePath(_strSceneFilePath);
	// CResMgr 에서 상대경로를 키값으로, CSceneFile 을 찾아냄
	CResMgr::GetInst()->Load<CSceneFile>(keyAsRelPath, keyAsRelPath);
	// Load 할 Scene 생성 및 파일로부터 로딩
	CScene* pLoadScene = new CScene{};
	pLoadScene->SetResKey(keyAsRelPath);

	FILE* pFile = nullptr;
	_wfopen_s(&pFile, _strSceneFilePath.c_str(), L"rb");

	assert(pFile);
	if (nullptr == pFile) return nullptr;

	// Scene 이름 불러오기
	pLoadScene->LoadFromScene(pFile);
	// Scene 의 Layer 정보 불러오기
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		// Layer 의 이름 
		CLayer* pLayer = pLoadScene->GetLayer(i);
		pLayer->LoadFromScene(pFile);

		// Layer 보유 오브젝트 개수
		size_t objectCount{};
		fread(&objectCount, sizeof(size_t), 1, pFile);

		// Root 오브젝트
		for (size_t j = 0; j < objectCount; ++j)
		{
			CGameObject* pLoadObj = LoadGameObject(pFile);
			pLoadScene->AddObject(pLoadObj, i);
		}
	}
	fclose(pFile);

	return pLoadScene;
}

CGameObject* CSceneSaveLoad::LoadGameObject(FILE* _pFile)
{
	CGameObject* pLoadObj = new CGameObject{};
	// 이름, 상태, 컴포넌트 불러오기
	pLoadObj->LoadFromScene(_pFile);

	// Script 불러오기        
	size_t scriptCount{};
	fread(&scriptCount, sizeof(size_t), 1, _pFile);
	for (size_t i = 0; i < scriptCount; ++i)
	{
		std::wstring scriptName{};
		LoadWStringFromFile(scriptName, _pFile);
		CScript* pLoadScript = CScriptMgr::GetScript(scriptName);
		pLoadObj->AddComponent(pLoadScript);
		pLoadScript->LoadFromScene(_pFile);
	}

	size_t childCount{};
	fread(&childCount, sizeof(size_t), 1, _pFile);
	for (size_t i = 0; i < childCount; ++i)
	{
		CGameObject* pChild = LoadGameObject(_pFile);
		pLoadObj->AddChild(pChild);
	}

	return pLoadObj;
}

void CSceneSaveLoad::SaveGameObject(CGameObject* _pObj, FILE* _pFile)
{
	// 이름, 상태, 컴포넌트 저장
	_pObj->SaveToScene(_pFile);

	// Script 저장
	const vector<CScript*>& vecScript   = _pObj->GetScripts();
	const size_t            scriptCount = vecScript.size();

	fwrite(&scriptCount, sizeof(size_t), 1, _pFile);

	for (auto& pScript : vecScript)
	{
		std::wstring scriptName = CScriptMgr::GetScriptName(pScript);
		SaveWStringToFile(scriptName, _pFile);
		pScript->SaveToScene(_pFile);
	}
	// Child Object
	const vector<CGameObject*>& vecChild  = _pObj->GetChild();
	const size_t                childCont = vecChild.size();
	fwrite(&childCont, sizeof(size_t), 1, _pFile);
	for (auto& pChild : vecChild)
	{
		SaveGameObject(pChild, _pFile);
	}
}
