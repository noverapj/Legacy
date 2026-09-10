#include "StdAfx.h"

#include "ioUserInfoMgr.h"
#include "../ioRSoldierInfoManager.h"

UserInfoCharData::UserInfoCharData()
{
	m_dwSyncCheckTime      = 0;
	m_bDataSync			   = false;
	m_iMaxVirtualCharCount = 0;
	m_bScrollSyncBlock	   = false;
	m_dwScrollSyncTime     = 0;
	m_dwLeaderMotion       = 0;
}

UserInfoCharData::~UserInfoCharData()
{
}

void UserInfoCharData::SelectLeader( CharInfoData &rkData )
{
	// 리더 결정
	if( m_LeaderCharData.m_CharInfo.m_class_type == 0 )
	{	
		m_LeaderCharData = rkData;
	}
	else
	{
		if( rkData.m_CharInfo.m_sLeaderType == CLT_LEADER )
		{	
			m_LeaderCharData = rkData;
		}
		else if( m_LeaderCharData.m_CharInfo.m_sLeaderType == CLT_NORMAL )
		{
			if( m_LeaderCharData.m_iClassLevel < rkData.m_iClassLevel )
			{	
				m_LeaderCharData = rkData;
			}
		}	
	}
	m_LeaderCharData.m_dwMotionEtcItem = m_dwLeaderMotion;
}

void UserInfoCharData::SelectLeader()
{
	m_LeaderCharData.Init();
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		SelectLeader( *iter );
	}
	m_LeaderCharData.m_bChangeData = true;
}

void UserInfoCharData::SetCharacter( const CHARACTER &rkCharInfo, int iClassLevel )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for( ; iter != m_CharDataList.end(); ++iter )
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == rkCharInfo.m_class_type )
		{
			rkData.m_iClassLevel	= iClassLevel;
			rkData.m_CharInfo		= rkCharInfo;
			rkData.m_bChangeData	= true;
			rkData.m_iAwakeType		= rkCharInfo.m_iAwakeType;

			SelectLeader( rkData );
			CharacterSort();
			return;
		}
	}

	CharInfoData kData;
	kData.m_iClassLevel		  = iClassLevel;
	kData.m_CharInfo		  = rkCharInfo;
	kData.m_bChangeData		  = true;

	m_CharDataList.push_back( kData );
	SelectLeader( kData );
	CharacterSort();
}

void UserInfoCharData::SetEquipItem( int iClassType, const ITEM_DATA kEquipItem[MAX_INVENTORY] )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			for(int i = 0;i < MAX_INVENTORY;i++)
			{
				rkData.m_EquipItemInfo[i] = kEquipItem[i];
				rkData.m_bChangeData = true;
			}			

			if( rkData.m_CharInfo.m_class_type == m_LeaderCharData.m_CharInfo.m_class_type )
			{
				m_LeaderCharData = rkData;
				SetLeaderMotion( m_dwLeaderMotion );
			}
			break;
		}
	}
}

void UserInfoCharData::SetEquipMedal( int iClassType, MedalSyncInfoVec &rkEquipMedal )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			rkData.m_EquipMedalItem.clear();
			if( !rkEquipMedal.empty() )
			{
				rkData.m_EquipMedalItem.resize( rkEquipMedal.size() );
				std::copy( rkEquipMedal.begin(), rkEquipMedal.end(), rkData.m_EquipMedalItem.begin() );
			}
			rkData.m_bChangeData = true;
			rkData.m_bSubDataSync= true;

			if( rkData.m_CharInfo.m_class_type == m_LeaderCharData.m_CharInfo.m_class_type )
			{
				m_LeaderCharData = rkData;
				SetLeaderMotion( m_dwLeaderMotion );
			}
			break;
		}
	}
}

void UserInfoCharData::SetCharGrowth( int iClassType, int kCharGrowth[MAX_CHAR_GROWTH], int kItemGrowth[MAX_ITEM_GROWTH], int iAwakeType )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			int i = 0;
			for(i = 0;i < MAX_CHAR_GROWTH;i++)
				rkData.m_CharGrowth[i] = kCharGrowth[i];
			for(i = 0;i < MAX_ITEM_GROWTH;i++)
				rkData.m_ItemGrowth[i] = kItemGrowth[i];
			rkData.m_bChangeData	= true;
			rkData.m_bSubDataSync	= true;
			rkData.m_iAwakeType		= iAwakeType;

			if( rkData.m_CharInfo.m_class_type == m_LeaderCharData.m_CharInfo.m_class_type )
			{
				m_LeaderCharData = rkData;
				SetLeaderMotion( m_dwLeaderMotion );
			}
			break;
		}
	}
}

void UserInfoCharData::SetCharAwake( int iClassType, int iAwakeType )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			rkData.m_iAwakeType	= iAwakeType;
		}
	}
}

void UserInfoCharData::SetCharPowerUp( int iClassType, int iCharPowerUpGrade )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			rkData.m_CharInfo.m_eCharPowerUpType = iCharPowerUpGrade;
		}
	}
}

void UserInfoCharData::SetLeaderMotion( DWORD dwEtcItem )
{
	m_dwLeaderMotion = dwEtcItem;
	m_LeaderCharData.m_dwMotionEtcItem = dwEtcItem;
	m_LeaderCharData.m_bChangeData = true;
}

void UserInfoCharData::ResetCharacterData()
{
	m_CharDataList.clear();
	m_LeaderCharData.Init();
	m_iMaxVirtualCharCount = 0;
}

void UserInfoCharData::DeleteCharData( int iClassType )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			m_CharDataList.erase( iter );
			break;
		}
	}

	// 리더 캐릭터 삭제되었음
	if( m_LeaderCharData.m_CharInfo.m_class_type == iClassType )
		SelectLeader();
}

