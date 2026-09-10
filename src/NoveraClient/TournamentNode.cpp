
#include "stdafx.h"

#include "TournamentNode.h"
#include "ioTournamentTeamManager.h"

TournamentNode::TournamentNode()
{
	m_dwIndex					= 0;
	m_dwOwnerIndex				= 0;
	m_dwStartDate				= 0;
	m_dwEndDate					= 0;
	m_Type						= 0;
	m_State						= 0;
	m_dwStateEndDate			= 0;
	m_TournamentStartTeamCount	= 0;
	m_TournamentMaxRound		= 0;
	m_MaxPlayer					= 0;
	m_iPlayMode					= 0;
	m_WinTeamCamp				= CAMP_NONE;
	m_dwCustomBannerB			= 0;
	m_dwCustomBannerS			= 0;
	m_iExpectedTeamCount        = 0;
	m_dwReguralTotalCheerCount	= 0;
	m_bReguralDisableTournament	= false;
	m_dwAdjustCheerPeso			= 0;

	m_pResBanner		= NULL;
	m_pResCampBanner	= NULL;
	m_pResBannerSmall	= NULL;
	m_pResBannerMark	= NULL;
	m_pResBlueIcon		= NULL;
	m_pResRedIcon		= NULL;
	m_pResBlueSmallIcon = NULL;
	m_pResRedSmallIcon	= NULL;

	m_dwDataSyncTimer		  = 0;
	m_bUpdateMainUI			  = false;
	m_bUpdateSubUI 			  = false;
	m_bUpdateInfoUI			  = false;
	m_bUpdateDetailUI		  = false;
	
	m_dwScheduleSyncTimer	  = 0;
	m_bUpdateScheduleUI       = false;

	m_dwCustomRewardSyncTimer = 0;
	m_bUpdateCustomRewardUI   = false;
	m_CustomProgressType      = CPT_NONE;

	m_AllocateTeamDataMap.clear();	
	m_dwAllocateTotalCount = 0;

	m_bLogSend = false;
}

TournamentNode::~TournamentNode()
{
	SAFEDELETE( m_pResBanner );
	SAFEDELETE( m_pResCampBanner );
	SAFEDELETE( m_pResBannerSmall );
	SAFEDELETE( m_pResBannerMark );
	SAFEDELETE( m_pResBlueIcon );
	SAFEDELETE( m_pResRedIcon );
	SAFEDELETE( m_pResBlueSmallIcon );
	SAFEDELETE( m_pResRedSmallIcon );

	RewardDataMap::iterator iCreator1 = m_RewardDataMap.begin();
	for(;iCreator1 != m_RewardDataMap.end();iCreator1++)
	{
		RewardData &rkData = iCreator1->second;
		rkData.m_TitleList.clear();
	}
	m_RewardDataMap.clear();

	TournamentRoundMap::iterator iCreator2 = m_TournamentRoundMap.begin();
	for(;iCreator2 != m_TournamentRoundMap.end();++iCreator2)
	{
		TournamentRoundData &rkRoundData = iCreator2->second;
		rkRoundData.m_TeamList.clear();
	}
	m_TournamentRoundMap.clear();

	DestroyRewardData();
	m_AllocateTeamDataMap.clear();	
	m_TeamCreateRequestUserVec.clear();
	m_CustomRewardList.clear();
}

void TournamentNode::CreateRegularData( int iReguralResourceType )
{
	if( iReguralResourceType == 0 )
		return;

 	char szTitle[MAX_PATH] = "";
	sprintf_s( szTitle, sizeof( szTitle ), "resource%d", iReguralResourceType );

	ioINILoader_e kLoader( "config/sp2_regular_tournament.ini" );
	kLoader.SetTitle( szTitle );

	char szBuf[MAX_PATH];
	SAFEDELETE( m_pResBanner );
	kLoader.LoadString_e( "main_banner", "", szBuf, MAX_PATH );
	m_pResBanner = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	//
	SAFEDELETE( m_pResCampBanner );
	kLoader.LoadString_e( "main_camp_banner", "", szBuf, MAX_PATH );
	m_pResCampBanner = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	//
	SAFEDELETE( m_pResBannerSmall );
	kLoader.LoadString_e( "main_banner_small", "", szBuf, MAX_PATH );
	m_pResBannerSmall = g_UIImageSetMgr.CreateImageByFullName( szBuf );

	//
	SAFEDELETE( m_pResBannerMark );
	kLoader.LoadString_e( "main_banner_mark", "", szBuf, MAX_PATH );
	m_pResBannerMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	
	//
	SAFEDELETE( m_pResBlueIcon );
	kLoader.LoadString_e( "blue_camp_icon", "", szBuf, MAX_PATH );
	m_pResBlueIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	m_szBlueIconName = szBuf;

	//
	SAFEDELETE( m_pResRedIcon );
	kLoader.LoadString_e( "red_camp_icon", "", szBuf, MAX_PATH );
	m_pResRedIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	m_szRedIconName = szBuf;

	//
	SAFEDELETE( m_pResBlueSmallIcon );
	kLoader.LoadString_e( "blue_camp_small_icon", "", szBuf, MAX_PATH );
	m_pResBlueSmallIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	m_szBlueSmallIconName = szBuf;

	//
	SAFEDELETE( m_pResRedSmallIcon );
	kLoader.LoadString_e( "red_camp_small_icon", "", szBuf, MAX_PATH );
	m_pResRedSmallIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	m_szRedISmallconName = szBuf;

	kLoader.LoadString_e( "main_title", "", szBuf, MAX_PATH );
	m_szTournamentTitle = szBuf;

	kLoader.LoadString_e( "blue_camp_title", "", szBuf, MAX_PATH );
	m_szBlueCampName = szBuf;
	kLoader.LoadString_e( "red_camp_title", "", szBuf, MAX_PATH );
	m_szRedCampName	 = szBuf;
		
	kLoader.LoadString_e( "result_win_motion", "win_motion_02.ani", szBuf, MAX_PATH );
	m_szResultWinMotion = szBuf;
	kLoader.LoadString_e( "result_lose_motion", "draw_motion_00.ani", szBuf, MAX_PATH );
	m_szResultLoseMotion = szBuf;

	kLoader.LoadString_e( "disable_tournament_ment", "", szBuf, MAX_PATH );
	m_szDisableTournamentMent = szBuf;


	m_bLogSend = kLoader.LoadBool_e( "log_print", true );

	m_dwAdjustCheerPeso = kLoader.LoadInt_e( "adjust_cheer_peso", 0 );

	// 보상 텍스트 : 이건 외부로 빼야하지 않을까 - 코드로 만들어진 선물 네이밍이 적합한듯
	int iMaxReward = kLoader.LoadInt_e( "max_reward", 0 );
	for(int i = 0;i < iMaxReward;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_s_e( szKey, "reward_%d_count", i + 1 );
	
		RewardData kRewardData;
		int iMaxCount = kLoader.LoadInt( szKey, 0 );
		for(int j = 0;j < iMaxCount;j++)
		{
			sprintf_s_e( szKey, "reward_%d_%d", i + 1, j + 1 );

			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			kRewardData.m_TitleList.push_back( szBuf );
		}
		m_RewardDataMap.insert( RewardDataMap::value_type( i + 1, kRewardData ) );
	}
}

