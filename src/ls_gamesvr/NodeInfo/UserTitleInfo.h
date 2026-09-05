#pragma once
#include "../BoostPooler.h"
#include "DefaultTitleData.h"

class UserTitleInfo : public BoostPooler<UserTitleInfo>, public DefaultTitleData
{
public:
	UserTitleInfo();
	virtual ~UserTitleInfo();

	void Init();
	void Destroy();

public:
	void FillData(SP2Packet &rkPacket);
	void ApplyData(SP2Packet &rkPacket);

public:
	void CreateTitle(const DWORD dwCode, const __int64 iValue, const int iLevel, const BOOL bPremium, const BOOL bEquip, const TitleStatus eStatus);
	void Confirm();
	
	BOOL IsActiveTitle();
	void ActiveTitle();

	BOOL IsNewData();

	TitleStatus GetTitleStatus();
	void SetTitleStatus(TitleStatus eStatus);

public:
	void ApplySQLData(CQueryResultData *query_data);

protected:
	TitleStatus		m_eStatus;
	__int64			m_iPremiumSecond;
};