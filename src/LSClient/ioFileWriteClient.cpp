#include "stdafx.h"

#include "ioFileWriteClient.h"

#include "SP2Packet.h"
#include "ioSP2TCPSocket.h"
#include "Crc32Static.h"
#include "TCPProtocol.h"

#include "GUI/GlobalWnd.h"

ioFileSocket::ioFileSocket( DWORD dwProcID ) : ioClientTCPSocket()
{
	m_dwProcID = dwProcID;
}

ioFileSocket::~ioFileSocket()
{
	SAFEDELETE( m_pNS );
}

void ioFileSocket::ConnectProcess()
{
	LOG.PrintTimeAndLog( 0, "Server Connect" );

	SAFEDELETE( m_pNS );

	ioINILoader_e kLoader( "config/sp2.ini" );
	if( kLoader.LoadInt_e( "security", "ON", 1 ) == 1 )
	{
		SetNS( new ioTCPClientSecurity );
	}

	g_FileWriteClient.ConnectProcess();
}

void ioFileSocket::DisconnectProcess()
{
	LOG.PrintTimeAndLog( 0, "ioSP2TCPSocket::DisconnectProcess - Disconnected Server" );
	SAFEDELETE( m_pNS );	
	g_FileWriteClient.DisconnectProcess();
}

bool ioFileSocket::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioFileSocket::CheckNS ( m_pNS == NULL )" );
		return true;
	}

	ioTCPClientSecurity *pCS = (ioTCPClientSecurity*)m_pNS;

	if( !pCS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioFileSocket::CheckNS Check Sum Fail!! [%d : %d]", rkPacket.GetPacketID(), rkPacket.GetCheckSum() );
		return false;
	}

	if( !pCS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "ioFileSocket::CheckNSType:%d State Not Same Client:%d, Server:%d", 
			rkPacket.GetPacketID(), rkPacket.GetState(), pCS->GetRcvState() );
		return false;
	}

	return true;
}

void ioFileSocket::ProcessPacket( CPacket &rkPacket )
{
	g_FileWriteClient.RecvPacket( (SP2Packet&)rkPacket );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern LONG __stdcall ExceptCallBack ( EXCEPTION_POINTERS * pExPtrs );

template<> ioFileWriteClient *Singleton< ioFileWriteClient >::ms_Singleton = 0;

ioFileWriteClient::ioFileWriteClient(void)
{
	m_dwWriteSendTime = 0;
	m_hFile           = NULL;
	m_dwFileSize      = 0;
	m_dwReadSize      = 0;
	m_dwFileCRC       = 0;
	m_pSocket         = NULL;
	m_iServerPort     = 0;
	m_dwReturnParentID= 0;
	m_dwReturnID      = 0;
	m_dwFileWidth     = 0;
	m_dwFileHeight    = 0;
	m_dwConnectCheckTime = 0;

	m_pSocket  = new ioFileSocket( WM_FILESOCKET );
}

ioFileWriteClient::~ioFileWriteClient(void)
{
	SAFEDELETE( m_pSocket );
	if( m_hFile )
	{
		CloseHandle( m_hFile );
		m_hFile = NULL;
	}
	m_TempCustomFileList.clear();
}

ioFileWriteClient &ioFileWriteClient::GetSingleton()
{
	return Singleton< ioFileWriteClient >::GetSingleton();
}

void ioFileWriteClient::LoadINI()
{
	ioINILoader_e kLoader( "config/sp2_guild.ini" );
	kLoader.SetTitle_e( "custom" );
	char szBuf[MAX_PATH]="";
	kLoader.LoadString_e( "file_server_ip", "", szBuf, MAX_PATH );
	m_szServerIP  = szBuf;
	m_iServerPort = kLoader.LoadInt_e( "file_server_port", 0 );
	
	//임시
	m_nMaxSubCnt = kLoader.LoadInt_e( "temp_max_sub_custom_cnt", 0 );
	m_bSetTempUpload = kLoader.LoadBool_e( "set_temp_upload", false );

	char szAppDir[MAX_PATH] = "";
	::GetCurrentDirectory( MAX_PATH, szAppDir );

	sprintf( szBuf, "%s/resource/", szAppDir );
	::CreateDirectory( szBuf, NULL );

	sprintf( szBuf, "%s/resource/texture/", szAppDir );
	::CreateDirectory( szBuf, NULL );
	m_szTempUploadDir = szBuf;
}

bool ioFileWriteClient::Start( const char *szFilePath, const char *szWriteFileName, DWORD dwMaxFileSize, DWORD dwFileWidth, DWORD dwFileHeight, DWORD dwReturnParentID, DWORD dwReturnID )
{
	if( m_hFile )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return false;
	}

	if( !m_pSocket )
	{
		LOG.PrintTimeAndLog( 0, "%s Socket == NULL", __FUNCTION__ );
		return false;
	}

	if( m_pSocket->IsConnected() ) 
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	m_dwReturnParentID = dwReturnParentID;
	m_dwReturnID       = dwReturnID;

	if( !m_pSocket->InitClientTCP( g_App.GetHWnd(), m_pSocket->GetProcID(), m_szServerIP.c_str(), m_iServerPort ) )
	{
		InitData();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		LOG.PrintTimeAndLog( 0, "%s - TCP Socket Failed(%d)", __FUNCTION__, WSAGetLastError() );
		return false;
	}

	m_hFile = CreateFile( szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		InitData();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}

	m_dwFileSize = GetFileSize( m_hFile, NULL );
	if( m_dwFileSize > dwMaxFileSize )
	{
		InitData();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), dwMaxFileSize );
		return false;
	}

	if( dwFileWidth == 0 || dwFileHeight == 0 )
	{
		InitData();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return false;
	}

	if( strcmp( szWriteFileName, "" ) == 0 )
	{
		InitData();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return false;
	}

	DWORD dwCRC = 0;
	CCrc32Static::FileCrc32Streams( szFilePath, dwCRC );

	m_dwFileCRC       = dwCRC;
	m_szWriteFileName = szWriteFileName;
	m_dwFileWidth     = dwFileWidth;
	m_dwFileHeight    = dwFileHeight;
	m_dwReadSize      = 0;

	g_App.SetMouseBusy( true );
	m_dwConnectCheckTime = REALGETTIME();
	m_bTempSend = false;
	return true;
}

