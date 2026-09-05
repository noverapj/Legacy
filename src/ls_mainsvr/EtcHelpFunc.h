

#ifndef _EtcHelpFunc_h_
#define _EtcHelpFunc_h_

namespace Help
{
	DWORD ConvertCTimeToDayCount( const CTime &rkTime );
	DWORD ConvertCTimeToDate( const CTime &rkTime );
	CTime ConvertDateToCTime( DWORD dwTime );
	SYSTEMTIME GetSafeValueForCTimeConstructor(SHORT iYear, SHORT iMonth,  SHORT iDay, SHORT iHour,  SHORT iMinute,  SHORT iSecond);
	void GetSafeTextWriteDB( OUT std::string &rResultString , const ioHashString &rSourceString );
	DWORD GetStringIPToDWORDIP( const char *szIP );
	bool GetLocalIpAddressList( OUT ioHashStringVec &rvIPList );

	int TournamentCurrentRoundWithTeam( int iMaxTeam, int iCurrentRound );
}

#endif