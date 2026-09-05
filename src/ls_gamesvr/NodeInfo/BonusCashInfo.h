#pragma once

#include "../BoostPooler.h"

class BonusCashInfo : public BoostPooler<BonusCashInfo>
{
public:
	BonusCashInfo();
	virtual ~BonusCashInfo();

	void Init();
	void Destroy();

public:
	void Create(const DWORD dwIndex, const int iTotalCash, const int iRemainingAmount, DBTIMESTAMP& dts);
	void Create(const DWORD dwIndex, const int iTotalCash, const int iRemainingAmount, DWORD dwExpirateDate);

	void ApplyCashInfo(SP2Packet& kPacket);
	void FillCashInfo(SP2Packet& kPacket); 

public:
	int GetTotalAmount();
	int GetRemainingAmount();
	__int64 GetExpirationDate();
	DWORD	GetIndex();

	void SetFlag(BOOL bVal);

	void UpdateRemainingAmount(const int iVal);

	BOOL SpendCash(int iVal);
	//BOOL IsActive();
	BOOL IsAvailable();
	BOOL IsExpired();

protected:
	bool operator< (const BonusCashInfo& rhs) { return m_iExpirationDate < rhs.m_iExpirationDate; }

protected:
	DWORD m_dwIndex;
	int	m_iTotalCash;
	int m_iRemainingAmount;
	__int64 m_iExpirationDate;

	BOOL m_bActive;
};