void ioFileWriteClient::Process()
{
	ProcessConnectCheck();

	if( !m_dwWriteSendTime || m_bTempSend )
		return;

	enum { WRITE_SEND_TIME = 100, };
	if( REALGETTIME() - m_dwWriteSendTime < WRITE_SEND_TIME )
		return;
	m_dwWriteSendTime = REALGETTIME();

	if( !m_hFile )
	{
		SP2Packet kPacket( CFTPK_FILE_WRITE );
		kPacket << FILE_WRITE_PROCESS_ERROR;
		SendLoop( kPacket );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		SetReturnWndParam( RETURN_WND_PARAM_FAIL );
		InitData();
		return;
	}

	FilePacket kFile;
	DWORD dwRead = 0;
	if( !ReadFile( m_hFile, kFile.m_FilePacket, sizeof(kFile.m_FilePacket), &dwRead, NULL) )
	{
		SP2Packet kPacket( CFTPK_FILE_WRITE );
		kPacket << FILE_WRITE_PROCESS_ERROR;
		SendLoop( kPacket );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		SetReturnWndParam( RETURN_WND_PARAM_FAIL );
		InitData();
		return;
	}

	m_dwReadSize += dwRead;

	FileWriteUploadWnd *pFileUploadWnd = dynamic_cast< FileWriteUploadWnd * >( g_GUIMgr.FindWnd( FILE_WRITE_SERVER_PROCESS ) );
	if( pFileUploadWnd && pFileUploadWnd->IsShow() )
	{
		pFileUploadWnd->UploadFileSize( m_dwReadSize, m_dwFileSize );
	}

	if( m_dwReadSize < m_dwFileSize )
	{
		SP2Packet kPacket( CFTPK_FILE_WRITE );
		kPacket << FILE_WRITE_PROCESS;
		kPacket << dwRead;
		kPacket << kFile;
		SendLoop( kPacket );
	}
	else
	{
		InitData();
		SP2Packet kPacket( CFTPK_FILE_WRITE );
		kPacket << FILE_WRITE_END;
		kPacket << dwRead;
		if( dwRead != 0 )
			kPacket << kFile;
		SendLoop( kPacket );
		
		LOG.PrintTimeAndLog( 0, "%s Complete", __FUNCTION__ );
	}
}

bool ioFileWriteClient::EvaluateSocket( WPARAM wParam, LPARAM lParam )
{
	if( !m_pSocket )
		return false;
	
	return m_pSocket->EvaluateSocket( wParam, lParam );	
}

