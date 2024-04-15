#include "pch.h"
#include "CUIText.h"

#include "CFontMgr.h"
#include "CEventMgr.h"
#include "CMeshRender.h"
#include "CTransform.h"
#include "CUIBase.h"
#include "CSerializer.h"
#include "CTimeMgr.h"

CUIText::CUIText()
	: CUIBase{COMPONENT_TYPE::UITEXT}
	, m_fontSize{16.f}
	, m_alphaEnable{false}
	, m_color{1.f, 1.f, 1.f, 1.f}
	, m_alignTextH{TEXT_ALIGN_HORIZONTAL::MIDDLE}
	, m_alignTextV{TEXT_ALIGN_VERTICAL::MIDDLE}
	, m_persistTime{0.f}
	, m_elapsedTime{0.f}
	, m_bHasTime{false} {}

CUIText::~CUIText() = default;

void CUIText::finalupdate()
{
	if (nullptr == MeshRender()
	    || false == MeshRender()->IsActive()
	    || nullptr == MeshRender()->GetMaterial(0))
	{
		return;
	}
	CUIBase::finalupdate();
	if (false == GetOwner()->IsDead())
	{
		RenderText();
	}
}

ComPtr<IDWriteTextFormat> CUIText::GetFontFormat()
{
	if (nullptr == m_pFont)
	{
		m_pFont = m_fontName.empty()
			          ? CFontMgr::GetInst()->LoadFontFromFile(L"font\\fa-solid-900.ttf")
			          : CFontMgr::GetInst()->LoadFontFromFile(ToWString(m_fontName));;
	}
	return m_pFont;
}

void CUIText::CreateTextLayout()
{
	if (nullptr == GetFontFormat())
	{
		return;
	}
	LOG_ASSERT(nullptr!= GetFontFormat(), "FONT NOT FOUND");

	Vec3 size{};
	size.x = m_text.size() * m_fontSize;
	size.y = m_fontSize;
	Transform()->SetRelativeScale(size);
	if (m_text.empty())
	{
		return;
	}
	m_pLayout = CFontMgr::GetInst()->CreateTextLayout(ToWString(m_text), m_pFont, size.x, size.y);

	DWRITE_TEXT_RANGE range{};
	range.startPosition = 0;
	range.length        = (UINT)m_text.size();

	m_pLayout->SetFontSize(m_fontSize, range);

	switch (m_alignTextH)
	{
	case TEXT_ALIGN_HORIZONTAL::LEFT:
		m_pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case TEXT_ALIGN_HORIZONTAL::MIDDLE:
		m_pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case TEXT_ALIGN_HORIZONTAL::RIGHT:
		m_pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	}

	switch (m_alignTextV)
	{
	case TEXT_ALIGN_VERTICAL::TOP:
		m_pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	case TEXT_ALIGN_VERTICAL::MIDDLE:
		m_pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case TEXT_ALIGN_VERTICAL::BOTTOM:
		m_pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	}
}

void CUIText::RenderText()
{
	if (m_bHasTime)
	{
		m_elapsedTime += DT;
		if (m_elapsedTime < m_persistTime)
		{
			m_elapsedTime = 0.f;
			m_text.clear();
			CreateTextLayout();
		}
	}

	tEventInfo info{};
	info.eType  = EVENT_TYPE::RENDER_TEXT;
	info.lParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(info);
}

void CUIText::AddText(const std::wstring& text)
{
	if (m_bHasTime)
	{
		m_elapsedTime = 0.f;
	}
	m_text += ToString(text);
	CreateTextLayout();
}

const std::string& CUIText::GetText() const
{
	return m_text;
}

void CUIText::SetText(const std::wstring& text)
{
	if (m_bHasTime)
	{
		m_elapsedTime = 0.f;
	}
	m_text = ToString(text);
	CreateTextLayout();
}

int CUIText::GetTextCount() const
{
	return static_cast<int>(m_text.size());
}

void CUIText::PopBack()
{
	if (false == m_text.empty())
	{
		m_text.pop_back();
		CreateTextLayout();
	}
}

