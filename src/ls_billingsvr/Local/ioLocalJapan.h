#ifndef __ioLocalJapan_h__
#define __ioLocalJapan_h__

#include "ioLocalParent.h"
#include "../Channeling/ioChannelingNodeParent.h"
#include "../NodeInfo/TestCashManager.h"

#define JAPAN_MILEAGE_KEY "fhtk!2akdlfflwl"

class MileageInfo : public SuperParent
{
public:
	DWORD        m_dwUserIndex;
	ioHashString m_sPrivateID;
	ioHashString m_sPublicID;
	ioHashString m_sServerIP;
	int          m_iServerPort;
	DWORD        m_dwCreateTime;
	ioHashString m_sPublicIP;
	int          m_iPresentType;
	int          m_iValue1;
	int          m_iValue2;
	bool         m_bPresent;


	void Clear()
	{
		m_dwUserIndex = 0;
		m_sPrivateID.Clear();
		m_sPublicID.Clear();
		m_sServerIP.Clear();
		m_iServerPort = 0;
		m_dwCreateTime= 0;
		m_sPublicIP.Clear();
		m_iPresentType = 0;
		m_iValue1      = 0;
		m_iValue2      = 0;
		m_bPresent     = false;
	}
public:
	MileageInfo(void){ Clear(); }
	virtual ~MileageInfo(void){}
};

typedef std::vector< MileageInfo* > vMileageInfo;

class BillingInfo : public SuperParent
{
public:
	DWORD        m_dwUserIndex;
	ioHashString m_sPrivateID;
	ioHashString m_sPublicID;
	ioHashString m_sBillingGUID;
	bool         m_bSetUserMouse;
	ioHashString m_sServerIP;
	int          m_iServerPort;
	DWORD        m_dwCreateTime;
	int          m_iItemType;
	int          m_iItemValueList[MAX_ITEM_VALUE];
	int          m_iTotalCash;
	int          m_iPayCash;

	void Clear()
	{
		m_dwUserIndex = 0;
		m_sPrivateID.Clear();
		m_sBillingGUID.Clear();
		m_bSetUserMouse = false;
		m_sServerIP.Clear();
		m_iServerPort = 0;
		m_dwCreateTime= 0;
		m_iItemType = 0;
		for (int i = 0; i <  MAX_ITEM_VALUE; i++)
		{
			m_iItemValueList[i] = 0;
		}
		m_iTotalCash = 0;
		m_iPayCash   = 0;
	}
public:
	BillingInfo(void){ Clear(); }
	virtual ~BillingInfo(void){}
};

typedef std::vector< BillingInfo* > vBillingInfo;

class ioLocalJapan  : public ioLocalParent
{
protected:
	enum
	{
		MAX_ALIVE_TIME    = 60000, // 1Ка 
	};

protected:
	TestCashManager m_TestCashManager;
	ioHashString    m_szCompanyCD;

	ioHashString m_sMileageGetURL;
	ioHashString m_sMileageAddURL;

protected:
	void GetTimeStamp( OUT char *szTimeStamp, IN int iTimeStampSize );
	int GetJapanBillingCode( DWORD dwGoodsNo );

public:
	virtual ioLocalManager::LocalType GetType();

	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName );
	virtual void OnGetMileage( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnAddMileage( ServerNode *pServerNode, SP2Packet &rkPacket );
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket );


	virtual void Init();

public:
	ioLocalJapan(void);
	virtual ~ioLocalJapan(void);
};

#endif // __ioLocalJapan_h__