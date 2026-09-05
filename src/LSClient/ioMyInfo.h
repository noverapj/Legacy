#pragma once


class SP2Packet;
class ioBaseChar;
class ioINILoader;
class ioPlayStage;
class ioSetItemInfo;
class ioUIRenderImage;
class ioPriateRullet;
class ioPriateRenewalRullet;
class ioMatchingGame;
class ioDiceGame;

#include "CharacterInfo.h"
#include "ioInventory.h"
#include "ioAward.h"
#include "ioUserRecord.h"
#include "ioClassExpert.h"
#include "ioEventUserManager.h"
#include "ioUserEtcItem.h"
#include "ioUserFishItem.h"
#include "ioUserExtraItem.h"
#include "ioGrowthLevel.h"
#include "ioQuest.h"
#include "ioUserMedalItem.h"
#include "ioCharRentalData.h"
#include "ioUserTournament.h"
#include "ioAnnounceGameAddiction.h"
#include "ioSelectShutDownAnnouncer.h"
#include "ioClover.h"
#include "ioUserPet.h"
#include "ioUserCostume.h"
#include "ioUserNamedTitle.h"
#include "ioUserAccessory.h"
#include "ioUserSpiritInven.h"
#include "GUI/MyInventoryWnd.h"

#define US_TUTORIAL_CLEAR      -1

#define EXERCISE_NONE                   0x00         // 구매 용병
#define EXERCISE_GENERAL                0x01         // 일반 체험 용병
#define EXERCISE_PCROOM                 0x02         // PC방 체험 용병
#define EXERCISE_EVENT                  0x03         // 레벨 제한 없이 본부에서만 사용 가능한 체험 용병( ExerciseSoldierEvent )
#define EXERCISE_RENTAL                 0x04         // 대여한 용병

#define REFERENCE_CHARACTER_DATA_NORMAL		1
#define REFERENCE_CHARACTER_DATA_ARENA		2

class ioBingo;
class ioUI3DRender;
class ioCreateChar;
//-------------------------------------------------------------------------
//용병 교체 UI에서 표시할 내 용병의 3D 캐릭터 + 장비 + 애니메이션을 첫 로딩
//할 때 같이 로딩하며 용병 교체 UI에서 표시할 때 아래의 포인터를 사용하여
//표시해준다. 캐릭터 삭제될 때 포인터가 삭제되므로 절대로 포인터를 멤버로
//참조하지말고 실시간으로 꺼내서 사용할 것.
//-------------------------------------------------------------------------
typedef struct tagUICharLoadingBuffer
{
	DWORD m_dwCharIndex;
	ioUI3DRender *m_pUI3DRender;
	ioCreateChar *m_aMyChar;			
	bool          m_bAnimationPlay;
	AwakeType	  m_iAwakeEffect;	

	tagUICharLoadingBuffer()
	{
		m_dwCharIndex    = 0;
		m_pUI3DRender	 = NULL;
		m_aMyChar		 = NULL;
		m_bAnimationPlay = false;
		m_iAwakeEffect   = AWAKE_NONE;
	}
}UICharLoadingBuffer;
class SoldierSelectUIBeforeLoading
{
protected:
	typedef std::vector< UICharLoadingBuffer * > CharBufferList;
	CharBufferList m_CharBufferList;
	CharBufferList m_ArenaCharBufferList;
	CharBufferList* m_pCurrentCharBufferList;

protected:
	bool IsAlreadyChar( DWORD dwIndex );

public:
	void PushCharList( const CHARACTERDATA &rkCharacter, bool bThread );
	void EraseCharList( DWORD dwCharIndex );

	void SetArenaBufferList( bool bArena );

public:
	UICharLoadingBuffer *GetUIChar( DWORD dwCharIndex );
	
public:
	void ClearList();

public:
	void Update( const CHARACTERDATA &rkCharacter );
	void UpdateAwake( UICharLoadingBuffer *pBuffer, const CHARACTERDATA &rkCharacter );
	void ChangeCharIndex( DWORD dwPrevIndex, DWORD dwNextIndex );

public:
	SoldierSelectUIBeforeLoading();
	virtual ~SoldierSelectUIBeforeLoading();
};

//-------------------------------------------------------------------------
class MannerTrialInfo
{
public:
	enum 
	{
		MAX_TRAIL = 5, 
	};

	enum ReturnType
	{
		RT_OK        = 0,
		RT_ERROR_MAX = 1,
		RT_ERROR_ID  = 2,
	};

protected:
	DWORD m_dwTrailDate; // 20100101
	ioHashStringVec m_vTrailPublicIDVec;

protected:
	void Clear();

public:
	void Load();
	void Save();
	void UpdateDate();
	void AddTrailInfo( const ioHashString &rszPublicID );
	ReturnType CheckRight( IN const ioHashString &rszPublicID ); 
public:
	MannerTrialInfo();	
	virtual ~MannerTrialInfo();
};

//-------------------------------------------------------------------------
struct ExcercisePCRoomCharInfo
{
	bool m_bNewChar;
	CHARACTERDATA m_Character;

	ExcercisePCRoomCharInfo()
	{
		m_bNewChar = false;
	}
};
typedef std::vector<ExcercisePCRoomCharInfo> ExcercisePCRoomCharInfoVec;

//-------------------------------------------------------------------------
class ioMyInfo : public Singleton< ioMyInfo >
{
private:
	USERDATA     m_user_data;	               //유저 정보.	
	UserRelativeGradeData m_user_relative_grade_data;
	int          m_select_char_array;          //사용중인 캐릭터.
	typedef std::vector< CHARACTERDATA > vCharList;
	vCharList	 m_char_data;
	vCharList	m_char_arena_data;
	vCharList*	m_pCharData;
	int			m_iPrevReferenceCharacterDataType;
	
	struct CharTimeLimit
	{
		DWORD m_dwLimitTimer;
		DWORD m_dwLimitSecond;
		CharTimeLimit()
		{
			m_dwLimitTimer = m_dwLimitSecond = 0;
		}
	};
	typedef std::map< DWORD, CharTimeLimit > CharTimeLimitMap;
	CharTimeLimitMap m_vCharTimeLimitMap;