void UserInfoCharData::CharacterSort()
{
	std::sort( m_CharDataList.begin(), m_CharDataList.end(), CharInfoSort() );
	UpdateUI();
}

void UserInfoCharData::UpdateUI()
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		rkData.m_bChangeData = true;
	}
}

int UserInfoCharData::GetMaxChar()
{
	return m_iMaxVirtualCharCount;
}

CharInfoData &UserInfoCharData::GetSlotChar( int iListArray )
{
	static CharInfoData kNoneData;
	if( !COMPARE( iListArray, 0, (int)m_CharDataList.size() ) ) return kNoneData;

	return m_CharDataList[iListArray];
}

CharInfoData &UserInfoCharData::GetSlotCharToClassType( int iClassType )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			return rkData;
		}
	}

	static CharInfoData kNoneData;
	return kNoneData;
}

void UserInfoCharData::SyncCharData( const ioHashString &rkUserName, bool bOnline )
{
	if( rkUserName.IsEmpty() )
		return;

	if( rkUserName == g_MyInfo.GetPublicID() )
	{
		m_bDataSync = true;
		m_iMaxVirtualCharCount = m_CharDataList.size();
		return;
	}

	if( m_dwSyncCheckTime != 0 && FRAMEGETTIME() - m_dwSyncCheckTime < 20000 )
		return;

	m_dwSyncCheckTime = FRAMEGETTIME();
	
	SP2Packet kPacket( CTPK_USER_CHAR_INFO_REFRESH );
	kPacket << rkUserName << 0 << CHARINFODATA_SYNC_COUNT;
	TCPNetwork::SendToServer( kPacket );

	// 첫 동기화할 때 데이터 초기화
	m_bDataSync = false;
	m_CharDataList.clear();
	m_LeaderCharData.Init();
	m_iMaxVirtualCharCount = 0;
}

void UserInfoCharData::ScrollSyncCharData( const ioHashString &rkUserName, int iListArray )
{
	if( !m_bDataSync ) return;
	if( rkUserName.IsEmpty() ) return;
	if( rkUserName == g_MyInfo.GetPublicID() )
	{
		m_bDataSync = true;
		m_iMaxVirtualCharCount = m_CharDataList.size();
		return;
	}

	if( m_bScrollSyncBlock )
	{
		if( FRAMEGETTIME() - m_dwScrollSyncTime < 10000 )
			return;
	}
	
	if( iListArray <= (int)m_CharDataList.size() )
		return;

	if( (int)m_CharDataList.size() < m_iMaxVirtualCharCount )
	{
		SP2Packet kPacket( CTPK_USER_CHAR_INFO_REFRESH );
		kPacket << rkUserName << (int)m_CharDataList.size() << CHARINFODATA_SYNC_COUNT;
		TCPNetwork::SendToServer( kPacket );
		m_bScrollSyncBlock = true;
		m_dwScrollSyncTime = FRAMEGETTIME();
	}
}

void UserInfoCharData::ApplyCharData( const ioHashString &rkUserName, SP2Packet &rkPacket )
{
	int iStartArray, iCurSize;
	rkPacket >> iStartArray >> m_iMaxVirtualCharCount >> iCurSize;

	if( iStartArray == 0 )
	{
		// 첫 동기화때 대표 캐릭 가져온다. - 대표 캐릭은 리스트에 포함시키지 않음
		bool bOK;
		rkPacket >> bOK;		
		if( bOK )
		{
			rkPacket >> m_dwLeaderMotion;

			int iClassLevel;
			CHARACTER kCharInfo;
			rkPacket >> kCharInfo >> iClassLevel;
			m_LeaderCharData.m_dwMotionEtcItem	= m_dwLeaderMotion;
			m_LeaderCharData.m_iClassLevel		= iClassLevel;
			m_LeaderCharData.m_CharInfo			= kCharInfo;
			m_LeaderCharData.m_bChangeData		= true;

			ITEM_DATA kEquipItem[MAX_INVENTORY];
			for(int k = 0;k < MAX_INVENTORY;k++)
			{
				rkPacket >> m_LeaderCharData.m_EquipItemInfo[k].m_item_code >> m_LeaderCharData.m_EquipItemInfo[k].m_item_reinforce 
						 >> m_LeaderCharData.m_EquipItemInfo[k].m_item_male_custom >> m_LeaderCharData.m_EquipItemInfo[k].m_item_female_custom;
			}
		}
	}
		
	for(int i = 0;i < iCurSize;i++)
	{
		bool bOK;
		rkPacket >> bOK;
		if( bOK )
		{
			int iClassLevel;
			CHARACTER kCharInfo;
			rkPacket >> kCharInfo >> iClassLevel;		

			SetCharacter( kCharInfo, iClassLevel );

			ITEM_DATA kEquipItem[MAX_INVENTORY];
			for(int k = 0;k < MAX_INVENTORY;k++)
			{
				rkPacket >> kEquipItem[k].m_item_code >> kEquipItem[k].m_item_reinforce >> kEquipItem[k].m_item_male_custom >> kEquipItem[k].m_item_female_custom;
			}
			SetEquipItem( kCharInfo.m_class_type, kEquipItem );
		}
	}
	m_bDataSync = true;
	m_bScrollSyncBlock = false;
}

