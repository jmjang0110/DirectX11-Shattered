#pragma once

class CScene;
class CGameObject;
class CPrefab;

class CSceneSaveLoad
{
public:
	static bool IsBinary();
public:
	// Save
	static void SaveScene(CScene* _pScene, const wstring& _strSceneFilePath, bool binary = false);
	static void SavePrefab(CPrefab* _Prefab, const wstring& _strFilePath, bool binary = false);

	// Load
	static CScene* LoadScene(const wstring& _strSceneFilePath, bool binary = false);
	static int     LoadPrefab(CPrefab* _Prefab, const wstring& _strFilePath, bool binary = false);

public:
	static void SaveYAML(CScene* _pScene, const wstring& _strSceneFilePath);
	static void SaveFILE(CScene* _pScene, const wstring& _strSceneFilePath);

	static CScene* LoadYAML(const std::wstring& _strSceneFilePath);
	static CScene* LoadFILE(const std::wstring& _strSceneFilePath);

	static CGameObject* LoadGameObject(FILE* _pFile);
	static void         SaveGameObject(CGameObject* _pObj, FILE* _pFile);

	static CGameObject* LoadGameObject(const YAML::Node& node);
	static void         SaveGameObject(CGameObject* _pObj, YAML::Emitter& emitter);

private:
	static bool s_isBinary;
};
