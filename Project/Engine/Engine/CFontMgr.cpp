#include "pch.h"
#include "CFontMgr.h"

#include "CDevice.h"

CFontMgr::CFontMgr()  = default;
CFontMgr::~CFontMgr() = default;

void CFontMgr::Init()
{
	const HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
	                                       __uuidof(m_pFactory),
	                                       reinterpret_cast<IUnknown**>(m_pFactory.GetAddressOf()));
	if (FAILED(hr))
	{
		LOG_ASSERT(hr==S_OK, "DWRITE CREATION FAILED");
	}
}

ComPtr<IDWriteTextFormat> CFontMgr::LoadFontFromFile(const std::wstring& _relativePath)
{
	auto pFont = FindFont(_relativePath);
	if (nullptr != pFont)
	{
		return pFont;
	}

	CreateFontCollection(_relativePath, _relativePath);
	const std::wstring fontFace = GetFontFaceName(_relativePath);

	return CreateFontFromFile(_relativePath, fontFace);
}

std::vector<std::wstring> CFontMgr::GetFonts() const
{
	std::vector<std::wstring> vecFont{};
	vecFont.reserve(m_mapFont.size());

	for (auto& [fontName,pFont] : m_mapFont)
	{
		vecFont.push_back(fontName);
	}

	return vecFont;
}

void CFontMgr::CreateFontCollection(const std::wstring& _key, const std::wstring& _relativePath)
{
	auto pFontCollection = FindFontCollection(_key);
	if (nullptr != pFontCollection)
	{
		return;
	}

	const std::wstring      fullPath = CPathMgr::GetInst()->GetContentPath() + _relativePath;
	ComPtr<IDWriteFontFile> pFontFile{};

	HRESULT hr = m_pFactory->CreateFontFileReference(fullPath.c_str(), nullptr, pFontFile.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT FILE CREATION FAILED");

	ComPtr<IDWriteFontSetBuilder1> pBuilder{};
	hr = m_pFactory->CreateFontSetBuilder(pBuilder.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT BUILDER CREATION FAILED");
	pBuilder->AddFontFile(pFontFile.Get());

	ComPtr<IDWriteFontSet> pFontSet{};
	hr = pBuilder->CreateFontSet(pFontSet.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT SET CREATION FAILED");

	hr = m_pFactory->CreateFontCollectionFromFontSet(pFontSet.Get(), pFontCollection.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT COLLECTION CREATION FAILED");

	m_mapCollection.insert({_key, pFontCollection});
}

ComPtr<IDWriteFontCollection1> CFontMgr::FindFontCollection(const std::wstring& _key)
{
	const auto iter = m_mapCollection.find(_key);
	if (iter != m_mapCollection.end())
	{
		return iter->second;
	}
	return nullptr;
}

ComPtr<IDWriteTextFormat> CFontMgr::FindFont(const std::wstring& _key)
{
	const auto iter = m_mapFont.find(_key);
	if (iter != m_mapFont.end())
	{
		return iter->second;
	}
	return nullptr;
}

std::wstring CFontMgr::GetFontFaceName(const std::wstring& _key)
{
	const auto pFontCollection = FindFontCollection(_key);
	if (nullptr == pFontCollection)
	{
		LOG_WARN("FONT FACE NOT FOUND");
		return std::wstring{};
	}

	ComPtr<IDWriteFontFamily> pFamily{};
	HRESULT                   hr = pFontCollection->GetFontFamily(0, pFamily.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT FAMILY CREATION FAILED");

	ComPtr<IDWriteLocalizedStrings> pLocalizedName{};
	hr = pFamily->GetFamilyNames(pLocalizedName.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT LANGUAGE NOT SUPPORTED");

	WCHAR szName[256]{};
	hr = pLocalizedName->GetString(0, szName,ARRAYSIZE(szName));
	LOG_ASSERT(hr==S_OK, "FONT FACE NOT FOUND");

	return szName;
}

ComPtr<IDWriteTextFormat> CFontMgr::CreateFontFromFile(const std::wstring& _key,
                                                       const std::wstring& _fontFace,
                                                       DWRITE_FONT_WEIGHT  _fontWeight,
                                                       DWRITE_FONT_STYLE   _fontStyle,
                                                       DWRITE_FONT_STRETCH _fontStretch,
                                                       float               _fontSize,
                                                       const std::wstring& _locale)
{
	ComPtr<IDWriteTextFormat> pFont = FindFont(_key);
	if (pFont)
	{
		return pFont;
	}

	const HRESULT hr = m_pFactory->CreateTextFormat(_fontFace.c_str(),
	                                                nullptr,
	                                                _fontWeight,
	                                                _fontStyle,
	                                                _fontStretch,
	                                                _fontSize,
	                                                _locale.c_str(),
	                                                pFont.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "FONT CREATION FAILED");

	m_mapFont.insert({_key, pFont});

	return pFont;
}


ComPtr<ID2D1SolidColorBrush> CFontMgr::GetBrush(const Vec4& _color)
{
	ComPtr<ID2D1SolidColorBrush> pBrush = FindColor(_color);
	if (nullptr != pBrush)
	{
		return pBrush;
	}

	const HRESULT hr = CDevice::GetInst()->GetRtv2D()->CreateSolidColorBrush(D2D1::ColorF
	                                                                         {_color.x, _color.y, _color.z, _color.w},
	                                                                         pBrush.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "BRUSH CREATION FAILED");

	m_mapBrush.insert({GetColorID(_color), pBrush});

	return pBrush;
}

ComPtr<ID2D1SolidColorBrush> CFontMgr::GetBrush(const D2D1::ColorF& _color)
{
	return GetBrush(Vec4{_color.r, _color.g, _color.b, _color.a});
}

ComPtr<IDWriteTextLayout> CFontMgr::CreateTextLayout(const std::wstring&       text,
                                                     ComPtr<IDWriteTextFormat> pFont,
                                                     float                     width,
                                                     float                     height) const
{
	ComPtr<IDWriteTextLayout> pLayout{};
	const HRESULT             hr = m_pFactory->CreateTextLayout(text.c_str(),
	                                                            (uint32_t)text.size(),
	                                                            pFont.Get(),
	                                                            width,
	                                                            height,
	                                                            pLayout.GetAddressOf());
	LOG_ASSERT(hr==S_OK, "TEXTURE LAYOUT CRAETION FAILED");

	return pLayout;
}


ComPtr<ID2D1SolidColorBrush> CFontMgr::FindColor(const Vec4& _color)
{
	const auto iter = m_mapBrush.find(GetColorID(_color));
	if (iter != m_mapBrush.end())
	{
		return iter->second;
	}
	return nullptr;
}

UINT CFontMgr::GetColorID(const Vec4& _color)
{
	const UCHAR R = static_cast<UCHAR>(_color.x * 255);
	const UCHAR G = static_cast<UCHAR>(_color.y * 255);
	const UCHAR B = static_cast<UCHAR>(_color.z * 255);
	const UCHAR A = static_cast<UCHAR>(_color.w * 255);

	UINT colorID{};
	colorID = (R << 24) | (G << 16) | (B << 8) | A;

	return colorID;
}