void CUIText::ClearText()
{
	m_text.clear();
	CreateTextLayout();
}

void CUIText::SetFontSize(float size)
{
	m_fontSize = size;
	CreateTextLayout();
}

void CUIText::SetAlphaEnable(bool enable)
{
	m_alphaEnable = enable;
}

void CUIText::SetFont(const std::wstring& fontKey)
{
	m_pFont = CFontMgr::GetInst()->LoadFontFromFile(fontKey);
	if (m_pFont)
	{
		m_fontName = ToString(fontKey);
	}
	CreateTextLayout();
}

void CUIText::SetAlignH(TEXT_ALIGN_HORIZONTAL alignment)
{
	m_alignTextH = alignment;
}

void CUIText::SetAlignV(TEXT_ALIGN_VERTICAL alignment)
{
	m_alignTextV = alignment;
}

void CUIText::SetColor(UINT r, UINT g, UINT b)
{
	m_color.x = r / 255.f;
	m_color.y = g / 255.f;
	m_color.z = b / 255.f;

	m_pColorBrush = CFontMgr::GetInst()->GetBrush(m_color);
}

void CUIText::SetColor(const Vec4& color)
{
	m_color       = color;
	m_pColorBrush = CFontMgr::GetInst()->GetBrush(m_color);
}

ComPtr<ID2D1SolidColorBrush> CUIText::GetBrush()
{
	if (nullptr == m_pColorBrush)
	{
		m_pColorBrush = CFontMgr::GetInst()->GetBrush(m_color);
	}
	return m_pColorBrush;
}

ComPtr<IDWriteTextLayout> CUIText::GetTextureLayout()
{
	if (nullptr == m_pLayout)
	{
		CreateTextLayout();
	}
	return m_pLayout;
}

void CUIText::Serialize(YAML::Emitter& emitter)
{
	CUIBase::Serialize(emitter);
	emitter << YAML::Key << NAME_OF(m_text) << YAML::Value << m_text;
	emitter << YAML::Key << NAME_OF(m_fontSize) << YAML::Value << m_fontSize;
	emitter << YAML::Key << NAME_OF(m_fontName) << YAML::Value << m_fontName;
	emitter << YAML::Key << NAME_OF(m_alphaEnable) << YAML::Value << m_alphaEnable;
	emitter << YAML::Key << NAME_OF(m_color) << YAML::Value << m_color;
	emitter << YAML::Key << NAME_OF(m_alignTextH) << YAML::Value << static_cast<int>(m_alignTextH);
	emitter << YAML::Key << NAME_OF(m_alignTextV) << YAML::Value << static_cast<int>(m_alignTextV);
	emitter << YAML::Key << NAME_OF(m_persistTime) << YAML::Value << m_persistTime;
	emitter << YAML::Key << NAME_OF(m_bHasTime) << YAML::Value << m_bHasTime;
}

void CUIText::Deserialize(const YAML::Node& node)
{
	CUIBase::Deserialize(node);
	m_text        = node[NAME_OF(m_text)].as<std::string>();
	m_fontSize    = node[NAME_OF(m_fontSize)].as<float>();
	m_fontName    = node[NAME_OF(m_fontName)].as<std::string>();
	m_alphaEnable = node[NAME_OF(m_alphaEnable)].as<bool>();
	m_color       = node[NAME_OF(m_color)].as<Vec4>();
	m_alignTextH  = static_cast<TEXT_ALIGN_HORIZONTAL>(node[NAME_OF(m_alignTextH)].as<int>());
	m_alignTextV  = static_cast<TEXT_ALIGN_VERTICAL>(node[NAME_OF(m_alignTextV)].as<int>());

	if (node[NAME_OF(m_persistTime)].IsDefined())
	{
		m_persistTime = node[NAME_OF(m_persistTime)].as<float>();
	}
	if (node[NAME_OF(m_bHasTime)].IsDefined())
	{
		m_bHasTime = node[NAME_OF(m_bHasTime)].as<bool>();
	}
}
