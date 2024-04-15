#include "pch.h"
#include "CScript.h"


CScript::CScript(int _iScriptType)
	: CComponent(COMPONENT_TYPE::SCRIPT)
	, m_iScriptID(_iScriptType) {}

CScript::~CScript() = default;

auto CScript::FindScalarParam(const std::string& paramName)
{
	const auto iter = std::find_if(m_vecParamInfo.begin(),
	                               m_vecParamInfo.end(),
	                               [&paramName](const tScriptParamInfo& info)
	                               {
		                               return info.strParamName == paramName;
	                               });
	return iter;
}

void CScript::AddScriptParam(string                _strParamName,
                             SCRIPTPARAM_TYPE      _eType,
                             void*                 _pData,
                             std::function<void()> callback)
{
	m_vecParamInfo.push_back(tScriptParamInfo{_strParamName, _eType, _pData, SHOW_TYPE::NORMAL, {}, callback});
}

void CScript::AddScriptParamAsDropDown(const std::string&       _strParameterName,
                                       SCRIPTPARAM_TYPE         _eType,
                                       void*                    _pData,
                                       std::vector<std::string> vecListMenu,
                                       std::function<void()>    callback)
{
	m_vecParamInfo.push_back(tScriptParamInfo{
		                         _strParameterName,
		                         _eType,
		                         _pData,
		                         SHOW_TYPE::DROP_DOWN,
		                         vecListMenu,
		                         callback
	                         });
}

void CScript::AddScriptParamAsCheckBox(const std::string&    _strParameterName,
                                       int*                  _pData,
                                       std::function<void()> callback)
{
	m_vecParamInfo.push_back(tScriptParamInfo{
		                         _strParameterName,
		                         SCRIPTPARAM_TYPE::INT,
		                         _pData,
		                         SHOW_TYPE::CHECK_BOX,
		                         {},
		                         callback
	                         });
}

void CScript::AddScriptParamAsColorPicker(const std::string&    _strParameterName,
                                          Vec4*                 _pData,
                                          std::function<void()> callback)
{
	m_vecParamInfo.push_back({
		                         _strParameterName,
		                         SCRIPTPARAM_TYPE::VEC4,
		                         _pData,
		                         SHOW_TYPE::COLOR_PICKER,
		                         {},
		                         callback
	                         });
}

void CScript::RemoveScalarParam(const std::string& paramName)
{
	const auto iter = FindScalarParam(paramName);
	if (iter != m_vecParamInfo.end())
	{
		m_vecParamInfo.erase(iter);
	}
}

void CScript::RenewScalarParam(const std::string& paramName, void* pValue, std::function<void()> callback)
{
	const auto iter = FindScalarParam(paramName);
	if (iter != m_vecParamInfo.end())
	{
		tScriptParamInfo& paramInfo = *iter;
		paramInfo.pParam            = pValue;
		if (callback)
		{
			paramInfo.callback = callback;
		}
	}
}