void ioFileWriteClient::RecvPacket( SP2Packet &rkPacket )
{
#ifndef _DEBUG
	__try
#endif
	{
		switch( rkPacket.GetPacketID() )
		{
		case SFTPK_FILE_WRITE:
			ErrorReport::SetPosition( 8000, 1 );
			OnFileWrite( rkPacket );
			ErrorReport::SetPosition( 8000, 2 );
			return;
		case SFTPK_ALL_FILE_WRITE:
			ErrorReport::SetPosition( 8001, 1 );
			OnTempFileWrite( rkPacket );
			ErrorReport::SetPosition( 8001, 2 );
			return;
		}
	}
#ifndef _DEBUG
	__except( ExceptCallBack( GetExceptionInformation() ) )
	{
		LOG.PrintTimeAndLog( 0, "ExitProgram - 9" );
		g_App.SetExitProgram();
	}
#endif
}

void ioFileWriteClient::OnFileWrite( SP2Packet &rkPacket )
{
	g_App.SetMouseBusy( false );

	int iMsg = 0;
	rkPacket >> iMsg;

	int iParam = RETURN_WND_PARAM_COMPLETE;
	if( iMsg == FILE_WRITE_ERROR_FILE_NAME )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( iMsg == FILE_WRITE_EXIST_FILE )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else if( iMsg == FILE_WRITE_ERROR_CREATE_FILE )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
	else if( iMsg == FILE_WRITE_ERROR_WRITE_FILE )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
	}
	else if( iMsg == FILE_WRITE_ERROR_FILE_CRC )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
	}
	else if( iMsg == FILE_WRITE_ERROR_MOVE_FILE )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
	}
	else if( iMsg == FILE_WRITE_ERROR_WRONG_FILE )
	{
		iParam = RETURN_WND_PARAM_FAIL;
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
	}
	else if( iMsg == FILE_WRITE_START_OK )
	{
		m_dwWriteSendTime = REALGETTIME();    // PROCESS 시작
		return;
	}

	if( m_pSocket )
		m_pSocket->CloseClientTCP();
	InitData();
	SetReturnWndParam( iParam );
}

bool ioFileWriteClient::SendLoop( SP2Packet &rkPacket )
{
	if( m_pSocket )
		return m_pSocket->SendLoop( rkPacket );

	return false;
}

void ioFileWriteClient::SetReturnWndParam( int iParam )
{
	ioWnd *pParent = g_GUIMgr.FindWnd( m_dwReturnParentID );
	if( pParent )
	{
		ioWnd *pWnd = pParent->FindChildWnd( m_dwReturnID );
		if( pWnd )
			pParent->iwm_command( pWnd, IOBN_BTNUP, iParam );
	}
}

void ioFileWriteClient::ConnectProcess()
{
	if ( !m_bTempSend )
	{
		SP2Packet kPacket( CFTPK_FILE_WRITE );
		kPacket << FILE_WRITE_START;
		kPacket << g_MyInfo.GetUserIndex();
		kPacket << g_MyInfo.GetPrivateID();
		kPacket << g_MyInfo.GetPublicID();
		kPacket << g_MyInfo.GetMyIP();
		kPacket << m_szWriteFileName;
		kPacket << m_dwFileCRC;
		kPacket << m_dwFileWidth;
		kPacket << m_dwFileHeight;
		SendLoop( kPacket );
	}
	else
	{
		SP2Packet kPacket( CFTPK_ALL_FILE_WRITE );
		kPacket << FILE_WRITE_START;
		kPacket << g_MyInfo.GetUserIndex();
		kPacket << g_MyInfo.GetPrivateID();
		kPacket << g_MyInfo.GetPublicID();
		kPacket << g_MyInfo.GetMyIP();
		kPacket << m_szWriteFileName;
		kPacket << m_dwFileCRC;
		kPacket << m_dwFileWidth;
		kPacket << m_dwFileHeight;
		SendLoop( kPacket );
	}

	m_dwConnectCheckTime = 0;
}

void ioFileWriteClient::DisconnectProcess()
{
	g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	SetReturnWndParam( RETURN_WND_PARAM_FAIL );

	InitData();
}

void ioFileWriteClient::InitData()
{
	if( m_hFile )
	{
		CloseHandle( m_hFile );
		m_hFile = NULL;
	}
	m_dwWriteSendTime = 0;
	m_szWriteFileName.Clear();
	m_dwFileWidth      = 0;
	m_dwFileHeight     = 0;
	m_dwConnectCheckTime = 0;
	m_bTempSend = false;
}

