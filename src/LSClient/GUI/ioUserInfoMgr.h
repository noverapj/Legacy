#pragma once


#include "../CharacterInfo.h"

class SP2Packet;

#define CHARINFODATA_SYNC_COUNT        6
#define CHARINFODATA_UI_LOAD_TIME      1000
struct CharInfoData 
{
	// 메인 동기화 데이터
	DWORD     m_dwMotionEtcItem;
	int       m_iClassLevel;
	CHARACTER m_CharInfo;
	ITEM_DATA m_EquipItemInfo[MAX_INVENTORY];  
	
	// 서브 동기화 데이터
	bool      m_bSubDataSync;
	MedalSyncInfoVec   m_EquipMedalItem;
	int       m_CharGrowth[MAX_CHAR_GROWTH];
	int       m_ItemGrowth[MAX_ITEM_GROWTH];
	
	// 캐릭터 로딩 대기 시간
	DWORD     m_dwCharLoadingTime;
	DWORD     m_dwRentalRequestTime;

	bool      m_bChangeData;
	int		  m_iAwakeType;

	CharInfoData()
	{
		Init();
	}

	void Init()
	{
		m_dwMotionEtcItem = 0;
		m_iClassLevel	  = 0;
		m_CharInfo.Init();

		int i = 0;
		for(i = 0;i < MAX_INVENTORY;i++)
			m_EquipItemInfo[i].Initialize();

		m_bSubDataSync = false;
		m_EquipMedalItem.clear();
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			m_CharGrowth[i] = 0;
		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			m_ItemGrowth[i] = 0;

		m_dwCharLoadingTime		= 0;
		m_dwRentalRequestTime	= 0;
		m_bChangeData			= false;
		m_iAwakeType			= 0;
	}

	int GetCharGrowth( int iSlot )
	{
		int iReturn = 0;
		if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
		{
			int nReinforce = m_EquipItemInfo[iSlot].m_item_reinforce;
			if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				iReturn += nReinforce;
			else
				iReturn += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );
		}
		if( COMPARE( iSlot, 0, MAX_CHAR_GROWTH ) )
			iReturn += m_CharGrowth[iSlot];

		return iReturn;
	}

	int GetItemGrowth( int iSlot )
	{
		int iReturn = 0;
		if( COMPARE( iSlot, 0, MAX_INVENTORY ) )
		{
			int nReinforce = m_EquipItemInfo[iSlot].m_item_reinforce;
			if ( nReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				iReturn += nReinforce;
			else
				iReturn += g_GrowthInfoMgr.GetReinforceAddStat( nReinforce );
		}
		if( COMPARE( iSlot, 0, MAX_ITEM_GROWTH ) )
			iReturn += m_ItemGrowth[iSlot];

		return iReturn;
	}

	CharInfoData& operator = ( const CharInfoData &rhs )
	{
		m_CharInfo = rhs.m_CharInfo;
		
		int i = 0;
		for(i = 0;i < MAX_INVENTORY;i++)
			m_EquipItemInfo[i] = rhs.m_EquipItemInfo[i];

		m_dwMotionEtcItem   = rhs.m_dwMotionEtcItem;
		m_iClassLevel		= rhs.m_iClassLevel;
		m_bChangeData		= rhs.m_bChangeData;
		m_bSubDataSync		= rhs.m_bSubDataSync;
		m_dwCharLoadingTime = rhs.m_dwCharLoadingTime;
		for(i = 0;i < MAX_CHAR_GROWTH;i++)
			m_CharGrowth[i] = rhs.m_CharGrowth[i];
		for(i = 0;i < MAX_ITEM_GROWTH;i++)
			m_ItemGrowth[i] = rhs.m_ItemGrowth[i];


		m_EquipMedalItem.clear();
		if( !rhs.m_EquipMedalItem.empty() )
		{
			m_EquipMedalItem.resize( rhs.m_EquipMedalItem.size() );
			std::copy( rhs.m_EquipMedalItem.begin(), rhs.m_EquipMedalItem.end(), m_EquipMedalItem.begin() );
		}

		m_iAwakeType = rhs.m_iAwakeType;

		return *this;
	}
};
typedef std::vector< CharInfoData > CharInfoDataVec;
class CharInfoSort : public std::binary_function< const CharInfoData&, const CharInfoData&, bool >
{
public:
	bool operator()( const CharInfoData &lhs , const CharInfoData &rhs ) const
	{
		if( lhs.m_CharInfo.m_sRentalType > rhs.m_CharInfo.m_sRentalType )
		{
			return true;
		}
		else if( lhs.m_CharInfo.m_sRentalType == rhs.m_CharInfo.m_sRentalType )
		{
			if( lhs.m_CharInfo.m_iSlotIndex < rhs.m_CharInfo.m_iSlotIndex )
				return true;
		}	
		return false;
	}
};

