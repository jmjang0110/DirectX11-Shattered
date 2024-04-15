
#include "pch.h"

#include "CScriptMgr.h"
#include "BossJugCombatMgrScript.h"
#include "BossJugHandScript.h"
#include "BossJugScript.h"
#include "CameraMoveScript.h"
#include "CameraShakeScript.h"
#include "CinemaCamScript.h"
#include "ColumnFlameScript.h"
#include "DestroyScript.h"
#include "DoorScript.h"
#include "EffectScript.h"
#include "EnergyBallScript.h"
#include "FieldMonsteScript.h"
#include "HammerScript.h"
#include "HandStateMgrScript.h"
#include "ItemScript.h"
#include "MagmaScript.h"
#include "MainMenuScript.h"
#include "MissileScript.h"
#include "MonsterGunScript.h"
#include "MonsterSearchScript.h"
#include "MonsterShieldScript.h"
#include "MonsterSwordScript.h"
#include "M_AttackScript.h"
#include "PaperBurnScript.h"
#include "Phase01TriggerScript.h"
#include "PlayerCamScript.h"
#include "PlayerScript.h"
#include "PotScript.h"
#include "SocketColliderScript.h"
#include "SwordTrailScript.h"
#include "TotemScript.h"
#include "TrailScript.h"

void CScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"BossJugCombatMgrScript");
	_vec.push_back(L"BossJugHandScript");
	_vec.push_back(L"BossJugScript");
	_vec.push_back(L"CameraMoveScript");
	_vec.push_back(L"CameraShakeScript");
	_vec.push_back(L"CinemaCamScript");
	_vec.push_back(L"ColumnFlameScript");
	_vec.push_back(L"DestroyScript");
	_vec.push_back(L"DoorScript");
	_vec.push_back(L"EffectScript");
	_vec.push_back(L"EnergyBallScript");
	_vec.push_back(L"FieldMonsteScript");
	_vec.push_back(L"HammerScript");
	_vec.push_back(L"HandStateMgrScript");
	_vec.push_back(L"ItemScript");
	_vec.push_back(L"MagmaScript");
	_vec.push_back(L"MainMenuScript");
	_vec.push_back(L"MissileScript");
	_vec.push_back(L"MonsterGunScript");
	_vec.push_back(L"MonsterSearchScript");
	_vec.push_back(L"MonsterShieldScript");
	_vec.push_back(L"MonsterSwordScript");
	_vec.push_back(L"M_AttackScript");
	_vec.push_back(L"PaperBurnScript");
	_vec.push_back(L"Phase01TriggerScript");
	_vec.push_back(L"PlayerCamScript");
	_vec.push_back(L"PlayerScript");
	_vec.push_back(L"PotScript");
	_vec.push_back(L"SocketColliderScript");
	_vec.push_back(L"SwordTrailScript");
	_vec.push_back(L"TotemScript");
	_vec.push_back(L"TrailScript");
}


