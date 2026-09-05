
#ifndef _ioUserSubscription_h_
#define _ioUserSubscription_h_

class User;
class CQueryResultData;

class ioUserSubscription
{
public:
	enum
	{
		SUBSCRIPTION_STATE_NORMAL,
		SUBSCRIPTION_STATE_RETRACT_WAIT,
	};

	enum DeleteSubscriptionType
	{
		DST_TIMEOVER = 1,
		DST_RECV     = 2,
		DST_RETR     = 3,
	};

public:
	struct SubscriptionData
	{
		DWORD m_dwIndex;					// DB 인덱스

		ioHashString m_szSubscriptionID;
		int m_iSubscriptionGold;

		short   m_iPresentType;            // ( 1.용병 - 2.치장 - 3.권한 - 4.페소 - 5.장비 )
		int     m_iPresentValue1;          // 용병:(용병타입), 치장,랜덤치장(ITEMSLOT의 m_item_type), 권한(ETCITEMSLOT의 m_iType), 페소(페소금액), 장비(장비코드), 장비상자(뽑기번호)
		int     m_iPresentValue2;          // 용병:(용병기간), 치장,랜덤치장(ITEMSLOT의 m_item_code), 권한(ETCITEMSLOT의 m_iValue1), 페소(NONE), 장비((확장 타입 * 100000000) + ( 장비 성장값 * 10000 ) + 장비기간)
		                                   // 장비상자(00[00]:1골드,0페소 | [00]00:페소Array 1부터99 -1 빼서 실제 array 구함 )
		short m_iSubscriptionState;
		DWORD   m_dwLimitDate;             // 기간 제한 : 0909141200(2009년9월14일12시까지 보관 이후 철회불가), 0이면 삭제된 상품
		int m_iRetractGold;
		int m_iUsedBonusCash;

		SubscriptionData()
		{
			m_dwIndex = 0;

			m_iSubscriptionGold = 0;
			m_iRetractGold = 0;
			m_iUsedBonusCash	= 0;

			m_iPresentType = 0;
			m_iPresentValue1 = m_iPresentValue2 = 0;
			m_dwLimitDate = 0;
			m_iSubscriptionState = SUBSCRIPTION_STATE_NORMAL;
		}
	};

protected:
	User *m_pUser;
	
	typedef std::vector< SubscriptionData > vSubscriptionData;
	vSubscriptionData m_vSubscriptionList;
	vSubscriptionData m_vTempMemoryData;
	
	DWORD m_dwLastDBIndex;

public:
	void Initialize( User *pUser );

	// DB Load
public:
	void DBtoSubscriptionData( CQueryResultData *query_data );	

public:
	DWORD GetLastSubscriptionDBIndex();

protected:
	ioUserSubscription::SubscriptionData& GetSubscriptionData( DWORD dwIndex, const ioHashString& szSuscriptionID );
	bool DeleteSubscriptionData( DWORD dwIndex, const ioHashString& szSuscriptionID, DeleteSubscriptionType eDeleteSubscriptionType );
	bool CompareSubscriptionData( SubscriptionData &kData );

public:
	bool CheckExistSubscriptionData( DWORD dwIndex, const ioHashString& szSuscriptionID );
	bool CheckSubscriptionState( DWORD dwIndex, const ioHashString& szSuscriptionID );

	void SubscriptionRecv( IN SP2Packet &rkRecvPacket, OUT SP2Packet &rkPacket );
	void SubscriptionRetr( DWORD dwIndex, const ioHashString& szSuscriptionID, int iGold, SP2Packet &rkPacket, bool bDefaultGold );
	void SubscriptionDisassemble( IN SP2Packet &rkRecvPacket, OUT SP2Packet &rkPacket );

	bool CheckLimitData( DWORD dwIndex, const ioHashString& szSuscriptionID );

	void SetSubscriptionState( DWORD dwIndex, const ioHashString& szSuscriptionID, short iState );
	void SetRetractGold( DWORD dwIndex, const ioHashString& szSuscriptionID, int iGold );

	int GetSubscriptionGold( DWORD dwIndex, const ioHashString& szSuscriptionID );
	int GetRetractGold( DWORD dwIndex, const ioHashString& szSuscriptionID );

	int GetDBRetractGold(DWORD dwIndex, const ioHashString& szSuscriptionID );
	int GetUsedBonusCash(DWORD dwIndex, const ioHashString& szSuscriptionID );

public:
	void AddSubscriptionMemory( DWORD dwUserIndex, const ioHashString &szSubscriptionID, int iSubscriptionGold,
								short iPresentType, int iPresentValue1, int iPresentValue2,
								CTime &rkLimitTime );
	void SendSubscriptionMemory();
	void LogoutMemorySubscriptionInsert();     // 유저 로그 아웃시에 저장

public:
	void SubscriptionMileageSend(SubscriptionData &rkData);

public:
	int GetSubscriptionDataCnt();

	void FillMoveData( SP2Packet &rkPacket );
	void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	ioUserSubscription();
	virtual ~ioUserSubscription();
};

#endif