class UserInfoCharData
{
protected:
	CharInfoDataVec m_CharDataList;
	CharInfoData    m_LeaderCharData;
	DWORD           m_dwLeaderMotion;

protected:
	void SelectLeader( CharInfoData &rkData );
	void SelectLeader();

public:
	void SetCharacter( const CHARACTER &rkCharInfo, int iClassLevel );
	void SetEquipItem( int iClassType, const ITEM_DATA kEquipItem[MAX_INVENTORY] );
	void SetEquipMedal( int iClassType, MedalSyncInfoVec &rkEquipMedal );
	void SetCharGrowth( int iClassType, int kCharGrowth[MAX_CHAR_GROWTH], int kItemGrowth[MAX_ITEM_GROWTH], int iAwakeType );	
	void SetCharAwake( int iClassType, int iAwakeType );
	void SetCharPowerUp( int iClassType, int iCharPowerUpGrade );
	void SetLeaderMotion( DWORD dwEtcItem );
	void ResetCharacterData();

public:
	void CharacterSort();
	void UpdateUI();

public:
	void DeleteCharData( int iClassType );	

public:
	int GetMaxChar();

public:
	CharInfoData &GetLeaderChar(){ return m_LeaderCharData; }
	CharInfoData &GetSlotChar( int iListArray );
	CharInfoData &GetSlotCharToClassType( int iClassType );

	/************************************************************************/
	/* 동기화                                                               */
	/************************************************************************/
protected:
	DWORD m_dwSyncCheckTime;
	bool  m_bDataSync;
	int   m_iMaxVirtualCharCount;
	bool  m_bScrollSyncBlock;
	DWORD m_dwScrollSyncTime;

public:
	void SyncCharData( const ioHashString &rkUserName, bool bOnline );
	void ScrollSyncCharData( const ioHashString &rkUserName, int iListArray );
	void ApplyCharData( const ioHashString &rkUserName, SP2Packet &rkPacket );
	bool IsDataSync(){ return m_bDataSync; }
	
	void SyncSubCharData( const ioHashString &rkUserName, int iClassType );
	void ApplyCharSubData( const ioHashString &rkUserName, SP2Packet &rkPacket );

public:
	void SetEquipCostume( int iClassType, const CostumeEquipInfo kEquipCostume[MAX_INVENTORY] );
	void SetEquipCostumebySubType( int iClassType, int nSubType, int nCostumeCode, DWORD dwCustomMale, DWORD dwCustomFemale );

public:
	UserInfoCharData();
	virtual ~UserInfoCharData();
};
//////////////////////////////////////////////////////////////////////////
class UserInfoNode
{
protected:	
	ioHashString		m_UserID;

	//계급 래밸
	int					m_iGradeLevel;
	int					m_iGradeExpPer;

	int					m_iPartyLevel;
	__int64				m_iPartyExp;
	
	//래더 등급 정보
	int					m_iLadderLevel;
	__int64				m_iLadderExp;

	//수상정보
	int					m_iAwardLevel;
	int					m_iAwardExp;

	//킬/데스 레벨(실력계급)
	int					m_iKillDeathLevel;

	//낚시 정보
	int					m_iFishingLevel;
	int					m_iFishingExpert;

	//탐사 정보
	int					m_iExcavationLevel;
	int					m_iExcavationExpert;

	//래더전 승률
	int					m_iHeroWin;
	int					m_iHeroLose;

	int					m_iHeroTitle;
	int					m_iHeroRank;

	BlockType			m_eBlockType;

	//래더정보
	int					m_iUserRank;
	int					m_iLadderPoint;

	//진영전 정보
	int					m_iCampPosition;
	bool				m_bCampChange;

	//길드정보
	DWORD				m_dwGuildIndex;
	bool				m_bGuildInfoChange;

