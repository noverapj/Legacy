#pragma once

#include "../io3DEngine/ioMapIterator.h"

#define HOLE_PACKET_SEND_TIME            1000
#define RECV_DELAY_CHECK_TIME            10000

class ioPlayStage;

struct NetworkAddr
{
	char m_IP[LENGTH_IP];
	int  m_Port;
	NetworkAddr()
	{
		strncpy( m_IP, "", LENGTH_IP );
		m_Port = -1;
	}
};

struct UserNetworkInfo
{
	DWORD m_dwUserIndex; //유저 인덱스.
	ioHashString m_Name; //유저 닉네임
	DWORD m_dwStyle;     //UDP 스타일(음성채팅/플레이)

	NetworkAddr m_addrPublic;   // 외부 주소
	NetworkAddr m_addrTransfer; // 서버 중계 주소
	NetworkAddr m_addrP2pHole;  // P2P 확인 위한 주소
	NetworkAddr m_addrSend;     // 패킷 전송 주소.

	bool  m_bHoleSendComplete;   // 유저에게 P2P로 보낸 패킷이 도착하면 TCP 패킷을 받고 TRUE가됨
	DWORD m_dwHoleSendTime;      // 홀 패킷 전송 타이머
	bool  m_bHoleRecvComplete;   // 이 유저가 나에게 P2P로 보낸 패킷을 받으면 TCP 패킷을 보내고 TRUE가됨
	int   m_iHoleRecvCompleteCallCount; // HoleRecvComplete == TRUE가 된 카운트
	DWORD m_dwOnlyP2PRecvTime;   // P2P로만 전송된 패킷 시간
	bool  m_bNatUser;	         // 공유기 유저
	DWORD m_dwRecvDelayLimitTime;  // 받은 패킷 딜레이 제한 시간
	
	int  m_iPingTime;
	DWORD m_dwLastRecvTime;

	inline bool HasStyle( DWORD dwStyle ) const
	{
		if( m_dwStyle & dwStyle )
			return true;
		return false;
	}
	inline bool IsNatUser() const { return m_bNatUser; }
	inline bool IsServerRelay() const
	{
		if( strcmp( m_addrSend.m_IP, m_addrTransfer.m_IP ) == 0 )
			return true;
		return false;
	}

	// 모든 UDP메세지에 대한 최근 100개 인덱스
	typedef std::deque<int> RecvIndexQueue;
	RecvIndexQueue m_RecvIndexQueue;

	int   m_iLastControlIndex;			// Control 메세지 한정의 인덱스
};

class ioUserNetworkTable
{
public:
	typedef std::map< ioHashString, UserNetworkInfo* > NetworkTable;
	typedef ioMapIterator< NetworkTable > NetworkInfoIter;

protected:
	NetworkTable m_NetworkTable;
	DWORD m_dwProcessTime;

public:
	UserNetworkInfo* AddNewUser( const DWORD dwUserIndex,
		                         const ioHashString &rkName,
								 const char *szPublicIP,
								 const char *szPrivateIP,
								 int iPort,
								 const char *szTransferIP,
								 int iTransferPort,
								 DWORD dwStyle,
								 int nDebugValue );

	void RemoveUser( const ioHashString &rkName , DWORD dwStyle );
	void RemoveAllUser();
	void RemoveAllUser( DWORD dwStyle );

	void CheckValidateNetTable( ioPlayStage *pStage );

public:
	void ServerToClientPortChange( const ioHashString &rkName, 
		                           const ioHashString &szPublicIP,					
								   const ioHashString &szPrivateIP,						
								   const ioHashString &szTransferIP,
								   int iPort,
								   int iTransferPort );

	void ClientToClientPortChange( const ioHashString &rkName, const char *szRcvIP, int iRcvPort );
	
public:
	void SetLastControlIndex( const ioHashString &rkName, int iIndex );
	int  GetLastControlIndex( const ioHashString &rkName );

public:
	const char *GetUserGlobalIP( const ioHashString &rkName );

public:
	//void  SetLastRecvTime( const ioHashString &rkName, DWORD dwHostTime, DWORD dwGlobalTime );
	void  SetLastRecvTime( const ioHashString &rkName, DWORD dwPingTime );
	int   GetP2pPingTime( const ioHashString &rkName );
	bool  IsServerRelay( const ioHashString &rkName );

public:
	bool CheckDuplicateIndex( UserNetworkInfo *pInfo, int iCurIndex );
	
public:
	bool  IsNetworkTimeOut( const ioHashString &rkName );
	bool  IsAllNetworkTimeOut();
	bool  IsNetworkTableEmpty(){ return m_NetworkTable.empty(); }

public:
	NetworkInfoIter GetNetworkInfoIter();
	UserNetworkInfo* GetNetworkInfo( const ioHashString &rkName );
	UserNetworkInfo* GetNetworkInfo( const char *szIP , const int iPort);
	UserNetworkInfo* GetNetworkInfo( DWORD dwUserIndex );

public:
	void HoleSendProcess();
	void HoleRecvProcess();

public:
	void HoleSendComplete( const ioHashString &rkName, const ioHashString &szIP, int iPort );
	void SetUDPRecvTimeOut( const ioHashString &rkName );
	void ReCheckUDPRecvTimeOut( const ioHashString &rkName );
	void AllNetworkTableServerRelay();

public:
	ioUserNetworkTable();
	~ioUserNetworkTable();
};