void UserInfoCharData::SyncSubCharData( const ioHashString &rkUserName, int iClassType )
{
	if( rkUserName.IsEmpty() )
		return;

	if( rkUserName == g_MyInfo.GetPublicID() )
		return;

	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_bSubDataSync )
			continue;

		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			rkData.m_bSubDataSync = true;

			SP2Packet kPacket( CTPK_USER_CHAR_SUB_INFO_REFRESH );
			kPacket << rkUserName << iClassType;
			TCPNetwork::SendToServer( kPacket );
			return;
		}
	}
}

void UserInfoCharData::ApplyCharSubData( const ioHashString &rkUserName, SP2Packet &rkPacket )
{
	int iClassType;
	rkPacket >> iClassType;
		
	// 성장 정보
	int kCharGrowth[MAX_CHAR_GROWTH];
	for(int i = 0; i < MAX_CHAR_GROWTH; i++ )
		rkPacket >> kCharGrowth[i];

	int kItemGrowth[MAX_ITEM_GROWTH];
	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
		rkPacket >> kItemGrowth[i];
	
	//각성 정보
	byte eAwakeType;
	rkPacket >> eAwakeType;
	SetCharGrowth( iClassType, kCharGrowth, kItemGrowth, eAwakeType );

	// 메달 정보
	MedalSyncInfoVec kMedalList;
	int iMaxMedal;
	rkPacket >> iMaxMedal;
	for( int i = 0; i < iMaxMedal; ++i )
	{
		MedalSyncInfo cInfo;
		rkPacket >> cInfo.m_iItemType;
		kMedalList.push_back( cInfo );
	}
	int iMaxCustomMedal;
	rkPacket >> iMaxCustomMedal;
	for( int i = 0; i < iMaxCustomMedal; ++i )
	{
		MedalSyncInfo cInfo;
		rkPacket >> cInfo.m_iItemType >> cInfo.m_iCustomIndex;
		if( cInfo.m_iCustomIndex == 0 )
			continue;

		for( int j = 0; j < 8; j++ )
			rkPacket >> cInfo.m_iStat[j];

		kMedalList.push_back( cInfo );
	}

	SetEquipMedal( iClassType, kMedalList );
}

void UserInfoCharData::SetEquipCostume( int iClassType, const CostumeEquipInfo kEquipCostume[MAX_INVENTORY] )
{
	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for( ; iter!=m_CharDataList.end(); ++iter )
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			for( int i=0; i<MAX_INVENTORY; i++ )
			{
				rkData.m_CharInfo.m_EquipCostume[i] = kEquipCostume[i];
				rkData.m_bChangeData = true;
			}			

			if( rkData.m_CharInfo.m_class_type == m_LeaderCharData.m_CharInfo.m_class_type )
			{
				m_LeaderCharData = rkData;
				SetLeaderMotion( m_dwLeaderMotion );
			}
			break;
		}
	}
}

