#pragma once

class SP2Packet;
class ioComplexStringPrinter;
class ioPlayStage;

class ioSubscriptionMgr : public Singleton< ioSubscriptionMgr >
{
public:
	struct SubscriptionData
	{
		DWORD m_dwIndex;					// DB 인덱스.

		ioHashString m_szSubscriptionID;
		int m_iSubscriptionGold;
		int m_iSubscriptionBonusGold;

		short m_iPresentType;				// ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
		int m_iPresentValue1;				// 용병:(용병타입), 치장,랜덤치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드)
		int m_iPresentValue2;				// 용병:(용병기간), 치장,랜덤치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비(( 장비 성장값 * 10000 ) + 장비기간)
											// 장비상자(00[00]:1골드,0페소 | [00]00:페소Array 1부터99 -1 빼서 실제 array 구함 )
		int m_iMentType;
		DWORD m_dwLimitDate;				// 기한

		SubscriptionData()
		{
			m_dwIndex = 0;
			m_iSubscriptionGold = 0;
			m_iSubscriptionBonusGold = 0;
			m_iMentType = 0;

			m_iPresentType = 0;
			m_iPresentValue1 = m_iPresentValue2 = 0;
			m_dwLimitDate = 0;
		}
	};

protected:
	typedef std::vector< SubscriptionData > vSubscriptionData;
	vSubscriptionData m_vSubscriptionList;

	int m_iSubscriptionMentType;
	int m_iSubscriptionBonusCashMentType;
	int m_iSubscriptionLimitDate;

	DWORD m_dwSubscriptionCurTimer;
	DWORD m_dwSubscriptionRequestTime;
	DWORD m_dwSubscriptionRequestCount;

protected:
	bool IsAlreadySubscription( DWORD dwIndex, const ioHashString &szSubscriptionID );
	void DeleteSubscription( DWORD dwIndex, const ioHashString &szSubscriptionID );
	
	void SendSubscriptionData();

	bool RecvEtcItem( SubscriptionData &rkData, int &iItemType, ComplexStringPrinterVec& vTitle, ComplexStringPrinterVec& vDesc, int &nGradeType );

public:
	void LoadINIInfo();

public:
	void SendSubscriptionDataToTime();
	void ProcessSubscription();

public:
	void ApplySubscriptionData( SP2Packet &rkPacket );
	void ApplySubscriptionRecv( SP2Packet &rkPacket, ioPlayStage *pStage );
	void ApplySubscriptionRetr( SP2Packet &rkPacket );

	void ApplySubscriptionDataAdd( SP2Packet &rkPacket );

public:
	bool GetSubscriptionDataToArray( IN int iArray, OUT SubscriptionData& rkData );
	bool GetSubscriptionDataToIndex( IN DWORD dwIndex, IN const ioHashString& szSubscriptionID, OUT SubscriptionData& rkData );

	bool CheckSubscriptionRetractLimit( DWORD dwIndex, const ioHashString &szSubscriptionID );

	bool CheckBuyDate( IN DWORD dwLimitDate, OUT time_t &rkBuyTime );

public:
	int GetMaxSubscription() { return m_vSubscriptionList.size(); }

public:
	static ioSubscriptionMgr& GetSingleton();

public:
	ioSubscriptionMgr();
	virtual ~ioSubscriptionMgr();
};

#define g_SubscriptionMgr ioSubscriptionMgr::GetSingleton()

