#ifndef _ioCriticalError_h_
#define _ioCriticalError_h_

class ioCriticalError
{
private:
	static ioCriticalError *sg_Instance;

	/************************************************************************
		네트웍 검사 - 위험한 상황
		- 연결 비정상 종료
		101. GS < - > GS 연결 종료  : 연결 종료된 서버 IP : PORT : Last Error
		102. GS < - > MS 연결 종료  : Last Error
		103. DBA < - > GS 연결 종료 : 연결 종료된 서버 IP : PORT : Last Error
		104. BS < - > GS 연결 종료  : Last Error
		105. MS - > GS 서버 종료    : 서버 종료 명령 로그 

		- 연결 정상 종료
		106. GS < - > GS 연결 종료  : 연결 종료된 서버 IP : PORT
		107. GS < - > MS 연결 종료  : 
		108. DBA < - > GS 연결 종료 : 연결 종료된 서버 IP : PORT
		109. BS < - > GS 연결 종료  : 

		메모리 검사 - 잠재적 위험
		201. Packet Pool 증가 : Pool 카운트 : 패킷 ID
		202. User Pool 증가   : Pool 카운트

		크래쉬 검사 - 위험한 상황
		301. 크래쉬 : 로그 기록

		게임 시스템 검사 - 잠재적 위험 ( INI에서 제한 세팅 )
		501. 용병 개수 초과 : 유저 닉네임 : 용병 개수 
			> 300개 제한(패킷 32Kbyte 제한 위험)
		502. 치장 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 120테이블 제한(패킷 32Kbyte 제한 위험)
		503. 특별 아이템 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 120테이블 제한(패킷 32Kbyte 제한 위험)
		504. 진행중인 퀘스트 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수 
			> 90테이블 제한(패킷 32Kbyte 제한 위험)
		505. 완료된 퀘스트 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 180테이블 제한(패킷 32Kbyte 제한 위험)
		506. 성장 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 150테이블 제한(패킷 32Kbyte 제한 위험)
		507. 낚시 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 280테이블 제한(패킷 32Kbyte 제한 위험)
		508. 장비 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 80테이블 제한(패킷 32Kbyte 제한 위험)
		509. 메달 DB 테이블 초과 : 유저 닉네임 : DB 테이블 개수
			> 140테이블 제한(패킷 32Kbyte 제한 위험)
		510. 페소 증가 제한 : 유저 닉네임 : 증가 페소
			> 1,000만 페소 제한(버그로 획득할 가능성)
		511. 페소 보유 제한 : 유저 닉네임 : 보유 페소
			> 5,000만 페소 제한(버그로 획득할 가능성)
		PS. 최대 패킷 사이즈 : 32768byte

		퀘스트 어뷰즈 검사
		601. 퀘스트 어뷰즈 발생
	************************************************************************/
protected:
	PROCESS_INFORMATION m_ProcessInfo;

protected:
	int m_iSoldierLimitCount;
	int m_iDecoTableLimitCount;
	int m_iEtcItemTableLimitCount;
	int m_iProgressQuestTableLimitCount;
	int m_iCompleteQuestTableLimitCount;
	int m_iGrowthTableLimitCount;
	int m_iFishInvenTableLimitCount;
	int m_iExtraItemTableLimitCount;
	int m_iMedalTableLimitCount;
	int m_iPesoAcquiredLimitCount;
	int m_iPesoPossessionLimitCount;
	int m_iCostumeLimitCount;
	int m_iAccessoryLimitCount;

protected:
	void ExcuteProcess( char* szFileName , char* szCmdLine );

public:
	static ioCriticalError &GetInstance();
	static void ReleaseInstance();

public:
	void Initialize();

public:
	void CheckGameServerExceptionDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort, DWORD dwLastError );
	void CheckMainServerExceptionDisconnect( DWORD dwLastError );
	void CheckDBAgentServerExceptionDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort, DWORD dwLastError );
	void CheckBillingServerExceptionDisconnect( DWORD dwLastError );
	void CheckGameServerDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort );
	void CheckMainServerDisconnect();
	void CheckDBAgentServerDisconnect( const ioHashString &rkDisconnectIP, DWORD dwDisconnectPort );
	void CheckBillingServerDisconnect();

	void CheckServerDownMsg( int iServerCount, int iConnectCount );
	void CheckPacketPool( int iPacketQueueCount, DWORD dwPacketID );
	void CheckUserPool( int iUserPoolCount );
	void CheckCrashLog( const ioHashString &rkCrashLog );

public:
	void CheckSoldierCount( const ioHashString &rkPublicID, int iSoldierCount );
	void CheckDecoTableCount( const ioHashString &rkPublicID, int iDecoTableCount );
	void CheckEtcItemTableCount( const ioHashString &rkPublicID, int iEtcItemTableCount );
	void CheckProgressQuestTableCount( const ioHashString &rkPublicID, int iProgressQuestTableCount );
	void CheckCompleteQuestTableCount( const ioHashString &rkPublicID, int iCompleteQuestTableCount );
	void CheckGrowthTableCount( const ioHashString &rkPublicID, int iGrowthTableCount );
	void CheckFishInvenTableCount( const ioHashString &rkPublicID, int iFishInvenTableCount );
	void CheckExtraItemTableCount( const ioHashString &rkPublicID, int iExtraItemTableCount );
	void CheckMedalTableCount( const ioHashString &rkPublicID, int iMedalTableCount );
	void CheckPesoAcquiredCount( const ioHashString &rkPublicID, int iPesoAcquiredCount );
	void CheckPesoPossessionCount( const ioHashString &rkPublicID, __int64 iPesoPossessionCount );
	void CheckQuestAbuse( const ioHashString &rkPublicID, DWORD dwMainIndex, DWORD dwSubIndex, DWORD dwGapTime, DWORD dwGapValue );

public:
	inline int GetMaxCostumeCount() { return m_iCostumeLimitCount; }
	inline int GetMaxAccessoryCount() { return m_iAccessoryLimitCount; }

private:
	ioCriticalError();
	virtual ~ioCriticalError();
};
#define g_CriticalError ioCriticalError::GetInstance()
#endif