void TournamentNode::CreateResultRewardData( int iReguralDateType )
{
	if( iReguralDateType == 0 )
		return;

	char szTitle[MAX_PATH] = "";
	sprintf_s( szTitle, sizeof( szTitle ), "%d", iReguralDateType );

	ioINILoader_e kLoader( "config/sp2_tournament_detail_reward.ini" );
	kLoader.SetTitle( szTitle );

	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];
	
	//대회 보상
	int iMaxReward = kLoader.LoadInt_e( "result_reward_max", 0 );

	for( int i = 0; i < iMaxReward; ++i )
	{
		sprintf_s_e( szKey, "result_reward%d_text_count", i + 1 );
		int iMax = kLoader.LoadInt( szKey, 0 );

		ResultData Result;
		sprintf_s_e( szKey, "result_reward%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Result.m_szTtitle = szBuf;
		
		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_reward%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Result.m_szResultList.push_back( szBuf );			
		}
		
		sprintf_s_e( szKey, "result_reward%d_icon_count", i + 1 );
		iMax = kLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_reward%d_icon%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			
			Result.m_vIconList.push_back( pImg );
		}

		m_vRewardResultDataMap.insert( ResultDataMap::value_type( i + 1 , Result ) );
	}

	//진영전 보상
	int iMaxCamp = kLoader.LoadInt_e( "result_camp_max", 0 );

	for( int i = 0; i < iMaxCamp; ++i )
	{		
		sprintf_s_e( szKey, "result_camp%d_text_count", i + 1 );
		int iMax = kLoader.LoadInt( szKey, 0 );

		ResultData Result;
		sprintf_s_e( szKey, "result_camp%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Result.m_szTtitle = szBuf;

		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_camp%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Result.m_szResultList.push_back( szBuf );
		}

		sprintf_s_e( szKey, "result_camp%d_icon_count", i + 1 );
		iMax = kLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_camp%d_icon%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			Result.m_vIconList.push_back( pImg );
		}

		m_vCampResultDataMap.insert( ResultDataMap::value_type( i + 1 , Result ) );
	}

	//우승응원 보상
	int iMaxCheer = kLoader.LoadInt_e( "result_cheer_max", 0 );

	for( int i = 0; i < iMaxCheer; ++i )
	{		
		sprintf_s_e( szKey, "result_cheer%d_text_count", i + 1 );
		int iMax = kLoader.LoadInt( szKey, 0 );

		ResultData Result;
		sprintf_s_e( szKey, "result_cheer%d_title", i + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		Result.m_szTtitle = szBuf;

		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_cheer%d_text%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			Result.m_szResultList.push_back( szBuf );
		}

		sprintf_s_e( szKey, "result_cheer%d_icon_count", i + 1 );
		iMax = kLoader.LoadInt( szKey, 0 );
		for( int j = 0; j < iMax; ++j )
		{
			sprintf_s_e( szKey, "result_cheer%d_icon%d", i + 1, j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			Result.m_vIconList.push_back( pImg );
		}

		m_vCheerResultDataMap.insert( ResultDataMap::value_type( i + 1 , Result ) );
	}
}

void TournamentNode::CreateCustomData( DWORD dwBannerB, DWORD dwBannerS )
{
	ioINILoader_e kLoader( "config/sp2_custom_tournament.ini" );
	kLoader.SetTitle_e( "default" );
	char szBuf[MAX_PATH];
	char szKey[MAX_PATH];

	SAFEDELETE( m_pResBanner );
	SAFEDELETE( m_pResBannerMark );

	sprintf_s_e( szKey, "main_banner%d", dwBannerB );
	kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_pResBanner = g_UIImageSetMgr.CreateImageByFullName( szBuf );

	sprintf_s_e( szKey, "mark_banner%d", dwBannerS );
	kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
	m_pResBannerMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );
}

