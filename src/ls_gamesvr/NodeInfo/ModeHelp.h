

#ifndef _ModeHelp_h_
#define _ModeHelp_h_

#include "../Define.h"

class User;

bool IsWinTeam( WinTeamType eWinTeam, TeamType eTeam );

bool IsRedWin( WinTeamType eWinTeam );
bool IsBlueWin( WinTeamType eWinTeam );

TeamType ConvertStringToTeamType( const char *szType );

struct RankInfo
{
	ioHashString szName;
	int iTotalKill;
	int iTotalDeath;
	float fContributePer;
	bool bAbuse;

	RankInfo()
	{
		iTotalKill     = 0;
		iTotalDeath    = 0;
		fContributePer = 1.0f;
		bAbuse         = false;
	}
};

struct KillDeathRankInfo
{
	ioHashString szName;
	int iKillDeathLevel;

	KillDeathRankInfo()
	{
		iKillDeathLevel = 0;
	}
};

typedef std::vector< KillDeathRankInfo > KillDeathRankList;

struct RoundHistory
{
	int iBluePoint;
	int iRedPoint;

	RoundHistory()
	{
		iBluePoint = 0;
		iRedPoint  = 0;
	}
};

typedef std::vector< RoundHistory > vRoundHistory;

struct PlayingTimeInfo
{
	int iIndex;
	DWORD dwPlayingTime;

	PlayingTimeInfo()
	{
		iIndex = 0;
		dwPlayingTime = 0;
	}
};

struct ClassPlayTimeInfo
{
	DWORD dwClassCode;
	DWORD dwPlayingTime;

	ClassPlayTimeInfo()
	{
		dwClassCode = 0;
		dwPlayingTime  = 0;
	}
};

typedef std::vector< ClassPlayTimeInfo > vClassPlayTimeInfo;

enum RecordState
{
	RS_LOADING,
	RS_PLAY,
	RS_VIEW,
	RS_OBSERVER,
	RS_DIE,
};

enum BonusArray // array로사용되므로 값은 순차적으로 증가 시킬것
{
	BA_SOLDIER_CNT = 0,
	BA_GUILD,
	BA_PCROOM_EXP,
	BA_PLAYMODE,
	BA_FRIEND,
	BA_EVENT,
	BA_EVENT_PESO,
	BA_ETC_ITEM,
	BA_CAMP_BONUS,
	BA_AWARD_BONUS,
	BA_ETC_ITEM_PESO,
	BA_ETC_ITEM_EXP,
	BA_VICTORIES_PESO,
	BA_HERO_TITLE_PESO,
	BA_MODE_CONSECUTIVELY,
	BA_PCROOM_PESO,
	BA_MAX,
};

typedef std::map< int, int > KillDeathInfoMap;

struct ModeRecord
{
	User *pUser;
	RecordState eState;
	
	DWORD dwEnterRoomTime;
	DWORD dwPlayingStartTime;
	DWORD dwPlayingTime;
	DWORD dwClassPlayingStartTime;	

	bool  bClassPlayHireTimeCheck;
	bool  bClassPlayTimeSort;
	vClassPlayTimeInfo vClassPlayTime;

	IntVec iResultClassTypeList;
	IntVec iResultClassPointList;

	bool  bResultLevelUP;

	int iTotalDamage;

	KillDeathInfoMap iKillInfoMap;
	KillDeathInfoMap iDeathInfoMap;

	int iTotalPeso;
	int iTotalAddPeso; // 결과시 실제로 더해진 페소 ( DB 로그 기록을 위해서 )
	int iTotalExp;
	int iTotalLadderPoint;

	int iPreRank;
	int iCurRank;
	int iRevivalCnt;

	int iContribute;
	float fContributePer;
	float fBonusArray[BA_MAX];

	int iUniqueTotalKill;
	int iUniqueTotalDeath;
	int iVictories;

	DWORD dwCurDieTime;
	DWORD dwRevivalGap;
	DWORD dwDeathTime;

	bool bRecvDamageList;
	bool bDieState;
	bool bCatchState;
	bool bChatModeState;
	bool bFishingState;
	bool bExperienceState;
	ioHashString szExperienceID;
	int  iExperienceClassType;
	bool bWriteLog;
	bool bBonusAlarmSend;

