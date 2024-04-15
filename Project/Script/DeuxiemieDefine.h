#pragma once

namespace GAME
{
	namespace DEUXIEMIE
	{
		enum class TYPE
		{
			GUN
			, SWORD
			, SHIELD
			, END
		};

		inline constexpr const wchar_t* IDLE   = L"IDLE";
		inline constexpr const wchar_t* STUN   = L"STUN";
		inline constexpr const wchar_t* ATTACK = L"ATTACK";
		inline constexpr const wchar_t* DEATH  = L"DEATH";
		inline constexpr const wchar_t* RUN    = L"RUN";
		inline constexpr const wchar_t* WALK   = L"WALK";
		inline constexpr const wchar_t* SHIELD = L"SHIELD";

		namespace GUN
		{
			inline constexpr const wchar_t* ATTACK_FLOOR  = L"ATTACK_FLOOR";
			inline constexpr const wchar_t* DEATH         = L"DEATH";
			inline constexpr const wchar_t* GUN_AIM       = L"GUN_AIM";
			inline constexpr const wchar_t* IDLE          = L"IDLE";
			inline constexpr const wchar_t* IDLE_FIGHT    = L"IDLE_FIGHT";
			inline constexpr const wchar_t* RUN_FORWARD   = L"RUN_FORWARD";
			inline constexpr const wchar_t* STUN          = L"STUN";
			inline constexpr const wchar_t* WALK_FORWARD  = L"WALK_FORWARD";
			inline constexpr const wchar_t* WALK_LEFT     = L"WALK_LEFT";
			inline constexpr const wchar_t* WALK_RIGHT    = L"WALK_RIGHT";
			inline constexpr const wchar_t* WALK_BACKWARD = L"WALK_FORWARD";
		}
	}
}