void UserInfoCharData::SetEquipCostumebySubType( int iClassType, int nSubType, int nCostumeCode, DWORD dwCustomMale, DWORD dwCustomFemale )
{
	if ( !COMPARE( nSubType, 0, MAX_INVENTORY ) )
		return;

	CharInfoDataVec::iterator iter = m_CharDataList.begin();
	for(;iter != m_CharDataList.end();++iter)
	{
		CharInfoData &rkData = *iter;
		if( rkData.m_CharInfo.m_class_type == iClassType )
		{
			rkData.m_CharInfo.m_EquipCostume[nSubType].m_CostumeCode = nCostumeCode;
			rkData.m_CharInfo.m_EquipCostume[nSubType].m_Costume_Male_Custom = dwCustomMale;
			rkData.m_CharInfo.m_EquipCostume[nSubType].m_Costume_Female_Custom = dwCustomFemale;
			rkData.m_bChangeData = true;

			if( rkData.m_CharInfo.m_class_type == m_LeaderCharData.m_CharInfo.m_class_type )
			{
				m_LeaderCharData.m_CharInfo.m_EquipCostume[nSubType].m_CostumeCode = nCostumeCode;
				m_LeaderCharData.m_CharInfo.m_EquipCostume[nSubType].m_Costume_Male_Custom = dwCustomMale;
				m_LeaderCharData.m_CharInfo.m_EquipCostume[nSubType].m_Costume_Female_Custom = dwCustomFemale;
				SetLeaderMotion( m_dwLeaderMotion );
			}
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////////
UserInfoNode::UserInfoNode( const ioHashString & szUserID, int iGradeLevel, int iGradeExpPer, __int64 iPartyExp, __int64 iLadderExp, 
	int iAwardLevel, int iAwardExp, int iKillDeathLevel, int iFishingLevel, int iFishingExp, int iExcavationLevel, int iExcavationExp, 
	int iHeroWin, int iHeroLose, int iHeroTitle, int iHeroRank, int iBlockType, int iRanking, int iCampPosition, int iLadderPoint, int iClassLevel, int iClassType, 
	MedalSyncInfoVec &rvMedalItemTypeVec, IntVec &rvGrowthLevelVec, int nPetCode, int nPetLevel, DWORD dwNamedTitleCode, int nNamedTitleLevel, BYTE byNamedTitlePremium, IntVec &rvSoldierSetCntVec,
	int iCurSuccessionCnt, int iMaxSuccessionCnt )
{
	m_iPartyLevel	  = 0;
	m_iLadderLevel    = 0;
	m_iAwardLevel	  = 0;
	m_iFishingLevel   = 0;
	m_iExcavationLevel= 0;

	m_dwGuildIndex  = 0;
	m_iLadderPoint  = 0;
	m_iClassLevel   = 0;
	m_iClassType    = 0;

	m_UserID		= szUserID;
	SetGrade( iGradeLevel, iGradeExpPer );
	SetPartyExp( iPartyExp );
	SetLadderExp( iLadderExp );
	SetAwardExp( iAwardLevel, iAwardExp );
	SetFishingExp( iFishingLevel, iFishingExp );
	SetExcavationExp( iExcavationLevel, iExcavationExp );
	SetHeroWin( iHeroWin );
	SetHeroLose( iHeroLose );
	SetHeroTitle( iHeroTitle );
	SetHeroRank( iHeroRank );
	SetBlockType( iBlockType );
	SetRanking( iRanking );
	SetCampPosition( iCampPosition );
	SetLadderPoint( iLadderPoint );
	SetClassLevel( iClassLevel );
	SetClassType( iClassType );
	SetMedalItemType( rvMedalItemTypeVec );
	SetGrowthLevel( rvGrowthLevelVec );
	SetKillDeathLevel( iKillDeathLevel );
	SetPetCode( nPetCode );
	SetPetLevel( nPetLevel );
	SetNamedTitle( dwNamedTitleCode, nNamedTitleLevel, byNamedTitlePremium );
	SetSoldierSetCnt( rvSoldierSetCntVec );

	m_dwTCPSyncTime = 0;
	m_dwUDPSyncTime = 0;
	m_bSyncInfo     = false;
	m_bGuildInfoChange = false;
	m_bCampChange   = false;
	m_bForceUDPSync    = false;
	m_iRSoldierPossessionCount = 0;

	m_iCurSuccessionCnt = iCurSuccessionCnt;
	m_iMaxSuccessionCnt = iMaxSuccessionCnt;
}

UserInfoNode::~UserInfoNode(){}

int UserInfoNode::GetPartyExp()
{
	int iTmpLevel     = m_iPartyLevel;
	__int64 iTmpExp   = m_iPartyExp;

	__int64 iPrevExp = g_LevelMgr.GetNextPartyupExp( iTmpLevel - 1 );
	__int64 iNeedExp = g_LevelMgr.GetNextPartyupExp( iTmpLevel ) - iPrevExp;

	if( iNeedExp > 0 )
	{
		__int64 iCurExp = ( iTmpExp - iPrevExp );
		float fRate = (float)iCurExp / (float)iNeedExp;

		return min( 99, (int)( FLOAT100 * fRate ) );
	}
	return 0;
}

int UserInfoNode::GetLadderExp()
{
	int iTmpLevel     = m_iLadderLevel;
	__int64 iTmpExp   = m_iLadderExp;

	__int64 iPrevExp = g_LevelMgr.GetNextLadderupExp( iTmpLevel - 1 );
	__int64 iNeedExp = g_LevelMgr.GetNextLadderupExp( iTmpLevel ) - iPrevExp;

	if( iNeedExp > 0 )
	{
		__int64 iCurExp = ( iTmpExp - iPrevExp );
		float fRate = (float)iCurExp / (float)iNeedExp;

		return min( 99, (int)( FLOAT100 * fRate ) );
	}
	return 0;
}

int UserInfoNode::GetAwardExp()
{
	int iNextExp = g_LevelMgr.GetNextAwardupExp( m_iAwardLevel );
	if( iNextExp > 0 )
	{
		float fRate = (float)m_iAwardExp / (float)iNextExp;
		return min( 99, (int)( FLOAT100 * fRate ) );
	}
	return 0;
}

int UserInfoNode::GetFishingExp()
{
	int iNextExp = g_LevelMgr.GetNextFishingLevelUpExp( m_iFishingLevel );
	if( iNextExp > 0 )
	{
		float fRate = (float)m_iFishingExpert / (float)iNextExp;
		return min( 99, (int)( FLOAT100 * fRate ) );
	}
	return 0;
}

int UserInfoNode::GetExcavationExp()
{
	int iNextExp = g_LevelMgr.GetNextExcavationLevelUpExp( m_iExcavationLevel );
	if( iNextExp > 0 )
	{
		float fRate = (float)m_iExcavationExpert / (float)iNextExp;
		return min( 99, (int)( FLOAT100 * fRate ) );
	}
	return 0;
}

float UserInfoNode::GetHeroRecordRate()
{
	return (float)m_iHeroWin / (float)max( 1, m_iHeroWin + m_iHeroLose );
}

DWORD UserInfoNode::GetGuildMark()
{
	if( GetGuildIndex() == 0 ) return 0;

	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( GetGuildIndex() );
	if( pGuildData )
		return pGuildData->GetGuildMark();
	return 0;
}

void UserInfoNode::SetUserID( const ioHashString& szUserID )
{
	m_UserID = szUserID;
}

void UserInfoNode::SetGrade( int iGradeLevel, int iGradeExpPer )
{
	m_iGradeLevel = iGradeLevel;
	m_iGradeExpPer= iGradeExpPer;	
}

void UserInfoNode::SetPartyExp( __int64 iPartyExp )
{
	m_iPartyExp = iPartyExp;
	for(m_iPartyLevel = 0;m_iPartyLevel < 99;m_iPartyLevel++)
	{
		__int64 iExp = g_LevelMgr.GetNextPartyupExp( m_iPartyLevel );
		if( iExp > m_iPartyExp )
			break;
	}
}

void UserInfoNode::SetLadderExp( __int64 iLadderExp )
{
	m_iLadderExp = iLadderExp;
	for(m_iLadderLevel = 0;m_iLadderLevel < 99;m_iLadderLevel++)
	{
		__int64 iExp = g_LevelMgr.GetNextLadderupExp( m_iLadderLevel );
		if( iExp > m_iLadderExp )
			break;
	}
}

void UserInfoNode::SetAwardExp( int iAwardLevel, int iAwardExp )
{
	m_iAwardLevel = iAwardLevel;
	m_iAwardExp   = iAwardExp;
}

void UserInfoNode::SetFishingExp( int iFishingLevel, int iFishingExp )
{
	m_iFishingLevel  = iFishingLevel;
	m_iFishingExpert = iFishingExp;
}

void UserInfoNode::SetExcavationExp( int iExcavationLevel, int iExcavationExp )
{
	m_iExcavationLevel  = iExcavationLevel;
	m_iExcavationExpert = iExcavationExp;
}

void UserInfoNode::SetHeroWin( int iWin )
{
	m_iHeroWin = iWin;
}

void UserInfoNode::SetHeroLose( int iLose )
{
	m_iHeroLose = iLose;
}

void UserInfoNode::SetHeroTitle( int iTitle )
{
	m_iHeroTitle = iTitle;
}

void UserInfoNode::SetHeroRank( int iRank )
{
	m_iHeroRank = iRank;
}

void UserInfoNode::SetBlockType( int iBlockType )
{
	m_eBlockType = (BlockType) iBlockType;
}

void UserInfoNode::SetRanking( int iRanking )
{
	m_iUserRank = iRanking;
}

void UserInfoNode::SetGuildIndex( DWORD dwGuildIndex )
{
	if( dwGuildIndex != m_dwGuildIndex )
		m_bGuildInfoChange = true;
	m_dwGuildIndex = dwGuildIndex;
}

void UserInfoNode::SetLadderPoint( int iLadderPoint )
{
	// 대회 포인트가 작아지지는 않는다. 단, 0으로 초기화는 된다.
	if( iLadderPoint == 0 || iLadderPoint > m_iLadderPoint )
	{
		m_iLadderPoint = iLadderPoint;
	}
}

void UserInfoNode::SetCampPosition( int iCampPosition )
{
	if( m_iCampPosition != iCampPosition )
		m_bCampChange = true;
	m_iCampPosition = iCampPosition;
}

void UserInfoNode::SetClassLevel( int iClassLevel )
{
	m_iClassLevel = iClassLevel;
}

void UserInfoNode::SetClassType( int iClassType )
{
	m_iClassType = iClassType;
}

void UserInfoNode::SetMedalItemType( MedalSyncInfoVec &rvMedalItemTypeVec )
{
	m_vMedalItemTypeVec = rvMedalItemTypeVec;
}

void UserInfoNode::SetGrowthLevel( IntVec &rvGrowthLevelVec )
{
	m_vGrowthLevelVec = rvGrowthLevelVec;
}

void UserInfoNode::SetKillDeathLevel( int iKillDeathLevel )
{
	m_iKillDeathLevel = iKillDeathLevel;
}

void UserInfoNode::SetPetCode( int nPetCode )
{
	m_nPetCode = nPetCode;
}

void UserInfoNode::SetPetLevel( int nPetLevel )
{
	m_nPetLevel = nPetLevel;
}

void UserInfoNode::SetHouseOpen( bool bHouseOpen )
{
	m_bHouseOpen = bHouseOpen;
}

void UserInfoNode::SetNamedTitle( DWORD dwNamedTitleCode, int nNamedTitleLevel, BYTE byNamedTitlePremium )
{
	m_dwNamedTitleCode = dwNamedTitleCode;
	m_nNamedTitleLevel = nNamedTitleLevel;
	m_byNamedTitlePremium = byNamedTitlePremium;
}

void UserInfoNode::CheckSyncTime()
{
	if( m_UserID.IsEmpty() )
		return;

	if( m_UserID == g_MyInfo.GetPublicID() )
		return;
	
	if( m_dwTCPSyncTime == 0 || FRAMEGETTIME() - m_dwTCPSyncTime > 600000 )         //10Min 마다 TCP 동기화
	{
		SetSyncTime( FRAMEGETTIME() );
		m_dwUDPSyncTime = FRAMEGETTIME();

		// 정보 동기화
		SP2Packet kPacket( CTPK_USER_INFO_REFRESH );
		kPacket << m_UserID;
		TCPNetwork::SendToServer( kPacket );
	}
	else if( m_bForceUDPSync == true || m_dwUDPSyncTime == 0 || FRAMEGETTIME() - m_dwUDPSyncTime > 60000 )      //1Min 마다 UDP 동기화
	{
		m_dwUDPSyncTime = FRAMEGETTIME();

		// 해당 유저에게 정보 요청
		SP2Packet kPacket( CUPK_USER_INFO_REFRESH );
		kPacket << g_MyInfo.GetPublicID();
		P2PNetwork::SendP2PUserInfoSync( m_UserID, kPacket );
	}

	m_bForceUDPSync = false;
}

bool UserInfoNode::IsGuildInfoChange()
{
	bool bReturn = m_bGuildInfoChange;
	m_bGuildInfoChange = false;
	return bReturn;
}

bool UserInfoNode::IsCampPosChange()
{
	bool bReturn = m_bCampChange;
	m_bCampChange = false;
	return bReturn;
}

void UserInfoNode::CalcRCharPossessionCount()
{
	m_iRSoldierPossessionCount = 0;
	UserInfoCharData &rkCharData = GetUserCharData();
	int iMaxChar = rkCharData.GetMaxChar();
	for ( int i=0; i<iMaxChar ; ++i )
	{
		const CharInfoData& rkInfo = rkCharData.GetSlotChar( i );
		if ( !g_RSoldierInfoManager.IsRSoldierType( rkInfo.m_CharInfo ) )
			continue;
		m_iRSoldierPossessionCount++;
	}
}

void UserInfoNode::SetSoldierSetCnt( IntVec &rvSoldierSetCntVec )
{
	m_vSoldierSetCntVec = rvSoldierSetCntVec;
}

int UserInfoNode::GetSoldierSetCnt( int nSetCode )
{
	if ( m_vSoldierSetCntVec.empty() || !COMPARE( nSetCode-1, 0, (int)m_vSoldierSetCntVec.size() ))
		return 0;

	return m_vSoldierSetCntVec[nSetCode-1];
}

template<> ioUserInfoMgr* Singleton< ioUserInfoMgr >::ms_Singleton = 0;

ioUserInfoMgr::ioUserInfoMgr()
{	
}

ioUserInfoMgr::~ioUserInfoMgr()
{
	vUserInfoNode_iter iter;
	for( iter=m_vUserInfoList.begin() ; iter!=m_vUserInfoList.end() ; ++iter )
	{
		UserInfoNode *pUserInfo = *iter;
		SAFEDELETE( pUserInfo );		
	}
	m_vUserInfoList.clear();
}

ioUserInfoMgr& ioUserInfoMgr::GetSingleton()
{
	return Singleton< ioUserInfoMgr >::GetSingleton();
}

UserInfoNode* ioUserInfoMgr::GetUserInfoNode( const ioHashString &szID, bool bNetwork )
{
	if( szID.IsEmpty() ) 
		return NULL;

	vUserInfoNode_iter iter;
	for( iter=m_vUserInfoList.begin() ; iter!=m_vUserInfoList.end() ; ++iter )
	{
		UserInfoNode *pUserInfo = *iter;
		if( pUserInfo == NULL )
			continue;

		if( pUserInfo->GetUserID() == szID )
		{
			if( bNetwork )
				pUserInfo->CheckSyncTime();

			return pUserInfo;
		}
	}

	if( szID == g_MyInfo.GetPublicID() )
		return NULL;
    
	MedalSyncInfoVec vMedalItemVec;
	IntVec vGrowthItemVec;
	IntVec vSoldierVec;
	UserInfoNode *pUserInfo = new UserInfoNode( szID, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, vMedalItemVec, vGrowthItemVec, 0, 0, 0, 0, 0, vSoldierVec, 0, 0 );

	m_vUserInfoList.push_back( pUserInfo );
	
	// 정보 요청
	if( bNetwork )
		pUserInfo->CheckSyncTime();

	return pUserInfo;
}

UserInfoNode * ioUserInfoMgr::GetUserInfoNodeExist( const ioHashString &szID )
{
	if( szID.IsEmpty() ) return NULL;
	if( szID == g_MyInfo.GetPublicID() ) return NULL;
	
	vUserInfoNode_iter iter;
	for( iter=m_vUserInfoList.begin() ; iter!=m_vUserInfoList.end() ; ++iter )
	{
		UserInfoNode *pUserInfo = *iter;
		if( pUserInfo == NULL ) continue;
		if( pUserInfo->GetUserID() == szID )
		{
			return pUserInfo;
		}
	}
	
	return NULL;
}

void ioUserInfoMgr::SetUserInfo( const ioHashString& szUserID, int iGradeLevel, int iGradeExpPer, __int64 iPartyExp, __int64 iLadderExp, 
	int iAwardLevel, int iAwardExp, int iKillDeathLevel, int iFishingLevel, int iFishingExp, int iExcavationLevel, int iExcavationExp, 
	int iHeroWin, int iHeroLose, int iHeroTitle, int iHeroRank, int iBlockType, int iRanking, int iCampPosition, int iLadderPoint, int iClassLevel, int iClassType, 
	MedalSyncInfoVec &rvMedalItemTypeVec, IntVec &rvGrowthLevelVec, int nPetCode, int nPetLevel, bool bHouseOpen, DWORD dwNamedTitleCode, int nNamedTitleLevel, BYTE byNamedTitlePremium, IntVec &rvSoldierSetCntVec,
	int iCurSuccessionCnt, int iMaxSuccessionCnt )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetGrade( iGradeLevel, iGradeExpPer );
		pUserInfoNode->SetPartyExp( iPartyExp );
		pUserInfoNode->SetLadderExp( iLadderExp );
		pUserInfoNode->SetAwardExp( iAwardLevel, iAwardExp );
		pUserInfoNode->SetFishingExp( iFishingLevel, iFishingExp );
		pUserInfoNode->SetExcavationExp( iExcavationLevel, iExcavationExp );
		pUserInfoNode->SetHeroWin( iHeroWin );
		pUserInfoNode->SetHeroLose( iHeroLose );
		pUserInfoNode->SetHeroTitle( iHeroTitle );
		pUserInfoNode->SetHeroRank( iHeroRank );
		pUserInfoNode->SetBlockType( iBlockType );
		pUserInfoNode->SetRanking( iRanking );
		pUserInfoNode->SetCampPosition( iCampPosition );
		pUserInfoNode->SetLadderPoint( iLadderPoint );
		pUserInfoNode->SetClassLevel( iClassLevel );
		pUserInfoNode->SetClassType( iClassType );
		pUserInfoNode->SetMedalItemType( rvMedalItemTypeVec );
		pUserInfoNode->SetGrowthLevel( rvGrowthLevelVec );
		pUserInfoNode->SetKillDeathLevel( iKillDeathLevel );
		pUserInfoNode->SetPetCode( nPetCode );
		pUserInfoNode->SetPetLevel( nPetLevel );
		pUserInfoNode->SetHouseOpen( bHouseOpen );
		pUserInfoNode->SetNamedTitle( dwNamedTitleCode, nNamedTitleLevel, byNamedTitlePremium );
		pUserInfoNode->SetSoldierSetCnt( rvSoldierSetCntVec );
		pUserInfoNode->SetCurSuccessionCount( iCurSuccessionCnt );
		pUserInfoNode->SetMaxSuccessionCount( iMaxSuccessionCnt );
		pUserInfoNode->SetSync( true );
	}
	else
	{
		UserInfoNode *NewInfo = new UserInfoNode( szUserID, iGradeLevel, iGradeExpPer, iPartyExp, iLadderExp, iAwardLevel, iAwardExp, 
			iKillDeathLevel, iFishingLevel, iFishingExp, iExcavationLevel, iExcavationExp, iHeroWin, iHeroLose,
			iHeroTitle, iHeroRank, iBlockType, iRanking, iCampPosition, iLadderPoint, iClassLevel, iClassType, 
			rvMedalItemTypeVec, rvGrowthLevelVec, nPetCode, nPetLevel, dwNamedTitleCode, nNamedTitleLevel, byNamedTitlePremium, rvSoldierSetCntVec,
			iCurSuccessionCnt, iMaxSuccessionCnt );
		NewInfo->SetSync( true );
		m_vUserInfoList.push_back( NewInfo );
	}
}

void ioUserInfoMgr::ChangeName( const ioHashString& szUserID, const ioHashString& szChangeID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetUserID( szChangeID );
}

void ioUserInfoMgr::SetGrade( const ioHashString& szUserID, int iGradeLevel )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetGrade( iGradeLevel, pUserInfoNode->GetGradeExp() );
}