	// LastAttackerInfo
	TeamType eLastAttackerTeam;
	ioHashString szLastAttackerName;
	DWORD dwLastAttackerWeaponItemCode;

	int  GetAllTotalKill();
	int  GetAllTotalDeath();

	DWORD GetAllPlayingTime();
	DWORD GetCharHireCheckTime();

	void SetDieLastAttackerInfo( const ioHashString &szName, TeamType eTeam, DWORD dwItemCode );
	
	virtual void AddKillCount( RoomStyle eRoomStyle, ModeType eModeType, int iKill );
	virtual void AddDeathCount( RoomStyle eRoomStyle, ModeType eModeType, int iDeath );

	int GetKillCount( int iClassType );
	int GetDeathCount( int iClassType );

	void PlayTimeInit();
	void AddPlayingTime( DWORD addTime = 0 ); // dwPlayingStartTime = 0	
	void AddDeathTime( DWORD dwCurRoundDeathTime );
	void StartPlaying();

	ClassPlayTimeInfo &GetClassPlayTimeInfo( DWORD dwClassCode );
	void  ClassPlayTimeInfoSort();
	void  AddClassPlayingTime( int iPrevCharType = 0 ); // dwClassPlayingStartTime = 0
	DWORD GetHighPlayingTime( IN int iCount, OUT IntVec &rkClassTypeList,OUT DWORDVec &rkPlayTimeList );	    // 모드 종료시 플레이 시간 종료된 후 시간
	DWORD GetCurrentHighPlayingTime( IN int iPrevCharType, IN int iCount, OUT IntVec &rkClassTypeList,OUT DWORDVec &rkPlayTimeList );	// 모드 진행중 플레이 시간 진행중인 상태에서의 시간
	DWORD GetClassPlayingTime( DWORD dwClassCode );
	int   GetHighPlayingClass();
	DWORD GetDeathTime();

	void CheckLoadingTime();

	ModeRecord();
};

struct TeamChange
{
	ModeRecord *pRecord;
	DWORD dwTeamSettingTime;
};

typedef std::vector< TeamChange > TeamChangeList;

class TeamChangeSort
{
public:
	bool operator()( const TeamChange &lhs, const TeamChange &rhs ) const
	{
		if( lhs.dwTeamSettingTime <= rhs.dwTeamSettingTime )
			return true;

		return false;
	}
};

typedef std::vector< RankInfo > RankList;

class KillDeathLevelSort
{
public:
	bool operator()( const KillDeathRankInfo &lhs , const KillDeathRankInfo &rhs ) const
	{
		if( lhs.iKillDeathLevel > rhs.iKillDeathLevel )
		{
			return true;
		}
		//else if( lhs.iKillDeathLevel == rhs.iKillDeathLevel )
		//{
		//	int iCmpValue = _stricmp( lhs.szName.c_str(), rhs.szName.c_str() );
		//	if( iCmpValue < 0 )
		//		return true;
		//}

		return false;
	}
};

// Score > Kill > DeathMin
class RankInfoSort
{
public:
	bool operator()( const RankInfo &lhs , const RankInfo &rhs ) const
	{
		if( lhs.iTotalKill > rhs.iTotalKill )
		{
			return true;
		}
		else if( lhs.iTotalKill == rhs.iTotalKill )
		{
			if( lhs.iTotalDeath < rhs.iTotalDeath )
			{
				return true;
			}
			//else if( lhs.iTotalDeath == rhs.iTotalDeath )
			//{
			//	int iCmpValue = _stricmp( lhs.szName.c_str(), rhs.szName.c_str() );
			//	if( iCmpValue < 0 )
			//		return true;
			//}
		}

		return false;
	}
};

