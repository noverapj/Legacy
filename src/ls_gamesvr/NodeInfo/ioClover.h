
#pragma once

class ioClover : public ioDBDataController
{
	enum
	{
		DEFAULT_YEAR	= 2010,			// 2010
		DATE_YEAR_VALUE	= 100000000,    // 년까지 나눈다.
		DATE_MONTH_VALUE= 1000000,      // 월까지 나눈다.
		DATE_DAY_VALUE	= 10000,        // 일까지 나눈다.
		DATE_HOUR_VALUE = 100,          // 시까지 나눈다.
	};

public:
	enum
	{
		CLOVER_TYPE_DEFAULT	= 0,
		CLOVER_TYPE_CHARGE,
		CLOVER_TYPE_REFILL,
		CLOVER_TYPE_SEND,
		CLOVER_TYPE_RECEIVE,
		CLOVER_TYPE_REMOVE,
	};

	enum GIFTCLOVER
	{
		GIFT_CLOVER_DEFAULT					= 0,
		GIFT_CLOVER_LOGIN,					// 로그인
		GIFT_CLOVER_CHARGE_SUCCESS,			// 충전 성공
		GIFT_CLOVER_CHARGE_FAIL,			// 충전 실패
		GIFT_CLOVER_REFILL,					// 전체리필
	};

	enum FRIENDCLOVER
	{
		FRIEND_CLOVER_DEFAULT						= 10,

		FRIEND_CLOVER_SEND_SUCCESS,					// 클로버 보내기 성공
		FRIEND_CLOVER_SEND_FAIL_ALREADY_SEND,		// 클로버 보내기 실패 - 보낼 수 있는 시간이 아직 안됐음.
		FRIEND_CLOVER_SEND_FAIL_NOT_FRIEND,			// 클로버 보내기 실패 - 친구가 아님.
		FRIEND_CLOVER_SEND_FAIL_FRIEND_REG_TIME,	// 클로버 보내기 실패 - 친구를 맺은 지 얼마 안 됏음.
		FRIEND_CLOVER_SEND_FAIL_NOT_ENOUGH_CLOVER,	// 클로버 보내기 실패 - 클로버 부족

		FRIEND_CLOVER_RECV_SUCCESS,					// 클로버 받기 성공
		FRIEND_CLOVER_RECV_FAIL_NOT_CLOVER,			// 클로버 받기 실패 - 받은 클로버가 없다.
		FRIEND_CLOVER_RECV_FAIL_NOT_FRIEND,			// 클로버 받기 실패 - 친구가 아님.

		FRIEND_CLOVER_COME_TO_FRIEND,				// 친구가 보낸 클로버가 도착했다.
	};

	enum
	{
		MAGIC_TIME = 2,
	};

private:
	bool m_bChangeData;			// 변경사항 있을시 DB 저장.
	int	m_iCloverCount;			// 가지고 있는 클로버 갯수.
	int m_iLastChargeTime;		// 마지막 충전 시간.
	short m_sRemainTime;		// 남은시간(분)
	DWORD m_dwCloverCheckTime;	// Check Time

public:
	void Init();
	void Destroy();

private:
	User* GetUser(){ return m_pUser; }
	void SetUser( User* pUser ){ m_pUser = pUser; }
	const bool GetChangeData(){ return m_bChangeData; }
	void SetChangeData( const bool bState ){ m_bChangeData = bState; }
	void SetCloverCount( const int iCount );
	void SetLastChargeTime( const int iChargeTime );
	void SetMaxRemainTime();

	void IncreaseCloverCount( const int iCount );	// 충전
	void DecreaseCloverCount( const int iCount );	// 소모
	void UpdateCloverCharge( const int iCount );
	void UpdateCloverRefill( const int iCount, CTime& CurrentTime );
	void SetConvertDate( CTime& CurrentTime );
	void CheckCloverCount();
	void CheckCloverRemainTime();
	const int ConvertYYMMDDToDate( const WORD wYear, const WORD wMonth, const WORD wDay );

public:
	const int GetCloverCount();
	const int GetLastChargeTime(){ return m_iLastChargeTime; }
	const short GetCloverRemainTime();

	CTime ConvertNumberToCTime( const int iDateTime );
	BOOL IsValidCharge( CTime& CurrentTime );
	void IsSaveRemainTime();

	// 클로버 정보 변경은 UpdateCloverInfo() 로..
	void UpdateCloverInfo( const int iType, const int iCount, CTime& CurrentTime );

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	ioClover();
	virtual ~ioClover();
};