CScript* CScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"BossJugCombatMgrScript"== _strScriptName)
		return new BossJugCombatMgrScript;
	if (L"BossJugHandScript"== _strScriptName)
		return new BossJugHandScript;
	if (L"BossJugScript"== _strScriptName)
		return new BossJugScript;
	if (L"CameraMoveScript"== _strScriptName)
		return new CameraMoveScript;
	if (L"CameraShakeScript"== _strScriptName)
		return new CameraShakeScript;
	if (L"CinemaCamScript"== _strScriptName)
		return new CinemaCamScript;
	if (L"ColumnFlameScript"== _strScriptName)
		return new ColumnFlameScript;
	if (L"DestroyScript"== _strScriptName)
		return new DestroyScript;
	if (L"DoorScript"== _strScriptName)
		return new DoorScript;
	if (L"EffectScript"== _strScriptName)
		return new EffectScript;
	if (L"EnergyBallScript"== _strScriptName)
		return new EnergyBallScript;
	if (L"FieldMonsteScript"== _strScriptName)
		return new FieldMonsteScript;
	if (L"HammerScript"== _strScriptName)
		return new HammerScript;
	if (L"HandStateMgrScript"== _strScriptName)
		return new HandStateMgrScript;
	if (L"ItemScript"== _strScriptName)
		return new ItemScript;
	if (L"MagmaScript"== _strScriptName)
		return new MagmaScript;
	if (L"MainMenuScript"== _strScriptName)
		return new MainMenuScript;
	if (L"MissileScript"== _strScriptName)
		return new MissileScript;
	if (L"MonsterGunScript"== _strScriptName)
		return new MonsterGunScript;
	if (L"MonsterSearchScript"== _strScriptName)
		return new MonsterSearchScript;
	if (L"MonsterShieldScript"== _strScriptName)
		return new MonsterShieldScript;
	if (L"MonsterSwordScript"== _strScriptName)
		return new MonsterSwordScript;
	if (L"M_AttackScript"== _strScriptName)
		return new M_AttackScript;
	if (L"PaperBurnScript"== _strScriptName)
		return new PaperBurnScript;
	if (L"Phase01TriggerScript"== _strScriptName)
		return new Phase01TriggerScript;
	if (L"PlayerCamScript"== _strScriptName)
		return new PlayerCamScript;
	if (L"PlayerScript"== _strScriptName)
		return new PlayerScript;
	if (L"PotScript"== _strScriptName)
		return new PotScript;
	if (L"SocketColliderScript"== _strScriptName)
		return new SocketColliderScript;
	if (L"SwordTrailScript"== _strScriptName)
		return new SwordTrailScript;
	if (L"TotemScript"== _strScriptName)
		return new TotemScript;
	if (L"TrailScript"== _strScriptName)
		return new TrailScript;
	return nullptr;
}


CScript* CScriptMgr::GetScript(UINT _iScriptType)
{
    switch((SCRIPT_TYPE)_iScriptType)
    {
	case SCRIPT_TYPE::BOSSJUGCOMBATMGRSCRIPT:
		return new BossJugCombatMgrScript;
	case SCRIPT_TYPE::BOSSJUGHANDSCRIPT:
		return new BossJugHandScript;
	case SCRIPT_TYPE::BOSSJUGSCRIPT:
		return new BossJugScript;
	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return new CameraMoveScript;
	case SCRIPT_TYPE::CAMERASHAKESCRIPT:
		return new CameraShakeScript;
	case SCRIPT_TYPE::CINEMACAMSCRIPT:
		return new CinemaCamScript;
	case SCRIPT_TYPE::COLUMNFLAMESCRIPT:
		return new ColumnFlameScript;
	case SCRIPT_TYPE::DESTROYSCRIPT:
		return new DestroyScript;
	case SCRIPT_TYPE::DOORSCRIPT:
		return new DoorScript;
	case SCRIPT_TYPE::EFFECTSCRIPT:
		return new EffectScript;
	case SCRIPT_TYPE::ENERGYBALLSCRIPT:
		return new EnergyBallScript;
	case SCRIPT_TYPE::FIELDMONSTESCRIPT:
		return new FieldMonsteScript;
	case SCRIPT_TYPE::HAMMERSCRIPT:
		return new HammerScript;
	case SCRIPT_TYPE::HANDSTATEMGRSCRIPT:
		return new HandStateMgrScript;
	case SCRIPT_TYPE::ITEMSCRIPT:
		return new ItemScript;
	case SCRIPT_TYPE::MAGMASCRIPT:
		return new MagmaScript;
	case SCRIPT_TYPE::MAINMENUSCRIPT:
		return new MainMenuScript;
	case SCRIPT_TYPE::MISSILESCRIPT:
		return new MissileScript;
	case SCRIPT_TYPE::MONSTERGUNSCRIPT:
		return new MonsterGunScript;
	case SCRIPT_TYPE::MONSTERSEARCHSCRIPT:
		return new MonsterSearchScript;
	case SCRIPT_TYPE::MONSTERSHIELDSCRIPT:
		return new MonsterShieldScript;
	case SCRIPT_TYPE::MONSTERSWORDSCRIPT:
		return new MonsterSwordScript;
	case SCRIPT_TYPE::M_ATTACKSCRIPT:
		return new M_AttackScript;
	case SCRIPT_TYPE::PAPERBURNSCRIPT:
		return new PaperBurnScript;
	case SCRIPT_TYPE::PHASE01TRIGGERSCRIPT:
		return new Phase01TriggerScript;
	case SCRIPT_TYPE::PLAYERCAMSCRIPT:
		return new PlayerCamScript;
	case SCRIPT_TYPE::PLAYERSCRIPT:
		return new PlayerScript;
	case SCRIPT_TYPE::POTSCRIPT:
		return new PotScript;
	case SCRIPT_TYPE::SOCKETCOLLIDERSCRIPT:
		return new SocketColliderScript;
	case SCRIPT_TYPE::SWORDTRAILSCRIPT:
		return new SwordTrailScript;
	case SCRIPT_TYPE::TOTEMSCRIPT:
		return new TotemScript;
	case SCRIPT_TYPE::TRAILSCRIPT:
		return new TrailScript;
	}
	return nullptr;
}