class FinalRankInfoSort
{
public:
	bool operator()( const RankInfo &lhs , const RankInfo &rhs ) const
	{
		if( !lhs.bAbuse && !rhs.bAbuse )
		{
			if( lhs.fContributePer > rhs.fContributePer )
				return true;

			if( lhs.fContributePer == rhs.fContributePer )
			{
				if( lhs.iTotalKill > rhs.iTotalKill )
					return true;

				if( lhs.iTotalKill == rhs.iTotalKill )
				{
					if( lhs.iTotalDeath < rhs.iTotalDeath )
						return true;

					//if( lhs.iTotalDeath == rhs.iTotalDeath )
					//{
					//	int iCmpValue = _stricmp( lhs.szName.c_str(), rhs.szName.c_str() );
					//	if( iCmpValue < 0 )
					//		return true;
					//}
				}
			}			
		}
		else if( lhs.bAbuse && rhs.bAbuse )
		{
			if( lhs.fContributePer > rhs.fContributePer )
				return true;

			if( lhs.fContributePer == rhs.fContributePer )
			{
				if( lhs.iTotalDeath < rhs.iTotalDeath )
					return true;

				//if( lhs.iTotalDeath == rhs.iTotalDeath )
				//{
				//	int iCmpValue = _stricmp( lhs.szName.c_str(), rhs.szName.c_str() );
				//	if( iCmpValue < 0 )
				//		return true;
				//}
			}
		}
		else if( rhs.bAbuse )	// !lhs.bAbuse && rhs.bAbuse
		{
			return true;
		}

		return false;
	}
};

enum StructCreateType
{
	SCT_NORMAL,
	SCT_KILLER,
	SCT_NO_OWNER,
};

struct PushStruct
{
	int m_iIndex;
	int m_iNum;

	Vector3    m_CreatePos;
	Quaternion m_TargetRot;
	ioHashString m_OwnerName;

	DWORD m_dwCreateTime;
	DWORD m_dwCreateEtcCode;

	PushStruct()
	{
		m_iIndex = 0;
		m_iNum = 0;
		m_dwCreateTime = 0;
		m_dwCreateEtcCode = 0;

		m_CreatePos.x = 0.0f;
		m_CreatePos.y = 0.0f;
		m_CreatePos.z = 0.0f;
	}
};

typedef std::vector< PushStruct > PushStructList;

struct BallStruct
{
	int m_iIndex;
	int m_iNum;

	DWORD m_dwSupplyTime;

	float m_fCreateRange;

	Vector3    m_CreatePos;
	Quaternion m_TargetRot;

	BallStruct()
	{
		m_iIndex = 0;
		m_iNum = 0;

		m_dwSupplyTime = 0;

		m_fCreateRange = 0.0f;

		m_CreatePos.x = 0.0f;
		m_CreatePos.y = 0.0f;
		m_CreatePos.z = 0.0f;
	}
};
typedef std::vector< BallStruct > BallStructList;

struct MachineStruct
{
	int m_iIndex;
	int m_iNum;

	Vector3    m_CreatePos;
	Quaternion m_TargetRot;
	ioHashString m_TakeCharName;

	DWORD m_dwSupplyTime;

	bool m_bTake;

	MachineStruct()
	{
		m_iIndex = 0;
		m_iNum = 0;
		m_dwSupplyTime = 0;

		m_CreatePos.x = 0.0f;
		m_CreatePos.y = 0.0f;
		m_CreatePos.z = 0.0f;

		Init();
	}

	void Init()
	{
		m_TakeCharName.Clear();
		m_bTake = false;
	}
};
typedef std::vector< MachineStruct > MachineStructList;


struct ObjectItem
{
	ioHashString m_ObjectItemName;
	float m_fPosX;
	float m_fPosZ;
};

typedef std::vector< ObjectItem > ObjectItemList;

struct SupplyItemTime
{
	DWORD m_dwSupplyTime;
	ioHashStringVec m_ItemList;
	Vector3Vec m_PosList;
	Vector3Deq m_ShufflePosList;

	SupplyItemTime()
	{
		m_dwSupplyTime = 0;
		m_ItemList.clear();
		m_PosList.clear();
		m_ShufflePosList.clear();
	}

	Vector3 GetRandomItemPos()
	{
		if( m_PosList.empty() )
		{
			return Vector3( 0.0f, 0.0f, 0.0f );
		}

		Vector3 vPos;
		if( !m_ShufflePosList.empty() )
		{
			vPos = m_ShufflePosList.front();
			m_ShufflePosList.pop_front();
			return vPos;
		}

		m_ShufflePosList.clear();
		m_ShufflePosList.insert( m_ShufflePosList.begin(), m_PosList.begin(), m_PosList.end() );

		io_random_shuffle( m_ShufflePosList.begin(), m_ShufflePosList.end() );

		vPos = m_ShufflePosList.front();
		m_ShufflePosList.pop_front();

		return vPos;
	}
};
typedef std::vector< SupplyItemTime > SupplyItemTimeList;