void ioFileWriteClient::ProcessConnectCheck()
{
	if( m_dwConnectCheckTime == 0 )
		return;

	enum { DELAY_TIME = 60000, };
	if( REALGETTIME() - m_dwConnectCheckTime < DELAY_TIME )
		return;

	m_dwConnectCheckTime = 0;
	if( m_pSocket )
	{
		if( !m_pSocket->IsConnected() )
		{
			m_pSocket->CloseClientTCP();
			g_App.SetMouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			SetReturnWndParam( RETURN_WND_PARAM_FAIL );
			InitData();
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//유저의 장비스킨 업로드 관련 (머신 사용 불가능)
void ioFileWriteClient::CreateUploadList( DWORDVec &dwCustomIdxList )
{
	if ( dwCustomIdxList.empty() )
		return;

	m_TempCustomFileList.clear();

	char szName[MAX_PATH] = "";
	char szPath[MAX_PATH] = "";
	for ( int i=0; i<(int)dwCustomIdxList.size(); ++i )
	{
		for ( int j=0; j<m_nMaxSubCnt; ++j )
		{
			sprintf( szName, "%d_%d.lsc", dwCustomIdxList[i], j+1 );
			sprintf( szPath, "%s%s", m_szTempUploadDir.c_str(), szName );
			if ( IsFileCheck( szPath ) )
				m_TempCustomFileList.push_back( szName );
		}
	}

	m_TempState = TS_NEW;

	if( Help::CheckEnableTimeGapUser2( 1 ) )
		LOG.PrintTimeAndLog( 0, "void ioFileWriteClient::CreateUploadList");
}

bool ioFileWriteClient::IsFileCheck( const ioHashString &rkFileName )
{
	FILE *pFile = fopen( rkFileName.c_str(), "r" );
	if( pFile ) 
	{
		fclose( pFile );		
		return true;
	}
	return false;
}

bool ioFileWriteClient::TempStart( const char *szWriteFileName, DWORD dwMaxFileSize )
{
	char szPath[MAX_PATH] = "";
	sprintf( szPath, "%s%s", m_szTempUploadDir.c_str(), szWriteFileName );

	if( m_hFile )
	{
		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "업로드할 파일이 이미 오픈되어 있습니다." );
		return false;
	}

	if( !m_pSocket )
	{
		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "%s Socket == NULL", __FUNCTION__ );
		return false;
	}

	if( m_pSocket->IsConnected() ) 
	{
		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "업로드할 서버에 이미 연결되어 있습니다." );
		return false;
	}

	m_dwReturnParentID = 0;
	m_dwReturnID       = 0;

	if( !m_pSocket->InitClientTCP( g_App.GetHWnd(), m_pSocket->GetProcID(), m_szServerIP.c_str(), m_iServerPort ) )
	{
		InitData();
		if( Help::CheckEnableTimeGapUser2( 1 ) )
		{
			LOG.PrintTimeAndLog( 0, "업로드할 서버에 연결이 실패하였습니다." );
			LOG.PrintTimeAndLog( 0, "%s - TCP Socket Failed(%d)", __FUNCTION__, WSAGetLastError() );
		}
		return false;
	}

	m_hFile = CreateFile( szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		InitData();

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "파일 오픈이 실패하였습니다." );
		return false;
	}

	m_dwFileSize = GetFileSize( m_hFile, NULL );
	if( m_dwFileSize > dwMaxFileSize )
	{
		InitData();

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "파일이 %d바이트를 초과하였습니다.", dwMaxFileSize );
		return false;
	}
	
	if( strcmp( szWriteFileName, "" ) == 0 )
	{
		InitData();

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "업로드할 파일 이름이 잘못되었습니다." );
		return false;
	}

	DWORD dwCRC = 0;
	CCrc32Static::FileCrc32Streams( szPath, dwCRC );

	m_dwFileCRC       = dwCRC;
	m_szWriteFileName = szWriteFileName;
	m_dwFileWidth     = 0;
	m_dwFileHeight    = 0;
	m_dwReadSize      = 0;

	g_App.SetMouseBusy( true );
	m_dwConnectCheckTime = REALGETTIME();
	m_bTempSend = true;
	return true;
}

void ioFileWriteClient::ProcessTemp()
{
	if ( m_TempCustomFileList.empty() || !m_bSetTempUpload )
		return;

	switch ( m_TempState )
	{
	case TS_NEW:
		TempStart( m_TempCustomFileList[0].c_str(), 1000000 );
		m_TempState = TS_SEND;

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "void ioFileWriteClient::ProcessTemp - %s", m_TempCustomFileList[0].c_str() );
		break;
	case TS_SEND:
		ProcessTempSend();
		break;
	case TS_END:
		NextUploadList();
		break;
	}
}

