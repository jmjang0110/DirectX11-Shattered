#pragma once

namespace GAME::BOSS::PHASE
{
	inline constexpr const wchar_t* NONE            = L"JUG_PHASE_NONE";
	inline constexpr const wchar_t* JUG_PHASE_INTRO = L"JUG_PHASE_INTRO";
	inline constexpr const wchar_t* JUG_PHASE_1     = L"JUG_PHASE_1";
	inline constexpr const wchar_t* JUG_PHASE_2     = L"JUG_PHASE_2";
	inline constexpr const wchar_t* JUG_PHASE_DEAD  = L"JUG_PHASE_DEAD";
}

namespace GAME::BOSS
{
	inline constexpr const char* NO_ANIM = "NULL";

	inline constexpr const wchar_t* JUG_NONE        = L"JUG_NONE";
	inline constexpr const wchar_t* JUG_INTRO       = L"JUG_INTRO";
	inline constexpr const wchar_t* JUG_NORM_IDLE   = L"JUG_NORM_IDLE";
	inline constexpr const wchar_t* JUG_NORM_HIT    = L"JUG_NORM_HIT";
	inline constexpr const wchar_t* JUG_HAMMER_IDLE = L"JUG_HAMMER_IDLE";
	inline constexpr const wchar_t* JUG_HAMMER_HIT  = L"JUG_HAMMER_HIT";
	inline constexpr const wchar_t* JUG_ATTACK_0    = L"JUG_ATTACK_0";
	inline constexpr const wchar_t* JUG_ATTACK_1    = L"JUG_ATTACK_1";
	inline constexpr const wchar_t* JUG_SPAWNHAMMER = L"JUG_SPAWNHAMMER";
	inline constexpr const wchar_t* JUG_WALKGROUND  = L"JUG_WALKGROUND";
	inline constexpr const wchar_t* JUG_FLY         = L"JUG_FLY";
	inline constexpr const wchar_t* JUG_DEAD        = L"JUG_DEAD";

	inline constexpr int DEFAULT_HP = 1000;
}