	//육성정보
	IntVec				m_vGrowthLevelVec;  
	MedalSyncInfoVec	m_vMedalItemTypeVec;

	//용병정보
	int					m_iClassLevel;
	int					m_iClassType;

	//펫
	int					m_nPetCode;
	int					m_nPetLevel;

	//개인본부
	bool				m_bHouseOpen;
	
	//칭호
	DWORD				m_dwNamedTitleCode;
	int					m_nNamedTitleLevel;
	BYTE				m_byNamedTitlePremium;

	UserInfoCharData	m_UserCharData;

	DWORD				m_dwTCPSyncTime;
	DWORD				m_dwUDPSyncTime;
	bool				m_bSyncInfo;         // 한번도 동기화 되지 않음.
	bool				m_bForceUDPSync;
	int					m_iRSoldierPossessionCount;

	IntVec m_vSoldierSetCntVec;

	//일대일모드
	int					m_iCurSuccessionCnt;
	int					m_iMaxSuccessionCnt;

public:
	inline const ioHashString &GetUserID(){ return m_UserID; }
	inline const int GetGradeLevel(){ return m_iGradeLevel; }
	inline const int GetPartyLevel(){ return m_iPartyLevel; }
	inline const int GetLadderLevel(){ return m_iLadderLevel; }
	inline const int GetAwardLevel(){ return m_iAwardLevel; }
	inline const int GetKillDeathLevel(){ return m_iKillDeathLevel; }
	inline const int GetFishingLevel(){ return m_iFishingLevel; }
	inline const int GetExcavationLevel(){ return m_iExcavationLevel; }
	inline const int GetHeroWin(){ return m_iHeroWin; }
	inline const int GetHeroLose(){ return m_iHeroLose; }
	inline const int GetHeroTitle(){ return m_iHeroTitle; }
	inline const int GetHeroRank(){ return m_iHeroRank; }
	inline const BlockType GetBlockType(){ return m_eBlockType; }
	inline const int GetGradeExp(){ return m_iGradeExpPer; }
	inline const int GetRanking(){ return m_iUserRank; }
	inline const DWORD GetGuildIndex(){ return m_dwGuildIndex; }
	inline const int GetLadderPoint(){ return m_iLadderPoint; }
	inline const int GetCampPosition(){ return m_iCampPosition; }
	inline const int GetClassLevel(){ return m_iClassLevel; }
	inline const int GetClassType(){ return m_iClassType; }
	inline const int& GetRSoldierPossessionCount() { return m_iRSoldierPossessionCount; } 
	inline const bool GetHouseOpen(){ return m_bHouseOpen; }
	inline const DWORD GetNamedTitleCode() { return m_dwNamedTitleCode; }
	inline const int GetNamedTitleLevel() { return m_nNamedTitleLevel; }
	inline const BYTE GetNamedTitlePremium(){ return m_byNamedTitlePremium; }
	inline const int& GetMaxSuccessionCount() const { return m_iMaxSuccessionCnt; }
	inline const int& GetCurSuccessionCount() const { return m_iCurSuccessionCnt; }

	DWORD GetGuildMark();

	int GetPartyExp();
	int GetLadderExp();
	int GetAwardExp();
	int GetFishingExp();
	int GetExcavationExp();
	float GetHeroRecordRate();

public:
	int GetPetCode() { return m_nPetCode; }
	int GetPetLevel() { return m_nPetLevel; }

