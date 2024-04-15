#pragma once


template <typename T>
void Safe_Del_Vec(vector<T*>& _vec)
{
	for (size_t i = 0; i < _vec.size(); ++i)
	{
		SAFE_DELETE(_vec[i]);
	}
	_vec.clear();
}

template <typename T>
void Safe_Del_list(list<T*>& _list)
{
	typename list<T*>::iterator iter = _list.begin();

	for (; iter != _list.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	_list.clear();
}

template <typename T1, typename T2>
void Safe_Del_Map(map<T1, T2>& _map)
{
	typename map<T1, T2>::iterator iter = _map.begin();
	for (; iter != _map.end(); ++iter)
	{
		SAFE_DELETE(iter->second);
	}
	_map.clear();
}

template <typename T, UINT _iSize>
void Safe_Del_Arr(T (&_Arr)[_iSize])
{
	for (UINT i = 0; i < _iSize; ++i)
	{
		SAFE_DELETE(_Arr[i]);
	}
}

template <typename T, UINT _Size>
void Safe_Del_Arr(std::array<T*, _Size> pArray)
{
	for (auto& pData : pArray)
	{
		SAFE_DELETE(pData);
	}
}


void SaveStringToFile(const string& _str, FILE* _pFile);
void LoadStringFromFile(string& _str, FILE* _pFile);

void SaveWStringToFile(const wstring& _str, FILE* _pFile);
void LoadWStringFromFile(wstring& _str, FILE* _pFile);


Matrix GetMatrixFromFbxMatrix(const FbxAMatrix& _mat);


std::wstring ToWString(COMPONENT_TYPE _type);
std::string  ToString(COMPONENT_TYPE _type);
std::wstring ToWString(RES_TYPE _type);
std::string  ToString(RES_TYPE _type);

inline std::string ToString(const std::wstring& _strInput, std::locale const& _locale = std::locale{""})
{
	using codecvt_t = std::codecvt<wchar_t, char, std::mbstate_t>;
	codecvt_t const&        converter = std::use_facet<codecvt_t>(_locale);
	std::mbstate_t          state{};
	std::vector<char>       vecBuffer((_strInput.size() + 1) * converter.max_length());
	wchar_t const*          in_next  = _strInput.c_str();
	char*                   out_next = vecBuffer.data();
	const codecvt_t::result result   = converter.out(state,
	                                                 _strInput.c_str(),
	                                                 _strInput.c_str() + _strInput.size(),
	                                                 in_next,
	                                                 vecBuffer.data(),
	                                                 vecBuffer.data() + vecBuffer.size(),
	                                                 out_next);
	assert(result == std::codecvt_base::ok);

	return std::string{vecBuffer.data()};
}

inline std::wstring ToWString(const std::string& _strInput, std::locale const& _locale = std::locale{""})
{
	using codecvt_t = std::codecvt<wchar_t, char, std::mbstate_t>;
	codecvt_t const&        converter = std::use_facet<codecvt_t>(_locale);
	std::mbstate_t          state{};
	std::vector<wchar_t>    vecBuffer(_strInput.size() + 1);
	char const*             in_next  = _strInput.c_str();
	wchar_t*                out_next = vecBuffer.data();
	const codecvt_t::result result   = converter.in(state,
	                                                _strInput.c_str(),
	                                                _strInput.c_str() + _strInput.size(),
	                                                in_next,
	                                                vecBuffer.data(),
	                                                vecBuffer.data() + vecBuffer.size(),
	                                                out_next);
	assert(result == std::codecvt_base::ok);

	return std::wstring{vecBuffer.data()};
}

#include "Ptr.h"
#include "CResMgr.h"


template <typename RES_TYPE>
wstring FindResName(const wstring& _strBaseName)
{
	wchar_t szName[256] = {};
	wchar_t szNum[10]   = {};

	wcscpy_s(szName, 256, _strBaseName.c_str());
	wstring strFindName;

	for (UINT i = 1; i < 100; ++i)
	{
		wsprintf(szNum, L"_%02d", i);
		strFindName = wstring(szName) + szNum;
		if (nullptr == CResMgr::GetInst()->FindRes<RES_TYPE>(strFindName))
		{
			return strFindName;
		}
	}

	return wstring();
}


template <typename RES>
void SaveResPtr(Ptr<RES> _ptr, FILE* _pFile)
{
	bool bNullPtr = nullptr == _ptr ? true : false;
	fwrite(&bNullPtr, sizeof(bool), 1, _pFile);

	if (!bNullPtr)
	{
		SaveWStringToFile(_ptr->GetKey(), _pFile);
		SaveWStringToFile(_ptr->GetRelativePath(), _pFile);
	}
}

template <typename RES>
void LoadResPtr(Ptr<RES>& _ptr, FILE* _pFile)
{
	bool bNull = false;
	fread(&bNull, sizeof(bool), 1, _pFile);

	if (!bNull)
	{
		wstring strKey, strRelativePath;

		LoadWStringFromFile(strKey, _pFile);
		LoadWStringFromFile(strRelativePath, _pFile);

		_ptr = CResMgr::GetInst()->Load<RES>(strKey, strRelativePath);
	}
}

Vec3 DecomposeRotMat(const Matrix& _matRot);

bool CloseEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon());

template <typename T>
T ClampData(T _target, T _min, T _max)
{
	T temp = _target;
	if (temp < _min)
	{
		temp = _min;
	}
	else if (temp > _max)
	{
		temp = _max;
	}
	return temp;
}
