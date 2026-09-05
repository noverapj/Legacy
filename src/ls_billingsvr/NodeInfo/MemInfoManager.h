#ifndef __MemInfoMgr_h__
#define __MemInfoMgr_h__

#include "../MainProcess.h"
//#include "../Channeling/ioChannelingNodeParent.h"
#include "../../include/MemPooler.h"

class BasicInfo : public SuperParent
{
public:
	ioHashString     m_szKey;
	DWORD            m_dwCreateTime;
	DWORD            m_dwUserIndex;
	ioHashString     m_szBillingGUID;
	ioHashString     m_szServrIP;
	int              m_iServerClientPort;

public:
	virtual void Clear();

public:
	BasicInfo();
	virtual ~BasicInfo();
};
//------------------------------------------------------------------------------------------------------------

class GetCashInfo : public BasicInfo
{
public:
	bool m_bSetUserMouse;

public:
	virtual void Clear();

public:
	GetCashInfo& operator=( const GetCashInfo &rhs );

public:
	GetCashInfo();
	virtual ~GetCashInfo();
};
//------------------------------------------------------------------------------------------------------------

class OutPutCashInfo : public BasicInfo
{
public:
	int          m_iItemType;
	int          m_iPayAmt;
	int          m_iChannelingType;
	ioHashString m_szValue;

	// 각각의 아이템 마다 순차적으로 값을 넣는다.
	int          m_iItemValueList[MAX_ITEM_VALUE];

public:
	virtual void Clear();

public:
	OutPutCashInfo& operator=( const OutPutCashInfo &rhs );

public:
	OutPutCashInfo();
	virtual ~OutPutCashInfo();
};

//------------------------------------------------------------------------------------------------------------


template < class T >
class MemInfoMgr
{
protected:
	enum 
	{ 
		MAX_ALIVE_TIME = 300000,
	};

protected:
	MemPooler<T>    m_MemoryPool;
	std::list< T* > m_PassList;

public:
	void New()
	{
		for(int i = 0;i < g_App.GetBillingPool() * 2 ;i++)//kyg Max값 조정 
		{
			T *pInfo = new T;
			if( !pInfo )
				continue;
			m_MemoryPool.Push( pInfo );
		}
	}

	void Release()
	{
		std::list< T* >::iterator iter, iEnd;	
		iEnd = m_PassList.end();
		for(iter = m_PassList.begin();iter != iEnd;++iter)
		{
			T *pInfo = *iter;
			if( !pInfo )
				continue;
			m_MemoryPool.Push( pInfo );
		}	
		m_PassList.clear();
		m_MemoryPool.DestroyPool();
	}

	bool Push( const T& rInfo )
	{
		T *pInfo = (T*) m_MemoryPool.Pop();
		if( !pInfo )
			return false;

		*pInfo = rInfo;
		m_PassList.push_back( pInfo );
		return true;
	}

	bool Pop( IN const ioHashString &rszKey, OUT T& rInfo )
	{
		std::list< T* >::iterator iter = m_PassList.begin();
		while ( iter != m_PassList.end() )
		{
			T *pInfo = *iter;
			if( !pInfo )
			{
				++iter;
				continue;
			}
			else if( pInfo->m_szKey != rszKey )
			{
				++iter;
				continue;
			}

			rInfo = *pInfo;
			m_PassList.erase( iter );
			pInfo->Clear();
			m_MemoryPool.Push( pInfo );		
			return true;
		}

		return false;
	}

	void PopDeadInfo()
	{
		std::list< T* >::iterator iter = m_PassList.begin();
		while ( iter != m_PassList.end() )
		{
			T *pInfo = *iter;
			if( !pInfo )
			{
				++iter;
				continue;
			}
			else if( TIMEGETTIME() - pInfo->m_dwCreateTime >= MAX_ALIVE_TIME )
			{
				LOG.PrintTimeAndLog( 0, "%s Delete past alive time : %s", __FUNCTION__,  pInfo->m_szKey.c_str() );
				iter = m_PassList.erase( iter );
				pInfo->Clear();
				m_MemoryPool.Push( pInfo );		
				continue;
			}
			++iter;
		}
	}

public:
	MemInfoMgr(){}
	virtual ~MemInfoMgr(){}
};

//-------------------------------------------------------------------------
class BillInfoMgr
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
	typedef struct tagBillingInfo
	{
		ioHashString     m_szKey;
		ActionType       m_eType;
		DWORD            m_dwCreateTime;
		DWORD			 m_iChannelingType;		//로그용
		SP2Packet        m_kPacket;

		tagBillingInfo()
		{
			InitData();
		}

		void InitData()
		{
			m_szKey.Clear();
			m_eType				= AT_GET;
			m_dwCreateTime		= 0;
			m_iChannelingType	= 0;
			m_kPacket.Clear();
			m_kPacket.SetPosBegin(); // 헤더부분에 데이터를 셋팅하지 않기 위해서
		}
	}BillingInfo;

protected:
	typedef std::vector< BillingInfo* > vBillingInfo; //kyg 벡터 말고 딴걸로 List로 하는방법이..? map이나 
	vBillingInfo m_vBillingInfo;
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
	int GetCount() { return m_vBillingInfo.size(); }
	
public:
	void DeleteMaxAliveTimeoutInfo( int maxAliveTime );
public:
	int GetMemoryPoolCount() { return m_billInfoPool.GetCount(); };
	int GetBillInfoCount() { return m_vBillingInfo.size(); };

public:
	BillInfoMgr(void);
	virtual ~BillInfoMgr(void);
};

#endif // __MemInfoMgr_h__