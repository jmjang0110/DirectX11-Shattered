#pragma once
#include "ComponentUI.h"
#include "ImGuizmo/ImGuizmo.h"

enum class GUIZMO_TYPE
{
	TRANSLATE = ImGuizmo::TRANSLATE,
	ROTATE = ImGuizmo::ROTATE,
	SCALE = ImGuizmo::SCALE,
};

class TransformUI : public ComponentUI
{
public:
	static void        SetEnableGuizmo(bool _enable) { s_enableGuizmo = _enable; }
	static bool        IsGuizmoEnabled() { return s_enableGuizmo; }
	static void        SetGuizmoType(GUIZMO_TYPE _type) { s_guizmoType = _type; }
	static GUIZMO_TYPE GetGuizmoType() { return s_guizmoType; }

private:
	inline static bool        s_enableGuizmo = true;
	inline static GUIZMO_TYPE s_guizmoType   = GUIZMO_TYPE::TRANSLATE;
private:
	Vec3 m_vRelativePos;
	Vec3 m_vRelativeScale;
	Vec3 m_vRelativeRot;

	bool m_bIgnorantParentScale;
public:
	void update() override;
	void render_update() override;

public:
	TransformUI();
	virtual ~TransformUI();
};