void ioUserInfoMgr::SetGrade( const ioHashString& szUserID, int iGradeLevel, int iGradeExpPer )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetGrade( iGradeLevel, iGradeExpPer );
}

void ioUserInfoMgr::SetPartyExp( const ioHashString& szUserID, __int64 iPartyExp )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetPartyExp( iPartyExp );
}

void ioUserInfoMgr::SetLadderExp( const ioHashString& szUserID, __int64 iLadderExp )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetLadderExp( iLadderExp );
}

void ioUserInfoMgr::SetAwardExp( const ioHashString& szUserID, int iAwardLevel, int iAwardExp )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetAwardExp( iAwardLevel, iAwardExp );
}

void ioUserInfoMgr::SetFishingExp( const ioHashString& szUserID, int iFishingLevel, int iFishingExp )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetFishingExp( iFishingLevel, iFishingExp );
}

void ioUserInfoMgr::SetExcavationExp( const ioHashString& szUserID, int iExcavationLevel, int iExcavationExp )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetExcavationExp( iExcavationLevel, iExcavationExp );
}

void ioUserInfoMgr::SetHeroWin( const ioHashString& szUserID, int iWin )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetHeroWin( iWin );
}

void ioUserInfoMgr::SetHeroLose( const ioHashString& szUserID, int iLose )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetHeroLose( iLose );
}

