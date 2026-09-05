
#ifndef _ioProcessChecker_h_
#define _ioProcessChecker_h_

class FunctionTimeChecker
{
protected:
	LARGE_INTEGER m_freq;
	UINT64		  m_start;
	UINT64		  m_end;

	bool		  m_bFrequency;
	LONG		  m_iFileLine;	
	LPSTR         m_pFileName;
	double        m_fCheckMicSec;
	DWORD         m_dwPacketID;

public:
	FunctionTimeChecker( LPSTR pFileName, LONG iFileLine, double fCheckMicSec, DWORD dwPacketID );
	virtual ~FunctionTimeChecker();
};
#define FUNCTION_TIME_CHECKER( f, d )        FunctionTimeChecker ftc( __FILE__, __LINE__, f, d )
//////////////////////////////////////////////////////////////////////////
class ioProcessChecker
{
private:
	static ioProcessChecker *sg_Instance;

protected:
	DWORD m_dwCurTime;
	DWORD m_dwLogTime;										// 몇초에 1번 로그를 남길것인가?

	// 쓰레드 루프 횟수
	DWORD m_dwMainLoop;										// LogTime동안 메인 루프 호출 횟수
	DWORD m_dwClientAcceptLoop;                             // LogTime동안 ClientAccept 루프 호출 횟수
	DWORD m_dwServerAcceptLoop;                             // LogTime동안 ServerAccept 루프 호출 횟수

	// 패킷 관련
	__int64 m_iUserSend;                                     // 서버가 유저에게 전송 요청한 패킷의 사이즈
	__int64 m_iUserSendComplete;                             // 서버가 유저에게 전송 처리한 패킷의 사이즈
	__int64 m_iUserRecv;                                     // 유저에게 받은 패킷 사이즈

	__int64 m_iServerSend;                                   // 서버가 서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iServerSendComplete;                           // 서버가 서버에게 전송 처리한 패킷의 사이즈
	__int64 m_iServerRecv;                                   // 서버에게 받은 패킷 사이즈

	__int64 m_iLogDBServerSend;                              // 서버가 로그DB서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iLogDBServerSendComplete;                      // 서버가 로그DB서버에게 전송 처리한 패킷의 사이즈
	__int64 m_iLogDBServerRecv;                              // 로그DB서버에게 받은 패킷 사이즈

	__int64 m_iDBServerSend;                                 // 서버가 DB서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iDBServerSendComplete;                         // 서버가 DB서버에게 전송 처리한 패킷의 사이즈
	__int64 m_iDBServerRecv;                                 // DB서버에게 받은 패킷 사이즈

	int     m_iMainProcessMaxPacket;                         // 메인 프로세스에서 매 루프마다 처리하는 패킷량 중 가장 많은 패킷

	// 쓰레드 루프 시간
protected:
	struct PerformanceTime
	{
		LARGE_INTEGER m_freq;
		UINT64		  m_start;
		UINT64		  m_end;
		bool		  m_bFrequency;		
	};
	PerformanceTime   m_MainThreadTime;
	double            m_fMainTreadMaxTime;

	PerformanceTime   m_ServerAThreadTime;
	double            m_fServerATreadMaxTime;

	PerformanceTime   m_ClientAThreadTime;
	double            m_fClientATreadMaxTime;

public:
	static ioProcessChecker &GetInstance();
	static void ReleaseInstance();

public:
	void LoadINI();
	void Initialize();
	void Process();
	void WriteLOG();

public:
	void MainThreadCheckTimeStart();
	void MainThreadCheckTimeEnd();
	void ServerAThreadCheckTimeStart();
	void ServerAThreadCheckTimeEnd();
	void ClientAThreadCheckTimeStart();
	void ClientAThreadCheckTimeEnd();

public:
	void CallMainThread(){ m_dwMainLoop++; }
	void CallClientAccept(){ m_dwClientAcceptLoop++; }
	void CallServerAccept(){ m_dwServerAcceptLoop++; }

public:
	void ProcessIOCP( int iConnectType, DWORD dwFlag, DWORD dwByteTransfer );
	void UserSendMessage( int iSize );
	void UserSendComplete( int iSize );
	void UserRecvMessage( int iSize );
	void ServerSendMessage( int iSize );
	void ServerSendComplete( int iSize );
	void ServerRecvMessage( int iSize );
	void LogDBServerSendMessage( int iSize );
	void LogDBServerSendComplete( int iSize );
	void LogDBServerRecvMessage( int iSize );
	void DBServerSendMessage( int iSize );
	void DBServerSendComplete( int iSize );
	void DBServerRecvMessage( int iSize );
	void MainProcessMaxPacket( int iPacketCnt );

private:
	ioProcessChecker();
	virtual ~ioProcessChecker();
};
#define g_ProcessChecker ioProcessChecker::GetInstance()
#endif