	ioInventory		m_Inventory;
	ioClassExpert	m_ClassExpert;
	ioAward			m_Award;
	ioUserRecord    m_UserRecord;
	ioUserEtcItem   m_UserEtcItem;
	ioUserFishItem  m_UserFishItem;
	ioUserExtraItem m_UserExtraItem;
	ioUserMedalItem m_UserMedalItem;
	UserHeroData    m_UserHeroData;
	ioCharRentalData m_CharRentalData;
	UserHeadquartersOption m_UserHeadquartersData;
	ioUserTournament m_UserTournament;
	ioUserPet m_UserPet;
	ioUserCostume m_UserCostume;
	ioUserNamedTitle m_pUserNamedTitle;
	ioUserAccessory m_UserAccessory;
	ioUserSpiritInven m_UserSpiritInven;

	ioQuest         m_Quest;

	IntVec m_vFishingItemList;
	CEncrypt<int> m_iCurFishingBait;
	CEncrypt<int> m_iCurFishingRod;
	
	ioGrowthLevel m_GrowthLevel;

	ioHashString m_HostName;
	ioHashString m_MyIP;
	int m_iMyPort;

	int m_iSealLevel;
	int m_iNextSealLevel;

	PlayCharInfoList m_vPlayCharInfoList;
	PlayCharInfoList m_vArenaCharInfoList;
	PlayCharInfoList* m_pCurrentCharInfoList;

	float m_fMaxRevivalPenalty;
	float m_fRecoverPenaltyPerSec;
	float m_fRecoverPenaltyPlayPerSec;

	float m_fRecoverItemGaugeRate;

	CHARACTER m_SingleCharInfo;

	DWORD m_dwRoomOutTimer;
	int m_iAbilityLevel;
	
	CEncrypt<int>   m_iPenaltyPeso;
	CEncrypt<float> m_fPenaltyPesoRate;
	bool m_bCurModeSafetyRoom;
	DWORD m_dwPCRoomAuthority;
	bool m_bExerciseEventSoldier;
	bool m_bUserHeroEvent;
	bool m_bBefore_HeroEvent;
	
	bool m_bViewState;

	bool m_bReserveEnterTraining;
	bool m_bReserveEnterFighting;

	int  m_iTotalUserRegCount;

	ioHashString m_szReserveKickReason;
	//수련장 암호화용 id
	ioHashString m_szGuid;

	SoldierSelectUIBeforeLoading m_SoldierSelectUIBeforeLoading;

protected:
	int m_iLimiteMaxCharSlot;
	int m_iDefaultMaxCharSlot;
	int m_iCurMaxCharSlot;
	int m_iArenaMaxCharSlot;

protected:
	ioHashString m_szKickSubInfo;

	// 광장 강퇴
	struct KickOutPlaza
	{
		int   m_iPlazaIndex;
		DWORD m_dwKickOutTimer;
		KickOutPlaza()
		{
			m_iPlazaIndex    = -1;
			m_dwKickOutTimer = 0;
		}
	};
	typedef std::vector< KickOutPlaza > vKickOutPlaza;
	vKickOutPlaza m_vKickOutPlaza;	

	EventUserManager m_EventUserMgr;
	int m_iGradeExpRateBeforeCon;

	// Release ExtraItem Info
	struct ReleaseNameInfo
	{
		ioHashString m_szClassName;
		ioHashString m_szItemName;
		ioHashString m_szIconName;
		int          m_iReinforce;
		int			 m_nItemCode;
	};
	typedef std::vector<ReleaseNameInfo> vReleaseNameList;
	vReleaseNameList m_vReserveReleaseInfoList;

	// Release Expand Medal Info
	struct ReleaseExMedallSlotInfo
	{
		ioHashString m_szIconName;
		ioHashString m_szSubIconName;
		ioHashString m_szClassName;
		int          m_iSlotLevel;
	};
	typedef std::vector<ReleaseExMedallSlotInfo> vReleaseExMedallSlotInfo;
	vReleaseExMedallSlotInfo m_vReserveReleaseExMedalSlotList;

	// End TimeGrowth Info
	struct TimeGrowthClassInfo
	{
		int m_iClassType;
		int m_iTimeSlot;

		bool m_bConfirm;

		TimeGrowthClassInfo()
		{
			m_iClassType = 0;
			m_iTimeSlot = 0;

			m_bConfirm = false;
		}
	};
	typedef std::vector<TimeGrowthClassInfo> vTimeGrowthInfoList;
	vTimeGrowthInfoList m_vTimeGrowthInfoList;

	// Release Costume Info
	struct ReleaseCostumeInfo
	{
		ioHashString m_szClassName;
		ioHashString m_szItemName;
		ioHashString m_szIconName;
		int			 m_nCostumeCode;
	};
	typedef std::vector<ReleaseCostumeInfo> vReleaseCostumeInfoList;
	vReleaseCostumeInfoList m_vReserveReleaseCostumeInfoList;

	// Release Accessory Info
	struct ReleaseAccessoryInfo
	{
		ioHashString m_szClassName;
		ioHashString m_szItemName;
		ioHashString m_szIconName;
		int			 m_iAccessoryCode;
	};
	typedef std::vector<ReleaseAccessoryInfo> vReleaseAccessoryInfoList;
	vReleaseAccessoryInfoList m_vReserveReleaseAccessoryInfoList;

protected:
	struct LevelUPData
	{
		int m_iBonusType;     //레벨업 보상 속성.(용병단(용병) / 용병단(페소) / 용병(용병) / 용병(페소)
		int m_iValue1;        //용병단 레벨 / 레벨업 용병 타입
		int m_iValue2;        //보너스 페소 / 보너스 용병 타입
		int m_iValue3;        //보너스 용병 기간
	};
	typedef std::vector< LevelUPData > vLevelUPData;
	vLevelUPData m_vLevelUPList;

// For Praise
protected:
	// Contribute
	int m_iDefaultContribute;
	int m_iMaxContribute;
	int m_iIncreaseContribute;

	int m_iCurContribute;

protected:
	time_t m_iServerDate;
	time_t m_iLocalDate;
	int    m_iLogOutLaterDay; // 로그아웃 후 로그인까지 몇일만에 접속했는지(DAY)
	int    m_iLogOutLaterSec; // 로그아웃 후 로그인까지 몇초만에 접속했는지(SEC)

protected:
	int m_iRefillCoinCurTime;

