#pragma once

#include "../NodeInfo/MemInfoManager.h"

class ServerNode;
class ioData;
 

class ioChannelingNodeParent
{
public: 

protected:
	friend class ioLocalUS;
	friend class ioLocalTaiwan;
	friend class ioLocalIndonesia;
	friend class ioLocalThailand;
	friend class ioLocalChina;
	friend class ioLocalJapan;
	friend class ioLocalLatin;
	friend class ioLocalEU;

	typedef struct tagItemInfo
	{
		// soldier
		int m_iClassType;
		int m_iKindred;
		int m_iSex;
		int m_iLimitSecond;
		int m_iPeriodType;

		// deco
		int m_iDecoType;
		int m_iDecoCode;
		int m_iCharArray;

		// etc
		DWORD m_dwType;
		int   m_iArray;

		// extra
		int   m_iMachineCode;

		// present
		short m_iPresentType;
		int   m_iBuyValue1;
		int   m_iBuyValue2;
		int	  m_iBuyValue3;
		DWORD m_dwRecvUserIndex;

		tagItemInfo()
		{
			m_iClassType   = 0;
			m_iKindred     = 0;
			m_iSex         = 0;
			m_iLimitSecond = 0;
			m_iPeriodType  = 0;

			m_iDecoType    = 0;
			m_iDecoCode    = 0;
			m_iCharArray   = 0;

			m_dwType	   = 0;
			m_iArray	   = 0;

			m_iMachineCode = 0;

			m_iPresentType = 0;
			m_iBuyValue1   = 0;
			m_iBuyValue2   = 0;
			m_iBuyValue3   = 0;
			m_dwRecvUserIndex = 0;
		}
	}ItemInfo;
	
protected:
	static void GetItemInfo( IN SP2Packet &rkPacket, IN int iType, OUT ItemInfo &rkBuyInfo );
	static void SetItemInfo( OUT SP2Packet &rkPacket, IN int iType, IN const ItemInfo &rkBuyInfo );

	static void GetItemValueList( IN SP2Packet &rkPacket, IN int iType, OUT int iItemValueList[MAX_ITEM_VALUE] );
	static void SetItemValueList( OUT SP2Packet &rkPacket, IN int iType, IN const int iItemValueList[MAX_ITEM_VALUE] );

protected:
	virtual void _OnGetCash( ServerNode *pServerNode, SP2Packet &rkPacket ){};
	virtual void _OnOutputCash( ServerNode *pServerNode, SP2Packet &rkPacket, DWORD dwGoodsNo, const ioHashString &rszGoodsName ){};
	virtual void _OnPCRoom( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void _OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket); //kyg 가상함수로 바꿈 
	virtual void _OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPacket);
	virtual void _OnSubscriptionRetractFail( ServerNode* pServerNode, SP2Packet& rkPacket);
	
protected:
	virtual void SendSubscriptionRetractErrorPacket( ServerNode* pServerNode, unsigned int errorId, const DWORD dwUserIndex, const ioHashString& szBillingGUID );
public:
	void OnGetCash( ServerNode *pServerNode,  SP2Packet &rkPacket );
	void OnOutputCash( ServerNode *pServerNode,  SP2Packet &rkPacket );
	void OnPCRoom( ServerNode *pServerNode,  SP2Packet &rkPacket );
	void OnQuestComplete(ServerNode *pServerNode,  SP2Packet &rkPacket );

public:
	virtual void OnCancelCash( ServerNode *pServerNode, SP2Packet &rkPacket ) {}
	virtual void OnAddCash( ServerNode *pServerNode, SP2Packet &rkPacket ){}
	virtual void OnFillCashUrl( ServerNode *pServerNode, SP2Packet &rkPacket ){}

public:
	void OnSubscriptionRetractCheck( ServerNode* pServerNode, SP2Packet &rkPacket );
	void OnSubscriptionRetract( ServerNode* pServerNode, SP2Packet& rkPakcet );
	void OnSubscriptionRetractFail( ServerNode* pServerNode, SP2Packet& rkPakcet );

public:
	virtual void ThreadGetCash( const ioData &rData ) {}
	virtual void ThreadOutputCash( const ioData &rData ) {}
	virtual void ThreadFillCashUrl( const ioData &rData ) {}
	virtual void ThreadSubscriptionRetract (const ioData &rData );
	virtual void ThreadSubscriptionRetractCheck (const ioData &rData );
	
public:
	virtual ChannelingType GetType();

	bool TestState() const { return m_testState; }
	void TestState(bool val) { m_testState = val; }

protected:
	void MultiToUni(const char* multiStr, wchar_t* uniStr, int maxLen);
	void UniToUTF8(const wchar_t* uniStr, char* utf8String, int maxLen);

public:
	void MultiToUTF8(IN const char* szMultiStr, OUT char* szUTF8Str);

protected:
	void SendTestGetCash( ServerNode *pServerNode,SP2Packet& rkPacket);
	void SendTestOutputCash( ServerNode *pServerNode,SP2Packet& rkPacket);

public:
	virtual int GetCount() { return 0; }


protected:
	bool m_testState;


public:
	ioChannelingNodeParent(void);
	virtual ~ioChannelingNodeParent(void);
};