void TournamentNode::DestroyRewardData()
{
	ResultDataMap::iterator iCreator3 = m_vRewardResultDataMap.begin();
	for(;iCreator3 != m_vRewardResultDataMap.end(); ++iCreator3 )
	{		
		ResultData Result = iCreator3->second;

		ResultIconList::iterator iCreator4 = Result.m_vIconList.begin();
		for(;iCreator4 != Result.m_vIconList.end(); ++iCreator4 )
		{
			ioUIRenderImage* pImage = (*iCreator4);
			SAFEDELETE( pImage );
		}
	}
	m_vRewardResultDataMap.clear();

	ResultDataMap::iterator iCreator5 = m_vCampResultDataMap.begin();
	for(;iCreator5 != m_vCampResultDataMap.end(); ++iCreator5 )
	{		
		ResultData Result = iCreator5->second;

		ResultIconList::iterator iCreator6 = Result.m_vIconList.begin();
		for(;iCreator6 != Result.m_vIconList.end(); ++iCreator6 )
		{
			ioUIRenderImage* pImage = (*iCreator6);
			SAFEDELETE( pImage );
		}
	}
	m_vCampResultDataMap.clear();

	ResultDataMap::iterator iCreator7 = m_vCheerResultDataMap.begin();
	for(;iCreator7 != m_vCheerResultDataMap.end(); ++iCreator7 )
	{		
		ResultData Result = iCreator7->second;

		ResultIconList::iterator iCreator8 = Result.m_vIconList.begin();
		for(;iCreator8 != Result.m_vIconList.end(); ++iCreator8 )
		{
			ioUIRenderImage* pImage = (*iCreator8);
			SAFEDELETE( pImage );
		}
	}
	m_vCheerResultDataMap.clear();
}

TournamentNode::TournamentRoundData &TournamentNode::GetTournamentRound( DWORD dwRound )
{
	TournamentRoundMap::iterator iCreator = m_TournamentRoundMap.find( dwRound );
	if( iCreator == m_TournamentRoundMap.end() )
	{
		TournamentRoundData kRoundData;
		m_TournamentRoundMap.insert( TournamentRoundMap::value_type( dwRound, kRoundData ) );
	}

	iCreator = m_TournamentRoundMap.find( dwRound );
	return iCreator->second;
}

void TournamentNode::SetTournamentRoundTeam( TournamentNode::TournamentRoundData &rkRoundData, RoundTeamData &rkTeamData )
{
	RoundTeamVec::iterator iter = rkRoundData.m_TeamList.begin();
	for(;iter != rkRoundData.m_TeamList.end();iter++)
	{
		RoundTeamData &rkRoundTeamData = *iter;
		if( rkRoundTeamData.m_Position != rkTeamData.m_Position ) continue;

		rkRoundTeamData.m_dwTeamIndex = rkTeamData.m_dwTeamIndex;
		rkRoundTeamData.m_szTeamName  = rkTeamData.m_szTeamName;
		rkRoundTeamData.m_TourPos     = rkTeamData.m_TourPos;
		rkRoundTeamData.m_CampPos     = rkTeamData.m_CampPos;
		return;
	}

	rkRoundData.m_TeamList.push_back( rkTeamData );
}

TournamentNode::TournamentRoundSyncTable &TournamentNode::GetTournamentRoundSyncTable( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray )
{
	TournamentRoundSyncVec::iterator iter = m_TournamentRoundSyncList.begin();
	for(;iter != m_TournamentRoundSyncList.end();++iter)
	{
		TournamentRoundSyncTable &rkSyncTable = *iter;
		if( rkSyncTable.m_iStartRound != iStartRound ) continue;
		if( rkSyncTable.m_iTotalRoundCount != iTotalRoundCount ) continue;
		if( rkSyncTable.m_iRoundTeamCount != iRoundTeamCount ) continue;
		if( rkSyncTable.m_iStartRountTeamArray != iStartRountTeamArray ) continue;

		return rkSyncTable;
	}

	// 없으면 생성!!!!
	TournamentRoundSyncTable kSyncTable;
	kSyncTable.m_iStartRound = iStartRound;
	kSyncTable.m_iTotalRoundCount = iTotalRoundCount;
	kSyncTable.m_iRoundTeamCount = iRoundTeamCount;
	kSyncTable.m_iStartRountTeamArray = iStartRountTeamArray;
	kSyncTable.m_bDataChange = false;
	kSyncTable.m_dwRoundSyncTimer = 0;
	m_TournamentRoundSyncList.insert( m_TournamentRoundSyncList.begin(), kSyncTable );
	return *m_TournamentRoundSyncList.begin();
}

ioUIRenderImage *TournamentNode::GetResCampIcon( BYTE CampPos )
{
	if( m_Type == TYPE_REGULAR )
	{
		if( CampPos == CAMP_BLUE )
		{
			if( m_pResBlueIcon )
				m_pResBlueIcon->SetScale( FLOAT1 );
			return m_pResBlueIcon;
		}

		if( m_pResRedIcon )
			m_pResRedIcon->SetScale( FLOAT1 );
		return m_pResRedIcon;
	}

	// 유저 대회는 다른 아이콘 사용
	return NULL;
}

ioUIRenderImage *TournamentNode::GetResCampSmallIcon( BYTE CampPos )
{
	if( m_Type == TYPE_REGULAR )
	{
		if( CampPos == CAMP_BLUE )
		{
			if( m_pResBlueSmallIcon )
				m_pResBlueSmallIcon->SetScale( FLOAT1 );
			return m_pResBlueSmallIcon;
		}

		if( m_pResRedSmallIcon )
			m_pResRedSmallIcon->SetScale( FLOAT1 );
		return m_pResRedSmallIcon;
	}

	// 유저 대회는 다른 아이콘 사용
	return NULL;
}

ioUIRenderImage *TournamentNode::GetCampIcon( BYTE CampPos )
{
	if( m_Type == TYPE_REGULAR )
	{
		if( CampPos == CAMP_BLUE )
		{			
			return g_UIImageSetMgr.CreateImageByFullName( m_szBlueIconName );
		}

		return g_UIImageSetMgr.CreateImageByFullName( m_szRedIconName );
	}
	return NULL;
}

ioUIRenderImage *TournamentNode::GetCampSmallIcon( BYTE CampPos )
{
	static ioHashString kNone;
	if( m_Type == TYPE_REGULAR )
	{
		if( CampPos == CAMP_BLUE )
		{
			return g_UIImageSetMgr.CreateImageByFullName( m_szBlueSmallIconName );
		}
		
		return g_UIImageSetMgr.CreateImageByFullName( m_szRedISmallconName );
	}
	
	return NULL;
}

