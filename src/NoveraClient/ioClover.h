
#pragma once

class ioClover
{
public:
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
		DAY_SEC = 86400,
		HOUR_SEC = 3600,
		MINUTE_SEC = 60,
	};

	enum
	{
		GIFT_CLOVER_MAX = 5,
		PROCESS_TIME    = 1000,
	};

private:
	int	m_iGiftCloverCount;	// 가지고 있는 클로버 갯수.
	int m_iRemainChargeTime;	// 마지막 충전 시간 (분).
	int m_iRemainChargeSec;	// 마지막 충전 시간 (분).
	DWORD m_dwCheckTime;

	CEncrypt<int> m_iEnableSendTime;
	CEncrypt<int> m_iEnableRecvTime;
	CEncrypt<int> m_iAbusingTime;
	CEncrypt<int> m_iChargingTime;
	CEncrypt<int> m_iChargingCount;

	bool m_bRequestServer;

public:
	void Initialize();
	void LoadINI();

public:
	void UpdateClover();

	void ApplyCloverInfo( SP2Packet &rkPacket );
	void ApplyEtcItemClover( SP2Packet &rkPacket );

	bool CheckSendRemainTime( const ioHashString &szUserName );
	bool CheckFriendRegTime( const ioHashString &szUserName );
	bool EnableSendClover( const ioHashString &szUserName );
	bool EnableRecvClover( const ioHashString &szUserName );

	int GetRemainChargeMinute();
	int GetRemainChargeSecond();
	void GetSendRemainTime( IN const ioHashString &szUserName, OUT ioHashString &szRemainTime );
	void GetRecvRemainTime( IN const ioHashString &szUserName, OUT ioHashString &szRemainTime );
	void GetFriendRegRemainTime( IN const ioHashString &szUserName, OUT ioHashString &szRemainTime );
	int GetRecvCloverCount();
	const ioHashString &GetRemainString( int iRemainSec );

	const int GetChargingTime() { return m_iChargingTime; }
	const int GetChargingCount() { return m_iChargingCount; }
	const int GetSendTimeGap() { return m_iEnableSendTime; }
	const int GetRecvTimeGap() { return m_iEnableRecvTime; }
	const int GetAbusingTime() { return m_iAbusingTime; }

protected:
	void SendChargeCloverReqeust();

public:
	const int GetCurGiftCloverCount(){ return m_iGiftCloverCount; }
	const int GetMaxGiftCloverCount(){ return GIFT_CLOVER_MAX; }

	void SetCurGiftCloverCount( int iGiftCloverCount ){ m_iGiftCloverCount = iGiftCloverCount; }

	const bool IsMaxCharge();

public:
	ioClover();
	virtual ~ioClover();
};
