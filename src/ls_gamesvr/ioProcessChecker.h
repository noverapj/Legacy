
#ifndef _ioProcessChecker_h_
#define _ioProcessChecker_h_

#include "ioPacketStatistics.h"
#include <unordered_map>


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
	DWORD m_dwLogDBLoop;									// LogTime동안 LogDB 루프 호출 횟수
	DWORD m_dwUDPLoop;                                      // LogTime동안 UDP 루프 호출 횟수
	DWORD m_dwClientAcceptLoop;                             // LogTime동안 ClientAccept 루프 호출 횟수
	DWORD m_dwServerAcceptLoop;                             // LogTime동안 ServerAccept 루프 호출 횟수
	DWORD m_dwMonitoringAcceptLoop;                         // LogTime동안 MonitoringAccept 루프 호출 횟수

	// 패킷 관련
	__int64 m_iUserSend;                                     // 서버가 유저에게 전송 요청한 패킷의 사이즈
	__int64 m_iUserSendComplete;                             // 서버가 유저에게 전송 처리한 패킷의 사이즈
	__int64 m_iUserRecv;                                     // 유저에게 받은 패킷 사이즈
	__int64 m_iServerSend;                                   // 서버가 서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iServerSendComplete;                           // 서버가 서버에게 전송 처리한 패킷의 사이즈
	__int64 m_iServerRecv;                                   // 서버에게 받은 패킷 사이즈
	__int64 m_iMServerSend;                                  // 서버가 메인서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iMServerSendComplete;                          // 서버가 메인서버에게 전송 처리한 패킷의 사이즈
	__int64 m_iMServerRecv;                                  // 메인서버에게 받은 패킷 사이즈
	__int64 m_iDBServerSend;                                 // 서버가 DB서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iDBServerSendComplete;                         // 서버가 DB서버에게 전송 처리한 패킷의 사이즈
	__int64 m_iDBServerRecv;                                 // DB서버에게 받은 패킷 사이즈
	__int64 m_iLogDBSend;									 // 서버가 LogDB서버에게 전송 요청한 패킷의 사이즈
	__int64 m_iLogDBRecv;									 // LogDB서버에게 받은 패킷 사이즈
	__int64 m_iUDPSend;										 // 서버가 UDP 전송 요청한 패킷의 사이즈
	__int64 m_iUDPRecv;										 // UDP 받은 패킷 사이즈
	__int64 m_iBillingRelayServerSend;                       // 서버가 BillingRelayServer에게 전송 요청한 패킷의 사이즈
	__int64 m_iBillingRelayServerSendComplete;               // 서버가 BillingRelayServer에게 전송 처리한 패킷의 사이즈
	__int64 m_iBillingRelayServerRecv;                       // BillingRelayServer에게 받은 패킷 사이즈
	__int64 m_iMonitoringSend;                               // 서버가 Monitoring에게 전송 요청한 패킷의 사이즈
	__int64 m_iMonitoringSendComplete;                       // 서버가 Monitoring에게 전송 처리한 패킷의 사이즈
	__int64 m_iMonitoringRecv;                               // Monitoring에게 받은 패킷 사이즈
	int     m_iMainProcessMaxPacket;                         // 메인 프로세스에서 매 루프마다 처리하는 패킷량 중 가장 많은 패킷

	ioPacketStatistics m_ServerStatistics;

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

	PerformanceTime   m_BroadcastThreadTime;
	double            m_fBroadcastTreadMaxTime;

	PerformanceTime   m_WorkThreadTime;
	double            m_fWorkTreadMaxTime;

	PerformanceTime   m_ServerAThreadTime;
	double            m_fServerATreadMaxTime;

	PerformanceTime   m_ClientAThreadTime;
	double            m_fClientATreadMaxTime;

	PerformanceTime   m_MonitoringAThreadTime;
	double            m_fMonitoringATreadMaxTime;

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
	void BroadcastThreadCheckTimeStart();
	void BroadcastThreadCheckTimeEnd();
	void WorkThreadCheckTimeStart();
	void WorkThreadCheckTimeEnd();
	void ServerAThreadCheckTimeStart();
	void ServerAThreadCheckTimeEnd();
	void ClientAThreadCheckTimeStart();
	void ClientAThreadCheckTimeEnd();
	void MonitoringAThreadCheckTimeStart();
	void MonitoringAThreadCheckTimeEnd();

public:
	void CallMainThread(){ m_dwMainLoop++; }
	void CallLogDBThread(){ m_dwLogDBLoop++; }
	void CallUDPThread(){ m_dwUDPLoop++; }
	void CallClientAccept(){ m_dwClientAcceptLoop++; }
	void CallServerAccept(){ m_dwServerAcceptLoop++; }
	void CallMonitoringAccept(){ m_dwMonitoringAcceptLoop++; }

public:
	void ProcessIOCP( int iConnectType, DWORD dwFlag, DWORD dwByteTransfer );
	void UserSendMessage( DWORD dwID, int iSize );
	void UserSendComplete( int iSize );
	void UserRecvMessage( int iSize );
	void ServerPacketProcess( DWORD dwID );
	void ServerSendMessage( DWORD dwID, int iSize );
	void ServerSendComplete( int iSize );
	void ServerRecvMessage( int iSize );
	void MainServerSendMessage( DWORD dwID, int iSize );
	void MainServerSendComplete( int iSize );
	void MainServerRecvMessage( int iSize );
	void DBServerSendMessage( DWORD dwID, int iSize );
	void DBServerSendComplete( int iSize );
	void DBServerRecvMessage( int iSize );
	void LogDBSendMessage( DWORD dwID, int iSize );
	void LogDBRecvMessage( int iSize );
	void UDPSendMessage( DWORD dwID, int iSize );
	void UDPRecvMessage( int iSize );
	void BillingRelayServerSendMessage( DWORD dwID, int iSize );
	void BillingRelayServerSendComplete( int iSize );
	void BillingRelayServerRecvMessage( int iSize );
	void MonitoringSendMessage( DWORD dwID, int iSize );
	void MonitoringSendComplete( int iSize );
	void MonitoringRecvMessage( int iSize );
	void MainProcessMaxPacket( int iPacketCnt );

private:
	ioProcessChecker();
	virtual ~ioProcessChecker();
};
#define g_ProcessChecker ioProcessChecker::GetInstance()
#endif