	MannerTrialInfo m_MannerTrialInfo;

protected:
	bool m_bFirstCameraFixedHelp;
	int  m_iMileage;

protected:
	ioHashString m_szJoinHeadquartersRequestName;
	ioHashString m_szJoinHouseRequestName;

protected:
	struct RentalTimeData
	{
		DWORD m_dwCurrentTime;
		RentalTimeData()
		{
			m_dwCurrentTime = 0;
		}
	};
	typedef std::map< DWORD, RentalTimeData > RentalTimeDataMap;
	RentalTimeDataMap m_RentalTimeDataMap;
	DWORD m_dwRentalProcessTime;
	
protected:
	bool m_bReservePCRoomCheck;

protected:
    DWORD m_dwOneMinuteTimeCheck;

	bool m_bShutDownUser;

protected:
	CEncrypt<DWORD> m_dwLobbyLockType;
	CEncrypt<DWORD> m_dwBattleLockType;
	DWORD m_dwLobbyLockTypeMsgBoxID;

protected:
	CEncrypt<DWORD>				m_dwMacroNotifyTime;
	ioAnnounceGameAddiction		m_AnnounceGameAddiction;
	ioSelectShutDownAnnouncer	m_SelectShutDownAnnouncer;
	ioClover					m_Clover;
	ioBingo*					m_pBingo;
	ioPriateRullet*				m_pPriateRullet;
	ioPriateRenewalRullet*		m_pPriateRenewalRullet;
	ioMatchingGame*				m_pMatchingGame;
	ioDiceGame*					m_pDiceGame;
protected:
	DWORDVec m_TournamentConfirmList;
	struct TournamentConfirmRequest
	{
		DWORD m_dwTourIndex;
		DWORD m_dwRequestTime;
		TournamentConfirmRequest()
		{
			m_dwTourIndex = m_dwRequestTime = 0;
		}
	};
	typedef std::vector< TournamentConfirmRequest > TournamentConfirmRequestVec;
	TournamentConfirmRequestVec m_TournamentConfirmRequestList;

//PC방용병 관련
protected:
	IntVec m_PcRoomCharVec;
	CEncrypt<int> m_iExcercisePCRoomCharMax;

//출석체크 관련
protected:
	typedef std::vector<DWORD> AttendanceRecord;
	AttendanceRecord m_AttendanceRecord;
	DWORD m_dwConnectAttendanceTime;

//추가 장비 교체 실패 관련 임시 변수
protected:
	int m_iEquipType;  // 1. ItemChange, 2. ItemRelese, 3. 봉인아이템 장착
	int m_iCharArray;
	int m_iEquipSlot;
	int m_iExtraItemIndex;

	//튜토리얼
protected:
	bool	m_bTutorialMode;

protected:
	CTime m_PreLogoutTime;

//R용병
protected:
	int m_iRSoldierPossessionCount;

protected:
	IntVec m_vSoldierSetCntVec;

//1vs1모드
	int m_iSuccessionMMRPoint;
	int m_iMaxSuccessionCnt;
	int m_iCurSuccessionCnt;
	bool m_bSuccessionRevengeMode;

	CTime m_TimeGateTime;

public:
	void SetExtraItemChange( int iType, int iCharArr, int iSlot, int iIndex );
	void GetExtraItemChange( int& iType, int& iCharArr, int& iSlot, int& iIndex );

public:
	void ProcessAnnounceGameAddiction();
	void SetSelectShutDownTime( int iYear, int iMonth, int iDay, int iHour, int iMinute ); 
	void ProcessSelectShutDownAnnouncer();

public:
	bool IsHost() const;
	void SetMyAddress( const ioHashString &szIP, int iPort );

	inline bool IsViewState() const { return m_bViewState; }
	inline void SetViewState( bool bViewState ) { m_bViewState = bViewState; }

public:
	static ioMyInfo& GetSingleton();

public:
	void SetPrivateID( ioHashString szPrivateID );
	void SetPublicID( ioHashString szPublicID );
	void SetHostName(ioHashString hostName){ m_HostName = hostName; }
	
	void SetMoney( __int64 iMoney );
	void SetCash( int iCash );
	void SetPurchasedCash( int iPurchasedCash );
	void SetChannelingCash( int iCash );
	void SetLadderPoint( int iLadderPoint );	
	void SetAccrueHeroExpert( int iAccrueHeroExp );
	void SetHeroExpert( int iHeroExp );
	void SetUserCampPos( int iCampPosition );
	void SetFirstSolderBonus();
	void SetJoinHeadquartersRequestName( const ioHashString &rkName ){ m_szJoinHeadquartersRequestName = rkName; };
	void SetJoinHouseRequestName( const ioHashString &rkName ){ m_szJoinHouseRequestName = rkName; };

public:
	void SetKickedInfo( const char *szSubInfo );
	void ClearKickedInfo();

	const ioHashString& GetKickSubInfo() const { return m_szKickSubInfo; }

	ChannelingType GetChannelingType() const;
	const ioHashString &GetChannelingUserID() const;
	int GetChannelingCash() const;
	int GetTotalCash(); // Cash + ChannelingCash

	BlockType GetBlockType() const;
	DWORD     GetBlockYearMonthDay() const;
	DWORD     GetBlockHourMinute() const;
	float     GetBlockPointPer();

public:
	const DWORD& GetUserIndex() const { return m_user_data.m_user_idx; }
	const ioHashString& GetPrivateID() const { return m_user_data.m_private_id; }  
	const ioHashString& GetPublicID() const { return m_user_data.m_public_id; }    
	const ioHashString& GetGUID() const { return m_szGuid; }
	const ioHashString& GetHostName() const { return m_HostName; } //방장 닉네임.
	const int GetUserCampPos() const { return m_user_data.m_camp_position; }
	const int GetUserRanking() const { return m_user_data.m_user_rank; }
	const int GetUserEventType() const { return m_user_data.m_user_event_type; }
	int GetLadderPoint() const { return m_user_data.m_ladder_point; }
	int GetCampRank() const { return m_user_data.m_camp_rank; }
	int GetHeroTodayRank() const { return m_UserHeroData.m_iHeroTodayRank; }
	int GetHeroSeasonRank( int iSeason );
	bool IsBroadcastUser(){ return ( GetUserEventType() == USER_TYPE_BROADCAST_AFRICA || GetUserEventType() == USER_TYPE_BROADCAST_MBC ); }

	int  GetRelativeGradeInitCode() { return m_user_relative_grade_data.m_init_code; }
	void SetGradeExp( int iExp ) { m_user_data.m_grade_exp = iExp; }

	int GetCharIndex(int array);
	DWORD GetCharIndexByClassType( int iClassType );
	int GetCharArray( DWORD dwIndex );
	int GetClassArray( int iClassType );
	int GetClassArrayExceptExercise( int iClassType );
	int GetExerciseCharArrayByClass( int iClassType );

