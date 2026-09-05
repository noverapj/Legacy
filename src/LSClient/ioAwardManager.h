#pragma once


#include "GUI/ResultInfo.h"

class ioUIRenderImage;
class SP2Packet;

struct AwardUser
{
	ioHashString m_szName;
	int	         m_iContribute;
	int	         m_iValue;
	int          m_iOriginalValue;

	AwardUser()
	{
		m_iContribute	= 0;
		m_iValue		= 0;
		m_iOriginalValue= 0;
	}
};
typedef std::vector< AwardUser > vAwardUser;
typedef vAwardUser::iterator vAwardUser_iter;

class AwardInfoSort : public std::binary_function< const AwardUser&, const AwardUser&, bool >
{
public:
	bool operator()( const AwardUser &lhs , const AwardUser &rhs ) const
	{
		if( lhs.m_iValue > rhs.m_iValue )
		{
			return true;
		}				
		else if( lhs.m_iValue == rhs.m_iValue )
		{
			if( lhs.m_iContribute > rhs.m_iContribute )
				return true;
		}
		return false;
	}
};

class ioAwardManager  : public Singleton< ioAwardManager >
{
public:
	enum
	{
		// ALL
		AC_CONTRIBUTE = 1,                 //챔피온
		AC_DAMAGE,                         //광전사
		AC_HIT,                            //난타왕
		AC_SHOPPING,                       //지름신
		AC_DROP,                           //낚시왕
		AC_WOUNDED,                        //동네북
		AC_DEFENSE,                        //몸  빵
		AC_TEAMKILL,                       //스파이
		AC_COMBO,                          //콤보킹
		
		// 파워스톤
		AC_TEAM_STONE_DAMAGE,              //수호자
		AC_OTHER_TEAM_STONE_DAMAGE,        //파괴자
		AC_BLOCK_STONE_ATTACK,             //삽질맨

		// 포로탈출
		AC_PRISONER_ESCAPE,                //구원자
		AC_PRISONER_DROP,                  //처형자
		AC_PRISONER_LAST_KILL,             //종결자
		AC_PRISONER_DIE,                   //심청이

		// 히든크라운
		AC_KING_GUARD,                     //친위대
		AC_THE_KING,                       //임금님
		AC_OTHER_KING_ATTACK,              //특공대
		AC_TEAM_KING_ATTACK,               //반역자

		// 스페셜 
		AC_LUCKY,                          //행운상     - 서버에서 결정하여 지급한다.

		// 축구
		AC_GOAL,							//득점왕
		AC_ASSIST,							//도움왕
		AC_OWNER_GOAL,						//자책골
		AC_GOAL_SAVE,						//선방

		// 강시
		AC_GANGSI_KING,                     //강시왕(인간을 강시로...)
		AC_GANGSI_SURVIVOR,                 //생존자(인간으로 버팀)

		//셔플
		AC_SHUFFLE_FUL_STAR,				//별이 많은 유저
		AC_SHUFFLE_LESS_STAR,				//별이 적은 유저

		// 깃발뺏기
		AC_FLAG_CAPTURE						// 깃발왕
	};

	enum
	{
		MAX_AWARDING = 4,
	};

	enum
	{
		AWARD_SELECT_ALL = 0,
		AWARD_SELECT_WIN,
		AWARD_SELECT_LOSE,
	};

protected:
	typedef std::map< ModeType, DWORD > AwardSelectMap;
	AwardSelectMap m_AwardSelectMap;

	WinTeamType m_eWinTeam;
	FinalInfoList m_FinalInfoList;

protected:
	struct AwardData
	{
		int    m_iType;
		IntVec m_ModeTypeList;
		int    m_iLimitValue;
		int    m_iSortPoint;
		int    m_iGoodBadType;
		int    m_iMultiplySortPoint;
		bool   m_bDescVariable; 
		DWORD  m_dwDescColor;

		ioHashString m_szTitle;
		ioHashString m_szDesc;
		ioUIRenderImage *m_pTitleRes;
		ioUIRenderImage *m_pIcon;
		ioHashString m_szCharAni;
		ioHashString m_szSound;
		vAwardUser m_vUserList;

		AwardData()
		{
			m_iType		= 0;
			m_iLimitValue = 0;
			m_iSortPoint= 0;
			m_iGoodBadType = 0;
			m_dwDescColor = 0;
			m_bDescVariable = false;
			m_iMultiplySortPoint = 1;

			m_pTitleRes = NULL;
			m_pIcon		= NULL;
		}
	};

	class AwardDataSort : public std::binary_function< const AwardData *, const AwardData *, bool >
	{
	public:
		bool operator()( const AwardData *lhs , const AwardData *rhs ) const
		{
			if( lhs->m_iSortPoint > rhs->m_iSortPoint )
			{
				return true;
			}			
			return false;
		}
	};
	typedef std::vector< AwardData* > vAwardData;
	vAwardData m_vAwardList;

	//////////////////////////////////////////////////////////////////////////
	// 독식을 막기위해 랜덤하게 정렬
	struct AwardRandSortData
	{
		AwardData *m_pAwardData;
		int        m_iSortPoint;
		ioHashString m_szTopName;
		AwardRandSortData()
		{
			m_pAwardData = NULL;
			m_iSortPoint = 0;
		}
	};
	typedef std::vector< AwardRandSortData > vAwardRandSortData;

