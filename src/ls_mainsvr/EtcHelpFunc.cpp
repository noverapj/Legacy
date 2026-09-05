

#include "stdafx.h"

#include "EtcHelpFunc.h"
#include <WinINet.h>
#include <strsafe.h>

namespace Help
{

DWORD ConvertCTimeToDayCount( const CTime &rkTime )
{
	DWORD dwDay = rkTime.GetYear() * 10000 +
				  rkTime.GetMonth() * 100 +
				  rkTime.GetDay();

	return dwDay;
}

DWORD ConvertCTimeToDate( const CTime &rkTime )
{
	DWORD dwDate = ( rkTime.GetYear() % 100 ) * 100000000 +
		           rkTime.GetMonth() * 1000000 + 
				   rkTime.GetDay() * 10000 +
				   rkTime.GetHour() * 100 +
				   rkTime.GetMinute();
	return dwDate;
}

CTime ConvertDateToCTime( DWORD dwTime )
{
	int iYear	= 2000 + ( dwTime / 100000000 );
	int iMonth	= ( dwTime % 100000000 ) / 1000000;
	int iDay	= ( dwTime % 1000000 ) / 10000;
	int iHour	= ( dwTime % 10000 ) / 100;
	int iMinute	= ( dwTime % 100 );

	CTime kTime( GetSafeValueForCTimeConstructor( iYear, iMonth, iDay, iHour, iMinute, 0 ) );

	return kTime;
}

SYSTEMTIME GetSafeValueForCTimeConstructor( SHORT iYear, SHORT iMonth,  SHORT iDay, SHORT iHour,  SHORT iMinute,  SHORT iSecond )
{
	SYSTEMTIME returnSt={0,0,0,0,0,0,0,0};

	if( !COMPARE(iYear, 1971, 3001) )
		returnSt.wYear = 1971;
	else
		returnSt.wYear = iYear;

	if( !COMPARE(iMonth, 1, 13) )
		returnSt.wMonth = 1;
	else
		returnSt.wMonth = iMonth;

	if( !COMPARE(iDay, 1, 32) )
		returnSt.wDay = 1;
	else
		returnSt.wDay = iDay;

	if( !COMPARE(iHour, 0, 25) )
		returnSt.wHour = 0;
	else
		returnSt.wHour = iHour;

	if( !COMPARE(iMinute, 0, 61) )
		returnSt.wMinute = 0;
	else
		returnSt.wMinute = iMinute;

	if( !COMPARE(iSecond, 0, 61) )
		returnSt.wSecond = 0;
	else
		returnSt.wSecond = iSecond;

	return returnSt;
}

void GetSafeTextWriteDB( OUT std::string &rResultString , const ioHashString &rSourceString )
{
	char ch = NULL;
	bool bFirstLeadByte = false;
	for (int i = 0; i < rSourceString.Length() ; i++)
	{
		ch = rSourceString.At(i);
		if( IsDBCSLeadByte( ch ) )
		{
			if( !bFirstLeadByte )
				bFirstLeadByte = true;
			else
				bFirstLeadByte = false;
		}
		else
			bFirstLeadByte = false;

		if( !bFirstLeadByte )
		{
			if( ch == '\'' )
				rResultString += "''";
			else if( ch == '<' )
				rResultString += "&lt;";
			else if( ch == '>' )
				rResultString += "&gt;";
			else if( ch == '\n' )
				rResultString += "<br>";
			else
				rResultString += ch;
		}
		else
			rResultString += ch;
	}
}

DWORD GetStringIPToDWORDIP( const char *szIP )
{
	int  count       = 0;
	int  cut_ip		 = 0;
	char szCut_ip[4][4];
	memset( szCut_ip, 0, sizeof( szCut_ip ) );
	int  len	     = strlen( szIP );
	for(int i = 0;i < len;i++)
	{
		if( szIP[i] == '.')
		{
			count = 0;
			cut_ip++;
		}
		else
			szCut_ip[cut_ip][count++] = szIP[i];
	}
	return (DWORD)(atoi(szCut_ip[0])<<24) | (DWORD)(atoi(szCut_ip[1])<<16) | (DWORD)(atoi(szCut_ip[2])<<8) | atoi(szCut_ip[3]);	
}

// WSAStartup() 호출 이후에 호출해야함
bool GetLocalIpAddressList( OUT ioHashStringVec &rvIPList )
{
	char szHostName[MAX_PATH];
	ZeroMemory( szHostName, sizeof( szHostName ) );
	gethostname(szHostName, sizeof(szHostName));

	LPHOSTENT lpstHostent = gethostbyname(szHostName);
	if ( !lpstHostent ) 
	{
		LOG.PrintTimeAndLog(0,"%s lpstHostend == NULL.", __FUNCTION__ );
		return false;
	}

	enum { MAX_LOOP = 100, };
	LPIN_ADDR lpstInAddr = NULL;
	if( lpstHostent->h_addrtype == AF_INET )
	{
		for (int i = 0; i < MAX_LOOP ; i++) // 100개까지 NIC 확인
		{
			lpstInAddr = (LPIN_ADDR)* lpstHostent->h_addr_list;

			if( lpstInAddr == NULL )
				break;

			char szTemp[MAX_PATH]="";
			StringCbCopy( szTemp, sizeof( szTemp ), inet_ntoa(*lpstInAddr) );
			ioHashString sTemp = szTemp;
			rvIPList.push_back( sTemp );			

			lpstHostent->h_addr_list++;
		}
	}

	if( rvIPList.empty() )
	{
		LOG.PrintTimeAndLog(0,"%s Local IP empty.", __FUNCTION__ );
		return false;
	}

	return true;
}

int TournamentCurrentRoundWithTeam( int iMaxTeam, int iCurrentRound )
{
	for(int i = 0;i < iCurrentRound;i++)
	{
		iMaxTeam /= 2;
	}
	return iMaxTeam;
}

}	// namespace



