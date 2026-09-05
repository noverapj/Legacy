

#include "stdafx.h"

#include "ioSP2TCPSocket.h"
#include "GUI/DisconnectWnd.h"
#include "GUI/ioFriendManager.h"
#include "GUI/ioChannelManager.h"

extern bool g_bHappenError;
extern int  g_iErrorNumber;
extern char g_szErrorLog[2048];

ioSP2TCPSocket::ioSP2TCPSocket( DWORD dwProcID ) : ioClientTCPSocket()
{
	m_dwProcID = dwProcID;

	ioINILoader_e kLoader( "config/sp2.ini" );
	if( kLoader.LoadInt_e( "security", "ON", 1 ) == 1 )
		SetNS( new ioTCPClientSecurity );
}

ioSP2TCPSocket::~ioSP2TCPSocket()
{
	SAFEDELETE( m_pNS );
}

void ioSP2TCPSocket::InitState()
{
	if( m_pNS )
	{
		ioTCPClientSecurity *pCS = dynamic_cast<ioTCPClientSecurity *>(m_pNS);
		pCS->InitState();
	}
}

void ioSP2TCPSocket::ConnectProcess()
{
	LOG.PrintTimeAndLog( 0, "Server Connect" );

	//로컬 아이피를 정확히 알아내기위해 여기서 UDP 초기화
	g_App.InitUDPSocket();
	g_TCPSocketMgr.ConnectProcess();
}

void ioSP2TCPSocket::DisconnectProcess()
{
	if( P2PNetwork::IsNetworkPlaying() )
	{
		if( !g_TCPSocketMgr.IsChangeServer() )
		{
			bool bNotifyGUI = false;
			if( ioSP2GUIManager::GetSingletonPtr() )
			{
				DisconnectWnd *pDisWnd = dynamic_cast<DisconnectWnd*> ( g_GUIMgr.FindWnd(DISCONNECT_WND) );
				if( pDisWnd )
				{
					pDisWnd->SetType( DisconnectWnd::TYPE_LOCAL_EXIT );
					pDisWnd->ShowWnd();
					bNotifyGUI = true;
				}
			}

			if( !bNotifyGUI )
			{
				MBox( NULL, STR(1), STR(2) );
				LOG.PrintTimeAndLog( 0, "ExitProgram - 21" );

				char szLogMessage[MAX_PATH];
				wsprintf( szLogMessage, "%s DisconnectProcess", g_MyInfo.GetPublicID().c_str() );

				SP2Packet kPacket( LUPK_LOG );
				kPacket << "CheckError";
				kPacket << szLogMessage;
				kPacket << 319;  // 오류번호
				kPacket << false; // write db
				P2PNetwork::SendLog( kPacket );

#if defined( USE_GA )
				if( g_App.GetGAStart() == true )
				{
					char chLabel[32] = {0,};

					if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
						sprintf_e( chLabel, "%d", 21 );
					else
						SafeSprintf( chLabel, sizeof(chLabel), "%1", 21 );

					// GAME_END_ERR_POS
					g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
						, "Game"
						, "Error"
						, chLabel
						, 1
						, "%2FGAME%2FOVER%2FERR"
						, 1 );

					g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
				}				
#endif

				g_App.SetExitProgram();
			}

			LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::DisconnectProcess - Disconnected Server" );
		}	
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::DisconnectProcess - Single Mode Disconnected Server" );
	}	
}

bool ioSP2TCPSocket::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::CheckNS ( m_pNS == NULL )" );
		return true;
	}
	
	ioTCPClientSecurity *pCS = (ioTCPClientSecurity*)m_pNS;

	if( !pCS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::CheckNS Check Sum Fail!! [%d : %d]", rkPacket.GetPacketID(), rkPacket.GetCheckSum() );
		return false;
	}
	
	if( !pCS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::CheckNSType:%d State Not Same Client:%d, Server:%d", 
								 rkPacket.GetPacketID(), rkPacket.GetState(), pCS->GetRcvState() );
		return false;
	}
	
	return true;
}