	UserInfoCharData &GetUserCharData(){ return m_UserCharData; }

public:
	void SetUserID( const ioHashString& szUserID );
	void SetGrade( int iGradeLevel, int iGradeExpPer );
	void SetPartyExp( __int64 iPartyExp );
	void SetLadderExp( __int64 iLadderExp );
	void SetAwardExp( int iAwardLevel, int iAwardExp );
	void SetFishingExp( int iFishingLevel, int iFishingExp );
	void SetExcavationExp( int iExcavationLevel, int iExcavationExp );
	void SetHeroWin( int iWin );
	void SetHeroLose( int iLose );
	void SetHeroTitle( int iTitle );
	void SetHeroRank( int iRank );
	void SetBlockType( int iBlockType );
	void SetGuildIndex( DWORD dwGuildIndex );
	void SetRanking( int iRanking );
	void SetLadderPoint( int iLadderPoint );
	void SetCampPosition( int iCampPosition );
	void SetClassLevel( int iClassLevel );
	void SetClassType( int iClassType );
	void SetMedalItemType( MedalSyncInfoVec &rvMedalItemTypeVec );
	void SetGrowthLevel( IntVec &rvGrowthLevelVec );
	void SetKillDeathLevel( int iKillDeathLevel );
	void SetPetCode( int nPetCode );
	void SetPetLevel( int nPetLevel );
	void SetHouseOpen( bool bHouseOpen );
	void SetNamedTitle( DWORD dwNamedTitleCode, int nNamedTitleLevel, BYTE byNamedTitlePremium );
	void CalcRCharPossessionCount();
	inline void SetMaxSuccessionCount(int iMax ) { m_iMaxSuccessionCnt = iMax; }
	inline void SetCurSuccessionCount(int iCur ) { m_iCurSuccessionCnt = iCur; }

public:
	bool IsSyncInfo(){ return m_bSyncInfo; }
	void SetSync( bool bSync ){ m_bSyncInfo = bSync; }

public:
	bool IsGuildInfoChange();
	bool IsCampPosChange();

public:
	void CheckSyncTime(); 
	void SetSyncTime( DWORD dwTime ){ m_dwTCPSyncTime = dwTime; }

public:
	void SetForceUDPSync( bool bForceUDPSync ) { m_bForceUDPSync = bForceUDPSync; }

public:
	void SetSoldierSetCnt( IntVec &rvSoldierSetCntVec );
	int GetSoldierSetCnt( int nSetCode );

public:
	UserInfoNode( const ioHashString& szUserID, int iGradeLevel, int iGradeExpPer, __int64 iPartyExp, __int64 iLadderExp, int iAwardLevel, int iAwardExp,
		int iKillDeathLevel, int iFishingLevel, int iFishingExp, int iExcavationLevel, int iExcavationExp, int iHeroWin, int iHeroLose, 
		int iHeroTitle, int iHeroRank, int iBlockType, int iRanking, int iCampPosition, int iLadderPoint, int iClassLevel, int iClassType, 
		MedalSyncInfoVec &rvMedalItemTypeVec, IntVec &rvGrowthLevelVec, int nPetCode, int nPetLevel, DWORD dwNamedTitleCode, int nNamedTitleLevel, BYTE byNamedTitlePremium, IntVec &rvSoldierSetCntVec,
		int iCurSuccessionCnt, int iMaxSuccessionCnt );
	
	virtual ~UserInfoNode();
};

typedef std::vector< UserInfoNode *> vUserInfoNode;
typedef vUserInfoNode::iterator vUserInfoNode_iter;

class ioUserInfoMgr : public Singleton< ioUserInfoMgr >
{
protected:
	vUserInfoNode m_vUserInfoList;
	ioHashString  m_szOverUserID;

public:
	void SetUserInfo( const ioHashString& szUserID, int iGradeLevel, int iGradeExpPer, __int64 iPartyExp, __int64 iLadderExp, int iAwardLevel, int iAwardExp, 
		int iKillDeathLevel, int iFishingLevel, int iFishingExp, int iExcavationLevel, int iExcavationExp, int iHeroWin, int iHeroLose, 
		int iHeroTitle, int iHeroRank, int iBlockType, int iRanking, int iCampPosition, int iLadderPoint, int iClassLevel, int iClassType, 
		MedalSyncInfoVec &rvMedalItemTypeVec, IntVec &rvGrowthLevelVec, int nPetCode, int nPetLevel, bool bHouseOpen, DWORD dwNamedTitleCode, int nNamedTitleLevel, BYTE byNamedTitlePremium, IntVec &rvSoldierSetCntVec,
		int iCurSuccessionCnt, int iMaxSuccessionCnt );
	
