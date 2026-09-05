

#ifndef _EtcHelpFunc_h_
#define _EtcHelpFunc_h_

namespace Help
{
	void InitHelpConstant();

	int GetFirstConnectBonusPeso();

	int GetGradeUPBonus();
	int GetLevelUPBonus();
	int GetCreateGuildPeso();
	int GetChangeGuildMarkPeso();
	int GetGuildPointCorrection();
	int GetFriendBonusBeforeDay();
	int GetFriendBonusBeforeHour();
	int GetFriendBonusBeforeMin();
	float GetFriendBonus();
	float GetMaxFriendBonus();
	float GetPcRoomFriendBonus();	
	float GetPcRoomMaxFriendBonus();
	bool  IsRoomFriendCnt();
	float GetPCRoomBonusExp();
	float GetPCRoomBonusPeso();
	float GetLadderBonus();
	float GetModeConsecutivelyBonus();
	float GetModeConsecutivelyMaxBonus();
	int   GetLadderTeamLimitGrade();
	int   GetLimitCampPoint();
	float GetCampBonusCorrectionA();
	float GetCampBonusCorrectionB();
	float GetCampBonusDefaultA();
	float GetCampBonusDefaultB();
	int   GetCampBonusMinEntry();
	int   GetCampBonusMaxEntry();
	int   GetHeroTop100SyncHour();
	int   GetHeroUserDataSyncHour();

	int GetFriendDefaultSlotSize();

	float GetPesoExpRate();
	float GetSellReturnRate();

	float GetContributePerRate();
	float GetKillDeathLevelRate();

	int GetFirstVictories();
	float GetFirstVictoriesRate();
	float GetVictoriesRate();
	float GetMaxVictoriesRate();

	DWORD ConvertCTimeToDayCount( const CTime &rkTime );
	DWORD ConvertCTimeToDate( const CTime &rkTime );
	DWORD ConvertCTimeToYearMonthDay( const CTime &rkTime );
	DWORD ConvertCTimeToHourMinute( const CTime &rkTime );
	DWORD CheckDiffTimeToTime( DWORD dwRemainMinute, CTime &rkCurTime, CTime &rkBeforeTime );
	DWORD ConvertYYMMDDHHMMToDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute );
	DWORD ConvertYYMMDDToDate( WORD wYear, WORD wMonth, WORD wDay );
	DWORD ConvertYYMMToDate( WORD wYear, WORD wMonth );
	CTime ConvertDateToCTime( DWORD dwDate, bool bTenBase = true );
	CTime ConvertNumberToCTime( const int iDateTime );
	SYSTEMTIME GetSysTimeForDWORD( DWORD dwDate );

	int GetEquipSlot( int iItemCode );

	SYSTEMTIME GetSafeValueForCTimeConstructor(SHORT iYear, SHORT iMonth,  SHORT iDay, SHORT iHour,  SHORT iMinute,  SHORT iSecond);

	int GetTutorialBonusStep();
	int GetTutorialBonusPeso();
	int GetTutorialBonusEventPeso();

	void GetSafeTextWriteDB( OUT std::string &rResultString , const ioHashString &rSourceString );
	ioHashString GetTeamTypeString( TeamType eTeamType );

	int GetRefillCoinCount();
	int GetRefillCoinMax();
	int GetRefillCoinSec();

	void GetGUID(OUT char *szGUID, IN int iSize);

	DWORD GetStringIPToDWORDIP( const char *szIP );
	bool  IsPCRoomBonus();
	bool  IsSameZoneIP( DWORD dwIP_A, DWORD dwIP_B );

	struct SoldierBonus 
	{
		int   iPossessionCnt;
		float fBonusPer;
		SoldierBonus()
		{
			iPossessionCnt = 0;
			fBonusPer = 0.0f;
		}
	};
	typedef std::vector< SoldierBonus > vSoldierBonus;
	float GetSoldierPossessionBonus( int iPossessionCnt );

	ioHashString GetGuildMarkBlockURL();
	bool CallURL( const char *szCallURL ); //URL만 호출한다.

	bool IsStringCheck( char *szFullText, char *szFindText, char cSection );
	bool IsStringCheck( char *szFullText, int iFindNumber, char cSection );
	void SplitString( char *szFullText, IntVec &rkReturn, char cSection );

	void SetNagleAlgorithm( bool bOn );
	bool IsNagleAlgorithm();

	void SetPlazaNagleAlgorithm( bool bOn );
	bool IsPlazaNagleAlgorithm();

	void SetCharChangeToUDP( bool bUDP );
	bool IsCharChangeToUDP();

	void SetOnlyServerRelay( bool bRelay );
	bool IsOnlyServerRelay();

	void SetWholeChatOn( bool bOn );
	bool IsWholeChatOn();

	void SetServerRelayToTCP( bool bTCP );
	bool IsServerRelayToTCP();

	int GetAwardEtcItemBonus();
	int GetAwardEtcItemBonusMent();
	int GetAwardEtcItemBonusAbusePoint();

	int GetDefaultBestFriendCount();
	int GetBestFriendDismissDelayHour();
	int GetCharRentalMinute();
	int GetBestFriendCharRentalDelayHour();
	int GetCharRentalCount();
	int GetCharRentalGrade();
	int GetMaxCharRentSet();

	DWORD GetQuestAbuseSecond();
	bool  IsQuestAbuseDeveloperPass();
	bool  IsQuestAbuseLogOut();

	int GetCloverMaxCount();
	int GetCloverDefaultCount();
	int GetGiveCloverTimeMinute();
	int GetCloverChargeCount();
	int GetCloverChargeTimeMinute();
	int GetCloverSendCount();
	int GetCloverReceiveCount();
	int GetCloverSendTimeMinute();
	int GetCloverReceiveTimeMinute();
	int GetCloverRefill_Hour();
	int GetCloverRefill_Min();
	bool GetCloverRefillTest();
	int GetCloverRefillCycle();

	DWORD GetUserDBAgentID( const ioHashString &rPrivateID );

	bool GetLocalIpAddressList( OUT ioHashStringVec &rvIPList, IN bool bMessageBox );

	// Monster Dungeon
	bool IsMonsterDungeonMode(ModeType eMode);

	bool IsSpawnNpc();
	void SetSpawnNpc(bool bSpawn);

	int ToGetDay( int iYear, int iMonth );
	bool IsAvailableDate( int iYear, int iMonth, int iDay );

	//소수점 반올림
	float RoundOff(float fVal);

	enum PeriodType
	{
		PT_DAY,
		PT_HOUR,
		PT_MIN,
	};
	int GetDatePeriod( int iStartYear,
		int iStartMonth,
		int iStartDay,
		int iStartHour,
		int iStartMin,
		int iEndYear,
		int iEndMonth,
		int iEndDay,
		int iEndHour,
		int iEndMin,
		PeriodType eType=PT_DAY);

	//Token
	void TokenizeToINT(const string& str, const string& delimiters, IntVec& vToken);
	void TokenizeToSTRING(const string& str, const string& delimiters, std::vector<std::string>& vToken);

	DWORD ConvertDBTIMESTAMPToDWORD(DBTIMESTAMP& RecvDate);


	// 레이드티켓
	int GetRefillRaidTicketCount();
	int GetRefillRaidTicketMax();
	int GetRefillRaidTicketTime();
	int GetDefaultRaidTicketCnt();

}

#endif