void ioUserInfoMgr::SetHeroTitle( const ioHashString& szUserID, int iTitle )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetHeroTitle( iTitle );
}

void ioUserInfoMgr::SetHeroRank( const ioHashString& szUserID, int iRank )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetHeroRank( iRank );
}

void ioUserInfoMgr::SetBlockType( const ioHashString& szUserID, int iBlockType )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetBlockType( iBlockType );
}


void ioUserInfoMgr::SetRanking( const ioHashString& szUserID, int iRanking )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetRanking( iRanking );
}

void ioUserInfoMgr::SetGuildIndex( const ioHashString& szUserID, DWORD dwGuildIndex )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetGuildIndex( dwGuildIndex );
}

void ioUserInfoMgr::SetSyncTime( const ioHashString& szUserID, DWORD dwTime )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetSyncTime( dwTime );
}

void ioUserInfoMgr::SetLadderPoint( const ioHashString& szUserID, int iLadderPoint )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
		pUserInfoNode->SetLadderPoint( iLadderPoint );
}

void ioUserInfoMgr::SetEnterRoomUserSync( const ioHashString& szUserID, int iGradeLevel, int iLadderPoint )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetGrade( iGradeLevel, 0 );
		pUserInfoNode->SetLadderPoint( iLadderPoint );
	}
}