const wchar_t* CScriptMgr::GetScriptName(CScript* _pScript)
{
    switch((SCRIPT_TYPE)_pScript->GetScriptType())
    {
	case SCRIPT_TYPE::BOSSJUGCOMBATMGRSCRIPT:
		return L"BossJugCombatMgrScript";
	case SCRIPT_TYPE::BOSSJUGHANDSCRIPT:
		return L"BossJugHandScript";
	case SCRIPT_TYPE::BOSSJUGSCRIPT:
		return L"BossJugScript";
	case SCRIPT_TYPE::CAMERAMOVESCRIPT:
		return L"CameraMoveScript";
	case SCRIPT_TYPE::CAMERASHAKESCRIPT:
		return L"CameraShakeScript";
	case SCRIPT_TYPE::CINEMACAMSCRIPT:
		return L"CinemaCamScript";
	case SCRIPT_TYPE::COLUMNFLAMESCRIPT:
		return L"ColumnFlameScript";
	case SCRIPT_TYPE::DESTROYSCRIPT:
		return L"DestroyScript";
	case SCRIPT_TYPE::DOORSCRIPT:
		return L"DoorScript";
	case SCRIPT_TYPE::EFFECTSCRIPT:
		return L"EffectScript";
	case SCRIPT_TYPE::ENERGYBALLSCRIPT:
		return L"EnergyBallScript";
	case SCRIPT_TYPE::FIELDMONSTESCRIPT:
		return L"FieldMonsteScript";
	case SCRIPT_TYPE::HAMMERSCRIPT:
		return L"HammerScript";
	case SCRIPT_TYPE::HANDSTATEMGRSCRIPT:
		return L"HandStateMgrScript";
	case SCRIPT_TYPE::ITEMSCRIPT:
		return L"ItemScript";
	case SCRIPT_TYPE::MAGMASCRIPT:
		return L"MagmaScript";
	case SCRIPT_TYPE::MAINMENUSCRIPT:
		return L"MainMenuScript";
	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"MissileScript";
	case SCRIPT_TYPE::MONSTERGUNSCRIPT:
		return L"MonsterGunScript";
	case SCRIPT_TYPE::MONSTERSEARCHSCRIPT:
		return L"MonsterSearchScript";
	case SCRIPT_TYPE::MONSTERSHIELDSCRIPT:
		return L"MonsterShieldScript";
	case SCRIPT_TYPE::MONSTERSWORDSCRIPT:
		return L"MonsterSwordScript";
	case SCRIPT_TYPE::M_ATTACKSCRIPT:
		return L"M_AttackScript";
	case SCRIPT_TYPE::PAPERBURNSCRIPT:
		return L"PaperBurnScript";
	case SCRIPT_TYPE::PHASE01TRIGGERSCRIPT:
		return L"Phase01TriggerScript";
	case SCRIPT_TYPE::PLAYERCAMSCRIPT:
		return L"PlayerCamScript";
	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"PlayerScript";
	case SCRIPT_TYPE::POTSCRIPT:
		return L"PotScript";
	case SCRIPT_TYPE::SOCKETCOLLIDERSCRIPT:
		return L"SocketColliderScript";
	case SCRIPT_TYPE::SWORDTRAILSCRIPT:
		return L"SwordTrailScript";
	case SCRIPT_TYPE::TOTEMSCRIPT:
		return L"TotemScript";
	case SCRIPT_TYPE::TRAILSCRIPT:
		return L"TrailScript";
		}
	return nullptr;
}