	int GetCharEmptyFirstSlot();
	void GetCharEmptySlotArray( int iArraySize, IntVec& vSlotArray );

	int GetCharSlotIndexToArray( int iSlotArray );
	int GetCharArrayToSlotIndex( int iArray );
	int GetCharIndexToSlotIndex( DWORD dwIndex );

	__int64 GetMoney() const { return m_user_data.m_money; }
	int GetCash() const { return m_user_data.m_cash; }
	int GetPurchasedCash() const { return m_user_data.m_purchased_cash; }

	bool IsGeneralGrade();
	int GetGradeLevel();
	int GetGradeExpert();
	int GetClassLevel( int iClassType, bool bOriginal );
	int GetExpRate( int iClassType );
	int GetClassExpert( int iClassType );
	int GetGradeExpRate();
	int GetClassType( int iCharArray );
	int GetClassType( DWORD dwIndex );
	int GetClassType();
	int GetTopLevelClassType();
	const char *GetClassName( int iClassType, int eCharPowerUpType = PUGT_NONE );
	int GetUserState() const { return m_user_data.m_user_state; }
	void SetUserState( int iState );
	bool IsTutorialUser();
	bool IsBankruptcy();
	bool IsCharBankruptcy();
	
	const ioClassExpert::CLASSSLOT* GetClassExpertSlot( int iClassType );	

	const CHARACTERDATA* GetCharacterData();		//캐릭터 및 아이템 정보
	const CHARACTERDATA* GetCharacterData(int eid);
	const CHARACTERDATA* GetCharacterDataByIndex( DWORD dwIdx );
	const CHARACTERDATA* GetCharacterDataByClassType( int iClassType );

	const CHARACTER& GetCharacter();                //현재 사용중인 캐릭터 외형 정보
	const CHARACTER& GetCharacter(DWORD eid);			//슬롯 장착중인 캐릭터 외형 정보.
	const CHARACTER& GetCharacterToClassType(int iClassType);			//슬롯 장착중인 캐릭터 외형 정보.
	void CopyCharacter(int eid, CHARACTER *charInfo);         
	bool IsOriginalCharacter();

	bool IsCharUse( int iIdx );
	bool IsItemSlotUse( int iIdx );
	bool IsCharEquipItem( int eid );                   //캐릭터가 아이템 장착 중인가?
	bool IsCharRentalToClassType( int iClassType );	   //캐릭터가 대여 설정된 캐릭터인가?
	DWORD GetCharRentalMinuteToArray( int eid );		   //캐릭터가 대여중 남은 분
	bool IsCharRentalToExtraItem( int iSlotIndex );	   //장비를 착용하고 있는 캐릭터가 대여 설정된 캐릭터인가?

	const ITEM_DATA& GetCharItem( int index );         //현재 사용중인 캐릭터 착용 아이템 정보.
	const ITEM_DATA& GetCharItem( int eid, int index ); //슬롯 장착중인 캐릭터 착용 아이템 정보.
	const ITEM_DATA& GetCharItemToClassType( int iClassType, int index ); //슬롯 장착중인 캐릭터 착용 아이템 정보.

	ITEM_DATA& GetCharWeaponItem();						//현재 사용중인 캐릭터 착용중인 무기 아이템 정보
	ITEM_DATA& GetCharItemData( int index );
    void InitCharWeaponItem();							//캐릭터들의 무기 아이템 정보중 Bullet 초기화 처리

	int GetCharTotalSecond( int iCharArray );          //용병 남은 시간
	const ioHashString &GetJoinHeadquartersRequestName(){ return m_szJoinHeadquartersRequestName; }
	const ioHashString &GetJoiHousesRequestName(){ return m_szJoinHeadquartersRequestName; }
public:
	void AddCharacter( const CHARACTERDATA &character );
	void AddArenaCharacter( DWORD dwType, int iPowerUp, int iFace, int iHair, int iSkinColor, int iHairColor, int iUnderWear );
	void AddGradeExp( int iExp );
	void AddClassExp( int iClassType, int iExp );
	void AddKill( ModeType eMode, int iCount, bool bDropKill, bool bBestAttacker );
	void AddDeath( ModeType eMode, int iCount );
	void AddWinCount( RoomStyle eRoomStyle, ModeType eMode, int iCount );
	void AddLoseCount( RoomStyle eRoomStyle, ModeType eMode, int iCount );

	void SetArenaModeCharacter( ModeType eMode );

	void SetClassExpNLevel( int iClassType, int iExp, int iLevel );

	void SetGrade( int iGradeLevel, int iExp );
	void SetChangeChar( const DWORD dwIndex );
	void SetCharacter( int iCharArray , const CHARACTERDATA &character );
	void SetCharChangePeriod( int iCharArray );
	void SetRefillCoinData( int iRefillData, int iRefillCount );
	void SetBeforeCharLoading( bool bThread );
	void SetBeforeCharLoading_NormalCharacter( bool bThread );
	void SetBeforeCharLoading_ArenaCharacter( bool bThread );

	void ChangeReferenceCharacterData( int iType );
	void RestoreReferenceCharacterData();

	int GetOriginalCharacterCount()		{ return (int)m_char_data.size(); }

public:
	UICharLoadingBuffer *GetSoldierSelectUIChar( DWORD dwCharIndex );
	int  GetSelectCharArray() const { return m_select_char_array; }
	int  GetCharCount() const;
	int  GetCharRentalSetCount();
	int  GetActiveCharCount();
	int  GetInAtciveCharCount();
	int  GetActiveExerciseCharCount();
	int  GetExerciseCharCount();
	int  GetExerciseCharCount( int chExerciseStyle );
	int  GetNoneExerciseCharCount();
	DWORD GetExerciseCharIndex( int chExerciseStyle );
	int   GetMortmainCharCount();

public:
	void SetArenaCharacterIndex( int iType, DWORD dwIndex );
	void SetArenaCharacterSlot( int iType, int iSlot );

public:
	int  GetKillDeathLevel();
	bool IsSafetyLevel();

public:
	void SetUserData( IN SP2Packet &rkPacket ,OUT DWORD &dwPublicIDHashCode );
	void SetUserHeroData( SP2Packet &rkPacket );
	const USERDATA& GetUserData() const { return m_user_data; }

