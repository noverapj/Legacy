#pragma once

enum TitleStatus
{
	TITLE_DISABLE	= 0,
	TITLE_ACTIVE	= 1,
	TITLE_NEW		= 2,
};

class DefaultTitleData
{
public:
	DefaultTitleData();
	virtual ~DefaultTitleData();

	void Init();
	void Destroy();

public:
	void CreateTitleInfo(const DWORD dwCode, const __int64 iValue, const int iLevel, const BOOL bPremium, const BOOL bEquip);

public:
	DWORD GetCode();
	int	GetTitleLevel();
	__int64 GetCurValue();

	void SetEquipInfo(BOOL bEquip);
	void SetValue(__int64 iValue);
	void SetLevel(int iLevel);
	void SetPremium(BOOL bPrimium);
	
	void ActivePrimium();

	BOOL IsPremium();

	BOOL IsEquip();
	
	void LevelUpTitle();

	void AddValue(const __int64 iValue);

protected:
	DWORD	m_dwTitleCode;
	
	__int64	m_iCurValue;
	int		m_iLevel;
	
	BOOL	m_bPrimium;
	BOOL	m_bEquip;
};