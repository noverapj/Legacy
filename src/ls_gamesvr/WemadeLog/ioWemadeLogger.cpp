#include "stdafx.h"
#include "WemadeLog.h"
#include "ioWemadeLogger.h"

CLog WemadeLOG;

ioWemadeLogger::ioWemadeLogger() : m_hSocket(INVALID_SOCKET), m_iLogType(0), m_bEnable(TRUE)
{
	Init();
}

ioWemadeLogger::~ioWemadeLogger()
{
	Destroy();
}

void ioWemadeLogger::Init()
{
	m_vServers.clear();

	m_Buffer.Create( 1024 * 32 );

	m_iDelimiter = 1;
	_tcscpy_s( m_szDelimiter, _T("\t") );

	ZeroMemory(&m_LogServer, sizeof(m_LogServer));
}

void ioWemadeLogger::Destroy()
{
	Close();
}

void ioWemadeLogger::Register(const TCHAR* szServerIP)
{
	m_vServers.push_back(szServerIP);
}

BOOL ioWemadeLogger::Create()
{
	if(m_vServers.size() > 0 )
	{
		m_hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (INVALID_SOCKET == m_hSocket ) 
		{
			return FALSE;
		}

		int iIndex = rand() % m_vServers.size();
		WemadeLOG.PrintTimeAndLog( 0, "ioWemadeLogger::Create - %s", m_vServers[iIndex].c_str() );

		m_LogServer.sin_addr.s_addr = inet_addr(m_vServers[iIndex].c_str());
	}
	else
	{
		m_bEnable = FALSE;
		WemadeLOG.PrintTimeAndLog( 0, "ioWemadeLogger::Create - Empty" );	
	}

	return TRUE;
 }

void ioWemadeLogger::Close()
{
	if (INVALID_SOCKET != m_hSocket ) 
	{
		closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}
}

BOOL ioWemadeLogger::Begin(const int iLogType)
{
	m_iLogType = iLogType;

	if(m_iLogType != 0)
	{
		TCHAR szLogType[64];
		int iLength = _stprintf_s(szLogType, sizeof(szLogType), _T("%d"), iLogType);

		m_Buffer.Erase();
		m_Buffer.Append(szLogType, iLength);
		return TRUE;
	}
	return FALSE;
}

BOOL ioWemadeLogger::Write(const TCHAR* szData)
{
	if(!IsEnable())	return FALSE;

	if(!m_Buffer.Append( m_szDelimiter, m_iDelimiter ))		return FALSE;
	if(!m_Buffer.Append( szData, strlen(szData) ))			return FALSE;
	return TRUE;
}

BOOL ioWemadeLogger::Write(const int32 nData)
{
	if(!IsEnable())	return FALSE;

	TCHAR szData[64];
	int iLength = _stprintf_s(szData, sizeof(szData), _T("%d"), nData);

	if(!m_Buffer.Append( m_szDelimiter, m_iDelimiter ))		return FALSE;
	if(!m_Buffer.Append( szData, iLength ))					return FALSE;
	return TRUE;
}

BOOL ioWemadeLogger::Write(const uint32 nData)
{
	if(!IsEnable())	return FALSE;

	TCHAR szData[64];
	int iLength = _stprintf_s(szData, sizeof(szData), _T("%I32u"), nData);

	if(!m_Buffer.Append( m_szDelimiter, m_iDelimiter ))		return FALSE;
	if(!m_Buffer.Append( szData, iLength ))					return FALSE;
	return TRUE;
}

BOOL ioWemadeLogger::Write(const int64 nData)
{
	if(!IsEnable())	return FALSE;

	TCHAR szData[64];
	int iLength = _stprintf_s(szData, sizeof(szData), _T("%I64d"), nData);

	if(!m_Buffer.Append( m_szDelimiter, m_iDelimiter ))		return FALSE;
	if(!m_Buffer.Append( szData, iLength ))					return FALSE;
	return TRUE;
}

BOOL ioWemadeLogger::Write(const uint64 nData)
{
	if(!IsEnable())	return FALSE;

	TCHAR szData[64];
	int iLength = _stprintf_s(szData, sizeof(szData), _T("%I64u"), nData);

	if(!m_Buffer.Append( m_szDelimiter, m_iDelimiter ))		return FALSE;
	if(!m_Buffer.Append( szData, iLength ))					return FALSE;
	return TRUE;
}

