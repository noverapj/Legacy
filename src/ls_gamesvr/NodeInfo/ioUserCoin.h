
#pragma once

enum USER_COIN_TYPE 
{
	USER_COIN_NONE = 0,
	// 레이드티켓 시간관리는 유저코인으로.
	USER_COIN_RAID,
	MAX_USER_COIN,
};
struct UserCoinInfo
{
	USER_COIN_TYPE m_eType;
	CTime m_lastUpdateTIme;
	UserCoinInfo()
	{
		m_eType = USER_COIN_NONE;
	}
};

typedef std::vector< UserCoinInfo > UserCoinLIst;
class ioUserCoin : public ioDBDataController
{

private:
	UserCoinLIst m_vCoinList;
	bool m_bNeedResult;
	CTimeSpan m_DBGapTime;

public:
	void Init();
	void Destroy();
	void UpdateCoinData( USER_COIN_TYPE eType );
	void UpdateCoinTime( USER_COIN_TYPE eType, CTime & saveTime );
	bool GetLastCoinTime( CTime & outCoinTime, USER_COIN_TYPE eType );
	UserCoinInfo * FindCoinInfo( USER_COIN_TYPE eType );

	bool GetNeedResult() const { return m_bNeedResult; }
public:
	User* GetUser(){ return m_pUser; }
	void SetUser( User* pUser ){ m_pUser = pUser; }



public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	ioUserCoin();
	virtual ~ioUserCoin();
};
