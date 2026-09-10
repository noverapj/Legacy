#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

struct GaugeSlotInfo
{
	int m_iSlotNum;
	float m_fGaugeRate;

	GaugeSlotInfo()
	{
		m_iSlotNum = -1;
		m_fGaugeRate = 0.0f;
	}
};
typedef std::vector<GaugeSlotInfo> vGaugeSlotInfoList;

class GaugeSlotInfoSort : std::binary_function< const GaugeSlotInfo&, const GaugeSlotInfo&, bool >
{
public:
	bool operator()( const GaugeSlotInfo &lhs , const GaugeSlotInfo &rhs ) const
	{
		if( lhs.m_fGaugeRate < rhs.m_fGaugeRate )
		{
			return true;
		}

		return false;
	}
};


class ioRestorationGaugeBuff : public ioBuff
{
protected:
	enum SortType
	{
		ST_SORT,
		ST_RANDOM_SHUFFLE,
	};
	// 정렬타입
	// 0 : 일반 Sort 
	// 1 : RandomShuffle
	// ※ 정렬타입이 추가될 경우를 위해 int형으로 선언.
	int				m_iSortType;
	int				m_iSortReduceFullGauge;

	CEncrypt<float> m_fRecoveryGauge;
	CEncrypt<DWORD> m_dwRecoveryTick;
	CEncrypt<DWORD> m_dwTickGap;

	CEncrypt<bool>	m_bLimitMax;
	CEncrypt<bool>	m_bOneItem;
	CEncrypt<int>	m_iRecoverySlot;

	CEncrypt<int>		m_iRecoveryItemCnt;
	CEncrypt<int>		m_iCurRecoveryItemCnt;
	CEncrypt<bool>		m_bRecoveryCountByRate;
	FloatVec			m_RecoveryCountRateList;
	vGaugeSlotInfoList	m_vRecoverySlotList;
	CEncrypt<bool>		m_bRecoveryByRate;
	FloatVec			m_RecoveryRate;

	CEncrypt<float> m_fGaugeLimitRate;

	CEncrypt<bool>	m_bRestoreOwnerItem;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

protected:
	int GetLowestItem();
	void GetLowestItemList();

public:
	ioRestorationGaugeBuff();
	ioRestorationGaugeBuff( const ioRestorationGaugeBuff &rhs );
	virtual ~ioRestorationGaugeBuff();
};