BOOL ioWemadeLogger::Write(const double nData)
{
	if(!IsEnable())	return FALSE;

	TCHAR szData[64];
	int iLength = _stprintf_s(szData, sizeof(szData), _T("%lf"), nData);

	if(!m_Buffer.Append( m_szDelimiter, m_iDelimiter ))		return FALSE;
	if(!m_Buffer.Append( szData, iLength ))					return FALSE;
	return TRUE;
}

void ioWemadeLogger::End()
{
	if( IsEnable() && (m_Buffer.GetLength() > 0) )
	{
		// 전송준비
		Prepare();

		int iResult = sendto(
			m_hSocket,
			m_Buffer.GetString(), 
			m_Buffer.GetLength(), 
			0, 
			(SOCKADDR*)&m_LogServer, 
			sizeof(m_LogServer));

		BOOL bResult = TRUE;
		if(SOCKET_ERROR == iResult)
		{
			bResult = FALSE;
			Close();
			Create();
		}

		Debug( _T("WemadeLog[%u] : %s\r\n"), ntohs(m_LogServer.sin_port), m_Buffer.GetString() );
		//WemadeLOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[%lu] Result(%d) %s", ntohs(m_LogServer.sin_port), bResult, m_Buffer.GetString() );
	}
}

void ioWemadeLogger::Prepare()
{
	SYSTEMTIME systime;
	GetLocalTime( &systime );

	TCHAR szData[64];
	int iLength = _stprintf_s(szData, sizeof(szData), _T("%04d-%02d-%02d %02d:%02d:%02d"),
		systime.wYear,
		systime.wMonth,
		systime.wDay,
		systime.wHour,
		systime.wMinute,
		systime.wSecond);

	m_Buffer.Append( m_szDelimiter, m_iDelimiter );
	m_Buffer.Append( szData, iLength );
	m_Buffer.Append( _T(""), 1 );

	if (m_iLogType != 0)
	{
		int iPort = GetPort();

		m_LogServer.sin_family	= AF_INET;
		m_LogServer.sin_port	= htons(iPort);
	}
}

int ioWemadeLogger::GetPort()
{
	int iPort = 0;
	switch(m_iLogType)
	{
	case WE_LOG_CONCURRENT :
		iPort = 35101 + (rand() % 5);
		break;
	case WE_LOG_DATA_PCROOM :
	case WE_LOG_DATA_PIECE_OBTAIN :
	case WE_LOG_DATA_PIECE_MIX :
	case WE_LOG_DATA_PIECE_DIVIDE :
	case WE_LOG_BUY_ITEM_GOLD :
	case WE_LOG_DATA_MEDAL :
	case WE_LOG_DATA_MEDAL_EXTEND :
	case WE_LOG_DATA_TRADE :
	case WE_LOG_DATA_LOCALINFO :
	case WE_LOG_DATA_TUTORIAL :
		iPort = 35106 + (rand() % 5);
		break;

	case WE_LOG_BUY_ITEM_CLASS :
	case WE_LOG_BUY_ITEM_DECORATION :
	case WE_LOG_BUY_ITEM_EQUIP :
	case WE_LOG_DATA_LEAGUE_PRESENT :
	case WE_LOG_USE_ITEM :
		iPort = 35111 + (rand() % 5);
		break;

	case WE_LOG_BUY_ITEM_SPECIAL :
	case WE_LOG_DATA_CLOVER :
		iPort = 35116 + (rand() % 5);
		break;

	case WE_LOG_DATA_PLAY :
	case WE_LOG_DATA_TIME :
		iPort = 35121 + (rand() % 5);
		break;

	case WE_LOG_DATA_CHARACTER :
		iPort = 35126 + (rand() % 5);
		break;
	case WE_LOG_DATA_PESO :
		iPort = 35131 + (rand() % 5);
		break;
	case WE_LOG_DATA_QUEST :
		iPort = 35136 + (rand() % 5);
		break;
	case WE_LOG_PRESENT :
		iPort = 35141 + (rand() % 5);
		break;
	}
	return iPort;
}

BOOL ioWemadeLogger::IsEnable()
{
	if((m_bEnable == FALSE)	|| (m_iLogType == 0)) return FALSE;
	return TRUE;

}