void ioFileWriteClient::ProcessTempSend()
{
	ProcessConnectCheck();

	if( !m_dwWriteSendTime || !m_bTempSend )
		return;

	enum { WRITE_SEND_TIME = 100, };
	if( REALGETTIME() - m_dwWriteSendTime < WRITE_SEND_TIME )
		return;
	m_dwWriteSendTime = REALGETTIME();

	if( !m_hFile )
	{
		SP2Packet kPacket( CFTPK_ALL_FILE_WRITE );
		kPacket << FILE_WRITE_PROCESS_ERROR;
		SendLoop( kPacket );

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "파일 업로드 중 파일 컨트롤에 실패하였습니다." );
		InitData();
		return;
	}

	FilePacket kFile;
	DWORD dwRead = 0;
	if( !ReadFile( m_hFile, kFile.m_FilePacket, sizeof(kFile.m_FilePacket), &dwRead, NULL) )
	{
		SP2Packet kPacket( CFTPK_ALL_FILE_WRITE );
		kPacket << FILE_WRITE_PROCESS_ERROR;
		SendLoop( kPacket );

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "파일 업로드 중 파일 읽기에 실패하였습니다." );
		InitData();
		return;
	}

	m_dwReadSize += dwRead;

	if( m_dwReadSize < m_dwFileSize )
	{
		SP2Packet kPacket( CFTPK_ALL_FILE_WRITE );
		kPacket << FILE_WRITE_PROCESS;
		kPacket << dwRead;
		kPacket << kFile;
		SendLoop( kPacket );
	}
	else
	{
		InitData();
		SP2Packet kPacket( CFTPK_ALL_FILE_WRITE );
		kPacket << FILE_WRITE_END;
		kPacket << dwRead;
		if( dwRead != 0 )
			kPacket << kFile;
		SendLoop( kPacket );

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "%s Complete", __FUNCTION__ );
	}
}

void ioFileWriteClient::NextUploadList()
{	
	m_TempCustomFileList.erase( m_TempCustomFileList.begin() );
	if ( m_TempCustomFileList.empty() )
		return;

	m_TempState = TS_NEW;

	if( Help::CheckEnableTimeGapUser2( 1 ) )
		LOG.PrintTimeAndLog( 0, "void ioFileWriteClient::NextUploadList - %s", m_TempCustomFileList[0].c_str() );
}

void ioFileWriteClient::OnTempFileWrite( SP2Packet &rkPacket )
{
	g_App.SetMouseBusy( false );

	int iMsg = 0;
	rkPacket >> iMsg;
		
	bool bLog = false;
	if( Help::CheckEnableTimeGapUser2( 1 ) )
		bLog = true;

	if( iMsg == FILE_WRITE_ERROR_FILE_NAME && bLog )
		LOG.PrintTimeAndLog( 0, "등록할 파일 이름이 잘못되었습니다." );
	else if( iMsg == FILE_WRITE_EXIST_FILE && bLog )
		LOG.PrintTimeAndLog( 0, "이미 등록된 파일입니다." );
	else if( iMsg == FILE_WRITE_ERROR_CREATE_FILE && bLog )
		LOG.PrintTimeAndLog( 0, "등록할 파일 생성이 실패하였습니다." );
	else if( iMsg == FILE_WRITE_ERROR_WRITE_FILE && bLog )
		LOG.PrintTimeAndLog( 0, "등록할 파일 쓰기가 실패하였습니다." );
	else if( iMsg == FILE_WRITE_ERROR_FILE_CRC && bLog )
		LOG.PrintTimeAndLog( 0, "등록할 파일이 손상되었습니다." );
	else if( iMsg == FILE_WRITE_ERROR_MOVE_FILE && bLog )
		LOG.PrintTimeAndLog( 0, "등록할 파일 이동이 실패하였습니다." );
	else if( iMsg == FILE_WRITE_ERROR_WRONG_FILE && bLog )
		LOG.PrintTimeAndLog( 0, "등록할 파일이 JPG 파일 아니거나 사이즈가 잘못되었습니다." );
	else if( iMsg == FILE_WRITE_EXEPTION && bLog )
		LOG.PrintTimeAndLog( 0, "유저 업로드가 비활성화 상태입니다." );
	else if( iMsg == FILE_WRITE_START_OK )
	{
		m_dwWriteSendTime = REALGETTIME();    // PROCESS 시작
		return;
	}

	if( m_pSocket )
		m_pSocket->CloseClientTCP();
	InitData();

	m_TempState = TS_END;
}