	ioInventory* GetInventory(){ return &m_Inventory; }
	ioUserEtcItem* GetUserEtcItem() { return &m_UserEtcItem; }
	ioUserExtraItem* GetUserExtraItem() { return &m_UserExtraItem; }
	ioUserMedalItem* GetUserMedalItem() { return &m_UserMedalItem; }
	ioCharRentalData* GetCharRentalData() { return &m_CharRentalData; }
	UserHeadquartersOption *GetUserHeadquartersOption(){ return &m_UserHeadquartersData; }
	ioUserTournament* GetTournament(){ return &m_UserTournament; }
	ioClover* GetClover() { return &m_Clover; }
	ioBingo* GetBingo() { return m_pBingo; }
	ioUserPet* GetUserPet() { return &m_UserPet; }
	ioUserCostume* GetUserCostume() { return &m_UserCostume; }
	ioUserNamedTitle* GetUserNamedTitle() { return &m_pUserNamedTitle; }
	ioPriateRullet* GetPriateRullet(){ return m_pPriateRullet; }
	ioPriateRenewalRullet* GetPriateRenewalRullet(){ return m_pPriateRenewalRullet; }
	ioMatchingGame* GetMatchingGame() { return m_pMatchingGame; }
	ioDiceGame* GetDiceGame() { return m_pDiceGame; }
	ioUserAccessory* GetUserAccessory() { return &m_UserAccessory; }
	ioUserSpiritInven* GetSpiritInventory() { return &m_UserSpiritInven; }

	// 유저 정보에 사용할 항목들의 경험치
	int GetPartyLevel();
	__int64 GetPartyExp();
	int GetLadderLevel();
	__int64 GetLadderExp();
	int  GetHeroTitle();
	int  GetHeroRank();
	int  GetAwardLevel();
	int  GetAwardExp();
	int  GetRecordWin( RecordType eRecordType );
	int  GetRecordLose( RecordType eRecordType );
	float GetRecordWinLoseRate( RecordType eRecordType );
	int  GetHeroSeasonWin();
	int  GetHeroSeasonLose();
	float GetHeroSeasonWinLoseRate();
	void SetUserInfoMgr();
	void SerUserInfoAllCharData();
	void SerUserInfoCharData( int iClassType );

	int GetCharEquipedItemCnt( int iCharArray ) const;

public:
	void SetCharTotalSecond( int iCharArray, int iTotalSecond );
	void SetCharTotalSecond( DWORD dwIndex, int iTotalSecond );

public:
	void SetCharDied( int iArray, bool bDie );
	void SetCharDiedPenalty( int iArray, bool bDie );
	void ClearCharDiedInfo();
	void ClearCharDiedPenaltyInfo();
	bool IsCharDied( int iArray ) const;
	bool IsCharDiedPenalty( int iArray ) const;

	void SetCharJoined( int iArray, bool bJoin );
	void SetCharJoined( const DWORD dwIndex, bool bJoin );
	void ClearCharJoinedInfo();
	bool IsCharJoined( int iArray ) const;

	bool IsSlotItem( int iType, int iCode );
	void AddSlotItem( int iType, int iCode );
	bool RemoveSlotItem( int iType, int iCode );
	void SetEquipItem( int iType, int iCode );
	void GetEquipItemCode( CHARACTER &charInfo );
	int GetEquipItemCode( const int iItemType );
	bool IsClassTypeMale( int iClassType );
	bool IsClassTypeInventory( int iClassType );
	bool IsClassTypeInventory( int iClassType, int iSexType, int iDecoType );

	void RecoverRevivalPenalty( bool bPlay = false );
	float GetRevivalPenalty( int iArray ) const;
	float GetMaxRevivalPenalty() const { return m_fMaxRevivalPenalty; }

public:
	void ApplyCharDecoration( int iCharArray, SP2Packet &rkPacket );
	void ApplyCharDecoration( DWORD dwCharIndex, CHARACTER &rkInfo );
	DWORD ApplyCharPackage( CHARACTER &rkInfo );
	void ApplyCharExtend( int iCharArray, SP2Packet &rkPacket );
	void ApplyInventorySlotData( SP2Packet &rkPacket );

	void ApplyRoundEndInfo( SP2Packet &rkPacket );

	void ApplyBlockType( SP2Packet &rkPacket );
	void ApplyClassExpert( SP2Packet &rkPacket );
	void ApplyAwardData( SP2Packet &rkPacket );
	void ApplyUserRecordData( SP2Packet &rkPacket );
	void ApplyUserHeroSeasonRecord( SP2Packet &rkPacket );
	void ApplyFishItemData( SP2Packet &rkPacket );
	void ApplyQuestData( SP2Packet &rkPacket );
	void ApplyQuestCompleteData( SP2Packet &rkPacket );

public:
	void FillCharItemData( DWORD dwCharIndex, SP2Packet &rkPacket );
	void ChangeCharEquipItemCode( DWORD dwCharIndex, int iEquipIndex, int iItemCode );


public:
	void ApplyGrowthLevelData( SP2Packet &rkPacket );

	ioGrowthLevel* GetGrowthLevel();

public:
	void AddAwardData( int iCategory, int iPoint );

	
public:
	bool CharSlotChange( int iLeftArray, int iLeftSlotIndex, int iRightArray, int iRightSlotIndex );
	bool CharDelete( int eid );
	int  CharDelete( DWORD dwIdx );
	void CharLeaderChange( DWORD dwCharIndex );
	void CharSetMyRental( DWORD dwCharIndex, short sRentalType );
	void SetCharActive( DWORD dwIdx, bool bActive );
	bool IsCharActive( int eid );
	bool IsCharRental( int eid );
	bool IsCharExerciseRentalInActive( int eid );
	bool IsCharInActive( int iClassType );
	bool IsCharExercise( int iArray );
	bool IsCharExerciseStyle( int iArray, int chExerciseStyle );
	bool IsCharExerciseStyleToClassType( int iClassType, int chExerciseStyle );
	int  GetAutoSelectChar( int iSelectId, int iPreSelectId );
	bool IsCharMortmain( int iClassType, bool bPowerUp = false );
	float GetSoldierExpBonusByGradeExp( int iClassType, int iExp );

public:
	void LoadINIInfo( ioINILoader &rkLoader );

