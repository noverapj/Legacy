#pragma once

#include "TitleTriggerBase.h"

class UserTitleInfo;

class UserTitleInven
{
public:
	UserTitleInven();
	virtual ~UserTitleInven();

	void Initialize(User* pUser);
	void Init();
	void Destroy();

public:
	void DBtoData(CQueryResultData *query_data);

	void FillMoveData(SP2Packet &rkPacket);
	void ApplyMoveData(SP2Packet &rkPacket, bool bDummyNode = false);

public:
	void SQLUpdateTitle(UserTitleInfo *pData, TitleUpdateType eType);
	void ConfirmNewData();

public:
	UserTitleInfo* GetEquipTitle();

	UserTitleInfo* GetTitle(const DWORD dwCode);

	int GetAllTitleCount();
	int	GetActiveTitleCount();

	BOOL IsActive() { return m_bActive; }
	void SetActive(BOOL bVal) {  m_bActive = bVal; }

	void SetGetCashFlag(BOOL bVal) { m_bEarlyGetCash = bVal; }

public:
	void CheckTitleValue(TitleTriggerClasses eClass, const __int64 iValue);

	BOOL IsExist(const DWORD dwCode);

	void UpdateTitle(const DWORD dwCode, const __int64 iValue, const int iLevel, const BYTE byPremium, const BYTE byEquip, const BYTE byStatus, const BYTE byActionType);
	void AddTitle(const DWORD dwCode, const __int64 iValue, const int iStatus);
	
	void ConvertNewToActiveStatus();

	BOOL PremiumLevelUpCheck(const DWORD dwCode);
	BOOL IsRightAccumulateSeconds(const __int64 iCurData, const __int64 iNewData);
	BOOL CheckHavingPrecedeTitle(const DWORD dwCode);
	BOOL IsBoradCastingInfo(const int iState);

	void EquipTitle(const DWORD dwCode);
	void ReleaseTitle(const DWORD dwCode);

protected:
	typedef std::vector<UserTitleInfo*> vTitleData;

protected:
	vTitleData	m_vTitleInven;
	User*		m_pUser;

	BOOL		m_bActive;
	BOOL		m_bEarlyGetCash;
};