void ioUserInfoMgr::SetCampPosition( const ioHashString& szUserID, int iCampPosition )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetCampPosition( iCampPosition );
	}
}

void ioUserInfoMgr::SetClassLevel( const ioHashString& szUserID, int iClassLevel )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetClassLevel( iClassLevel );
	}
}

void ioUserInfoMgr::SetClassType( const ioHashString& szUserID, int iClassType )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetClassType( iClassType );
	}
}

void ioUserInfoMgr::SetMedalItemType( const ioHashString& szUserID, MedalSyncInfoVec &rvMedalItemTypeVec )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetMedalItemType( rvMedalItemTypeVec );
	}
}

void ioUserInfoMgr::SetGrowthLevel( const ioHashString& szUserID, IntVec &rvGrowthLevelVec )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetGrowthLevel( rvGrowthLevelVec );
	}
}

void ioUserInfoMgr::SetKillDeathLevel( const ioHashString& szUserID, int iKillDeathLevel )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetKillDeathLevel( iKillDeathLevel );
	}
}

// 유저의 용병 정보
void ioUserInfoMgr::SetCharacter( const ioHashString& szUserID, const CHARACTER &rkCharInfo, int iClassLevel )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetCharacter( rkCharInfo, iClassLevel );
	}
}

void ioUserInfoMgr::SetEquipItem( const ioHashString& szUserID, int iClassType, const ITEM_DATA kEquipItem[MAX_INVENTORY] )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetEquipItem( iClassType, kEquipItem );
	}
}

