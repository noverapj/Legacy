
#pragma once

struct stRouletteRewardData;

class ioPirateRoulette : public ioDBDataController
{
public:
	ioPirateRoulette();
	virtual ~ioPirateRoulette();
	
	void Init();
	void Destroy();

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

	void SetFromDB( bool isFromDB ){ m_bIsFromDB = isFromDB;}
	bool IsFromDB(){return m_bIsFromDB;}
	// set db table
	void DBtoData_RouletteBoard( CQueryResultData *query_data );
	void DBtoData_Present( CQueryResultData *query_data );

	enum
	{
		ROULETTE_BOARD_MAX	= 40,
		ROULETTE_PRESENT_MAX  = 10,
		ROULETTE_BONUS_MAX = 5,
		ROULETTE_HP_MAX		= 1000
	};

	enum OakSwordType
	{
		OST_NONE = 0,
		OST_WOOD = 1,
		OST_SILVER = 2,
		OST_GOLD = 3,
	};

private:
	int m_RouletteBoard[ROULETTE_BOARD_MAX];
	int m_present[ROULETTE_PRESENT_MAX];
	int m_iHP;								// HP Max = 1000   ( 클라이언트 표기는 100 소수점 연산 제거. )
	bool m_bChangeRoulette;
	bool m_bSendPresentState;
	bool m_bChangePresent;
	bool m_bIsFromDB;
	
public:
	User* GetUser(){ return m_pUser; }
	void SetUser( User* pUser ){ m_pUser = pUser; }
	
	void InitAll();
//	void InitRouletteBoard();
	void InitRoulettePresent();
//	void ResetRouletteAndGetPresent(bool isGiveBonus);
	void UseSword(int iRouletteBoardPosition, int iSwordType, SP2Packet& rkPacket);
	void GetRouletteBoardData( BYTE *pArray );
	void GetRouletteBoardData( SP2Packet &rkPacket );
	void GetPresentData(BYTE *pArray );
	int	 GetHP(){ return m_iHP;}

	bool GetChangeRoulette(){ return m_bChangeRoulette; }
	void SetChangeRoulette( bool state ){ m_bChangeRoulette = state; }

private:
	void CheckCriticalType(int iSwordType, int& iCritical, int& iDamage);
//	void CheckPresent( const int index, bool isEnd = false );
	void SendPresent( stRouletteRewardData& presentInfo );
	void SendBonus( stRouletteRewardData& bonusInfo );

	bool GetChangePresent(){ return m_bChangePresent; }
	void SetChangePresent( bool state ){ m_bChangePresent = state; }
	void SetSendPresentState( const bool bState ){ m_bSendPresentState = bState; }
	bool GetSendPresentState(){ return m_bSendPresentState; }

public:
	void OnRouletteStart( User* pUser );
//	void OnRouletteInitialize( User* pUser );
	void OnRouletteReset( User* pUser );

	void FillRouletteData( SP2Packet& rkPacket );
};
