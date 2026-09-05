
#include "stdafx.h"
#include "User.h"
#include "UserNodeManager.h"

#include "../network/ioServerSecurity.h"
#include "../network/ioPacketQueue.h"
#include "../network/Protocol.h"
#include "../ioMainProcess.h"
#include "../Util/Crc32Static.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool User::m_bUseSecurity = false;
int  User::m_iSecurityOneSecRecv = 30;

void User::LoadHackCheckValue()
{
	ioINILoader kLoader( "FileWriteServerInfo.ini" );
	kLoader.SetTitle( "SECURITY" );

	if( kLoader.LoadInt( "ON", 0 ) == 1 )
		m_bUseSecurity = true;
	else
		m_bUseSecurity = false;

	m_iSecurityOneSecRecv = kLoader.LoadInt( "ONE_SEC_RCV", 30 );
}

User::User( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize ) : CConnectNode( s, dwSendBufSize, dwRecvBufSize )
{
	m_hFile = NULL;
	InitData();

	if( m_bUseSecurity )
	{
		SetNS( new ioServerSecurity );
	}
}

User::~User()
{	
	SAFEDELETE( m_pNS );
}

void User::InitData()
{
	m_sync_time       = 0;
	m_dwSyncCheckTime = 300000;
	m_eSessionState   = SS_DISCONNECT;
	m_iUserState = 0;
	m_dwUserIndex = 0;

	m_dwFileCRC  = 0;
	m_szFileName = "";
	if( m_hFile )
	{
		CloseHandle( m_hFile );
		//kyg 0byte면 삭제 
		DWORD  dwFileSize  = 0;
		if(GetFileSize(m_hFile,&dwFileSize) == TRUE)
		{
			if(dwFileSize == 0 && (m_iUserState == FILE_WRITE_START || m_iUserState == FILE_WRITE_PROCESS))
			{
				ioHashString sPath;
				ioHashString sTempPath;
				ioHashString sErrorPath;

				g_App.GetAllPath( m_szFileName, sPath, sTempPath, sErrorPath );
				char szNewFilePath[MAX_PATH*2]="";
				StringCbPrintf( szNewFilePath, sizeof( szNewFilePath ), "%s\\%s", sTempPath.c_str(), m_szFileName.c_str() );

				SetFileAttributes(szNewFilePath,FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(szNewFilePath);
			}
			
		}
		
		m_hFile = NULL;
	}

	m_dwFileWidth  = 0;
	m_dwFileHeight = 0;
}

void User::OnCreate()
{
	CConnectNode::OnCreate();

	if( m_bUseSecurity )
	{		
		ioServerSecurity *pSS = (ioServerSecurity *)m_pNS;
		if( pSS ) 
		{
			pSS->InitDoSAttack( m_iSecurityOneSecRecv );
			pSS->InitState( m_socket );		
		}
	}
	m_eSessionState = SS_CONNECT;
	m_sync_time     = TIMEGETTIME();

	Information( "\n User Connect... \n" );
}

void User::OnDestroy()
{
	CConnectNode::OnDestroy();	
	m_eSessionState = SS_DISCONNECT;
}

void User::OnSessionDestroy()
{
	InitData();	
}

bool User::CheckNS( CPacket &rkPacket )
{
	if( m_pNS == NULL ) return true;

	ioServerSecurity *pSS = (ioServerSecurity*)m_pNS;
	if( !pSS->IsCheckSum( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "User::CheckNS Check Sum Fail!! [%s : 0x%x]",
								GetPublicIP(), rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}
	
	if( !pSS->CheckState( rkPacket ) )
	{
		LOG.PrintTimeAndLog( 0, "User::CheckNS State Not Same Client:%d, Server:%d [%s : 0x%x]", 
			rkPacket.GetState(), pSS->GetRcvState(), GetPublicIP(), rkPacket.GetPacketID() );
		ExceptionClose( 0 );
		return false;
	}
	
	if( !pSS->UpdateReceiveCount() )
	{
		LOG.PrintTimeAndLog( 0, "User::CheckNS ONE SEC MANY PACKET(%d)!! [%s : 0x%x]", 
			pSS->GetRcvCount(), GetPublicIP(), rkPacket.GetPacketID() );

		ExceptionClose( 0 );
		return false;
	}
	
	return true;
}

bool User::SendMessage( CPacket &rkPacket )
{
	// 임시
	return CConnectNode::SendMessage( rkPacket );
}

int User::GetConnectType()
{
	//return CONNECT_TYPE_USER;
	return 1;
}

void User::ReceivePacket( CPacket &packet )
{
	g_RecvQueue.InsertQueue( (DWORD)this, packet, PK_QUEUE_SESSION );
}

void User::PacketParsing( CPacket &packet )
{
	SP2Packet &kPacket = (SP2Packet&)packet;
	switch( packet.GetPacketID() )
	{
		//TCP
	case CFTPK_FILE_WRITE:            // 첫 접속
		OnFileWrite( kPacket );
		break;

	case CFTPK_ALL_FILE_WRITE:
		OnAllFileWrite( kPacket );
		break;

	case CFTPK_CLOSE_SESSION:
		OnClose();
		break;

	default:
		LOG.PrintTimeAndLog( 0, "0x%x Unknown CPacket",  kPacket.GetPacketID() );
		break;
	}
}

void User::SessionClose( BOOL safely )
{
	if( IsActive() )
	{
		CPacket packet( CFTPK_CLOSE_SESSION );
		ReceivePacket( packet );
	}
}

void User::OnClose()
{
	if( ! IsDisconnectState() )
	{
		OnDestroy();
		OnSessionDestroy();	         //접속 종료 저장 및 노드 초기화( 서버 이동한 유저는 초기화만하고 저장하지 않는다. )	
		g_UserNodeManager.RemoveNode( this );

		Information( "\n User DisConnected... \n" );
	}
}

bool User::IsExistFile( const char* fileName ) // 파일 존재 확인 
{
	FILE* pfile;
	if (fopen_s(&pfile, fileName, "rt") != 0)
	{
		return false;
	}
	fclose(pfile);
	return true;
}

bool User::IsRightFile( DWORD dwFileWidth, DWORD dwFileHeight, const char *szFileName )
{
	WCHAR wszPath[MAX_PATH+1];
	MultiByteToWideChar(CP_ACP, NULL, szFileName, -1, wszPath, MAX_PATH);

	Bitmap bmp( wszPath );
	if (bmp.GetLastStatus() != Ok)
	{
		LOG.PrintTimeAndLog(0,"%s Error bmp File (%d)",__FUNCTION__,bmp.GetLastStatus());
		return false;
	}
	int iWidth  = bmp.GetWidth() ;
	int iHeight = bmp.GetHeight();

	if( iWidth != dwFileWidth )
	{
		LOG.PrintTimeAndLog( 0, "%s Wrong Size [%d:%d]%d:%d", __FUNCTION__, iWidth, iHeight, dwFileWidth, dwFileHeight );
		return false;
	}

	if( iHeight != dwFileHeight )
	{
		LOG.PrintTimeAndLog( 0, "%s Wrong Size [%d:%d]%d:%d", __FUNCTION__, iWidth, iHeight, dwFileWidth, dwFileHeight );
		return false;
	}

	GUID guid;
	bmp.GetRawFormat( &guid );

	char szExt[MAX_PATH]="";
	_splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));

	if (_stricmp(szExt, ".jpg"))
	{
		if (ImageFormatJPEG != guid)
		{
			LOG.PrintTimeAndLog(0, "%s Wrong Format JPG", __FUNCTION__);
			return false;
		}
	}
	else if (_stricmp(szExt, ".bmp"))
	{
		if (ImageFormatBMP != guid)
		{
			LOG.PrintTimeAndLog(0, "%s Wrong Format BMP", __FUNCTION__);
			return false;
		}
	}
	else if (_stricmp(szExt, ".png"))
	{
		if (ImageFormatPNG != guid)
		{
			LOG.PrintTimeAndLog(0, "%s Wrong Format PNG", __FUNCTION__);
			return false;
		}
	}
	else if (_stricmp(szExt, ".gif"))
	{
		if (ImageFormatGIF != guid)
		{
			LOG.PrintTimeAndLog(0, "%s Wrong Format GIF", __FUNCTION__);
			return false;
		}
	}
	else
	{
		LOG.PrintTimeAndLog(0, "%s Wrong Format UNKNOWN", __FUNCTION__);
		return false;
	}

	return true;
}