	void ChangeName( const ioHashString& szUserID, const ioHashString& szChangeID );
	void SetGrade( const ioHashString& szUserID, int iGradeLevel );
	void SetGrade( const ioHashString& szUserID, int iGradeLevel, int iGradeExpPer );
	void SetPartyExp( const ioHashString& szUserID, __int64 iPartyExp );
	void SetLadderExp( const ioHashString& szUserID, __int64 iLadderExp );	
	void SetAwardExp( const ioHashString& szUserID, int iAwardLevel, int iAwardExp );
	void SetFishingExp( const ioHashString& szUserID, int iFishingLevel, int iFishingExp );
	void SetExcavationExp( const ioHashString& szUserID, int iExcavationLevel, int iExcavationExp );
	void SetHeroWin( const ioHashString& szUserID, int iWin );
	void SetHeroLose( const ioHashString& szUserID, int iLose );
	void SetHeroTitle( const ioHashString& szUserID, int iTitle );
	void SetHeroRank( const ioHashString& szUserID, int iRank );
	void SetBlockType( const ioHashString& szUserID, int iBlockType );
	void SetRanking( const ioHashString& szUserID, int iRanking );
	void SetGuildIndex( const ioHashString& szUserID, DWORD dwGuildIndex );
	void SetSyncTime( const ioHashString& szUserID, DWORD dwTime );
	void SetLadderPoint( const ioHashString& szUserID, int iLadderPoint );
	void SetEnterRoomUserSync( const ioHashString& szUserID, int iGradeLevel, int iLadderPoint );
	void SetCampPosition( const ioHashString& szUserID, int iCampPosition );
	void SetClassLevel( const ioHashString& szUserID, int iClassLevel );
	void SetClassType( const ioHashString& szUserID, int iClassType );
	void SetMedalItemType( const ioHashString& szUserID, MedalSyncInfoVec &rvMedalItemTypeVec );
	void SetGrowthLevel( const ioHashString& szUserID, IntVec &rvGrowthLevelVec );
	void SetKillDeathLevel( const ioHashString& szUserID, int iKillDeathLevel );

	// 유저의 용병 정보
	void SetCharacter( const ioHashString& szUserID, const CHARACTER &rkCharInfo, int iClassLevel );
	void SetEquipItem( const ioHashString& szUserID, int iClassType, const ITEM_DATA kEquipItem[MAX_INVENTORY] );
	void SetEquipMedal( const ioHashString& szUserID, int iClassType, MedalSyncInfoVec &rkEquipMedal );
	void SetCharGrowth( const ioHashString& szUserID, int iClassType, int kCharGrowth[MAX_CHAR_GROWTH], int kItemGrowth[MAX_ITEM_GROWTH], int iAwakeType );
	void SetCharAwake( const ioHashString& szUserID, int iClassType, int iAwakeType );
	void SetEquipPet( const ioHashString& szUserID, int nPetCode, int nPetLevel );
	void SetCharPowerUp( const ioHashString& szUserID, int iClassType, int iCharPowerUpGrade );
	void SetLeaderMotion( const ioHashString& szUserID, DWORD dwEtcItem );	
	void DeleteCharData( const ioHashString& szUserID, int iClassType );
	void ResetCharacterData( const ioHashString& szUserID );

public:
	UserInfoNode *GetUserInfoNode( const ioHashString& szID, bool bNetwork = true );
	UserInfoNode *GetUserInfoNodeExist( const ioHashString &szID );

public:
	void SetOverUserID( const ioHashString& szID ){ m_szOverUserID = szID;}
	ioHashString GetOverUserID(){ return m_szOverUserID; }
	void TestRender( int iXPos, int iYPos );

public:
	BlockType GetUserBlockType( const ioHashString &szID );
	DWORD GetGuildIndex( const ioHashString &szID );
	DWORD GetGuildMark( const ioHashString &szID );
	void GetGuildInfo( const ioHashString &szID, DWORD &dwGuildIndex, DWORD &dwGuildMark );
	int GetLadderPoint( const ioHashString &szID );
	int GetCampPosition( const ioHashString &szID );
	
public:
	void UserInfoCharDataSync( const ioHashString &szID, bool bOnline );
	void ApplyUserInfoCharDataSync( SP2Packet &rkPacket );
	void ApplyUserInfoCharSubDataSync( SP2Packet &rkPacket );

public:
	void SetEquipCostume( const ioHashString& szUserID, int iClassType, const CostumeEquipInfo kEquipCostume[MAX_INVENTORY] );
	void SetEquipCostumebySubType( const ioHashString& szUserID, int nClassType, int nSubType, int nCostumeCode, DWORD dwCustomMale, DWORD dwCustomFemale );

public:
	static ioUserInfoMgr &GetSingleton();
		
public:
	ioUserInfoMgr();
	virtual ~ioUserInfoMgr();	
};
#define g_UserInfoMgr ioUserInfoMgr::GetSingleton()