void ioSP2TCPSocket::ProcessPacket( CPacket &rkPacket )
{
	TCPNetwork::RecvPacket( (SP2Packet&)rkPacket );
}
//////////////////////////////////////////////////////////////////////////
template<> ioTCPSocketManager* Singleton< ioTCPSocketManager >::ms_Singleton = 0;
ioTCPSocketManager::ioTCPSocketManager()
{
	m_pMainSocket   = new ioSP2TCPSocket( WM_GAMESOCKET1 );
	m_pChangeSocket = new ioSP2TCPSocket( WM_GAMESOCKET2 );

	m_dwState       = CHANGE_NONE;
	m_hWnd          = 0L;
	m_iServerPort   = 0;
	m_iMovingValue	= 0;
	m_iServerIndex  = 0;
	m_iRoomIndex    = 0;

	m_dwUDPHoleTime = 0;
	m_dwChangeStartTime = 0;
	m_bTestZone = false;
}

ioTCPSocketManager::~ioTCPSocketManager()
{
	SAFEDELETE( m_pMainSocket );
	SAFEDELETE( m_pChangeSocket );
}

ioTCPSocketManager& ioTCPSocketManager::GetSingleton()
{
	return Singleton< ioTCPSocketManager >::GetSingleton();
}

bool ioTCPSocketManager::Connect( HWND hWnd, const char *szIP, int iPort )
{
	m_hWnd = hWnd;
	m_pMainSocket->InitState();

	m_bTestZone = false; 
	if( strcmp( szIP, "172.20.20.75" ) == 0 || strcmp( szIP, "172.20.20.76" ) == 0 )
		m_bTestZone = true; 

	return m_pMainSocket->InitClientTCP( hWnd, m_pMainSocket->GetProcID(), szIP, iPort );
}

bool ioTCPSocketManager::MoveConnect( HWND hWnd, const char *szIP, int iPort )
{
	m_hWnd = hWnd;
	m_pChangeSocket->InitState();

	m_bTestZone = false; 
	if( strcmp( szIP, "172.20.20.75" ) == 0 || strcmp( szIP, "172.20.20.76" ) == 0 )
		m_bTestZone = true; 

	SetServerNumber( szIP );
	SetChangeStateText("Menyambungkan ke Game Server %s.");
	//SetChangeStateText("%s번게임서버로 네트워크 연결 중.");

	return m_pChangeSocket->InitClientTCP( hWnd, m_pChangeSocket->GetProcID(), szIP, iPort );
}

void ioTCPSocketManager::CloseClientTCP()
{
	m_pMainSocket->CloseClientTCP();
	m_pChangeSocket->CloseClientTCP();
}

