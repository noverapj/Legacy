#pragma once

#include "../MainProcess.h"
#include "../../include/MemPooler.h"
#include "../Define.h"


class BillInfoManager
{
public:
	enum
	{
		MAX_INFO         = 5000,
		MAX_ALIVE_TIME   = 300000, // 5분 
	};

	enum ActionType 
	{
		AT_GET    = 1,
		AT_OUTPUT = 2,
		AT_SUBSCRIPTIONRETRACT = 3,
	};

public:
	struct BillingInfo
	{
		ioHashString     m_szKey;
		ActionType       m_eType;
		DWORD			 m_dwKey;				//넥슨 유럽 PacketNo(accountidx) Key 로 사용하기 위함
		DWORD			 m_dwReqKey;
		DWORD            m_dwCreateTime;
		DWORD			 m_iChannelingType;		//로그용
		SP2Packet        m_kPacket;

		TwoOfINTVec m_vBonusCashForConsume;

		BillingInfo()
		{
			InitData();
		}

		void InitData()
		{
			m_szKey.Clear();
			m_eType				= AT_GET;
			m_dwKey				= 0;
			m_dwReqKey			= 0;
			m_dwCreateTime		= 0;
			m_iChannelingType	= 0;
			m_kPacket.Clear();
			m_kPacket.SetPosBegin(); // 헤더부분에 데이터를 셋팅하지 않기 위해서
			m_vBonusCashForConsume.clear();
		}
		void AddBonusCashInfoForConsume(const int iIndex, const int iValue)
		{
			TwoOfINT stInfo;
			stInfo.value1	= iIndex;
			stInfo.value2	= iValue;

			m_vBonusCashForConsume.push_back(stInfo);
		}
		void GetBonusCashInfo(TwoOfINTVec& vInfo) const { vInfo = m_vBonusCashForConsume; }
	};

public:
	typedef std::list< BillingInfo* > BillInfoList;//vector -> list

protected:
	BillInfoList m_billInfos;
	MemPooler< BillingInfo > m_billInfoPool;

public:
	void InitMemoryPool();
	void ReleaseMemoryPool();
	
public:
	BillingInfo * PopBillInfo();

public:
	BillingInfo *Get( const ioHashString &rszKey );
	bool Add( BillingInfo *pBillingInfo );
	void Delete( const ioHashString &rszKey );

	
	                                                          
	BillingInfo *GetEUInfo( const DWORD dwKey );
	bool AddEUInfo( BillingInfo *pBillingInfo );
	void DeleteEUInfo( const DWORD dwKey );

	int GetCount() { return m_billInfos.size(); }
	
public:
	void DeleteMaxAliveTimeoutInfo( int maxAliveTime );
	
public:
	int GetMemoryPoolCount() { return m_billInfoPool.GetCount(); };
	int GetBillInfoCount() { return m_billInfos.size(); };
	
public:
	void RemainBillInfoCount();

public:
	BillInfoManager();
	virtual ~BillInfoManager(void);
};
typedef cSingleton<BillInfoManager> S_BillInfoManager;
#define g_BillInfoManager S_BillInfoManager::GetInstance()

