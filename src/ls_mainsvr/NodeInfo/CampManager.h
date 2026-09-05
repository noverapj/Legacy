#ifndef _CampManager_h_
#define _CampManager_h_

class ServerNode;
class CampManager : public SuperParent
{
protected:
	static CampManager *sg_Instance;

public:
	enum SeasonState
	{
		SS_NONE			= 0,
		SS_PLAY_DELAY	= 1,
		SS_PLAY_PROCEED = 2,
		SS_RECESS		= 3,
	};

	enum RecessState
	{
		RECESS_NONE			= 0,
		RECESS_FIRST_BACKUP = 1,
		RECESS_RESULT		= 2,
		RECESS_LAST_BACKUP  = 3,
		RECESS_INIT			= 4,
	};

protected:     // 시즌 시간
	struct SeasonDate
	{
		// 시즌 종료된 시간
		DWORD m_dwLastSeasonDate;

		// 다음 체크 시간
		DWORD m_dwNextSeasonDate;

		// 시즌 시간
		WORD m_wPeriodTime;

		// 시즌 휴식
		WORD m_wRecessTime;

		// 시즌중 일일 플레이 시작 시간
		WORD m_wPlayStartHour;

		// 시즌중 일일 플레이 시간
		WORD m_wPlayPreceedTime;

		SeasonDate()
		{
			m_dwLastSeasonDate = m_dwNextSeasonDate = 0;
			m_wPeriodTime = m_wRecessTime = 0;
			m_wPlayStartHour = m_wPlayPreceedTime = 0;
		}

		void SetLastSeasonTime( const CTime &rkTime )
		{
			m_dwLastSeasonDate = ( rkTime.GetYear() * 1000000 ) + ( rkTime.GetMonth() * 10000 ) + ( rkTime.GetDay() * 100 ) + rkTime.GetHour();
		}

		CTime GetLastSeasonTime()
		{
			DWORD dwYear = m_dwLastSeasonDate / 1000000;
			DWORD dwMonth= ( m_dwLastSeasonDate % 1000000 ) / 10000;
			DWORD dwDay  = ( m_dwLastSeasonDate % 10000 ) / 100;
			DWORD dwHour = m_dwLastSeasonDate % 100;
			return CTime( dwYear, dwMonth, dwDay, dwHour, 0, 0 );
		}

		void SetNextSeasonTime( const CTime &rkTime )
		{
			m_dwNextSeasonDate = ( rkTime.GetYear() * 1000000 ) + ( rkTime.GetMonth() * 10000 ) + ( rkTime.GetDay() * 100 ) + rkTime.GetHour();
		}

		CTime GetNextSeasonTime()
 		{
			DWORD dwYear = m_dwNextSeasonDate / 1000000;
			DWORD dwMonth= ( m_dwNextSeasonDate % 1000000 ) / 10000;
			DWORD dwDay  = ( m_dwNextSeasonDate % 10000 ) / 100;
			DWORD dwHour = m_dwNextSeasonDate % 100;
			return CTime( dwYear, dwMonth, dwDay, dwHour, 0, 0 );
		}
	};
	SeasonDate  m_SeasonDate;
	SeasonState m_SeasonState;
	RecessState m_RecessState;

protected:
	struct CampData
	{
		int          m_iCampType;						// 진영 타입
		ioHashString m_szCampName;						// 진영 이름
		int          m_iCampPoint;						// 진영 포인트
		int          m_iCampTodayPoint;					// 금일 획득한 진영 포인트
		int          m_iCampBonusPoint;                 // 오늘의 승부로 획득한 포인트
		int          m_iCampEntryUserCount;				// 진영에 소속된 유저 수
		int          m_iCampSpecialEntryUserCount;		// 진영에 소속된 유저 중 N포인트 이상 획득한 유저