	class AwardRandSort : public std::binary_function< const AwardRandSortData&, const AwardRandSortData&, bool >
	{
	public:
		bool operator()( const AwardRandSortData &lhs , const AwardRandSortData &rhs ) const
		{
			if( lhs.m_iSortPoint > rhs.m_iSortPoint )
				return true;
			return false;
		}
	};
	//////////////////////////////////////////////////////////////////////////

public:
	void LoadAwardInfo();

protected:
	void ClearAwardInfoList();

	void AddValue( AwardData *pAward, const ioHashString &rkName, int iValue, int iOriginalValue );
	void AddUser( AwardData *pAward, const ioHashString &rkName );
	void RemoveUser( AwardData *pAward, const ioHashString &rkName );
	int  GetContribute( const ioHashString &rkName );

	AwardData* GetAwardData( int iCategory );
	bool IsAwardSameModeType( AwardData *pAward );

protected:
	const FinalInfo &GetFindUserInfo( const ioHashString &rkName );
	void AwardSelectUserSplitCheck();
	void AwardSelectWinUserSplit();
	void AwardSelectLoseUserSplit();

public:
	inline int GetMaxAward() const { return m_vAwardList.size(); }
	int GetRandAwardType();
	int GetGoodBadType( int iCategory );

	const ioHashString& GetAwardTitle( int iCategory );
	const ioHashString& GetAwardDesc( int iCategory );
	const DWORD GetAwardDescColor( int iCategory );
	bool IsAwardDescVariable( int iCategory );
	ioUIRenderImage* GetAwardTitleRes( int iCategory );
	ioUIRenderImage* GetAwardIcon( int iCategory );

	const ioHashString& GetAwardAniName( int iCategory );
	const ioHashString& GetAwardSound( int iCategory );
	const AwardUser &GetTopAwardUser( int iCategory );
	const AwardUser &GetTopAwardUser( AwardData *pAward );

	void GetDebugContent( int iCategory, int iUserArray, char *szContent );

public:
	void AddNewAwardUser( const ioHashString &rkName );
	void RemoveAwardUser( const ioHashString &rkName );
	void RemoveAllUser();

public:
	void AddContribute( const ioHashString &rkName, int iContribute );
	void AddDamage( const ioHashString &rkName, int iDamage, int iOriginalValue );
	void AddHit( const ioHashString &rkName, int iHit = 1 );
	void AddShopping( const ioHashString &rkName, int iBuyCount = 1 );
	void AddDrop( const ioHashString &rkName, int iDropCount, int iOriginalValue = 1 );
	void AddWounded( const ioHashString &rkName, int iWoundedCount, int iOriginalValue = 1 );
	void AddDefense( const ioHashString &rkName, int iDefenseCount, int iOriginalValue = 1 );
	void AddTeamKill( const ioHashString &rkName, int iTeamKill = 1 );
	void AddCombo( const ioHashString &rkName, int iComboCount );

	void AddTeamStoneDamage( const ioHashString &rkName, int iDamage, int iOriginalValue );
	void AddOtherTeamStoneDamage( const ioHashString &rkName, int iDamage, int iOriginalValue );
	void AddBlockStoneAttack( const ioHashString &rkName, int iCount = 1 );

	void AddPrisonerEscape( const ioHashString &rkName, int iEscapeCount, int iOriginalValue = 1 );	
	void AddPrisonerDrop( const ioHashString &rkName, int iDropCount, int iOriginalValue = 1 );
	void AddPrisonerLastKill( const ioHashString &rkName, int iLastKill, int iOriginalValue = 1 );     
	void AddPrisonerDie( const ioHashString &rkName, int iDie = 1 );

	void AddKingGuard( const ioHashString &rkName, int iTime );
	void AddKing( const ioHashString &rkName, int iTime );
	void AddOtherKingAttack( const ioHashString &rkName, int iAttackCount = 1 );
	void AddTeamKingAttack( const ioHashString &rkName, int iAttackCount = 1 );

	void AddGoal( const ioHashString &rkName, int iValue, int iOriginalValue = 1 );
	void AddAssist( const ioHashString &rkName, int iValue, int iOriginalValue = 1 );
	void AddOwnerGoal( const ioHashString &rkName, int iValue, int iOriginalValue = 1 );
	void AddGoalSave( const ioHashString &rkName, int iValue, int iOriginalValue = 1 );


	void AddGangsiKing( const ioHashString &rkName, int iValue = 1 );
	void AddGangsiSurvivor( const ioHashString &rkName, int iTime );

	void AddShuffleFulStar( const ioHashString &rkName, int iStar );
	void AddShuffleLessStar( const ioHashString &rkName, int iStar );

	void AddFlagCapture( const ioHashString &rkName, int iValue = 1 );

public:
	void SendAwardInfoSync( const ioHashString &rkJoiner );
	void FillAwardInfo( SP2Packet &rkPacket );
	void FillAwardResult( SP2Packet &rkPacket );

public:
	void ApplyAwardInfo( SP2Packet &rkPacket );

public:
	void SetFinalResult( WinTeamType eWinTeam );	
	void AddFinalInfo( const FinalInfo &rkInfo );

public:
	static ioAwardManager& GetSingleton();

public:
	ioAwardManager();
	virtual ~ioAwardManager();
};

#define g_AwardMgr ioAwardManager::GetSingleton()