	inline void SetSinglePlayCharInfo( const CHARACTER &rkInfo ) { m_SingleCharInfo = rkInfo; }
	inline const CHARACTER& GetSinglePlayCharInfo() const { return m_SingleCharInfo; }
	
public:
	inline const ioHashString& GetMyIP() const {return m_MyIP;}
	bool IsSameMyIP( const ioHashString &szCheckIP );

public:
	void GetFinalResultChar( IntVec &rvClassTypeList );

public:
	inline void SetAbilityLevel( const int iLevel ) { m_iAbilityLevel = iLevel; }
	inline int GetAbilityLevel() const { return m_iAbilityLevel; }

	void SetCurSafetyLevelRoom( bool bSafety ) { m_bCurModeSafetyRoom = bSafety; }
	int GetCurPenaltyPeso() const;

public:
	void SetKickOutPlaza( int iRoomIndex );
	bool IsKickOutPlaza( int iRoomIndex );

public:
	void SetTotalUserRegCnt( int iTotal ){ m_iTotalUserRegCount = iTotal; }
	int  GetTotalUserRegCnt(){ return m_iTotalUserRegCount; }

public:
	void SetRoomOutTimer( DWORD dwTimer );
	DWORD GetRoomOutSec();

	DWORD DeleteCharData( int iCharArray );
	bool DeleteExerciseChar( byte chExerciseStyle );
	void FixSelectChar();
	void ApplyExerciseIndex( SP2Packet &rkPacket);
	bool IsDeleteExerciseChar( ModeType eType , bool bSafetyLevelRoom );
	bool IsCharExerciseRentalCharGrowthByClassType( int iClassType, int iSlot, BYTE &kGrowthLevel );
	bool IsCharExerciseRentalItemGrowthByClassType( int iClassType, int iSlot, BYTE &kGrowthLevel );

	// TEST Code
protected:
	int m_iMyContributePer;

public:
	bool IsSortContribute();
	void SetMyContribute( int iMyPer ){ m_iMyContributePer = iMyPer; }
	int  GetMyContribute(){ return m_iMyContributePer; }

public:
	void StartCharLimitDate( DWORD dwIndex, DWORD dwCheckSecond );
	void StopCharLimitDate( );

	EventUserManager& GetEventUserMgr();

	inline int GetGradeExpRateBeforeCon() const { return m_iGradeExpRateBeforeCon; }
	inline void SetGradeExpRateBeforeCon(int iGradeExpRateBeforeCon) { m_iGradeExpRateBeforeCon = iGradeExpRateBeforeCon; }


	EntryType GetEntryType() const { return m_user_data.m_eEntryType; }
	bool IsEntryFormality() const;
	void SetEntryType( EntryType eEntryType ) { m_user_data.m_eEntryType = eEntryType; } 
	bool IsExpertEntryTemporary();

	bool IsCanBuyItem( const ioSetItemInfo *pSetItemInfo );
	void GetNeedLevelInfo( IN const ioSetItemInfo *pSetItemInfo , OUT int &riNeedType, OUT int &riNeedLevel );

public:
	void InitLevelUPData();
	bool IsClassORGradeLevelUP(){ return ( !m_vLevelUPList.empty() ); }
	void SetLevelUP( int iBonusType, int iValue1, int iValue2, int iValue3 );
	bool IsGradeLevelUP();
	void GetGradeUPBonusInfo( int &iBonusType, int &iBonusValue1, int &iBonusValue2 );
	bool IsClassLevelUP( int iClassType );
	int  GetClassLevelUPPeso( int iClassType );
	bool IsGradeLevelUPEtcItem();

public:
	void SetPCRoomAuthority( DWORD bPCRoomAuthority, int iExcercisePCRoomCharMax );
	DWORD GetPCRoomAuthority(){ return m_dwPCRoomAuthority; }

	bool IsUserEvent() { return m_bUserHeroEvent; }

	void SetTimeGateTime( __int64 iTime )	{ m_TimeGateTime = CTime( iTime ); }
	CTime GetTimeGateTime()					{ return m_TimeGateTime; }

public:
	void SetExerciseEventSoldier( bool bExerciseEventSoldier ){ m_bExerciseEventSoldier = bExerciseEventSoldier; }	
	bool IsExerciseEventSoldier(){ return m_bExerciseEventSoldier; }

public:
	ioUIRenderImage *GetSoldierIcon( int iClassType, bool bMale );
	const ioHashString &GetSoldierIconName( int iClassType, bool bMale );
	ioUIRenderImage *GetMySoldierIcon( int iClassType );
	ioHashString &GetMySoldierIconName( int iClassType );
	ioUIRenderImage *GetSoldierSubIcon( int iClassType );
	ioHashString &GetSoldierSubIconName( int iClassType );

// For Praise
public:
	void InitPraise();
	
	int CheckContributePraise( int iNewContribute );

public:
	void LoadCharSlotInfo();
	void CheckCurMaxCharSlot();
	const int GetCurMaxCharSlot();
	inline int GetLimiteMaxCharSlot() const { return m_iLimiteMaxCharSlot; }

// Fishing
protected:
	int GetCurEnableFishingRod();

public:
	int GetCurMaxFishingInventory();

	void SetFishingLevel( int iLevel, int iExpert );
	int GetFishingLevel();
	int GetFishingExpert();

	bool CheckFishingRod();
	int GetCurFishingBaitCnt();

	void AddFishingItem( BYTE iType, int iIndexArray );
	void RemoveFishingItem( int iIndexArray );

	int GetFishingItemType( int iListArray );
	int GetFishingItemArray( int iListArray );

	int GetFishingRodItemType( int iListArray );
	int GetFishingBaitItemType( int iListArray );
	int GetFishingGroundItemType( int iListArray );

	void ChangeFishingGroundItemOwn( int iListArray, bool bOwn );

	bool GetFishingGearOwnByTabType( int iListArray, FishingGearType eFishingGearType );
	bool GetFishingGearOwnByEtcCode( int iEtcCode, FishingGearType eFishingGearType );
	bool CheckUseFishingGear( int iListArray, FishingGearType eFishingGearType );
	DWORD GetSelectFishingGearEtcCode( int iListArray, FishingGearType eFishingGearType );
	int GetFishingGearCountByTabType( FishingGearType eFishingGearType );

	void AlignFishingGearByOwn();

	// 가지고 있는 낚싯대, 미끼들 중 제일 작은 값의 순번(Array)의 아이템 EtcCode 반환
	DWORD GetFishingRodByFirstOrder();
	DWORD GetFishingBaitByFirstOrder();
	void SetFirstFishingRod_Bait( int iCurFishingRod, int iCurFishingBait, bool bNoneSelect = false );
	