void User::OnFileWrite( SP2Packet &rkPacket )
{
	m_sync_time = TIMEGETTIME();

	int iMsgType = 0;
	rkPacket >> iMsgType;
	m_iUserState = iMsgType;

	if( iMsgType == FILE_WRITE_START )
	{
		ioHashString szPrivateID;
		ioHashString szPublicID;
		ioHashString szPublicIP;

		rkPacket >> m_dwUserIndex;
		rkPacket >> szPrivateID;
		rkPacket >> szPublicID;
		rkPacket >> szPublicIP;
		rkPacket >> m_szFileName;
		rkPacket >> m_dwFileCRC;
		rkPacket >> m_dwFileWidth;
		rkPacket >> m_dwFileHeight;

		LOG.PrintTimeAndLog( 0, "[newSkin] FILE START : [%u:%s:%s][%u:%s]", m_dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), m_dwFileCRC, m_szFileName.c_str() );

		ioHashString sPath;
		ioHashString sTempPath;
		ioHashString sErrorPath;
		g_App.GetAllPath( m_szFileName, sPath, sTempPath, sErrorPath );

		if( m_szFileName.IsEmpty() || sPath.IsEmpty() || sTempPath.IsEmpty() || sErrorPath.IsEmpty() )
		{
			SP2Packet kPacket( SFTPK_FILE_WRITE );
			kPacket << FILE_WRITE_ERROR_FILE_NAME;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Empty Name %u", __FUNCTION__, m_dwUserIndex );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		char szTempFilePath[MAX_PATH*2]="";
		StringCbPrintf( szTempFilePath, sizeof( szTempFilePath ), "%s\\%s", sPath.c_str(), m_szFileName.c_str() );
		if( IsExistFile( szTempFilePath ) )
		{
			SP2Packet kPacket( SFTPK_FILE_WRITE );
			kPacket << FILE_WRITE_EXIST_FILE;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Exist File %u %s", __FUNCTION__, m_dwUserIndex, szTempFilePath );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		// exist del
		char szNewFilePath[MAX_PATH*2]="";
		StringCbPrintf( szNewFilePath, sizeof( szNewFilePath ), "%s\\%s", sTempPath.c_str(), m_szFileName.c_str() );

		SetFileAttributes(szNewFilePath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(szNewFilePath);

		// create file
		if( m_hFile )
		{
			CloseHandle( m_hFile );
			m_hFile = NULL;
		}

		m_hFile = CreateFile( szNewFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE) 
		{
			if( m_hFile )
			{
				CloseHandle( m_hFile );
				m_hFile = NULL;
			}

			SP2Packet kPacket( SFTPK_FILE_WRITE );
			kPacket << FILE_WRITE_ERROR_CREATE_FILE;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Error Create File %u %s", __FUNCTION__, m_dwUserIndex, szNewFilePath );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		StringCbCopy( m_public_ip, sizeof( m_public_ip ), szPublicIP.c_str() );

		SP2Packet kPacket( SFTPK_FILE_WRITE );
		kPacket << FILE_WRITE_START_OK;
		SendMessage( kPacket );
	}
	else if( iMsgType == FILE_WRITE_PROCESS 
		  || iMsgType == FILE_WRITE_END )
	{
		// check
		if( !m_hFile )
		{
			SP2Packet kPacket( SFTPK_FILE_WRITE );
			kPacket << FILE_WRITE_ERROR_CREATE_FILE;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Error File Handle %u", __FUNCTION__, m_dwUserIndex );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		// write
		DWORD  dwRead = 0;
		rkPacket >> dwRead;

		if( dwRead != 0 )
		{
			FilePacket kFile;
			rkPacket >> kFile;

			DWORD	dwBytesWritten = 0;
			if( WriteFile( m_hFile, kFile.m_FilePacket, dwRead, &dwBytesWritten, NULL ) == false )  
			{
				if( m_hFile )
				{
					CloseHandle( m_hFile );
					m_hFile = NULL;
				}

				SP2Packet kPacket( SFTPK_FILE_WRITE );
				kPacket << FILE_WRITE_ERROR_WRITE_FILE;
				SendMessage( kPacket );
				LOG.PrintTimeAndLog( 0, "%s Error WriteFile %u : %d", __FUNCTION__, m_dwUserIndex, GetLastError() );
				m_dwFileCRC = 0;
				m_szFileName.Clear();
				return;
			}
		}

		ioHashString sPath;
		ioHashString sTempPath;
		ioHashString sErrorPath;
		g_App.GetAllPath( m_szFileName, sPath, sTempPath, sErrorPath );

		if( iMsgType == FILE_WRITE_END )
		{
			if( m_hFile )
			{
				CloseHandle( m_hFile );
				m_hFile = NULL;
			}

			// crc
			char szTempFilePath[MAX_PATH*2]="";
			StringCbPrintf( szTempFilePath, sizeof( szTempFilePath ), "%s\\%s", sTempPath.c_str(), m_szFileName.c_str() );

			DWORD dwNewFileCRC = 0;
			CCrc32Static::FileCrc32Streams( szTempFilePath , dwNewFileCRC );
			if( m_dwFileCRC != dwNewFileCRC )
			{
				SetFileAttributes(szTempFilePath,FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(szTempFilePath);

				SP2Packet kPacket( SFTPK_FILE_WRITE );
				kPacket << FILE_WRITE_ERROR_FILE_CRC;
				SendMessage( kPacket );
				LOG.PrintTimeAndLog( 0, "%s Error CRC %u : %u", __FUNCTION__, m_dwUserIndex, dwNewFileCRC );
				m_dwFileCRC = 0;
				m_szFileName.Clear();
				return;
			}

			enum { EXT_SIZE = 3, };
			bool bJPG = false;
			char szExt[MAX_PATH]="";
			_splitpath_s(m_szFileName.c_str(), NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
			if (_strnicmp(&szExt[1], "jpg", EXT_SIZE) == 0) // szExt[1]은 .을 제외
				bJPG = true;

			if( bJPG && ! IsRightFile( m_dwFileWidth, m_dwFileHeight, szTempFilePath ) )
			{
				char szErrorFilePath[MAX_PATH*2]="";
				StringCbPrintf( szErrorFilePath, sizeof( szErrorFilePath ), "%s\\%s", sErrorPath.c_str(), m_szFileName.c_str() );

				SetFileAttributes(szErrorFilePath,FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(szErrorFilePath);

				if( MoveFile( szTempFilePath, szErrorFilePath ) == false )
				{
					SetFileAttributes(szTempFilePath,FILE_ATTRIBUTE_NORMAL);
					::DeleteFile(szTempFilePath);
					LOG.PrintTimeAndLog( 0, "%s Wrong File Move Error", __FUNCTION__ );
				}

				SP2Packet kPacket( SFTPK_FILE_WRITE );
				kPacket << FILE_WRITE_ERROR_WRONG_FILE;
				SendMessage( kPacket );
				LOG.PrintTimeAndLog( 0, "%s Wrong File %u : %u", __FUNCTION__, m_dwUserIndex, dwNewFileCRC );
				m_dwFileCRC = 0;
				m_szFileName.Clear();
				return;
			}

			// move
			char szRealFilePath[MAX_PATH*2]="";
			StringCbPrintf( szRealFilePath, sizeof( szRealFilePath ), "%s\\%s", sPath.c_str(), m_szFileName.c_str() );
			if( MoveFile( szTempFilePath, szRealFilePath ) == false ) 
			{
				SetFileAttributes(szTempFilePath,FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(szTempFilePath);

				SP2Packet kPacket( SFTPK_FILE_WRITE );
				kPacket << FILE_WRITE_ERROR_MOVE_FILE;
				SendMessage( kPacket );
				LOG.PrintTimeAndLog( 0, "%s Error MoveFile %u : %s", __FUNCTION__, m_dwUserIndex, m_szFileName.c_str()  );
				m_dwFileCRC = 0;
				m_szFileName.Clear();
				return;
			}

			m_dwFileCRC = 0;
			m_szFileName.Clear();

			SP2Packet kPacket( SFTPK_FILE_WRITE );
			kPacket << FILE_WRITE_SUCCESS;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s COMPLATE %u : %s", __FUNCTION__, m_dwUserIndex, szRealFilePath  );
		}
	}
	else if( iMsgType == FILE_WRITE_PROCESS_ERROR )
	{
		LOG.PrintTimeAndLog( 0, "CFTPK_FILE_WRITE error" );
	}
}

void User::OnAllFileWrite( SP2Packet &rkPacket )
{
	m_sync_time = TIMEGETTIME();

	int iMsgType = 0;
	rkPacket >> iMsgType;
	m_iUserState = iMsgType;

	if( iMsgType == FILE_WRITE_START )
	{
		ioHashString szPrivateID;
		ioHashString szPublicID;
		ioHashString szPublicIP;

		rkPacket >> m_dwUserIndex;
		rkPacket >> szPrivateID;
		rkPacket >> szPublicID;
		rkPacket >> szPublicIP;
		rkPacket >> m_szFileName;
		rkPacket >> m_dwFileCRC;
		rkPacket >> m_dwFileWidth;
		rkPacket >> m_dwFileHeight;

		//LOG.PrintTimeAndLog( 0, "[existSkin] FILE START : [%u:%s:%s][%u:%s]", m_dwUserIndex, szPrivateID.c_str(), szPublicID.c_str(), m_dwFileCRC, m_szFileName.c_str() );

		ioHashString sPath;
		ioHashString sTempPath;
		ioHashString sErrorPath;
		
		if( !g_App.IsPossibleUserUpload() )
		{
			SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
			kPacket << FILE_WRITE_EXEPTION;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s FILE_WRITE_EXEPTION %u", __FUNCTION__, m_dwUserIndex );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		g_App.GetUserUploadPath( sPath, sTempPath, sErrorPath );

		if( sPath.IsEmpty() || sTempPath.IsEmpty() || sErrorPath.IsEmpty() )
		{
			SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
			kPacket << FILE_WRITE_ERROR_FILE_NAME;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Empty Name %u", __FUNCTION__, m_dwUserIndex );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		char szTempFilePath[MAX_PATH*2]="";
		StringCbPrintf( szTempFilePath, sizeof( szTempFilePath ), "%s\\%s", sPath.c_str(), m_szFileName.c_str() );
		if( IsExistFile( szTempFilePath ) )
		{
			SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
			kPacket << FILE_WRITE_EXIST_FILE;
			SendMessage( kPacket );
			//LOG.PrintTimeAndLog( 0, "%s Exist File %u %s", __FUNCTION__, m_dwUserIndex, szTempFilePath );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		// exist del
		char szNewFilePath[MAX_PATH*2]="";
		StringCbPrintf( szNewFilePath, sizeof( szNewFilePath ), "%s\\%s", sTempPath.c_str(), m_szFileName.c_str() );

		SetFileAttributes(szNewFilePath,FILE_ATTRIBUTE_NORMAL);
		::DeleteFile(szNewFilePath);

		// create file
		if( m_hFile )
		{
			CloseHandle( m_hFile );
			m_hFile = NULL;
		}

		m_hFile = CreateFile( szNewFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if (m_hFile == INVALID_HANDLE_VALUE) 
		{
			if( m_hFile )
			{
				CloseHandle( m_hFile );
				m_hFile = NULL;
			}

			SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
			kPacket << FILE_WRITE_ERROR_CREATE_FILE;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Error Create File %u %s", __FUNCTION__, m_dwUserIndex, szNewFilePath );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		StringCbCopy( m_public_ip, sizeof( m_public_ip ), szPublicIP.c_str() );

		SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
		kPacket << FILE_WRITE_START_OK;
		SendMessage( kPacket );
	}
	else if( iMsgType == FILE_WRITE_PROCESS 
		|| iMsgType == FILE_WRITE_END )
	{
		// check
		if( !m_hFile )
		{
			SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
			kPacket << FILE_WRITE_ERROR_CREATE_FILE;
			SendMessage( kPacket );
			LOG.PrintTimeAndLog( 0, "%s Error File Handle %u", __FUNCTION__, m_dwUserIndex );
			m_dwFileCRC = 0;
			m_szFileName.Clear();
			return;
		}

		// write
		DWORD  dwRead = 0;
		rkPacket >> dwRead;

		if( dwRead != 0 )
		{
			FilePacket kFile;
			rkPacket >> kFile;

			DWORD	dwBytesWritten = 0;
			if( WriteFile( m_hFile, kFile.m_FilePacket, dwRead, &dwBytesWritten, NULL ) == false )  
			{
				if( m_hFile )
				{
					CloseHandle( m_hFile );
					m_hFile = NULL;
				}

				SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
				kPacket << FILE_WRITE_ERROR_WRITE_FILE;
				SendMessage( kPacket );
				LOG.PrintTimeAndLog( 0, "%s Error WriteFile %u : %d", __FUNCTION__, m_dwUserIndex, GetLastError() );
				m_dwFileCRC = 0;
				m_szFileName.Clear();
				return;
			}
		}

		ioHashString sPath;
		ioHashString sTempPath;
		ioHashString sErrorPath;
		g_App.GetUserUploadPath( sPath, sTempPath, sErrorPath );

		if( iMsgType == FILE_WRITE_END )
		{
			if( m_hFile )
			{
				CloseHandle( m_hFile );
				m_hFile = NULL;
			}
			char szTempFilePath[MAX_PATH*2]="";
			StringCbPrintf( szTempFilePath, sizeof( szTempFilePath ), "%s\\%s", sTempPath.c_str(), m_szFileName.c_str() );

			// move
			char szRealFilePath[MAX_PATH*2]="";
			StringCbPrintf( szRealFilePath, sizeof( szRealFilePath ), "%s\\%s", sPath.c_str(), m_szFileName.c_str() );
			if( MoveFile( szTempFilePath, szRealFilePath ) == false ) 
			{
				SetFileAttributes(szTempFilePath,FILE_ATTRIBUTE_NORMAL);
				::DeleteFile(szTempFilePath);

				SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
				kPacket << FILE_WRITE_ERROR_MOVE_FILE;
				SendMessage( kPacket );
				LOG.PrintTimeAndLog( 0, "%s Error MoveFile %u : %s", __FUNCTION__, m_dwUserIndex, m_szFileName.c_str()  );
				m_dwFileCRC = 0;
				m_szFileName.Clear();
				return;
			}

			m_dwFileCRC = 0;
			m_szFileName.Clear();

			SP2Packet kPacket( SFTPK_ALL_FILE_WRITE );
			kPacket << FILE_WRITE_SUCCESS;
			SendMessage( kPacket );
			//LOG.PrintTimeAndLog( 0, "%s COMPLATE %u : %s", __FUNCTION__, m_dwUserIndex, szRealFilePath  );
		}
	}
	else if( iMsgType == FILE_WRITE_PROCESS_ERROR )
	{
		LOG.PrintTimeAndLog( 0, "CFTPK_FILE_WRITE error" );
	}
}
