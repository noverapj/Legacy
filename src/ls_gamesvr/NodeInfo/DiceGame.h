#pragma once

class DiceGame : public ioDBDataController
{
private:	
	// DB : account idx / int / int  ...		int/ byte  / reward1...reward9 / time / etc 
	// DB : account idx / pos / trace1 ... trace 5 / board / reward1...reward9 / time / etc 

	bool	m_bIsProgressDice;

	int		m_iPosition;
	int		m_iTrace[DICE_GAME_TRACE_DB];
	BYTE	m_byBoradIndex;
	int		m_iRewardIndex[DICE_GAME_REWARD_DB];

	BYTE	m_UserTrace[DICE_GAME_SLOT_COUNT];		// 이진 

	int		m_iDiceCountUsed;
	//////////////////////////////////////////////////

public:
	DiceGame();
	virtual ~DiceGame();
	
	void Init();
	void Destroy();

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

public:
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	User* GetUser(){ return m_pUser; }
	void SetUser( User* pUser ){ m_pUser = pUser; }	

	//void SetDiceGameData( int iPosition, int iTrace[DICE_GAME_TRACE_DB], BYTE byBoradIndex, int iRewardIndex[DICE_GAME_REWARD_DB]);	  // DB로 부터 주사위 정보 받아와 메모리 저장.
	void SetDiceGameData( int iPosition, int *pArrayTrace, BYTE byBoradIndex, int *pArrayRewardIndex);

	// m_bIsProgressDice
	bool GetProgressStateDice(){ return m_bIsProgressDice; }
	void SetProgressStateDice( bool bFlag ){ m_bIsProgressDice = bFlag; }

	//m_byPosition
	int GetDiceGamePosition(){ return m_iPosition; }
	void SetDiceGamePosition( int iPosition ){ m_iPosition = iPosition; }

	//m_iTrace
	int GetDiceGameTrace( int iStep);
	void AllGetDiceGameTrace(int *pArray);
	void AllSetDiceGameTrace(int *pArray);
	void SetDiceGameTrace( int iStep, int iTrace );
	void SetDetailDiceGameTrace( int iStep, int iTrace );
	void InitDiceGameTrace();

	// m_byBoradIndex;
	BYTE GetBoradIndex(){ return m_byBoradIndex; }
	void SetBoradIndex( BYTE BoradIndex ){ m_byBoradIndex = BoradIndex; }

	// m_iRewardIndex[DICE_GAME_REWARD_DB];
	void AllGetRewardIndex( int *pArray );
	void AllSetRewardIndex( int *pArray );
	void SetRewardIndex( int iGroupNum, int iIndex);	
	int GetRewardIndex( int iGroupNum);
	void InitRewardIndex();

	// m_UserTrace[]
	void SetUserTrace( int iPositoin ) { m_UserTrace[iPositoin] = ON_STEPED;}
	int GetUserTrace( int iPositoin ) { return m_UserTrace[iPositoin];}
	int GetNumberOfTraceCount();
	

	//m_iDiceCountUsed
	void InitDiceCountUsed() { m_iDiceCountUsed = 0;}
	int GetDiceCountUsed() { return m_iDiceCountUsed;}
	void InicreaseDiceCountUsed() { m_iDiceCountUsed++;}

	// 클라이언트 정보 전송 
	void SendDiceGameData( User *pUser );
	void SendDiceGameData_Re( User *pUser);

	void DiceGameDataFill( SP2Packet &rkPacket );

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

