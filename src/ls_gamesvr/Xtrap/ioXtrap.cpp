#include "stdafx.h"

#ifdef XTRAP

#include ".\ioxtrap.h"
#include "XTrap4Server.h"
#include "../MainServerNode/MainServerNode.h"
#include <strsafe.h>

#define CS_FILE_PATH "Xtrap\\ls.CS3"

ioXtrap *ioXtrap::sg_Instance = NULL;

BYTE  ioXtrap::m_byCS3Data[MAX_CS3FILE_COUNT][MAX_CS3FILE_DATA_SIZE];

ioXtrap::ioXtrap(void)
{
	InitializeCriticalSection(&m_CriticalSection);
	ZeroMemory( m_byCS3Data, MAX_CS3FILE_COUNT*MAX_CS3FILE_DATA_SIZE );
	ZeroMemory( m_byTempCS3Data, MAX_CS3FILE_DATA_SIZE );
	ZeroMemory( m_szServerIP, MAX_SERVER_IP );
	// XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS가 주로사용하는 기본 옵션이며, 해킹이 심한 경우 Xtrap과 협의후 RData와 EData 늘려 갈 수 있음. FilePatchOpiont은 Xtrap 호환성 테스트를 받아야함.
	m_dwMethod = XTRAP_PROTECT_PE | XTRAP_PROTECT_TEXT | XTRAP_PROTECT_EXCEPT_VIRUS; // 서버, 클라이언트, X-Protect 값이 동일해야 함.
	for (int i = 0; i <  MAX_CS3FILE_COUNT; i++)
		m_iCS3FileVersion[i] = 0;

	m_hFile             = NULL;
	m_dwReadSize        = 0;

	m_bUse              = false;
	m_iOpenVersion      = 0;
	m_iLastDeletVersion = 0;
	m_iChangeArray      = -1; //초기화
}

ioXtrap::~ioXtrap(void)
{
	DeleteCriticalSection(&m_CriticalSection);
}

ioXtrap &ioXtrap::GetInstance()
{
	if( sg_Instance == NULL )
		sg_Instance = new ioXtrap;

	return (*sg_Instance);
}

void ioXtrap::ReleaseInstance()
{
	if( sg_Instance )
		delete sg_Instance;
	
	sg_Instance = NULL;
}


bool ioXtrap::LoadDll( bool bUse )
{
	m_bUse = bUse;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap Use : %d", bUse );
	if( !m_bUse )
		return true;
 
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadDll start" );
	DWORD dwResult = XTrap_S_LoadDll();
	if( dwResult !=  XTRAP_API_RETURN_OK )
	{
		//MessageBox( NULL, "Xtrap DLL Load Fail.", "IOEnter", MB_OK );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadDll end - Fail :%d:%d", dwResult, GetLastError() );
		return false;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadDll end" );
	return true;
}

bool ioXtrap::FreeDll()
{
	if( !m_bUse )
		return true;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::FreeDll start" );
	DWORD dwResult = XTrap_S_FreeDll();
	if( dwResult !=  XTRAP_API_RETURN_OK )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::FreeDll end - Fail :%d:%d" , dwResult, GetLastError() );
		return false;
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::FreeDll end" );
	return true;
}

bool ioXtrap::Start( const char *pServerIP )
{
	if( !m_bUse )
		return true;

	StringCbCopy( m_szServerIP, sizeof( m_szServerIP ), pServerIP );
	__try
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Start start" );
		DWORD dwResult = XTrap_S_Start( MAX_TIMEOUT_SEC, MAX_CS3FILE_COUNT, m_byCS3Data, NULL );
		if( dwResult != XTRAP_API_RETURN_OK )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Start end - Fail :%d:%d", dwResult, GetLastError() );
			return false;
		}
		// For Themida Packer
		dwResult = XTrap_S_SetActiveCode(  XTRAP_ACTIVE_CODE_LEVEL2 );
		if( dwResult != XTRAP_API_RETURN_OK )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Start end - Fail 2 :%d:%d", dwResult, GetLastError() );
			return false;
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Start end" );
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Start : Crash : %d", GetLastError() );
		return false;
	}
	return true;
}

