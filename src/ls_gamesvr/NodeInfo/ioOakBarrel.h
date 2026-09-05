#pragma once

class ioOakBarrel : public ioDBDataController
{
private:
	BYTE	m_byOakBarrelStep;					// 오크통 진행 단계
	BYTE	m_byOakBarrelHole[OAK_BARREL_HOLE];	// 12개의 오크통 구멍 상태
	DWORD	m_dwOakBarrelTime;					// 칼 개수가 3개 미만 일때 수량 변동 된 시각값
	BYTE	m_byHoleIndex;						// 클라로 부터 받은 오크통 구멍 인덱스
	int		m_iLimitSword;						// 현재 오크통 칼 사용량
	
	bool	m_bChangeOakBarrelData;
	bool	m_bOakSuccess;						// 성공 여부

	bool	m_bTimeCheckStart;					// 오크통 타임 체크 시작

public:
	ioOakBarrel();
	virtual ~ioOakBarrel();

	void Init();
	void Destroy();

public:
	virtual void Initialize( User *pUser );
	virtual bool DBtoNewIndex( DWORD dwIndex );
	virtual void DBtoData( CQueryResultData *query_data );
	virtual void SaveData();
	virtual void FillMoveData( SP2Packet &rkPacket );
	virtual void ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode = false  );

	void DBtoTimeData( CQueryResultData *query_data );
public:
	User* GetUser(){ return m_pUser; }
	void SetUser( User* pUser ){ m_pUser = pUser; }
	
	bool GetChangeOakBarrelData(){ return m_bChangeOakBarrelData; }
	void SetChangeOakBarrelData( bool bChange ){ m_bChangeOakBarrelData = bChange; }

	BYTE GetOakBarrelStep(){ return m_byOakBarrelStep; }
	void SetOakBarrelStep( BYTE byStep ){ m_byOakBarrelStep = byStep; }

	void GetOakBarrelHole( BYTE *pArray );
	void SetOakBarrelHole( int iHoleIndex );
	void InitOakBarrelHole();

	void SetHoleIndex( BYTE byIndex ){ m_byHoleIndex = byIndex; }
	BYTE GetHoleIndex(){ return m_byHoleIndex; }

	bool GetOakBarrelTimeCheckStart(){ return m_bTimeCheckStart; }
	void SetOakBarrelTimeCheckStart( bool bStart ){ m_bTimeCheckStart = bStart; }

	DWORD GetOakBarrelTimeStamp(){ return m_dwOakBarrelTime; }
	void SetOakBarrelTimeStamp( DWORD dwTime ){ m_dwOakBarrelTime = dwTime; }

	int GetLimitSword() const { return m_iLimitSword; }
	void SetLimitSword( int val ) { m_iLimitSword = val; }


	// S->C 로그인 시 오크통 정보 전송
	void SendOakBarrelData( User *pUser );
	
	// 칼 사용에 대한 결과
	void UseSwordResult( BYTE byIndex );

	// S->C 오크통 결과 전송
	void SendOakBarrelResult( User *pUser, bool bSuccess, BYTE byStep );

	// 보상 요청
	void RewardReq( User *pUser );

	// S->C 요청 된 보상 전송
	void SendOakBarrelReward( User *pUser, BYTE byStep );

	// DB로 부터 오크통 정보를 받아와 메모리 저장.
	void SetOakBarrelData( BYTE byStep, BYTE byHole[OAK_BARREL_HOLE], DWORD dwTime, int iLimitSword, bool bInit );
	// DB로 부터 오크통 초기화 시간값을 받아와 메모리 저장
	void SetOakBarrelTimeData( DWORD dwTime );
};