ioHashString &TournamentNode::GetCampName( BYTE CampPos )
{
	if( m_Type == TYPE_REGULAR )
	{
		if( CampPos == CAMP_BLUE )
		{
			return m_szBlueCampName;
		}
	}
	return m_szRedCampName;
}

ioHashStringVec &TournamentNode::GetRewardTitle( int iRound )
{
	static ioHashStringVec kNone;
	RewardDataMap::iterator iter = m_RewardDataMap.find( iRound );
	if( iter == m_RewardDataMap.end() )
		return kNone;

	RewardData &rkData = iter->second;
	return rkData.m_TitleList;
}

ioHashString& TournamentNode::GetResultRewardTitle( int iRound )
{
	static ioHashString kNone;
	ResultDataMap::iterator iter = m_vRewardResultDataMap.find( iRound );
	if( iter == m_vRewardResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szTtitle;
}

ioHashStringVec& TournamentNode::GetResultRewardTextList( int iRound )
{
	static ioHashStringVec kNone;
	ResultDataMap::iterator iter = m_vRewardResultDataMap.find( iRound );
	if( iter == m_vRewardResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szResultList;
}

ResultIconList& TournamentNode::GetResultRewardIconList( int iRound )
{
	static ResultIconList kNone;
	ResultDataMap::iterator iter = m_vRewardResultDataMap.find( iRound );
	if( iter == m_vRewardResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_vIconList;
}

ioHashString& TournamentNode::GetResultCampTitle( int iIdx )
{
	static ioHashString kNone;
	ResultDataMap::iterator iter = m_vCampResultDataMap.find( iIdx );
	if( iter == m_vCampResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szTtitle;
}

ioHashStringVec& TournamentNode::GetResultCampTextList( int iIdx )
{
	static ioHashStringVec kNone;
	ResultDataMap::iterator iter = m_vCampResultDataMap.find( iIdx );
	if( iter == m_vCampResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szResultList;
}

ResultIconList& TournamentNode::GetResultCampIconList( int iIdx )
{
	static ResultIconList kNone;
	ResultDataMap::iterator iter = m_vCampResultDataMap.find( iIdx );
	if( iter == m_vCampResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_vIconList;
}

ioHashString& TournamentNode::GetResultCheerTitle( int iIdx )
{
	static ioHashString kNone;
	ResultDataMap::iterator iter = m_vCheerResultDataMap.find( iIdx );
	if( iter == m_vCheerResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szTtitle;
}

ioHashStringVec& TournamentNode::GetResultCheerTextList( int iIdx )
{
	static ioHashStringVec kNone;
	ResultDataMap::iterator iter = m_vCheerResultDataMap.find( iIdx );
	if( iter == m_vCheerResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_szResultList;
}

ResultIconList& TournamentNode::GetResultCheerIconList( int iIdx )
{
	static ResultIconList kNone;
	ResultDataMap::iterator iter = m_vCheerResultDataMap.find( iIdx );
	if( iter == m_vCheerResultDataMap.end() )
		return kNone;

	ResultData &rkData = iter->second;
	return rkData.m_vIconList;
}

void TournamentNode::GetCustomRewardData( DWORD dwTourIndex, BYTE TourPos, DWORDVec &rkReturnReward )
{
	rkReturnReward.clear();
	int iTourArray = (int)TourPos - 1;
	if( COMPARE( iTourArray, 0, (int)m_CustomRewardList.size() ) )
	{
		if(m_CustomRewardList[iTourArray].m_dwRewardA != 0) rkReturnReward.push_back( m_CustomRewardList[iTourArray].m_dwRewardA );
		if(m_CustomRewardList[iTourArray].m_dwRewardB != 0) rkReturnReward.push_back( m_CustomRewardList[iTourArray].m_dwRewardB );
		if(m_CustomRewardList[iTourArray].m_dwRewardC != 0) rkReturnReward.push_back( m_CustomRewardList[iTourArray].m_dwRewardC );
		if(m_CustomRewardList[iTourArray].m_dwRewardD != 0) rkReturnReward.push_back( m_CustomRewardList[iTourArray].m_dwRewardD );
	}
}

TournamentNode::AllocateSyncTable& TournamentNode::GetAllocateSyncTable( DWORD dwPage )
{	
	AllocateSyncTableMap::iterator iter = m_AllocateSyncTableMap.find( dwPage );
	if( iter != m_AllocateSyncTableMap.end() )
	{
		AllocateSyncTable& rkTable = iter->second;
		return rkTable;
	}

	AllocateSyncTable Table;
	m_AllocateSyncTableMap.insert( AllocateSyncTableMap::value_type( dwPage, Table ) );
	iter = m_AllocateSyncTableMap.find( dwPage );

	return iter->second; 
}

bool TournamentNode::IsUnAllocatePageSync( DWORD dwPage )
{
	if( GetState() == STATE_TEAM_APP ) // 팀 모집 상태에서는 동기화 데이터 필요없음
		return true;

	AllocateSyncTable& rkTable = GetAllocateSyncTable( dwPage );

	if( rkTable.m_dwSyncTime == 0 )
		return false;
	
	if( FRAMEGETTIME() - rkTable.m_dwSyncTime < DATA_SYNC_TIME )
		return true;

	return false;
}

bool TournamentNode::IsUnAllocatePageUpdate( DWORD dwPage )
{
	AllocateSyncTable& rkTable = GetAllocateSyncTable( dwPage );
	bool bReturn = rkTable.m_bUpdate;
	rkTable.m_bUpdate = false;
	return bReturn;	
}

void TournamentNode::SendAllocatePageSync( DWORD dwPage, DWORD dwTeamIdx )
{
	SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_ALLOCATE_LIST );
	kPacket << GetIndex() << dwPage << TournamentHelp::PAGE_TEAM_MAX << dwTeamIdx;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );

	AllocateSyncTable& rkTable = GetAllocateSyncTable( dwPage );
	rkTable.m_dwSyncTime = FRAMEGETTIME();
}

void TournamentNode::AddUnAllocateTeamData( DWORD dwTeamIdx, ioHashString& szTeamName, int iPage, short Position, int iCampPos, int iCheerPoint )
{
	TeamData kTeamData;
	kTeamData.m_dwTourIndex = m_dwIndex;
	kTeamData.m_dwTeamIndex = dwTeamIdx;
	kTeamData.m_szTeamName = szTeamName;

	TeamGlobalData* pTeamData = NULL;
	TournamentNode::AllocateTeamDataMap::iterator iter = m_AllocateTeamDataMap.find( Position );
	if( iter == m_AllocateTeamDataMap.end() )
	{
		TeamGlobalData* pTeamData = g_TournamentTeamMgr.SetDefaultData( kTeamData, false );
		TournamentNode::AllocateTeamData rkTeamData;
		rkTeamData.m_dwTeamIndex = dwTeamIdx;
		m_AllocateTeamDataMap.insert( TournamentNode::AllocateTeamDataMap::value_type( Position, rkTeamData ) );
	}

	pTeamData = g_TournamentTeamMgr.GetTeamData( dwTeamIdx, false );
	if( pTeamData && GetType() == TYPE_REGULAR )
	{
		if( pTeamData->GetCampPos() == CAMP_NONE )
			pTeamData->SetCampPos( iCampPos );

		pTeamData->SetCheerPoint( iCheerPoint );
	}
	
	AllocateSyncTable& rkTable = GetAllocateSyncTable( iPage );
	rkTable.m_bUpdate = true;
}


void TournamentNode::ResetUnAllocateTeamData()
{
	TournamentNode::AllocateTeamDataMap::iterator iter = m_AllocateTeamDataMap.begin();
	for( ; iter != m_AllocateTeamDataMap.end(); ++iter )
	{
		TournamentNode::AllocateTeamData& rkTeamData = iter->second;
		rkTeamData.m_AllocatedPos = 0;
	}
}

void TournamentNode::ClearUnAllocateTeamData()
{
	m_AllocateTeamDataMap.clear();
}

TournamentNode::AllocateTeamData& TournamentNode::GetUnAllocateTeamData( short Position )
{
	static TournamentNode::AllocateTeamData kNone;
	TournamentNode::AllocateTeamDataMap::iterator iter = m_AllocateTeamDataMap.find( Position );
	if( iter == m_AllocateTeamDataMap.end() )	
		return kNone;

	return iter->second;
}

const TournamentNode::TeamCreateRequestUser& TournamentNode::GetTournamentJoinConfirmUser( DWORD dwIdx ) const
{
	static const TeamCreateRequestUser KNone;
	if( (DWORD)m_TeamCreateRequestUserVec.size() < dwIdx )
		return KNone;

	return m_TeamCreateRequestUserVec[dwIdx];
}

TournamentNode::TeamCreateRequestUser& TournamentNode::FindTournamentJoinConfirmUser( DWORD dwUserIdx )
{	
	TeamCreateRequestUserVec::iterator iter = m_TeamCreateRequestUserVec.begin();
	for( ; iter != m_TeamCreateRequestUserVec.end(); ++iter )
	{
		if( iter->m_dwUserIndex == dwUserIdx )
			return *iter;
	}

	TournamentNode::TeamCreateRequestUser NewUser;
	m_TeamCreateRequestUserVec.push_back( NewUser );

	return *m_TeamCreateRequestUserVec.rbegin();
}

void TournamentNode::RemoveTournamentJoinConfirmUser( DWORD dwUserIdx )
{	
	TeamCreateRequestUserVec::iterator iter = m_TeamCreateRequestUserVec.begin();
	for( ; iter != m_TeamCreateRequestUserVec.end(); ++iter )
	{
		if( iter->m_dwUserIndex == dwUserIdx )
		{
			m_TeamCreateRequestUserVec.erase( iter );
			return;
		}
	}
}

DWORD TournamentNode::GetStateStartDate( BYTE State )
{
	if( !COMPARE( State - 1, 0, (BYTE)m_StateDate.size() ) )
	{
		return 0;
	}
	
	StateDate &rkDate = m_StateDate[State - 1];
	return rkDate.m_dwStartDate;
}

DWORD TournamentNode::GetStateEndDate( BYTE State )
{
	if( !COMPARE( State - 1, 0, (BYTE)m_StateDate.size() ) )
	{
		return 0;
	}

	StateDate &rkDate = m_StateDate[State - 1];
	return rkDate.m_dwEndDate;
}

RoundTeamData &TournamentNode::GetRoundTeamData( DWORD dwRoundIndex, SHORT Position )
{	
	TournamentRoundData &rkRoundData = GetTournamentRound( dwRoundIndex );
	
	RoundTeamVec::iterator iter = rkRoundData.m_TeamList.begin();
	for(;iter != rkRoundData.m_TeamList.end();++iter)
	{
		RoundTeamData &rkRoundTeam = *iter;
		if( rkRoundTeam.m_Position == Position )
			return rkRoundTeam;
	}

	static RoundTeamData kNoneTeam;
	return kNoneTeam;
}

bool TournamentNode::IsDataSync()
{
	if( m_dwDataSyncTimer == 0 ) return false;
	if( FRAMEGETTIME() - m_dwDataSyncTimer > DATA_SYNC_TIME ) return false;

	return true;
}

void TournamentNode::SendDataSync()
{
	SP2Packet kPacket( CTPK_TOURNAMENT_MAIN_INFO );
	kPacket << GetIndex();
	TCPNetwork::SendToServer( kPacket );

	m_dwDataSyncTimer = FRAMEGETTIME();
}

bool TournamentNode::IsUpdateMainUI()
{
	bool bReturnVal = m_bUpdateMainUI;
	m_bUpdateMainUI = false;
	return bReturnVal;
}

bool TournamentNode::IsUpdateSubUI()
{
	bool bReturnVal = m_bUpdateSubUI;
	m_bUpdateSubUI = false;
	return bReturnVal;
}

bool TournamentNode::IsUpdateInfoUI()
{
	bool bReturnVal = m_bUpdateInfoUI;
	m_bUpdateInfoUI = false;
	return bReturnVal;
}

bool TournamentNode::IsScheduleDataSync()
{
	if( m_dwScheduleSyncTimer == 0 ) return false;
	if( FRAMEGETTIME() - m_dwScheduleSyncTimer > SCHEDULE_SYNC_TIME ) return false;

	return true;
}

void TournamentNode::SendScheduleDataSync()
{
	SP2Packet kPacket( CTPK_TOURNAMENT_SCHEDULE_INFO );
	kPacket << GetIndex();
	TCPNetwork::SendToServer( kPacket );

	m_dwScheduleSyncTimer = FRAMEGETTIME();
}

bool TournamentNode::IsUpdateScheduleUI()
{
	bool bReturnVal = m_bUpdateScheduleUI;
	m_bUpdateScheduleUI = false;
	return bReturnVal;
}

bool TournamentNode::IsRoundTeamDataSyncCheck( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray )
{
	if( GetState() == STATE_TEAM_APP )
		return true;       // 팀 모집 상태에서는 데이터 필요없음

	TournamentRoundSyncTable &rkSyncTable = GetTournamentRoundSyncTable( iStartRound, iTotalRoundCount, iRoundTeamCount, iStartRountTeamArray );
	if( rkSyncTable.m_dwRoundSyncTimer == 0 )
		return false;
	if( FRAMEGETTIME() - rkSyncTable.m_dwRoundSyncTimer < ROUND_SYNC_TIME )
		return true;
	return false;
}

void TournamentNode::SendRoundTeamDataSync( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray )
{
	SP2Packet kPacket( CTPK_TOURNAMENT_ROUND_TEAM_DATA );
	kPacket << GetIndex() << iStartRound << iTotalRoundCount << iRoundTeamCount << iStartRountTeamArray;
	TCPNetwork::SendToServer( kPacket );
	
	TournamentRoundSyncTable &rkSyncTable = GetTournamentRoundSyncTable( iStartRound, iTotalRoundCount, iRoundTeamCount, iStartRountTeamArray );
	rkSyncTable.m_dwRoundSyncTimer = FRAMEGETTIME();
}

bool TournamentNode::IsCustomRewardDataSync()
{
	if( GetType() == TYPE_REGULAR ) return true;
	if( m_dwCustomRewardSyncTimer == 0 ) return false;
	if( FRAMEGETTIME() - m_dwCustomRewardSyncTimer > CUSTOM_REWARD_SYNC_TIME ) return false;

	return true;
}

void TournamentNode::SendCustomRewardDataSync()
{
	if( GetType() == TYPE_REGULAR ) return;

	SP2Packet kPacket( CTPK_TOURNAMENT_CUSTOM_REWARD_LIST );
	kPacket << GetIndex();
	TCPNetwork::SendToServer( kPacket );

	m_dwCustomRewardSyncTimer = FRAMEGETTIME();
}

bool TournamentNode::IsUpdateCustomRewardUI()
{
	bool bReturnVal = m_bUpdateCustomRewardUI;
	m_bUpdateCustomRewardUI = false;
	return bReturnVal;
}

bool TournamentNode::IsUpdateDetailUI()
{
	bool bReturnVal = m_bUpdateDetailUI;
	m_bUpdateDetailUI = false;
	return bReturnVal;
}

bool TournamentNode::IsEntryAllocated()
{	
	TournamentRoundData Round = GetTournamentRound( 1 );
	if( Round.m_TeamList.empty() )
	{	
		return false;
	}
	return true;
}

void TournamentNode::ApplyMainInfo( SP2Packet &rkPacket )
{
	BYTE PrevState = m_State;
	m_dwDataSyncTimer = FRAMEGETTIME();

	rkPacket >> m_dwOwnerIndex >> m_dwStartDate >> m_dwEndDate;	
	rkPacket >> m_Type >> m_State >> m_TournamentStartTeamCount >> m_dwStateEndDate;
	rkPacket >> m_MaxPlayer >> m_iPlayMode;

	if( GetType() == TYPE_REGULAR )
	{
		int iReguralResourceType;
		bool bDisable;
		DWORD dwTotalCheerCount;

		rkPacket >> iReguralResourceType >> m_WinTeamCamp >> bDisable >> dwTotalCheerCount;

		SetReguralResourceType( iReguralResourceType );
		SetReguralDateType( GetStartDate() );
		SetReguralDisableTournament( bDisable );
		SetReguralTotalCheerCount( dwTotalCheerCount );
	}
	else
	{
		DWORD dwBannerB, dwBannerS;

		/* 유저리그 전용 데이터 */
		rkPacket >> m_szTournamentTitle >> dwBannerB >> dwBannerS >> m_CustomProgressType >> m_szCustomAnnounce >> m_iExpectedTeamCount;
		SetCustomBanner( dwBannerB, dwBannerS );
	}

	m_TournamentMaxRound = Help::TournamentRound( m_TournamentStartTeamCount );
	m_bUpdateMainUI = true;
	m_bUpdateSubUI  = true;
	m_bUpdateInfoUI = true;
	m_bUpdateDetailUI = true;

	// 모집 상태가 되면 토너먼트 데이터 삭제
	if( GetState() == STATE_TEAM_APP )
	{
		if( !m_TournamentRoundMap.empty() )
		{
			TournamentRoundMap::iterator iCreator = m_TournamentRoundMap.begin();
			for(;iCreator != m_TournamentRoundMap.end();++iCreator)
			{
				TournamentRoundData &rkRoundData = iCreator->second;
				rkRoundData.m_TeamList.clear();
			}
			m_TournamentRoundMap.clear();
		}

		if( PrevState == STATE_WAITING )
		{
			m_dwScheduleSyncTimer = 0;        // 스케줄을 다시 가져온다.
			m_WinTeamCamp         = CAMP_NONE;
		}
	}

	if( !IsScheduleDataSync() )
	{
		SendScheduleDataSync();
	}
	SetCustomRoundSync( m_State );
}

void TournamentNode::ApplyScheduleInfo( SP2Packet &rkPacket )
{
	m_dwScheduleSyncTimer = FRAMEGETTIME();
	m_bUpdateScheduleUI   = true;

	m_StateDate.clear();
	int iScheduleSize;
	rkPacket >> iScheduleSize;
	for(int i = 0;i < iScheduleSize;i++)
	{
		StateDate kDate;
		rkPacket >> kDate.m_dwStartDate >> kDate.m_dwEndDate;
		m_StateDate.push_back( kDate );
	}
}

void TournamentNode::ApplyRoundTeamData( SP2Packet &rkPacket )
{
	int iStartRound, iTotalRoundCount;
	int iRoundTeamCount, iStartRountTeamArray;
	rkPacket >> iStartRound >> iTotalRoundCount >> iRoundTeamCount >> iStartRountTeamArray;
		
	int iTeamCount = iRoundTeamCount;
	for(int i = iStartRound;i < iStartRound + iTotalRoundCount;i++)
	{
		TournamentRoundData &rkRoundData = GetTournamentRound( i );
		int iStartArray = iTeamCount * iStartRountTeamArray;
		for(int j = iStartArray;j < iStartArray + iTeamCount;j++)
		{
			bool bTeamData;
			rkPacket >> bTeamData;
			if( !bTeamData ) continue;

			RoundTeamData kRoundTeamData;
			rkPacket >> kRoundTeamData.m_dwTeamIndex >> kRoundTeamData.m_szTeamName >> kRoundTeamData.m_CampPos;			
			kRoundTeamData.m_TourPos  = (BYTE)i;
			kRoundTeamData.m_Position = (SHORT)j + 1;
			SetTournamentRoundTeam( rkRoundData, kRoundTeamData );
		}
		iTeamCount /= 2;
	}

	//
	CheckRoundTeamDataDrop();

	//
	TournamentRoundSyncTable &rkSyncTable = GetTournamentRoundSyncTable( iStartRound, iTotalRoundCount, iRoundTeamCount, iStartRountTeamArray );
	rkSyncTable.m_bDataChange = true;
	m_bUpdateDetailUI = true;	
}

void TournamentNode::ApplyTournamentJoinConfirmList( SP2Packet &rkPacket )
{
	DWORD dwRequestUserIndex;
	ioHashString szRequestUserID;
	int iRequestUserLevel;

	rkPacket >> dwRequestUserIndex >> szRequestUserID >> iRequestUserLevel;
	TeamCreateRequestUser& rkUser = FindTournamentJoinConfirmUser( dwRequestUserIndex );
	rkUser.m_dwUserIndex = dwRequestUserIndex;
	rkUser.m_szUserID = szRequestUserID;
	rkUser.m_iUserLevel = iRequestUserLevel;
}

void TournamentNode::ApplyCustomAnnounce( SP2Packet &rkPacket )
{
	m_szCustomAnnounce.Clear();
	rkPacket >> m_szCustomAnnounce;
	
	m_bUpdateSubUI = true;
}

void TournamentNode::ApplyCustomRewardList( SP2Packet &rkPacket )
{
	m_CustomRewardList.clear();

	int iRewardSize;
	rkPacket >> iRewardSize;
	for(int i = 0;i < iRewardSize;i++)
	{
		CustomRewardInfo kRewrdInfo;
		rkPacket >> kRewrdInfo.m_dwRewardA >> kRewrdInfo.m_dwRewardB >> kRewrdInfo.m_dwRewardC >> kRewrdInfo.m_dwRewardD;

		m_CustomRewardList.push_back( kRewrdInfo );
	}

	m_bUpdateCustomRewardUI = true;
	m_dwCustomRewardSyncTimer = FRAMEGETTIME();
}

void TournamentNode::CheckRoundTeamDataDrop()
{
	int iCurrentRound = ( (int)m_State - STATE_TOURNAMENT );
	if( m_State == STATE_WAITING )
		iCurrentRound = m_TournamentMaxRound;

	if( iCurrentRound <= 0 ) return;
	
	{	// 모든 팀 탈락 처리 - 공식을 단순화 시키기 위해서
		TournamentRoundMap::iterator iter = m_TournamentRoundMap.begin();
		for(;iter != m_TournamentRoundMap.end();++iter)
		{
			TournamentRoundData &rkRoundData = iter->second;

			RoundTeamVec::iterator iter2 = rkRoundData.m_TeamList.begin();
			for(;iter2 != rkRoundData.m_TeamList.end();++iter2)
			{
				RoundTeamData &rkTeam = *iter2;
				rkTeam.m_bDropTeam = true;
			}
		}
	}

	{
		TournamentRoundData &rkPrevRoundData = GetTournamentRound( iCurrentRound );	// 이전 라운드 예) 32강
		RoundTeamVec::iterator iter = rkPrevRoundData.m_TeamList.begin();
		for(;iter != rkPrevRoundData.m_TeamList.end();++iter)
		{
			RoundTeamData &rkPrevTeam = *iter;

			bool bDropTeam = false;
			int iNextPos   = ( rkPrevTeam.m_Position + 1 ) / 2;      // 다음 라운드의 포지션
						
			TournamentRoundData &rkCurrentRoundData = GetTournamentRound( iCurrentRound + 1 );  // 현재 라운드 예) 16강
			RoundTeamVec::iterator iter2 = rkCurrentRoundData.m_TeamList.begin();
			for(;iter2 != rkCurrentRoundData.m_TeamList.end();++iter2)
			{
				RoundTeamData &rkCurrentTeam = *iter2;								
				if( iNextPos == rkCurrentTeam.m_Position )
				{
					if( rkPrevTeam.m_dwTeamIndex != rkCurrentTeam.m_dwTeamIndex )
						bDropTeam = true;
				}
			}
			
			if( !bDropTeam )
			{
				SetRoundTeamDataDrop( rkPrevTeam.m_dwTeamIndex, false );  // 진출한 팀
			}
		}
	}
}

void TournamentNode::SetRoundTeamDataDrop( DWORD dwTeamIndex, bool bDropTeam )
{
	TournamentRoundMap::iterator iter = m_TournamentRoundMap.begin();
	for(;iter != m_TournamentRoundMap.end();++iter)
	{
		TournamentRoundData &rkRoundData = iter->second;

		RoundTeamVec::iterator iter2 = rkRoundData.m_TeamList.begin();
		for(;iter2 != rkRoundData.m_TeamList.end();++iter2)
		{
			RoundTeamData &rkTeam = *iter2;
			if( rkTeam.m_dwTeamIndex == dwTeamIndex )
			{
				rkTeam.m_bDropTeam = bDropTeam;
			}
		}
	}
}

void TournamentNode::ChangeRoundTeamData( DWORD dwTeamIndex, const ioHashString &rkTeamName, BYTE CampPos, BYTE TourPos, SHORT Position )
{
	TournamentRoundData &rkRoundData = GetTournamentRound( TourPos );
	
	RoundTeamData kRoundTeamData;
	kRoundTeamData.m_dwTeamIndex = dwTeamIndex;
	kRoundTeamData.m_szTeamName  = rkTeamName;
	kRoundTeamData.m_CampPos     = CampPos;
	kRoundTeamData.m_TourPos     = TourPos;
	kRoundTeamData.m_Position    = Position;

	SetTournamentRoundTeam( rkRoundData, kRoundTeamData );

	//
	TournamentRoundSyncVec::iterator iter = m_TournamentRoundSyncList.begin();
	for(;iter != m_TournamentRoundSyncList.end();++iter)
	{
		TournamentRoundSyncTable &rkSyncTable = *iter;
		rkSyncTable.m_bDataChange = true;
	}
	m_bUpdateDetailUI = true;
}

bool TournamentNode::IsUpdateRoundTeamData( int iStartRound, int iTotalRoundCount, int iRoundTeamCount, int iStartRountTeamArray )
{
	TournamentRoundSyncTable &rkSyncTable = GetTournamentRoundSyncTable( iStartRound, iTotalRoundCount, iRoundTeamCount, iStartRountTeamArray );
	bool bReturn = rkSyncTable.m_bDataChange;
	rkSyncTable.m_bDataChange = false;
	return bReturn;
}

void TournamentNode::SetReguralResourceType( int iReguralResourceType )
{
	if( m_iReguralResourceType != iReguralResourceType )
	{
		CreateRegularData( iReguralResourceType );		
	}
	m_iReguralResourceType = iReguralResourceType;
}

void TournamentNode::SetReguralDateType( int iReguralDateType )
{
	if( m_iReguralDateType != iReguralDateType )
	{
		CreateResultRewardData( iReguralDateType );
	}
	m_iReguralDateType = iReguralDateType;
}

void TournamentNode::SetReguralDisableTournament( bool bDisable )
{
	m_bReguralDisableTournament = bDisable;
}

void TournamentNode::SetReguralTotalCheerCount( DWORD dwCheerCount )
{
	m_dwReguralTotalCheerCount = dwCheerCount;
}

void TournamentNode::SetCustomBanner( DWORD dwBannerB, DWORD dwBannerS )
{
	if( m_dwCustomBannerB != dwBannerB || m_dwCustomBannerS != dwBannerS )
	{
		CreateCustomData( dwBannerB, dwBannerS );
	}
	m_dwCustomBannerB = dwBannerB;
	m_dwCustomBannerS = dwBannerS;
}

void TournamentNode::SetCustomState( int State, DWORD dwStateStartDate, DWORD dwStateEndDate )
{
	m_State = State;
	switch( State )
	{
	case STATE_WAITING: //결승 -> 대기
		{
			StateDate &CurrDate = m_StateDate[(int)m_StateDate.size()-1];
			CurrDate.m_dwStartDate = dwStateStartDate;
			CurrDate.m_dwEndDate   = dwStateEndDate;
		}	
		break;
	default:
		{
			if( COMPARE( State - 2, 0, (int)m_StateDate.size() ) )
			{
				StateDate &CurrDate = m_StateDate[State- 2];
				CurrDate.m_dwStartDate = dwStateStartDate;
				CurrDate.m_dwEndDate   = dwStateEndDate;
			}
			else
			{
				LOG.PrintTimeAndLog(0 , "# TournamentNode::SetCustomState() current state setting failed");
			}

			if( COMPARE( State - 1, 0, (int)m_StateDate.size() ) )
			{
				StateDate &NextDate = m_StateDate[State-1];
				NextDate.m_dwStartDate = dwStateEndDate;
				NextDate.m_dwEndDate   = 0;
			}
			else
			{
				LOG.PrintTimeAndLog(0 , "# TournamentNode::SetCustomState() next state setting failed");
			}
		}
		break;
	}
}

void TournamentNode::SetCustomRoundSync( int State )
{
	if( m_dwOwnerIndex != g_MyInfo.GetUserIndex() )
		return;

	if( TournamentNode::STATE_TOURNAMENT <= State )
	{
		int iPos = State - TournamentNode::STATE_TOURNAMENT + 1;
		int iTeamCnt = Help::TournamentCurrentRoundWithTeam( GetTournamentStartTeamCount(), iPos-1 );
		if( !IsRoundTeamDataSyncCheck( iPos, 1, iTeamCnt, 0 ) )
			SendRoundTeamDataSync( iPos, 1, iTeamCnt, 0  );
	}
}

int TournamentNode::GetCustomRoundTeamCount( int iPos )
{
	TournamentNode::TournamentRoundData TourRound = GetTournamentRound( iPos );
	return TourRound.m_TeamList.size();
}

bool TournamentNode::IsLogSend()
{
	if( GetType() != TYPE_REGULAR )
		return false;

	return m_bLogSend; 
}