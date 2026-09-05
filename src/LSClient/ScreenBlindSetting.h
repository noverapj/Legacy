#pragma once

struct ScreenBlindSetting
{
	int m_iMaxAlpha;
	DWORD m_dwColor;
};

struct ScreenBlindTime
{
	DWORD m_dwUpTime;
	DWORD m_dwCenterTime;
	DWORD m_dwDownTime;
};

enum ScreenBlindState
{
	SBS_NONE,
	SBS_UP,
	SBS_CENTER,
	SBS_DOWN,
};

enum ScreenBlindType
{
	SBT_NONE,
	SBT_SKILL,
	SBT_KILL,
	SBT_DEATH,
	SBT_CATCH_KILL,
	SBT_CATCH_DEATH,
	SBT_BUFF,
	SBT_FIXED_BLIND,
};