bool ioTCPSocketManager::ChangeServerProcess()
{
	if( m_dwState == CHANGE_NONE )
		return false;

	// 1분 30초간 서버 이동이 되지 않으면 로그 남기고 클라이언트 종료.
	if( m_dwChangeStartTime != 0 )
	{
		if( REALGETTIME() - m_dwChangeStartTime > MAX_WAITING_CHANGE )
		{
			char szServerChangeLOG[MAX_PATH] = "";
			sprintf( szServerChangeLOG, "%s가 [%s:%d]로 1분30초동안 서버 이동이 완료되지 않았음(%d)", //Except Extracting Hangeul
					 g_MyInfo.GetPublicID().c_str(), m_szServerIP.c_str(), m_iServerPort, (int)m_dwState );
			
			SP2Packet kPacket( LUPK_LOG );
			kPacket << "CheckError";  // 로그 파일 타입
			kPacket << szServerChangeLOG;
			kPacket << 3183;  // 오류번호:3183
			kPacket << true; // write db
			P2PNetwork::SendLog( kPacket );
			m_dwChangeStartTime = 0;

			g_bHappenError = true;
			g_iErrorNumber = 327; // 오류번호
			StringCbPrintf( g_szErrorLog, sizeof( g_szErrorLog ), "%d초동안 게임서버와 \r\n\r\n정상적으로 네트워크 연결을\r\n\r\n하지 못하였습니다.\r\n\r\n네트워크 회선상에 문제는 없는지\r\n\r\n확인해 주시기 바랍니다.\r\n\r\n(오류번호:327)"
				            , MAX_WAITING_CHANGE/1000 );
			
			//StringCbPrintf( g_szErrorLog, sizeof( g_szErrorLog ), "Tidak bisa connect ke game server %s selama %d detik.\r\n\r\nSilahkan periksa network.\r\n\r\n(Error no : 327)"
			//	            , m_szChangeServerNumber.c_str(), MAX_WAITING_CHANGE/1000 );

#if defined( USE_GA )
			if( g_App.GetGAStart() == true )
			{
				char chLabel[32] = {0,};

				if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
					sprintf_e( chLabel, "%d", 22 );
				else
					SafeSprintf( chLabel, sizeof(chLabel), "%1", 22 );

				// GAME_END_ERR_POS
				g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
					, "Game"
					, "Error"
					, chLabel
					, 1
					, "%2FGAME%2FOVER%2FERR"
					, 1 );

				g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
			}			
#endif

			LOG.PrintTimeAndLog( 0, "ExitProgram - 22" );
			g_App.SetExitProgram();
		}
	}

	switch( m_dwState )
	{
	case CHANGE_RESERVE:
		{
			g_GlobalTimer.Process();
		}
		return true;
	case CHANGE_SERVER:
		{
			bool bRet = MoveConnect( m_hWnd, m_szServerIP.c_str(), m_iServerPort );
			g_App.SetRelayServerState(false); //kyg 릴레이서버 사용 X
			if( !bRet )
			{
				LOG.PrintTimeAndLog( 0, "ChangeServer Fail : %d - %s -%d", m_hWnd, m_szServerIP.c_str(), m_iServerPort );
				return true;
			}
			m_dwState = CHANGE_CONNECT;
		}
		return true;
	case CHANGE_CONNECT:
		return true;
	case CHANGE_UDPCHECK:
		if( REALGETTIME() - m_dwUDPHoleTime > 1000 )
		{
			m_dwUDPHoleTime = REALGETTIME();
			SP2Packet kPacket( CUPK_RESERVE_ROOM_JOIN );
			kPacket << g_MyInfo.GetPublicID() << m_iRoomIndex;
			P2PNetwork::SendToServer( kPacket );

			//SetChangeStateText("%s번게임서버로 UDP 데이터 전송.");
			SetChangeStateText("Mengirim data UDP ke game server %s.");
		}
		break;
	}

	g_GlobalTimer.Process();
	return true;
}

void ioTCPSocketManager::ReserveServerMoving()
{
	m_dwState           = CHANGE_RESERVE;
	m_dwChangeStartTime = REALGETTIME();
	//m_szChangeState     = "게임서버 이동 예약.";
	m_szChangeState     = "Reservasi pindah Game Server.";
}

void ioTCPSocketManager::SetServerChange( const ioHashString &szServerIP, int iServerPort, int iMovingValue, int iServerIndex, int iRoomIndex )
{
	m_dwChangeStartTime = REALGETTIME();

	m_szServerIP    = szServerIP;
	m_iServerPort   = iServerPort;
	m_iMovingValue	= iMovingValue;
	m_iServerIndex  = iServerIndex;
	m_iRoomIndex    = iRoomIndex;
	m_dwState       = CHANGE_SERVER;

	//m_szChangeState = "게임서버 이동 시작.";
	m_szChangeState = "Mulai pindah Game Server";
}