		// BackUP
		int          m_iBackCampPoint;
		int          m_iBackCampTodayPoint;
		int          m_iBackCampBonusPoint;
		CampData()
		{
			m_iCampType = m_iCampPoint = m_iCampEntryUserCount = 0;
			m_iCampSpecialEntryUserCount = m_iCampTodayPoint = m_iCampBonusPoint = 0;

			m_iBackCampPoint = m_iBackCampTodayPoint = m_iBackCampBonusPoint = 0;
		}

		bool IsChange()
		{
			if( m_iCampPoint != m_iBackCampPoint )
				return true;
			else if( m_iCampTodayPoint != m_iBackCampTodayPoint )
				return true;
			else if( m_iCampBonusPoint != m_iBackCampBonusPoint )
				return true;
			return false;
		}

		void BackUP()
		{
			m_iBackCampPoint = m_iCampPoint;
			m_iBackCampTodayPoint = m_iCampTodayPoint;
			m_iBackCampBonusPoint = m_iCampBonusPoint;
		}
	};
	typedef std::vector< CampData > vCampData;
	vCampData m_CampData;

protected:
	struct AlarmData
	{
		int      m_iCheckArray;
		FloatVec m_fCheckList; 
		
		AlarmData()
		{
			m_iCheckArray = 0;
		}

		bool IsChange( float fBlueInfluence )
		{	
			for(int i = 0;i < (int)m_fCheckList.size();i++)
			{
				float &rkInfluence = m_fCheckList[i];
				if( fBlueInfluence <= rkInfluence )
				{
					if( m_iCheckArray != i )
					{
						m_iCheckArray = i;
						return true;
					}
					return false;
				}
			}
			return false;
		}
	};
	AlarmData m_Alarm;

protected:
	char  m_szStringHelp[MAX_PATH];
	DWORD m_dwProcessTime;
	DWORD m_dwRecessTime;

protected:
	CampManager::CampData &GetCampData( CampType eCampType );
	int GetCampPoint( CampType eCampType );
	int GetCampBonusPoint( CampType eCampType );
	DWORD GetCurrentPlayState();
	int  GetNextTodayBattleSec();
	void SeasonStart( const CTime &rkNextTime );
	void SeasonEnd();
	void SeasonTodayPointEnd();
	void ProcessSeason( SeasonState eSeasonState );

public:
	void SeasonResult( bool bServerClose = false );
	void SeasonPrepare( bool bServerClose = false );

public:
	void LoadINIData();
	void LoadSeasonDateINI( bool bReload = false );
	void SaveSeasonDateINI();

public:
	void DBToCampData( int iBlueCampPoint, int iBlueCampTodayPoint, int iBlueCampBonusPoint, int iRedCampPoint, int iRedCampTodayPoint, int iRedCampBonusPoint );
	void DBToCampSpecialUserCount( int iCampType, int iCampSpecialEntry );
	void UpdateCampDataDB();

public:
	char *GetCampStringHelp();
	void UpdateCampStringHelp();

public:
	bool IsCampBattlePlay(){ return ( m_SeasonState == SS_PLAY_PROCEED ); }
	bool IsCampBattleRecess(){ return ( m_SeasonState == SS_RECESS ); }

public:
	void SendCampDataSync( ServerNode *pServerNode, SP2Packet &rkPacket );
	void SendCampRoomBattleInfo( ServerNode *pServerNode, SP2Packet &rkPacket );

public:
	void ChangeCampEntryCount( SP2Packet &rkPacket );
	void OnLadderModeResultUpdate( SP2Packet &rkPacket );

public:
	void ProcessCamp();	

public:
	void ProcessServerClose();

public:
		DWORD GetActiveCampDate()	{ return m_SeasonDate.m_dwLastSeasonDate; }

public:
	static CampManager &GetInstance();
	static void ReleaseInstance();

private:     	/* Singleton Class */
	CampManager();
	virtual ~CampManager();
};
#define g_CampMgr CampManager::GetInstance()
#endif