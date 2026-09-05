#pragma once
#include "DefaultTitleData.h"
#include "TitleTriggerBase.h"

class UserTitleInfo;

class TitleData : public BoostPooler<TitleData>, public DefaultTitleData
{
public:
	TitleData();
	virtual ~TitleData();

	void Init();
	void Destroy();

public:
	void CreateTitleByDesignated(TitleTriggerClasses eClass, TitleTriggerClasses ePrimiumClass, const DWORD dwCode, const __int64 iValue, const int iMaxLevel, const DWORD dwPrecedeCode);
	BOOL IsComplete(const __int64 iValue);

	BOOL DoTrigger(UserTitleInfo* pInfo, const __int64 iValue);

	TitleTriggerClasses GetClass() { return m_eClass; }

public:
	void SetClass(TitleTriggerClasses eClass) { m_eClass = eClass; }
	void SetPrecedeCode(const DWORD dwCode) { m_dwPrecedeCode = dwCode; }

	DWORD GetPrecedeCode()	{ return m_dwPrecedeCode; }

protected:
	//Trigger Ãß°¡.
	TitleTriggerBase* m_pTrigger;

	TitleTriggerClasses m_eClass;
	DWORD m_dwPrecedeCode;
};