	int GetFishingSlotExtendItem();
	int GetCurFishingItemCnt();
	bool IsFishingItemMax();

	void CheckCurFishingTackle( bool bFirst );

	bool SetCurFishingRod( int iType );
	bool SetCurFishingBait( int iType );
	int GetCurFishingRod();
	int GetCurFishingBait();

	// 낚시 용품 소유 여부 체크 용도
	bool HasEtcItem( DWORD dwEtcItemCode );

// For ExtraItem
public:
	void RefreshExtraItemEquip();
	void ApplyChangeExtraItem( int iCharArray, int iSlot, int iNewSlotIndex );
	void UpdateEquipExtraItem( int iSlotIndex );
	bool DeleteExtraItem( int iSlotIndex );
	bool DeleteExtraItemData( SP2Packet &rkPacket );
	bool IsReserveDeleteExtraItem();
	void GetFirstDeleteExtraItem( ioHashString &rkClassName, ioHashString &rkName, ioHashString &rkIconName, int &iReinforce, int &nItemCode );
	void EraseDeleteExtraItem( bool bAllErase );
	bool IsCharEquipExtraItemPeriodCheck( int iClassType, int iPeriodType );

// For Expand Medal Slot
	void SetReserveDeleteExMedalSlot( ioHashString &szIconName, ioHashString &szSubIconName, ioHashString &szClassName, int iSlotLevel );
	bool IsReserveDeleteExMedalSlot();
	void GetFirstDeleteExMedalSlot( ioHashString &szIconName, ioHashString &szSubIconName, ioHashString &szClassName, int &iSlotLevel );
	void EraseDeleteExMedalSlot( bool bAllErase );

// For Monster Coin
public:
	bool UseModeStartMonsterCoin( IN int iUseCnt, IN bool bUseGoldCoin, OUT int& nCoinUseCount, OUT int& nGoldCoinUseCount );
	bool UseGoldMonsterCoin();
	int  GetEtcMonsterCoin();
	int  GetEtcGoldMonsterCoin();
	int  GetEtcRaidCoin();
	bool AddEtcMonsterCoin( int iAddCoin );
	bool AddEtcGoldMonsterCoin( int iAddCoin );
	bool SetEtcMonsterCoin( int iCoin );
	bool SetEtcGoldMonsterCoin( int iCoin );
	bool SetEtcRaidCoin( int iCoin );
	int  RemainRefillMinute();

	// For Silver + Clover Coin
public:
	int  GetEtcCoinMoney( DWORD dwEtcItem );
	void UseEtcCoinMoney( DWORD dwEtcItem, int iUseCoin );

public:
	bool InitEtcItemUseBattleRecord();
	bool InitEtcItemUseLadderRecord();
	bool InitEtcItemUseHeroRecord();

	DWORD GetTimeGashaponEtcItem();

public:
	ioHashString GetEtcMotionAni( DWORD dwMotionType, bool bMale, const ioHashString &rkOriginalAni );
	DWORD GetEtcMotionAni( DWORD dwMotionType );

// For TimeGrowth
public:
	void AddTimeGrowthEndInfo( int iClassType, int iTimeSlot, bool bConfirm );
	void ShowTimeGrowthEndInfo();

	int GetTimeGrowingCount();        // 시간 성장중인 용병 갯수
	int GetTimeGrowthPossibleCount(); // 시간 성장 가능한 용병 갯수
	bool IsTimeGrowthCharCheck( int iClassType );

// For Vote Kick
public:
	void SetVoteKickReason( const ioHashString &rkReason );
	const ioHashString& GetVoteKickReason() const { return m_szReserveKickReason; }

// For Quest
public:
	ioQuest *GetQuest(){ return &m_Quest; }
	void NewLoginQuestCheck();

// For Battle & Ladder CharIndex Sync
public:
	void SyncBattleNLadderCharIndex();

public:
	int FindEquipedExtraItemChar( int iSlotIndex );

// For excavation
public:
	void SetExcavationLevel( int iLevel, int iExpert );
	int  GetExcavationLevel();
	int  GetExcavationExpert();

// For MedalItem
public:
	void ApplyChangeMedalItem( int iCharArray, int iItemType, int iCustomIndex, bool bEquip );
	bool DeleteMedalItemData( SP2Packet &rkPacket );
	bool DeleteMedalItem( int iItemType, int iCustomIndex );

	void GetMedalItemTypeVec( OUT MedalSyncInfoVec &rvItemTypeVec, IN int iClassType );
	void GetMedalTypeAndGrowthLevel( IN int iClassType, OUT MedalSyncInfoVec &rvMedalItemTypeVec, OUT IntVec &rvGrowthLevelVec );

public:
	bool IsExtraItemFull( bool bSetMsgBox );
	bool IsDecorationFull( bool bSetMsgBox );
	bool IsMedalItemFull( bool bSetMsgBox );
	bool IsPetFull( bool bSetMsgBox );
	bool IsCostumeFull( bool bSetMsgBox );
	bool IsAccessoryFull( bool bSetMsgBox );

public:
	inline const time_t &GetServerDate() const { return m_iServerDate; } 
	inline const time_t &GetLocalDate() const { return m_iLocalDate; } 
	void GetServerTime( OUT SYSTEMTIME &rkST );

public:
	MannerTrialInfo *GetMannerTrialInfo() { return &m_MannerTrialInfo; }

public:
	bool IsFirstCameraFixedHelp();
	void CharRentalTimeMsgBox( DWORD dwLimitTime );

	int GetMileage() const { return m_iMileage; }
	void SetMileage(int iMileage) { m_iMileage = iMileage; }

protected:
	ioMyInfo::RentalTimeData &GetRentalTimeData( DWORD dwCharIndex );

public:
	void ReturnRentalCharToOwenrID( const ioHashString &rkOwnerID );
	void SetRentalCharTime( int iClassType, DWORD dwMinute );
	void RemoveRentalTimeData( DWORD dwCharIndex ); 
	void RentalCharTimeCheck();

public:
	int  GetDisplaySoldierCount();
	void SetDisplaySoldier( int iClassType, int iXPos, int iZPos, bool bClear );
	bool IsDisplaySoldier( int iClassType );

public:
	bool IsReservePCRoomCheck(){ return m_bReservePCRoomCheck; }
	void ReservePCRoomCheck(){ m_bReservePCRoomCheck = true; }
	void PCRoomQuestCheck();

public:
	void OneMinuteTimeCheckProcess();
	bool IsShutDownUser() const { return m_bShutDownUser; }
	void SetShutDownUser(bool bShutDownUser) { m_bShutDownUser = bShutDownUser; }

