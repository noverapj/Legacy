#pragma once

#include "../Util/Singleton.h"
#include "TitleTriggerBase.h"

class User;
class UserTitleInfo;
class TitleData;

class TitleManager : public Singleton< TitleManager >
{
public:
	TitleManager();
	virtual ~TitleManager();

	void Init();
	void Destroy();

public:
	static TitleManager& GetSingleton();

public:
	BOOL LoadINI(BOOL bReload = FALSE);

public:
	BOOL CheckAchevement(UserTitleInfo* pInfo, User* pUser, __int64 iValue);

	BOOL IsComplete(UserTitleInfo* pInfo);
	BOOL IsComplete(const DWORD dwCode, const __int64 iValue);

	BOOL IsLevelUp(const DWORD dwCode, const int iLevel, const DWORD dwValue);
	BOOL IsAccumulateData(TitleTriggerClasses eClass);

	void FillAccumulateTitleInfo(UserTitleInven* pInfo);

public:
	TitleData* GetTitleData(const DWORD dwCode);

	int	GetMaxPremiumLevel(const DWORD dwCode);

	void GetTargetClassTitleInfo(TitleTriggerClasses eClass, IntVec& vData);

	int GetMinCheckTime() { return m_iCheckTime; }
	int GetAddPremiumTime() { return m_iAddPremiumTime; }

protected:
	typedef boost::unordered_map<DWORD, TitleData*> mTitleTable;
	typedef mTitleTable::iterator mTitleTable_iter;

	typedef std::vector<IntVec> vPremiumTable;
	typedef vPremiumTable::iterator vPremimumTable_iter;

protected:
	mTitleTable m_mTitleTable;
	vPremiumTable m_vPremiumTable;

	int m_iAddPremiumTime;
	int m_iCheckTime;
};

#define g_TitleManager TitleManager::GetSingleton()