void ioUserInfoMgr::SetEquipMedal( const ioHashString& szUserID, int iClassType, MedalSyncInfoVec &rkEquipMedal )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetEquipMedal( iClassType, rkEquipMedal );
	}
}

void ioUserInfoMgr::SetCharGrowth( const ioHashString& szUserID, int iClassType, int kCharGrowth[MAX_CHAR_GROWTH], int kItemGrowth[MAX_ITEM_GROWTH], int iAwakeType )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetCharGrowth( iClassType, kCharGrowth, kItemGrowth, iAwakeType );
	}
}

void ioUserInfoMgr::SetCharAwake( const ioHashString& szUserID, int iClassType, int iAwakeType )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetCharAwake( iClassType, iAwakeType );
	}
}

void ioUserInfoMgr::SetEquipPet( const ioHashString& szUserID, int nPetCode, int nPetLevel )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		pUserInfoNode->SetPetCode( nPetCode );
		pUserInfoNode->SetPetLevel( nPetLevel );
	}
}

void ioUserInfoMgr::SetCharPowerUp( const ioHashString& szUserID, int iClassType, int iCharPowerUpGrade )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetCharPowerUp( iClassType, iCharPowerUpGrade );
	}
}

void ioUserInfoMgr::SetLeaderMotion( const ioHashString& szUserID, DWORD dwEtcItem )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetLeaderMotion( dwEtcItem );
	}
}

void ioUserInfoMgr::DeleteCharData( const ioHashString& szUserID, int iClassType )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.DeleteCharData( iClassType );
	}
}

void ioUserInfoMgr::ResetCharacterData( const ioHashString& szUserID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.ResetCharacterData();
	}
}

BlockType ioUserInfoMgr::GetUserBlockType( const ioHashString &szID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
		return pUserInfoNode->GetBlockType();

	return BKT_NONE;
}

DWORD ioUserInfoMgr::GetGuildIndex( const ioHashString &szID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
		return pUserInfoNode->GetGuildIndex();

	return 0;
}

DWORD ioUserInfoMgr::GetGuildMark( const ioHashString &szID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
		return pUserInfoNode->GetGuildMark();

	return 0;
}

void ioUserInfoMgr::GetGuildInfo( const ioHashString &szID, DWORD &dwGuildIndex, DWORD &dwGuildMark )
{
	dwGuildIndex = dwGuildMark = 0;
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
	{
		dwGuildIndex = pUserInfoNode->GetGuildIndex();
		dwGuildMark  = pUserInfoNode->GetGuildMark();
	}
}

int ioUserInfoMgr::GetLadderPoint( const ioHashString &szID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
		return pUserInfoNode->GetLadderPoint();

	return 0;
}

int ioUserInfoMgr::GetCampPosition( const ioHashString &szID )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
		return pUserInfoNode->GetCampPosition();

	return 0;
}

void ioUserInfoMgr::TestRender( int iXPos, int iYPos )
{
	if( m_szOverUserID.IsEmpty() ) return;

	UserInfoNode *pUserInfoNode = GetUserInfoNode( m_szOverUserID );
	if( !pUserInfoNode ) return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 10, iYPos + 10, 0.50f, STR(1), pUserInfoNode->GetUserID().c_str(), pUserInfoNode->GetGradeLevel(), pUserInfoNode->GetGradeExp() );
	g_FontMgr.PrintText( iXPos + 10, iYPos + 24, 0.50f, STR(2), pUserInfoNode->GetPartyLevel(), pUserInfoNode->GetPartyExp() );	
	g_FontMgr.PrintText( iXPos + 10, iYPos + 52, 0.50f, STR(3), pUserInfoNode->GetAwardLevel(), pUserInfoNode->GetAwardExp() );
	g_FontMgr.PrintText( iXPos + 10, iYPos + 80, 0.50f, STR(5), pUserInfoNode->GetBlockType() );
	g_FontMgr.PrintText( iXPos + 10, iYPos + 94, 0.50f, STR(6), m_vUserInfoList.size() );
	m_szOverUserID.Clear();
}

void ioUserInfoMgr::UserInfoCharDataSync( const ioHashString &szID, bool bOnline )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SyncCharData( szID, bOnline );
	}
}

void ioUserInfoMgr::ApplyUserInfoCharDataSync( SP2Packet &rkPacket )
{
	ioHashString kUserID;
	rkPacket >> kUserID;

	UserInfoNode *pUserInfoNode = GetUserInfoNode( kUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.ApplyCharData( kUserID, rkPacket );
		pUserInfoNode->CalcRCharPossessionCount();
	}
}

void ioUserInfoMgr::ApplyUserInfoCharSubDataSync( SP2Packet &rkPacket )
{
	ioHashString kUserID;
	rkPacket >> kUserID;

	UserInfoNode *pUserInfoNode = GetUserInfoNode( kUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.ApplyCharSubData( kUserID, rkPacket );
	}
}

void ioUserInfoMgr::SetEquipCostume( const ioHashString& szUserID, int iClassType, const CostumeEquipInfo kEquipCostume[MAX_INVENTORY] )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetEquipCostume( iClassType, kEquipCostume );
	}
}

void ioUserInfoMgr::SetEquipCostumebySubType( const ioHashString& szUserID, int nClassType, int nSubType, int nCostumeCode, DWORD dwCustomMale, DWORD dwCustomFemale )
{
	UserInfoNode *pUserInfoNode = GetUserInfoNode( szUserID, false );
	if( pUserInfoNode )
	{
		UserInfoCharData &rkCharData = pUserInfoNode->GetUserCharData();
		rkCharData.SetEquipCostumebySubType( nClassType, nSubType, nCostumeCode, dwCustomMale, dwCustomFemale );
	}
}