bool ioXtrap::Init( BYTE *pSessionBuf, IN const char *szPublicID, IN const char *szPublicIP, IN int iLevel )
{
	if( !m_bUse )
		return true;

	__try
	{
		EnterCriticalSection(&m_CriticalSection);
		DWORD dwResult = XTrap_S_SessionInit( MAX_TIMEOUT_SEC, MAX_CS3FILE_COUNT, m_byCS3Data, pSessionBuf );
		if(  dwResult != XTRAP_API_RETURN_OK )
		{
			LeaveCriticalSection(&m_CriticalSection);
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Init : Fail : %d : %d : %s", dwResult, GetLastError() , szPublicID );
			return false;
		}
		LeaveCriticalSection(&m_CriticalSection);
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::Init : Crash : %d : %s", GetLastError() , szPublicID );
		return false;
	}
	return true;
}

bool ioXtrap::Step1( BYTE *pSessionBuf , OUT BYTE *pPacketBuf, IN const char *szPublicID, IN const char *szPublicIP, IN int iLevel )
{
	if( !m_bUse )
		return true;

	__try
	{
		DWORD dwResult = XTrap_CS_Step1( pSessionBuf, pPacketBuf );
		if(  dwResult != XTRAP_API_RETURN_OK )
		{
			unsigned int iDetectCode=0;
			memcpy(&iDetectCode, ((unsigned char *)pSessionBuf+8), 4);
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][xtrap]Step1 fail : [%d][%d][%s][%u]", dwResult, GetLastError() , szPublicID, iDetectCode );
			return false;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][xtrap]Step1 crash : [%d][%s]", GetLastError() , szPublicID );
		return false;
	}
	return true;
}

bool ioXtrap::Step3( BYTE *pSessionBuf, IN BYTE * pPacketBuf, IN const char *szPublicID, IN const char *szPublicIP, IN int iLevel )
{
	if( !m_bUse )
		return true;

	__try
	{
		DWORD dwResult = XTrap_CS_Step3( pSessionBuf, pPacketBuf );
		if( dwResult != XTRAP_API_RETURN_OK )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][xtrap]Step3 fail : [%d][%d][%s]", dwResult, GetLastError() , szPublicID );
			return false;
		}
	}
	__except (ExceptCallBack (GetExceptionInformation()))
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[error][xtrap]Step3 crash : [%d][%s]", GetLastError() , szPublicID );
		return false;
	}
	return true;
}

bool ioXtrap::LoadCS3File()
{
	if( !m_bUse )
		return true;

	bool bOk = false;
	for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
	{
		char szPath[MAX_PATH]="";
		StringCbPrintf( szPath, sizeof( szPath ), "Xtrap\\ls%d.cs3", i+1 );
		if( LoadOneCS3File( szPath, i ) )
			bOk = true;
	}

	if( !bOk )
	{
		LOG.PrintTimeAndLog(LOG_DEBUG_LEVEL, "Fail Read CS3 File.");
		return false;
	}

	int  iOldVersionArray = 0;
	int  iOldVersion      = 0;
	// 정상값 1개를 고른다.
	for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
	{
		if( m_iCS3FileVersion[i] > 0 )
		{
			iOldVersion      = m_iCS3FileVersion[i];
			iOldVersionArray = i;
			break;
		}
	}

	// 제일 작은 값을 고른다.
	for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
	{
		if( m_iCS3FileVersion[i] <= 0 )
			continue;
		if( m_iCS3FileVersion[i] < iOldVersion )
		{
			iOldVersionArray = i;
			iOldVersion      = m_iCS3FileVersion[i];
		}
	}

	// 모든 버퍼를 채운다.
	for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
	{
		if( m_iCS3FileVersion[i] != 0 )
			continue;
		memcpy( m_byCS3Data[i], m_byCS3Data[iOldVersionArray], MAX_CS3FILE_DATA_SIZE );
		m_iCS3FileVersion[i] = m_iCS3FileVersion[iOldVersionArray];
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Xtrap CS3 Copy Complete on Start. %d[%d]%d", i, m_iCS3FileVersion[iOldVersionArray], iOldVersionArray );
	}

	return true;
}