void ioTCPSocketManager::ConnectProcess()
{
	if( m_dwState == CHANGE_CONNECT )
	{
		char szLocalIP[LENGTH_IP];
		g_App.GetMyLocalIP( szLocalIP );

		// 연결 성공 알림
		SP2Packet kPacket( CTPK_MOVING_SERVER );
		kPacket << g_MyInfo.GetUserIndex() << m_iMovingValue << m_iServerIndex;
		kPacket << g_MyInfo.GetPrivateID() << g_MyInfo.GetPublicID() << szLocalIP;
		TCPNetwork::SendToServer( kPacket );

		//SetChangeStateText("%s번게임서버와 네트워크 연결 성공. TCP 데이터 전송.");
		SetChangeStateText("Sukses menyambung network ke Game server %s. Mengirim data TCP.");
	}
	m_iMovingValue	= 0;
	m_iServerIndex  = 0;
}

void ioTCPSocketManager::CompleteChangeServer()
{
	// 서버 이동이 완료되었으니 기존 서버의 소켓을 닫는다.
	ioSP2TCPSocket *pTempSocket = m_pMainSocket;
	m_pMainSocket   = m_pChangeSocket;
	m_pChangeSocket = pTempSocket;
	m_pChangeSocket->CloseClientTCP();
	
	// UDP 
	g_App.InitGlobalTimer();
	m_dwState       = CHANGE_UDPCHECK;
	m_dwUDPHoleTime = REALGETTIME();

	//SetChangeStateText("%s번게임서버와 TCP 데이터 처리 완료.");
	SetChangeStateText("Sukses mengirim TCP data ke game server %s.");

#ifdef XIGNCODE
	g_ioXignCode.OnDisconnect();
	g_ioXignCode.OnConnect();
#endif 
}

void ioTCPSocketManager::ChangeServerStateEnd()
{ 
	m_dwState = CHANGE_NONE; 
	m_dwChangeStartTime = 0;

	g_App.ClearLoadingChangeHelp();

	//SetChangeStateText("%s번게임서버와 UDP 데이터 처리 완료.");
	SetChangeStateText("Sukses mengirim UDP data ke game server %s.");
	
}

const char *ioTCPSocketManager::GetConnectedIP()
{ 
	return m_pMainSocket->GetConnectedIP(); 
}

int ioTCPSocketManager::GetConnectedPort() 
{ 
	return m_pMainSocket->GetConnectedPort(); 
}

bool ioTCPSocketManager::IsConnected() 
{ 
	return m_pMainSocket->IsConnected(); 
}

SOCKET ioTCPSocketManager::GetSocket() 
{ 
	return m_pMainSocket->GetSocket(); 
}

void ioTCPSocketManager::SetNagleAlgorithm( bool bOn )
{
	if( m_pMainSocket )
	{
		m_pMainSocket->SetNagleAlgorithm( bOn );
	}
}

bool ioTCPSocketManager::EvaluateSocket( DWORD dwProcID, WPARAM wParam, LPARAM lParam )
{
	if( m_pMainSocket->GetProcID() == dwProcID )
		return m_pMainSocket->EvaluateSocket( wParam, lParam );
	else if( m_pChangeSocket->GetProcID() == dwProcID )
		return m_pChangeSocket->EvaluateSocket( wParam, lParam );	

	LOG.PrintTimeAndLog( 0, "Unknown Socket ProcID : 0x%x", dwProcID );
	return false;
}

bool ioTCPSocketManager::SendLoop( SP2Packet &rkPacket )
{ 
	if( IsChangeServer() && m_pChangeSocket->IsConnected() )
		return m_pChangeSocket->SendLoop( rkPacket ); 
	return m_pMainSocket->SendLoop( rkPacket ); 
}


void ioTCPSocketManager::SetServerNumber( const char *szServerIP )
{
	char szServerNumber[MAX_PATH]="";
	int iServerNumberCnt = 0;
	int iDotCnt = 0;
	int iSize = strlen( szServerIP );
	for (int i = 0; i < iSize ; i++)
	{
		if( szServerIP[i] == '.' )	
		{
			iDotCnt++;
			continue;
		}

		if( iDotCnt == 3 && COMPARE( iServerNumberCnt, 0, MAX_PATH ) )
		{
			szServerNumber[iServerNumberCnt] = szServerIP[i];
			iServerNumberCnt++;
		}
	}

	m_szChangeServerNumber = szServerNumber;
}

