#pragma once

class CTexture;

#include "ListUI.h"

class ParamUI
{
public:
	static int KeyCount;

public:
	static int   Param_Int(const string& _strName, const int* _pInOut);
	static float Param_Float(const string& _strName, const float* _pInOut);
	static Vec2  Param_Vec2(const string& _strName, const Vec2* _pInOut);
	static Vec4  Param_Vec4(const string& _strName, const Vec4* _pInOut);

	static std::string Param_String(const string& _strName, const char* _pIn); // 문자열 표시 ( 읽기 전용 )
	static void        Param_String_ReadOnly(const std::string& _strName, const char* _pIn);

	// 반환값 : 리스트UI 활성화 했을 때 true 아니면 false
	static bool Param_Tex(const string& _strName, CTexture* _pCurTex, UI* _pInst = nullptr, DBCLKED _pFunc = nullptr);

	static int  Param_Int_DropDown(const string& _strName, const int* _pInOut, const std::vector<std::string>& vecList);
	static int  Param_Int_CheckBox(const string& _strName, const int* _pInOut);
	static Vec4 Param_Vec4_ColorPicker(const std::string& _strName, const Vec4* _pInOut);
};