void ioXtrap::OpenCS3File( const ioHashString &rszGUID, int iVersion, int iChange )
{
	if( !m_bUse )
		return;

	if( m_hFile != NULL ) // 열려 있는지 검사가 제일 처음.
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::OpenMapFile m_hFile != NULL :%s", rszGUID.c_str() );

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_FAIL;
		kPacket << rszGUID;
		kPacket << 0;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	// 중복 검사
	if( iChange == 0 ) // 변경할 버전이 없다면
	{
		for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
		{
			if( m_iCS3FileVersion[i] == iVersion )
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::OpenMapFile version is exist :%s:%d", rszGUID.c_str(), iVersion );

				SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
				kPacket << LOAD_CS3_FILE_EXIST_VERSION;
				kPacket << rszGUID;
				kPacket << 0;
				g_MainServer.SendMessage( kPacket );
				return;
			}
		}
	}

	// 변경할 버전이 있다.
	if( iChange != 0 ) 
	{
		bool bOk = false;
		for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
		{
			if( m_iCS3FileVersion[i] == iChange )
			{
				bOk = true;
				m_iChangeArray = i;
				break;
			}
		}

		if( bOk == false)
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::OpenMapFile Change version is wrong :%s:%d", rszGUID.c_str(), iChange );

			SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
			kPacket << LOAD_CS3_FILE_CHANGE_WRONG;
			kPacket << rszGUID;
			kPacket << 0;
			g_MainServer.SendMessage( kPacket );
			return;
		}
	}
	else
		m_iChangeArray = -1; // 초기화

	m_hFile = CreateFile( CS_FILE_PATH, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( m_hFile == INVALID_HANDLE_VALUE )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::OpenMapFile CreateFile Fail.%d:%s", GetLastError(), rszGUID.c_str() );
		m_hFile = NULL;
		m_dwReadSize = 0;

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_NOT_FILE;
		kPacket << rszGUID;
		kPacket << 0;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	m_iOpenVersion = GetCS3FileVersion( m_hFile );
	if( iVersion != m_iOpenVersion )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::OpenMapFile Version is Wrong.%d:%d:%s", iVersion, m_iOpenVersion, rszGUID.c_str() );
		CloseHandle( m_hFile );
		m_hFile = NULL; 
		m_dwReadSize = 0;

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_WRONG_VERSION;
		kPacket << rszGUID;
		kPacket << m_iOpenVersion;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::OpenMapFile Complete :%d:%s[%d:%d:%d]", m_hFile, rszGUID.c_str(), m_iOpenVersion, m_iChangeArray, iChange );

	m_MontoringGUID = rszGUID;
}