void ioTCPSocketManager::SetChangeStateText( const char *szText )
{
	char szTemp[MAX_PATH]="";
	StringCbPrintf( szTemp, sizeof( szTemp ), szText, m_szChangeServerNumber.c_str() );
	m_szChangeState = szTemp;
}


bool ioTCPSocketManager::GetChangeStateText( OUT ioHashString &rszStateText )
{
	if( m_dwChangeStartTime == 0  )
		return false;

	if( m_szChangeState.IsEmpty() )
		return false;

	DWORD dwCurrentMoveTime = REALGETTIME() - m_dwChangeStartTime;
	int iPassedSec = dwCurrentMoveTime/1000;

	char szText[MAX_PATH]="";
	StringCbPrintf( szText, sizeof( szText ), "%s[%d/%d]", m_szChangeState.c_str(), iPassedSec, MAX_WAITING_CHANGE/1000 );
	rszStateText = szText;

	return true;
}


//////////////////////////////////////////////////////////////////////////
ioTCPClientSecurity::ioTCPClientSecurity()
{
	InitState();
}

ioTCPClientSecurity::~ioTCPClientSecurity()
{

}

void ioTCPClientSecurity::InitState()
{
	m_SndState.InitState();
	m_RcvState.InitState();
}

// Packet CheckSum
void ioTCPClientSecurity::EncryptMsg( CPacket &rkPacket )
{
	// Check Sum
	DWORD dwResult = MakeDigest(  (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize()  );
	rkPacket.SetCheckSum( dwResult );
	
	// Encryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Encrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );		
}

void ioTCPClientSecurity::DecryptMsg( CPacket &rkPacket )
{
	// Decryption
	BYTE *pSrc;
	pSrc = (BYTE*)rkPacket.GetBuffer() + PK_CKSUM_ADDR;
	Decrypt( pSrc, rkPacket.GetBufferSize() - PK_CKSUM_ADDR );
}

bool ioTCPClientSecurity::IsCheckSum( CPacket &rkPacket )
{
	//Decrypt
	DecryptMsg( rkPacket );

	DWORD	dwTemp = rkPacket.GetCheckSum();   //임시 저장.
	rkPacket.SetCheckSum( 0 );
	
	DWORD dwResult = MakeDigest( (BYTE*)rkPacket.GetBuffer(), rkPacket.GetBufferSize() );

	rkPacket.SetCheckSum( dwTemp );

	// Check Sum
	return ( dwResult == rkPacket.GetCheckSum() );
}

// Packet Replay
int ioTCPClientSecurity::GetSndState()
{
	return m_SndState.GetState();
}

void ioTCPClientSecurity::UpdateSndState()
{
	m_SndState.UpdateState();
}

int ioTCPClientSecurity::GetRcvState()
{
	return m_RcvState.GetState();
}

void ioTCPClientSecurity::UpdateRcvState()
{
	m_RcvState.UpdateState();
}

bool ioTCPClientSecurity::CheckState( CPacket &rkPacket )
{
	if( GetRcvState() == MAGIC_TOKEN_FSM )
	{
		m_RcvState.SetState( rkPacket.GetState() );
		m_SndState.SetState( rkPacket.GetState() );		
		UpdateSndState();
	}
	else
	{
		if( GetRcvState() != rkPacket.GetState() )
			return false;
	}
	
	UpdateRcvState();	
	return true;
}

void ioTCPClientSecurity::PrepareMsg( CPacket &rkPacket )
{
	rkPacket.SetState( GetSndState() );

	EncryptMsg( rkPacket );

	UpdateSndState();
}

void ioTCPClientSecurity::CompletionMsg( CPacket &rkPacket )
{	
}

