#include "stdafx.h"
#include "DefaultTitleData.h"

DefaultTitleData::DefaultTitleData()
{
	Init();
}

DefaultTitleData::~DefaultTitleData()
{
	Destroy();
}

void DefaultTitleData::Init()
{
	m_dwTitleCode	= 0;
	
	m_iCurValue		= 0;
	m_iLevel		= 0;
	
	m_bPrimium		= FALSE;
	m_bEquip		= FALSE;
}

void DefaultTitleData::Destroy()
{
}

void DefaultTitleData::CreateTitleInfo(const DWORD dwCode, const __int64 iValue, const int iLevel, const BOOL bPremium, const BOOL bEquip)
{
	m_dwTitleCode	= dwCode;
	m_iCurValue		= iValue;
	m_iLevel		= iLevel;
	m_bPrimium		= bPremium;
	m_bEquip		= bEquip;
}

DWORD DefaultTitleData::GetCode()
{
	return m_dwTitleCode;
}

int	DefaultTitleData::GetTitleLevel()
{
	return m_iLevel;
}

__int64 DefaultTitleData::GetCurValue()
{
	return m_iCurValue;
}

void DefaultTitleData::SetEquipInfo(BOOL bEquip)
{
	m_bEquip = bEquip;
}

void DefaultTitleData::SetValue(__int64 iValue)
{
	m_iCurValue = iValue;
}

void DefaultTitleData::SetLevel(int iLevel)
{
	m_iLevel = iLevel;
}

void DefaultTitleData::SetPremium(BOOL bPrimium)
{
	m_bPrimium	= bPrimium;
}

void DefaultTitleData::ActivePrimium()
{
	m_iCurValue	= TRUE;
}

BOOL DefaultTitleData::IsPremium()
{
	return m_bPrimium;
}

BOOL DefaultTitleData::IsEquip()
{
	return m_bEquip;
}
	
void DefaultTitleData::LevelUpTitle()
{
	m_iLevel++;
}

void DefaultTitleData::AddValue(const __int64 iValue)
{
	m_iCurValue += iValue;
}