void ioXtrap::DivideLoadCS3File()
{
	if( !m_bUse )
		return;

	if( m_hFile == NULL )
		return;

	FUNCTION_TIME_CHECKER( 100000.0f, 0 );          // 0.1 초 이상 걸리면로그 남김

	if( !COMPARE( m_dwReadSize + SIZE_ONE_READ, SIZE_ONE_READ, MAX_CS3FILE_DATA_SIZE + 1 ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::DivideLoadCS3File Wrong Size :%d", m_dwReadSize );
		CloseHandle( m_hFile );
		m_hFile = NULL; 
		m_dwReadSize = 0;

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_FAIL;
		kPacket << m_MontoringGUID;
		kPacket << m_iOpenVersion;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	DWORD dwReaded = 0;
	if( !ReadFile( m_hFile, &m_byTempCS3Data[m_dwReadSize], SIZE_ONE_READ, &dwReaded , NULL ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::DivideLoadCS3File Size Error.%d:%d:%d", GetLastError(), m_hFile, m_dwReadSize );
		CloseHandle( m_hFile );
		m_hFile = NULL; 
		m_dwReadSize = 0;

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_FAIL;
		kPacket << m_MontoringGUID;
		kPacket << m_iOpenVersion;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	if( dwReaded != SIZE_ONE_READ )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::DivideLoadCS3File Size Error.%d:%d:%d:%d", GetLastError() , m_hFile, m_dwReadSize, dwReaded );
		CloseHandle( m_hFile );
		m_hFile = NULL; 
		m_dwReadSize = 0;

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_FAIL;
		kPacket << m_MontoringGUID;
		kPacket << m_iOpenVersion;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	m_dwReadSize += dwReaded;
	if( m_dwReadSize != MAX_CS3FILE_DATA_SIZE ) 
		return;

	// 완료
	CloseHandle( m_hFile );

	int  iOldVersionArray = 0;
	if( m_iChangeArray == -1 )
	{
		int  iOldVersion      = m_iCS3FileVersion[0];
		for (int i = 1; i < MAX_CS3FILE_COUNT ; i++)
		{
			if( m_iCS3FileVersion[i] < iOldVersion )
			{
				iOldVersionArray = i;
				iOldVersion = m_iCS3FileVersion[i];
			}
		}
	}
	else
	{
		iOldVersionArray = m_iChangeArray;
		m_iChangeArray = -1; // 초기화
	}

	if( !COMPARE( iOldVersionArray, 0, MAX_CS3FILE_COUNT ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::DivideLoadCS3File Wrong Array :%d", iOldVersionArray );
		CloseHandle( m_hFile );
		m_hFile = NULL; 
		m_dwReadSize = 0;

		SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
		kPacket << LOAD_CS3_FILE_FAIL;
		kPacket << m_MontoringGUID;
		kPacket << m_iOpenVersion;
		g_MainServer.SendMessage( kPacket );
		return;
	}

	m_iLastDeletVersion = m_iCS3FileVersion[iOldVersionArray];
	m_iCS3FileVersion[iOldVersionArray] = m_iOpenVersion;
	EnterCriticalSection(&m_CriticalSection);
	memcpy( m_byCS3Data[iOldVersionArray], m_byTempCS3Data, MAX_CS3FILE_DATA_SIZE );
	LeaveCriticalSection(&m_CriticalSection);

	m_hFile      = NULL; 
	m_dwReadSize = 0;

	SP2Packet kPacket( MSTPK_LOAD_CS3_FILE_RESULT );
	kPacket << LOAD_CS3_FILE_SUCCESS;
	kPacket << m_MontoringGUID;
	kPacket << m_iOpenVersion;
	g_MainServer.SendMessage( kPacket );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::DivideLoadCS3File Complete. %d:%s:Cur:%d:Pre:%d",iOldVersionArray, m_MontoringGUID.c_str(), m_iOpenVersion, m_iLastDeletVersion );

	SafeRename( iOldVersionArray );
}

int ioXtrap::GetCS3FileVersion( HANDLE hFile )
{
	if( !m_bUse )
		return -1;

	if( hFile == NULL )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::GetCS3FileVersion m_hFile == NULL" );
		return -1;
	}

	LONG dwTemp = 0;
	enum { CS3_DATA_SIZE = 13000, };
	DWORD dwReturn = SetFilePointer( hFile, CS3_DATA_SIZE, &dwTemp, FILE_BEGIN );
	if( dwReturn == INVALID_SET_FILE_POINTER )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::GetCS3FileVersion Moving Point Fail. %d", GetLastError() );
		return -1;
	}

	DWORD dwReaded = 0;
	int iReadVersion = 0;
	if( !ReadFile( hFile, &iReadVersion, sizeof( iReadVersion ), &dwReaded , NULL ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::GetCS3FileVersion Read Fail.%d:%d", GetLastError(), dwReaded );
		return -1;
	}

	dwTemp = 0;
	dwReturn = SetFilePointer( hFile, 0, &dwTemp, FILE_BEGIN );
	if( dwReturn == INVALID_SET_FILE_POINTER )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::GetCS3FileVersion Rewind Point Fail. %d", GetLastError() );
		return -1;
	}

	return iReadVersion;
}

bool ioXtrap::LoadOneCS3File( const char *szPath, int iArray )
{
	if( !COMPARE( iArray, 0, MAX_CS3FILE_COUNT ) )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadOneCS3File Array Fail. %d", iArray );
		return false;
	}

	HANDLE hFile = CreateFile( szPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadOneCS3File CreateFile Fail. %d:%d:%s", GetLastError(), iArray, szPath );
		return false;
	}

	int iVersion = GetCS3FileVersion( hFile );
	if( iVersion <= 0 )
	{
		//MessageBox( NULL, "Wrong CS3 File.", "IOEnter", MB_OK );
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadOneCS3File Wrong CS3 File. %d:%d[%d]", GetLastError(), iArray, iVersion );
		return false;
	}

	DWORD dwPos = 0;
	int iSize = MAX_CS3FILE_DATA_SIZE / SIZE_ONE_READ;
	for (int i = 0; i <  iSize; i++)
	{
		DWORD dwReaded = 0;
		if( !ReadFile( hFile, &m_byCS3Data[iArray][dwPos], SIZE_ONE_READ, &dwReaded , NULL ) )
		{
			CloseHandle( hFile );
			//MessageBox( NULL, "Fail Read CS3 File.(2)", "IOEnter", MB_OK );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadOneCS3File ReadFile Fail.%d:%d:%d", GetLastError(), iArray, iVersion );
			return false;
		}

		if( dwReaded != SIZE_ONE_READ )
		{
			CloseHandle( hFile );
			//MessageBox( NULL, "Fail Read CS3 File.(3)", "IOEnter", MB_OK );
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::LoadOneCS3File Size Error.%d:%d:%d", GetLastError(), iArray, iVersion );
			return false;
		}
		dwPos += dwReaded;
	}
	CloseHandle( hFile );
	m_iCS3FileVersion[iArray] = iVersion;

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Xtrap CS3 Load Complete on Start. %d[%d]", iArray, iVersion );

	return true;
}

void ioXtrap::SafeRename( int iMapFileArray )
{
	char szPath[MAX_PATH]="";
	StringCbPrintf( szPath, sizeof( szPath ), "Xtrap\\ls%d.cs3", iMapFileArray+1 );

	if( GetFileAttributes( szPath ) != INVALID_FILE_ATTRIBUTES ) // 파일 있다면
	{
		SetFileAttributes( szPath, FILE_ATTRIBUTE_ARCHIVE ) ; 
		if( DeleteFile( szPath ) == FALSE )
		{
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::SafeRename Fail DeleteFile :%s:%d", szPath, GetLastError() );
			return;
		}
	}

	if( MoveFile( CS_FILE_PATH, szPath ) == FALSE )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::SafeRename Fail MoveFile :%s:%d", szPath, GetLastError() );
		return;
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::SafeRename Success :%s", szPath );
}

void ioXtrap::SendCS3Version( const ioHashString &rszGUID )
{
	SP2Packet kPacket( MSTPK_CS3_FILE_VERSION_RESULT );
	kPacket << rszGUID;
	char szLogAll[MAX_PATH]="";
	for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
	{
		kPacket << m_iCS3FileVersion[i];
		char szLog[MAX_PATH]="";
		StringCbPrintf( szLog, sizeof( szLog ), "%d,", m_iCS3FileVersion[i] );
		StringCbCat( szLogAll, sizeof( szLogAll ), szLog );
	}

	kPacket << m_iLastDeletVersion;
	g_MainServer.SendMessage( kPacket );

	char szLog[MAX_PATH]="";
	StringCbPrintf( szLog, sizeof( szLog ), "(%d)", m_iLastDeletVersion );
	StringCbCat( szLogAll, sizeof( szLogAll ), szLog );

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioXtrap::SendCS3Version : %s:%s", rszGUID.c_str(), szLogAll );
}

void ioXtrap::GetTextCS3Version( OUT char *szText, IN int iTextSize )
{
	StringCbCopy( szText, iTextSize, "XTRAP CS3 VERSION ");
	for (int i = 0; i < MAX_CS3FILE_COUNT ; i++)
	{
		char szLog[MAX_PATH]="";
		StringCbPrintf( szLog, sizeof( szLog ), ":%d", m_iCS3FileVersion[i] );
		StringCbCat( szText, iTextSize, szLog );
	}
}

#endif