	void SendMgameShutDownDate();

public:
	void  SetLobbyLockType( DWORD dwLockType, DWORD dwLobbyLockTypeMsgBoxID );
	DWORD GetLobbyLockType(){ return m_dwLobbyLockType; }
	DWORD GetLobbyLockTypeMsgBoxID(){ return m_dwLobbyLockTypeMsgBoxID; }

public:
	void  SetBattleLockType( DWORD dwLockType );
	DWORD GetBattleLockType(){ return m_dwBattleLockType; }

public:
	void  SetMacroNotifyTime(){ m_dwMacroNotifyTime = FRAMEGETTIME(); }
	DWORD GetMacroNotifyTime(){ return m_dwMacroNotifyTime; }

public:
	void ApplyTournamentTeamList( SP2Packet &rkPacket );

	// 요청 완료됨
	bool IsTournamentConfirm( DWORD dwTourIndex );
	void AddTournamentConfirm( DWORD dwTourIndex );

	// 요청중
	bool IsTournamentConfirmRequest( DWORD dwTourIndex );
	void AddTournamentConfirmRequest( DWORD dwTourIndex );
	void RemoveTournamentConfirmRequest( DWORD dwTourIndex );

// For Clover
public:
	void UpdateClover();
	int GetCurGiftCloverCount();

public:
	void ClearBingoIcon();

public:
	void InsertPcRoomChar( int iClassType );
	void RemovePcRoomChar( int iClassType );
	inline int GetPcRoomCharMax(){	return m_iExcercisePCRoomCharMax; }

public:
	void ApplyAttendanceRecord( SP2Packet &rkPacket );

	void AddAttendanceRecord( DWORD dwDate );	
	void ResetAttendanceRecord();

	bool HasAttendanceRecord( DWORD dwDate );
	int GetAttendanceRecordCount();
	DWORD GetAttendanceConnectTime();

public:
	void ChangeAwakeInfo( int iCharDataIdx, int iAwakeType );
	void ChangeAwakeDate( int iCharDataIdx, int iLimitDate );
	void ChangeAwakeClear( int iCharDataIdx );
	byte GetAwakeType( int iClassType );
	int GetAwakeLimitTime( int iClassType );

public:
	void ApplyCharGrade( int iCharIndex, byte eCharGrade );
	PowerUpGradeType GetCharPowerUpGrade( DWORD dwIndex );
	PowerUpGradeType GetCharPowerUpGradeByClassType( int iClassType );

public:
	void AddPowerUpGrade( int iClassType, byte eCharPowerUpType );

public:
	void ApplyChangeCostume( int nCharArray, int nSlot, int nSlotIndex, bool bEquip );
	bool DeleteCostume( int nSlotIndex );
	bool DeleteCostumeData( SP2Packet &rkPacket );
	int FindEquipedCostumeChar( int iSlotIndex );
	bool IsReserveDeleteCostume();
	void GetFirstDeleteCostume( ioHashString &rkClassName, ioHashString &rkName, ioHashString &rkIconName, int &nCostumeCode );
	void EraseDeleteCostume( bool bAllErase );
	bool IsCharRentalToCostume( int iSlotIndex );
	void UpdateEquipCostume( int iSlotIndex );
	void EquipCostumeInit( int nCharArray, int nSlot, const CostumeSlot &sCostumeSlot );

//튜토리얼 모드
public:
	const bool& IsTutorialMode() { return m_bTutorialMode; }
	void		SetTutorialMode(bool bMode ) { m_bTutorialMode = bMode; }

//R 용병
public:
	void SetRSoldierPossessionCount( int iCount ) { m_iRSoldierPossessionCount = iCount; }
	inline const int& GetRSoldierPossessionCount() { return m_iRSoldierPossessionCount; }
	void CalcRCharPossessionCount();

//개인본부
	bool IsHouseOpen();

// 칭호 관련
public:
	void ApplyEquipChangeNamedTitle( DWORD dwCode, BYTE byEquip );
	void ApplyGradeChangeNamedTitle( DWORD dwCode, int nLevel );
	void ApplyPremiumChangeNamedTitle( DWORD dwCode );
	DWORD GetNamedTitleCode();
	int GetNamedTitleLevel();
	BYTE GetNamedTitlePremium();

public:
	void CalcSoldierSetCnt();
	int GetSoldierSetCnt( int nSetCode );
	void GetSoldierSetCnt( IntVec& vSoldierSetCnt );

//보너스 캐쉬
public:
	void SetBonusCash( int iCash );
	int GetBonusCash() const;
//액세서리
	void ApplyChangeAccessory( int iCharArray, int iSlot, int iSlotIndex, bool bEquip );
	bool DeleteAccessory( int iSlotIndex );
	bool DeleteAccessoryData( SP2Packet &rkPacket );
	int FindEquipedAccessoryChar( int iSlotIndex );
	bool IsReserveDeleteAccessory();
	void GetFirstDeleteAccessory( ioHashString &rkClassName, ioHashString &rkName, ioHashString &rkIconName, int &iAccessoryCode );
	void EraseDeleteAccessory( bool bAllErase );
	bool IsCharRentalToAccessory( int iSlotIndex );
	void UpdateEquipAccessory( int iSlotIndex );
	void EquipAccessoryInit( int iCharArray, int nSlot, const AccessorySlot &sAccessorySlot );
//1vs1모드
	inline const int& GetSuccessionMMRPoint() const { return m_iSuccessionMMRPoint; }
	inline const int& GetMaxSuccessionCount() const { return m_iMaxSuccessionCnt; }
	inline const int& GetCurSuccessionCount() const { return m_iCurSuccessionCnt; }
	inline const bool& GetSuccessionRevengeMode() const { return m_bSuccessionRevengeMode; }
	

	inline void SetSuccessionMMRPoint(int iPoint ) { m_iSuccessionMMRPoint = iPoint; }
	inline void SetMaxSuccessionCount(int iMax ) { m_iMaxSuccessionCnt = iMax; }
	inline void SetCurSuccessionCount(int iCur ) { m_iCurSuccessionCnt = iCur; }
	inline void SetSuccessionRevengeMode(bool bMode ) { m_bSuccessionRevengeMode = bMode; }
public:
	ioMyInfo();
	virtual ~ioMyInfo();
};

#define g_MyInfo ioMyInfo::GetSingleton()