//
typedef std::list< PlayingTimeInfo > PlayingTimeList;

class PlayingTimeListSort
{
public:
	bool operator()(  const PlayingTimeInfo &lhs, const PlayingTimeInfo &rhs ) const
	{
		if( lhs.dwPlayingTime > rhs.dwPlayingTime )
			return true;

		return false;
	}
};

class ClassPlayTimeSort
{
public:
	bool operator()(  const ClassPlayTimeInfo &lhs, const ClassPlayTimeInfo &rhs ) const
	{
		if( lhs.dwPlayingTime > rhs.dwPlayingTime )
			return true;

		return false;
	}
};

// Next Team 계산용 Mode::GetUserRankByNextTeam() 에서 사용
struct NextTeamInfo
{
	float fPoint;
	ioHashString szName;
	int iTotalKill;
	int iTotalDeath;
	
	NextTeamInfo()
	{
		fPoint         = 1.0f;
		iTotalKill     = 0;
		iTotalDeath    = 0;
	}
};

typedef std::vector< NextTeamInfo > NextTeamInfoList;

class NextTeamInfoSort
{
public:
	bool operator()( const NextTeamInfo &lhs , const NextTeamInfo &rhs ) const
	{
		if( lhs.fPoint > rhs.fPoint )
			return true;

		if( lhs.fPoint == rhs.fPoint )
		{
			if( lhs.iTotalKill > rhs.iTotalKill )
				return true;

			if( lhs.iTotalKill == rhs.iTotalKill )
			{
				if( lhs.iTotalDeath < rhs.iTotalDeath )
					return true;

				//if( lhs.iTotalDeath == rhs.iTotalDeath )
				//{
				//	int iCmpValue = _stricmp( lhs.szName.c_str(), rhs.szName.c_str() );
				//	if( iCmpValue < 0 )
				//		return true;
				//}
			}
		}			
		return false;
	}
};
//
struct NPCRecord                                      // Npc Record
{
	DWORD			dwCode; 
	ioHashString	szName;
	DWORD			dwMonsterID;
	int				iRoomIndex;
	float			fStartXPos;
	float			fStartZPos;
	DWORD			dwStartTime;
	DWORD			dwCurDieTime;
	RecordState		eState;
	ioHashString	szSyncUser;	
	ioEquipSlot*	m_pEquipSlot;
	DWORD			dwPresentCode;
	DWORD			dwDiceTable;
	TeamType		eTeam;

	NPCRecord& operator = ( const NPCRecord &rhs )
	{
		dwCode			= rhs.dwCode;
		szName			= rhs.szName;
		dwMonsterID		= rhs.dwMonsterID;
		fStartXPos		= rhs.fStartXPos;
		fStartZPos		= rhs.fStartZPos;
		dwStartTime		= rhs.dwStartTime;
		dwCurDieTime	= rhs.dwCurDieTime;
		eState			= rhs.eState;
		szSyncUser		= rhs.szSyncUser;
		dwPresentCode	= rhs.dwPresentCode;
		dwDiceTable		= rhs.dwDiceTable;
		iRoomIndex		= rhs.iRoomIndex;
		eTeam			= rhs.eTeam;
		m_pEquipSlot	= NULL;

		return *this;
	}

	NPCRecord()
	{
		dwCode			= 0;
		fStartXPos		= 0.0f;
		fStartZPos		= 0.0f;
		dwStartTime		= 0;
		eState			= RS_LOADING;
		dwCurDieTime	= 0;
		dwPresentCode	= 0;
		dwDiceTable		= 0;
		dwMonsterID		= 0;
		iRoomIndex		= 0;
		eTeam			= TEAM_RED;
		m_pEquipSlot    = NULL;
	}
};
typedef std::vector< NPCRecord > NPCRecordList;
typedef std::vector< DWORD > NPCCodeList;

#endif