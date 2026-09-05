
#include "stdafx.h"

#include "ioMyLevelMgr.h"

template<> ioMyLevelMgr* Singleton< ioMyLevelMgr >::ms_Singleton = 0;

ioMyLevelMgr::ioMyLevelMgr()
{
	// Soldier Level
	m_iSoldierMaxLevel= 100;

	// Grade Level
	m_iGradeMaxUP     = 50;
	m_iGradeMaxLevel  = 54;

	// Award Level
	m_iAwardMaxLevel= 100;

	// Medal Level
	m_iMedalMaxLevel= 100;

	// Party Level
	m_fPartyConstantA = 1.053f;
	m_iPartyConstantB = 1;
	m_iPartyConstantC = 950;
	m_iPartyConstantD = 920;

	// Ladder Level
	m_fLadderConstantA = 1.053f;
	m_iLadderConstantB = 1;
	m_iLadderConstantC = 950;
	m_iLadderConstantD = 920;

	// Hero Level 
	m_fHeroConstantA = 1.053f;
	m_iHeroConstantB = 1;
	m_iHeroConstantC = 950;
	m_iHeroConstantD = 920;

	// Solo Level
	m_fSoloConstantA = 1.053f;
	m_iSoloConstantB = 1;
	m_iSoloConstantC = 950;
	m_iSoloConstantD = 920;
		
	m_dwResGradeLevelUnit = 20;

	m_iAddGradeLevel        = 1;
	m_iRoomEnterSafetyLevel = 11;
	m_iRoomKillDeathSafetyLevel = 11;
	m_iRoomEnterLevelMax	= 64;
	m_iBattleEnterLevelMax  = 64;
	m_iLeaveSafetyRoomKo    = 100;
	m_iKillDeathMinLevelGap	= 0;

	m_iGradeLevelForEtcBonus = 50;	

	m_pGradeIOWhiteMark	= NULL;
	m_pGradeIOOrangeMark= NULL;
	m_pGradeIOBlueMark  = NULL;
	m_pGradeIORedMark   = NULL;
	m_pGradeIOGrayMark  = NULL;

	m_pGradeGMWhiteMark	= NULL;
	m_pGradeGMOrangeMark= NULL;
	m_pGradeGMBlueMark  = NULL;
	m_pGradeGMRedMark   = NULL;
	m_pGradeGMGrayMark	= NULL;

	m_pLobbyOffLineMark	= NULL;
	m_pMessengerOffLineMark = NULL;

	m_pBlueClassLevelText = NULL;
	m_pBlueClassLevelNum  = NULL;
	m_pRedClassLevelText  = NULL;
	m_pRedClassLevelNum   = NULL;
}

ioMyLevelMgr::~ioMyLevelMgr()
{	
	ClearAllResource();
}

ioMyLevelMgr& ioMyLevelMgr::GetSingleton()
{
	return Singleton<ioMyLevelMgr>::GetSingleton();
}

void ioMyLevelMgr::ClearAllResource()
{
	for(vMortmainCharInfo::iterator iter = m_vMortmainCharInfoVector.begin(); iter != m_vMortmainCharInfoVector.end(); ++iter)
	{
		SAFEDELETE( *iter );
	}
	m_vMortmainCharInfoVector.clear();

	TypeResourceMap::iterator iCreate;
	for( iCreate = m_GradeLevelIconMap.begin() ; iCreate != m_GradeLevelIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GradeLevelIconMap.clear();

	for( iCreate = m_GradeLevelBlueIconMap.begin() ; iCreate != m_GradeLevelBlueIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GradeLevelBlueIconMap.clear();

	for( iCreate = m_GradeLevelRedIconMap.begin() ; iCreate != m_GradeLevelRedIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GradeLevelRedIconMap.clear();

	for( iCreate = m_GradeLevelOrangeIconMap.begin() ; iCreate != m_GradeLevelOrangeIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GradeLevelOrangeIconMap.clear();

	for( iCreate = m_GradeLevelWhiteIconMap.begin() ; iCreate != m_GradeLevelWhiteIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GradeLevelWhiteIconMap.clear();

	for( iCreate = m_GradeLevelGrayIconMap.begin() ; iCreate != m_GradeLevelGrayIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_GradeLevelGrayIconMap.clear();

	for( iCreate = m_HeroTitleIconMap.begin() ; iCreate != m_HeroTitleIconMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_HeroTitleIconMap.clear();

	for( iCreate = m_HeroTitleTextMap.begin() ; iCreate != m_HeroTitleTextMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_HeroTitleTextMap.clear();
	m_HeroTitleStrMap.clear();
	m_HeroTitleBonusMap.clear();

	SAFEDELETE( m_pGradeIOWhiteMark );
	SAFEDELETE( m_pGradeIOOrangeMark );
	SAFEDELETE( m_pGradeIOBlueMark );
	SAFEDELETE( m_pGradeIORedMark );
	SAFEDELETE( m_pGradeIOGrayMark );

	SAFEDELETE( m_pGradeGMWhiteMark );
	SAFEDELETE( m_pGradeGMOrangeMark );
	SAFEDELETE( m_pGradeGMBlueMark );
	SAFEDELETE( m_pGradeGMRedMark );
	SAFEDELETE( m_pGradeGMGrayMark );

	SAFEDELETE( m_pLobbyOffLineMark	);
	SAFEDELETE( m_pMessengerOffLineMark );

	SAFEDELETE( m_pBlueClassLevelText );
	SAFEDELETE( m_pBlueClassLevelNum );
	SAFEDELETE( m_pRedClassLevelText );
	SAFEDELETE( m_pRedClassLevelNum );
}

void ioMyLevelMgr::LoadINIInfo()
{
	ClearAllResource();

	int i = 0;
	ioINILoader_e kLoader( "config/sp2_level_info.ini" );

	kLoader.SetTitle_e( "level_info" );
	m_iAddGradeLevel			= kLoader.LoadInt_e( "add_level", 1 );
	m_iRoomEnterSafetyLevel		= kLoader.LoadInt_e( "safety_level", 11 );
	m_iRoomKillDeathSafetyLevel	= kLoader.LoadInt_e( "kill_death_safety_level", 11 );
	m_iRoomEnterLevelMax		= kLoader.LoadInt_e( "room_kill_death_level_max", 64 );
	m_iBattleEnterLevelMax		= kLoader.LoadInt_e( "battle_kill_death_level_max", 64 );
	m_iLeaveSafetyRoomKo		= kLoader.LoadInt_e( "leave_safety_room_ko", 10 );
	m_iKillDeathMinLevelGap		= kLoader.LoadInt_e( "kill_death_min_level_gap", 5 );

	kLoader.SetTitle_e( "levelup" );
	m_iSoldierMaxLevel= kLoader.LoadInt_e( "max_level", 100 );
	for(i = 0;i < m_iSoldierMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "exp_%d", i + 1 );
		m_vSoldierLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle_e( "gradeup" );
	m_iGradeMaxUP     = kLoader.LoadInt_e( "constant_max_grade", 50 );
	m_iGradeMaxLevel  = kLoader.LoadInt_e( "constant_max_level", 54 );
	for(i = 0;i < m_iGradeMaxUP;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "exp_%d", i + 1 );
		m_vGradeLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle_e( "awardup" );
	m_iAwardMaxLevel= kLoader.LoadInt_e( "max_level", 100 );
	for(i = 0;i < m_iAwardMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "exp_%d", i + 1 );
		m_vAwardLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle_e( "medalup" );
	m_iMedalMaxLevel= kLoader.LoadInt_e( "max_level", 100 );
	for(i = 0;i < m_iMedalMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "exp_%d", i + 1 );
		m_vMedalLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle_e( "fishingup" );
	m_iFishingMaxLevel = kLoader.LoadInt_e( "max_fishing_level", 100 );
	for(i = 0;i < m_iFishingMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "fishing_exp_%d", i + 1 );
		m_vFishingLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle_e( "excavationup" );
	m_iExcavationMaxLevel = kLoader.LoadInt_e( "max_excavation_level", 100 );
	for(i = 0;i < m_iExcavationMaxLevel;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "excavation_exp_%d", i + 1 );
		m_vExcavationLevelExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}

	kLoader.SetTitle_e("TowerDef");
	m_nTowerDefMaxLvl = kLoader.LoadInt_e( "max_AOS_level", 99 );
	m_nAutolvltime = kLoader.LoadInt_e("auto_lvlup_time", 1000);
	m_nAutolvl = kLoader.LoadInt_e("auto_lvlup_amount", 100);

	for(i = 0;i < m_nTowerDefMaxLvl;i++)
	{
		char szKey[MAX_PATH] = "";
		sprintf_e( szKey, "AOS_exp_%d", i + 1 );
		m_vecTowerDefLvlExp.push_back( (DWORD)kLoader.LoadInt( szKey, MAX_INT_VALUE ) );
	}


	kLoader.SetTitle_e( "partyup" );
	m_fPartyConstantA = kLoader.LoadFloat_e( "constant_a", 1.053f );
	m_iPartyConstantB = kLoader.LoadInt_e( "constant_b", 1 );
	m_iPartyConstantC = kLoader.LoadInt_e( "constant_c", 950 );
	m_iPartyConstantD = kLoader.LoadInt_e( "constant_d", 920 );

	kLoader.SetTitle_e( "ladderup" );
	m_fLadderConstantA = kLoader.LoadFloat_e( "constant_a", 1.053f );
	m_iLadderConstantB = kLoader.LoadInt_e( "constant_b", 1 );
	m_iLadderConstantC = kLoader.LoadInt_e( "constant_c", 950 );
	m_iLadderConstantD = kLoader.LoadInt_e( "constant_d", 920 );

	kLoader.SetTitle_e( "heroup" );
	m_fHeroConstantA = kLoader.LoadFloat_e( "constant_a", 1.053f );
	m_iHeroConstantB = kLoader.LoadInt_e( "constant_b", 1 );
	m_iHeroConstantC = kLoader.LoadInt_e( "constant_c", 950 );
	m_iHeroConstantD = kLoader.LoadInt_e( "constant_d", 920 );

	kLoader.SetTitle_e( "soloup" );
	m_fSoloConstantA = kLoader.LoadFloat_e( "constant_a", 1.053f );
	m_iSoloConstantB = kLoader.LoadInt_e( "constant_b", 1 );
	m_iSoloConstantC = kLoader.LoadInt_e( "constant_c", 950 );
	m_iSoloConstantD = kLoader.LoadInt_e( "constant_d", 920 );	

	kLoader.SetTitle_e( "mortmain_char_info" );
	int iMax = kLoader.LoadInt( "max", 0 );
	for (int i = 0; i < iMax ; i++)
	{
		MortmainCharInfo *pInfo = new MortmainCharInfo;
		if( !pInfo )
		{
			LOG.PrintTimeAndLog( 0, "%s pInfo == NULL" , __FUNCTION__ );
			return;
		}

		char szKey[MAX_PATH]="";
		StringCbPrintf_e( szKey, sizeof( szKey ), "below_grade_level_%d", i + 1 );
		pInfo->m_iBelowGradeLevel = kLoader.LoadInt( szKey, 0 );
		StringCbPrintf_e( szKey, sizeof( szKey ), "have_mortmain_char_num_%d", i + 1 );
		pInfo->m_iHaveMortmainChar= kLoader.LoadInt( szKey, 0 );
		m_vMortmainCharInfoVector.push_back( pInfo );
	}

	kLoader.SetTitle_e( "etc_bonus" );
	m_iGradeLevelForEtcBonus = kLoader.LoadInt_e( "grade_level", 50 );

	kLoader.SetTitle_e( "resource" );
	m_dwResGradeLevelUnit = kLoader.LoadInt_e( "res_level_unit", 20 );

	int iGradeUnit = 0;
	char szKey[MAX_PATH], szBuf[MAX_PATH];
	while( true )
	{
		sprintf_e( szKey, "grade_level%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_GradeLevelIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "grade_blue_level%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_GradeLevelBlueIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "grade_red_level%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_GradeLevelRedIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "grade_orange_level%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_GradeLevelOrangeIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "grade_white_level%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_GradeLevelWhiteIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "grade_gray_level%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_GradeLevelGrayIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	//영웅전 아이콘
	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "hero_title_icon%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_HeroTitleIconMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	// 영웅전 텍스트
	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "hero_title_text%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
		m_HeroTitleTextMap.insert( TypeResourceMap::value_type( iGradeUnit++, pImg ) );		
	}	

	// 영웅전 스트링
	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "hero_title_str%d", iGradeUnit );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		if( strcmp( szBuf, "" ) == 0 )
			break;

		m_HeroTitleStrMap.insert( TypeStringMap::value_type( iGradeUnit++, szBuf ) );		
	}	

	// 영웅전 보너스
	iGradeUnit = 0;
	while( true )
	{
		sprintf_e( szKey, "hero_title_bonus%d", iGradeUnit );
		float fBonus = kLoader.LoadFloat( szKey, -FLOAT1 );
		if( fBonus == -FLOAT1 ) 
			break;

		m_HeroTitleBonusMap.insert( TypeFloatMap::value_type( iGradeUnit++, fBonus ) );		
	}

	// 개발자
	kLoader.LoadString_e( "grade_white_io", "", szBuf, MAX_PATH );
	m_pGradeIOWhiteMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		

	kLoader.LoadString_e( "grade_orange_io", "", szBuf, MAX_PATH );
	m_pGradeIOOrangeMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		

	kLoader.LoadString_e( "grade_blue_io", "", szBuf, MAX_PATH );
	m_pGradeIOBlueMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		

	kLoader.LoadString_e( "grade_red_io", "", szBuf, MAX_PATH );
	m_pGradeIORedMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );	

	kLoader.LoadString_e( "grade_gray_io", "", szBuf, MAX_PATH );
	m_pGradeIOGrayMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );	
	// 마스터
	kLoader.LoadString_e( "grade_white_gm", "", szBuf, MAX_PATH );
	m_pGradeGMWhiteMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
	
	kLoader.LoadString_e( "grade_orange_gm", "", szBuf, MAX_PATH );
	m_pGradeGMOrangeMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );	

	kLoader.LoadString_e( "grade_blue_gm", "", szBuf, MAX_PATH );
	m_pGradeGMBlueMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		

	kLoader.LoadString_e( "grade_red_gm", "", szBuf, MAX_PATH );
	m_pGradeGMRedMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		

	kLoader.LoadString_e( "grade_gray_gm", "", szBuf, MAX_PATH );
	m_pGradeGMGrayMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );		

	kLoader.LoadString_e( "class_level_blue_text", "", szBuf, MAX_PATH );
	m_pBlueClassLevelText = g_UIImageSetMgr.CreateImageByFullName( szBuf );		 
	kLoader.LoadString_e( "class_level_blue_num", "", szBuf, MAX_PATH );
	m_pBlueClassLevelNum = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
	kLoader.LoadString_e( "class_level_red_text", "", szBuf, MAX_PATH );
	m_pRedClassLevelText = g_UIImageSetMgr.CreateImageByFullName( szBuf );		
	kLoader.LoadString_e( "class_level_red_num", "", szBuf, MAX_PATH );
	m_pRedClassLevelNum = g_UIImageSetMgr.CreateImageByFullName( szBuf );	

	// 로비랑 메신저에 오프라인 유저 계급 대신 마크 적용
    kLoader.LoadString_e( "lobby_offline_mark", "", szBuf, MAX_PATH );
	m_pLobbyOffLineMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );	
	kLoader.LoadString_e( "messenger_offline_mark", "", szBuf, MAX_PATH );
	m_pMessengerOffLineMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );	
}

ioUIRenderImage *ioMyLevelMgr::GetGradeLevelIcon( DWORD dwGradeLevel )
{
	DWORD dwLevelUnit = dwGradeLevel / m_dwResGradeLevelUnit;
	TypeResourceMap::iterator iter = m_GradeLevelIconMap.find( dwLevelUnit );

	if( iter != m_GradeLevelIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetGradeBlueLevelIcon( DWORD dwGradeLevel )
{
	DWORD dwLevelUnit = dwGradeLevel / m_dwResGradeLevelUnit;
	TypeResourceMap::iterator iter = m_GradeLevelBlueIconMap.find( dwLevelUnit );

	if( iter != m_GradeLevelBlueIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetGradeRedLevelIcon( DWORD dwGradeLevel )
{
	DWORD dwLevelUnit = dwGradeLevel / m_dwResGradeLevelUnit;
	TypeResourceMap::iterator iter = m_GradeLevelRedIconMap.find( dwLevelUnit );

	if( iter != m_GradeLevelRedIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetGradeOrangeLevelIcon( DWORD dwGradeLevel )
{
	DWORD dwLevelUnit = dwGradeLevel / m_dwResGradeLevelUnit;
	TypeResourceMap::iterator iter = m_GradeLevelOrangeIconMap.find( dwLevelUnit );

	if( iter != m_GradeLevelOrangeIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetGradeWhiteLevelIcon( DWORD dwGradeLevel )
{
	DWORD dwLevelUnit = dwGradeLevel / m_dwResGradeLevelUnit;
	TypeResourceMap::iterator iter = m_GradeLevelWhiteIconMap.find( dwLevelUnit );

	if( iter != m_GradeLevelWhiteIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetGradeGrayLevelIcon( DWORD dwGradeLevel )
{
	DWORD dwLevelUnit = dwGradeLevel / m_dwResGradeLevelUnit;
	TypeResourceMap::iterator iter = m_GradeLevelGrayIconMap.find( dwLevelUnit );

	if( iter != m_GradeLevelGrayIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}


ioUIRenderImage *ioMyLevelMgr::GetHeroTitleIcon( DWORD dwHeroTitle )
{
	TypeResourceMap::iterator iter = m_HeroTitleIconMap.find( dwHeroTitle );
	if( iter != m_HeroTitleIconMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetHeroTitleText( DWORD dwHeroTitle )
{
	TypeResourceMap::iterator iter = m_HeroTitleTextMap.find( dwHeroTitle );
	if( iter != m_HeroTitleTextMap.end() )
	{
		return iter->second;		
	}
	return NULL;
}

const ioHashString &ioMyLevelMgr::GetHeroTitleString( DWORD dwHeroTitle )
{
	TypeStringMap::iterator iter = m_HeroTitleStrMap.find( dwHeroTitle );
	if( iter != m_HeroTitleStrMap.end() )
	{
		return iter->second;
	}
	static ioHashString szReturn = STR(1);
	return szReturn;
}

float ioMyLevelMgr::GetHeroTitleBonus( DWORD dwHeroTitle )
{
	TypeFloatMap::iterator iter = m_HeroTitleBonusMap.find( dwHeroTitle );
	if( iter != m_HeroTitleBonusMap.end() )
	{
		return iter->second;
	}
	return 0.0f;
}

void ioMyLevelMgr::RenderGrade( const ioHashString &szName, int iXPos, int iYPos, DWORD dwGradeLevel, TeamType eTeam, UIRenderType eRenderType, DWORD dwColor )
{
	if( g_App.IsDeveloperID( szName.c_str() ) )
	{
		ioUIRenderImage *pRenderImg = NULL;
		switch( eTeam )
		{
		case TEAM_BLUE:
			pRenderImg = m_pGradeIOBlueMark;
			break;
		case TEAM_RED:
			pRenderImg = m_pGradeIORedMark;
			break;
		case TEAM_PRIVATE_1:
			pRenderImg = m_pGradeIOOrangeMark;
			break;
		case TEAM_PRIVATE_2:
			pRenderImg = m_pGradeIOWhiteMark;
			break;
		case TEAM_PRIVATE_3:
			pRenderImg = m_pGradeIOGrayMark;
			break;
		default:
			pRenderImg = NULL;
			break;
		}

		if( pRenderImg )
			pRenderImg->Render( iXPos, iYPos, eRenderType );
	}
	else if( g_App.IsGameMasterID( szName.c_str() ) )
	{
		ioUIRenderImage *pRenderImg = NULL;
		switch( eTeam )
		{
		case TEAM_BLUE:
			pRenderImg = m_pGradeGMBlueMark;
			break;
		case TEAM_RED:
			pRenderImg = m_pGradeGMRedMark;
			break;
		case TEAM_PRIVATE_1:
			pRenderImg = m_pGradeGMOrangeMark;
			break;
		case TEAM_PRIVATE_2:
			pRenderImg = m_pGradeGMWhiteMark;
			break;
		case TEAM_PRIVATE_3:
			pRenderImg = m_pGradeGMGrayMark;
			break;
		default:
			pRenderImg = NULL;
			break;
		}

		if( pRenderImg )
			pRenderImg->Render( iXPos, iYPos, eRenderType );
	}
	else
	{
		RenderGrade( iXPos, iYPos, dwGradeLevel, eTeam, eRenderType, dwColor );
	}
}

void ioMyLevelMgr::RenderGrade( int iXPos, int iYPos, DWORD dwGradeLevel, TeamType eTeam, UIRenderType eRenderType, DWORD dwColor )
{
	ioUIRenderImage *pRenderImg = NULL;

	DWORD dwLevel = 0;
	dwLevel = dwGradeLevel;

	// HARDCODE : 팀컬러에 맞게 계급이 추가되었는데 TEAM_PRIVATE_1부터 새로운 컬러의 계급이미지로 사용한다.
	// 렌더하는 부분을 빼면 좋지만 이렇게 사용함.
	switch( eTeam )
	{
	case TEAM_BLUE:
		pRenderImg = GetGradeBlueLevelIcon( dwLevel );
		break;
	case TEAM_RED:
		pRenderImg = GetGradeRedLevelIcon( dwLevel );
		break;
	case TEAM_PRIVATE_1:
		pRenderImg = GetGradeOrangeLevelIcon( dwLevel );
		break;
	case TEAM_PRIVATE_2:
		pRenderImg = GetGradeWhiteLevelIcon( dwLevel );
		break;
	case TEAM_PRIVATE_3:
		pRenderImg = GetGradeGrayLevelIcon( dwLevel );
		break;
	default:
		pRenderImg = GetGradeLevelIcon( dwLevel );
		break;
	}

	if( pRenderImg )
	{
		int iRealWidth  = pRenderImg->GetWidth() / m_dwResGradeLevelUnit;
		int iStartWidth = iRealWidth * ( dwLevel % m_dwResGradeLevelUnit );
		pRenderImg->SetColor( dwColor ); 
		pRenderImg->RenderWidthCut( iXPos - iStartWidth, iYPos, iStartWidth, iStartWidth + iRealWidth, eRenderType );
	}
}

void ioMyLevelMgr::RenderClassLevel( int iXPos, int iYPos, int iClassLevel, TeamType eTeam, bool bOwner, UIRenderType eRenderType )
{
	ioUIRenderImage *pRenderText = NULL;
	ioUIRenderImage *pRenderNum  = NULL;

	int iLevel = 0;
	if( ioPlayMode::GetModeType() != MT_ARENA )
		iLevel = iClassLevel;

	switch( eTeam )
	{
	case TEAM_BLUE:
		pRenderText = m_pBlueClassLevelText;
		pRenderNum  = m_pBlueClassLevelNum;
		break;
	case TEAM_RED:
		pRenderText = m_pRedClassLevelText;
		pRenderNum  = m_pRedClassLevelNum;
		break;
	default:
		if( bOwner )
		{
			pRenderText = m_pBlueClassLevelText;
			pRenderNum  = m_pBlueClassLevelNum;
		}
		else
		{
			pRenderText = m_pRedClassLevelText;
			pRenderNum  = m_pRedClassLevelNum;
		}
		break;
	}

	if( !pRenderText || !pRenderNum ) return;

	char szLevel[MAX_PATH] = "";
	sprintf_e( szLevel, "%d", iLevel );
	int iLevelSize = ( pRenderNum->GetWidth() / 10 ) * strlen( szLevel );
	int iTextSize  = pRenderText->GetWidth();
	int iRealX     = iXPos - ( ( iLevelSize + iTextSize ) / 2 );
    
	pRenderText->Render( iRealX, iYPos, eRenderType );
	pRenderNum->RenderNum( iRealX + iTextSize + 1, iYPos, iLevel, -FLOAT1, FLOAT1, eRenderType );
}

void ioMyLevelMgr::RenderHeroTitleIcon( DWORD dwHeroTitle, int iXPos, int iYPos, float fScale )
{
	ioUIRenderImage *pHeroIcon = GetHeroTitleIcon( dwHeroTitle );
	if( pHeroIcon )
	{
		pHeroIcon->SetScale( fScale );
		if( fScale == FLOAT1 )
		{
			pHeroIcon->Render( iXPos, iYPos );
		}
		else
		{
			pHeroIcon->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void ioMyLevelMgr::RenderHeroTitleText( DWORD dwHeroTitle, int iXPos, int iYPos )
{
	ioUIRenderImage *pHeroText = GetHeroTitleText( dwHeroTitle );
	if( pHeroText )
	{
		pHeroText->SetScale( FLOAT1 );
		pHeroText->Render( iXPos, iYPos );
	}
}

void ioMyLevelMgr::RenderLobbyOfflineMark(int iXPos, int iYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */, DWORD dwColor /* = 0xFFFFFFFF */ )
{
	if( m_pLobbyOffLineMark )
	{	
		m_pLobbyOffLineMark->SetColor( dwColor ); 
		m_pLobbyOffLineMark->Render( iXPos, iYPos, eRenderType );
	}
}

void ioMyLevelMgr::RenderMessengerOfflineMark( int iXPos, int iYPos, UIRenderType eRenderType /* = UI_RENDER_NORMAL  */, DWORD dwColor /* = 0xFFFFFFFF */ )
{
	if( m_pMessengerOffLineMark )
	{	
		m_pMessengerOffLineMark->SetColor( dwColor ); 
		m_pMessengerOffLineMark->Render( iXPos, iYPos, eRenderType );
	}
}

int ioMyLevelMgr::GetNextLevelupExp( int iCurLv )
{
	if( !COMPARE( iCurLv, 0, (int)m_vSoldierLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vSoldierLevelExp[iCurLv];
}

int ioMyLevelMgr::GetNextGradeupExp( int iCurLv )
{
	if( !COMPARE( iCurLv, 0, (int)m_vGradeLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vGradeLevelExp[iCurLv];
}

int ioMyLevelMgr::GetMaxGradeUp()
{
	return m_iGradeMaxUP;
}

int ioMyLevelMgr::GetMaxGradeLevel()
{
	return m_iGradeMaxLevel;
}

int ioMyLevelMgr::GetNextAwardupExp( int iCurLv )
{
	if( !COMPARE( iCurLv, 0, (int)m_vAwardLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vAwardLevelExp[iCurLv];
}

int ioMyLevelMgr::GetNextMedalupExp( int iCurLv )
{
	if( !COMPARE( iCurLv, 0, (int)m_vMedalLevelExp.size() ) )
		return MAX_INT_VALUE;
	return m_vMedalLevelExp[iCurLv];
}

__int64 ioMyLevelMgr::GetNextPartyupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fPartyConstantA, iCurLv ) * m_iPartyConstantB * ( iCurLv + m_iPartyConstantC ) ) - m_iPartyConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

__int64 ioMyLevelMgr::GetNextLadderupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fLadderConstantA, iCurLv ) * m_iLadderConstantB * ( iCurLv + m_iLadderConstantC ) ) - m_iLadderConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

__int64 ioMyLevelMgr::GetNextHeroupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fHeroConstantA, iCurLv ) * m_iHeroConstantB * ( iCurLv + m_iHeroConstantC ) ) - m_iHeroConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

__int64 ioMyLevelMgr::GetNextSoloupExp( int iCurLv )
{
	__int64 iNextExp = ( pow( m_fSoloConstantA, iCurLv ) * m_iSoloConstantB * ( iCurLv + m_iSoloConstantC ) ) - m_iSoloConstantD;
	if( iNextExp < 0 )
		iNextExp = 0;
	return iNextExp;
}

ioUIRenderImage* ioMyLevelMgr::GetGradeBigIcon( int iGradeLevel ) const
{
	switch( iGradeLevel )
	{
	case 0:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "00" );
	case 1:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "01" );
	case 2:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "02" );
	case 3:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "03" );
	case 4:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "04" );
	}

	int iMajorLevel = iGradeLevel / 5;
	switch( iMajorLevel )
	{
	case 1:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "05" );
	case 2:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "10" );
	case 3:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "15" );
	case 4:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "20" );
	case 5:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "25" );
	case 6:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "30" );
	case 7:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "35" );
	case 8:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "40" );
	case 9:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "45" );
	}

	switch( iGradeLevel )
	{
	case 50:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "50" );
	case 51:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "51" );
	case 52:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "52" );
	case 53:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "53" );
	case 54:
		return g_UIImageSetMgr.CreateImage( "BaseImage007", "54" );
	}

	return NULL;
}

ioUIRenderImage *ioMyLevelMgr::GetGradeCircleIcon( int iGradeLevel ) const
{
	if( COMPARE( iGradeLevel, 0, 35 ) )
	{
		char szBuf[MAX_PATH] = "";
		StringCbPrintf( szBuf, sizeof(szBuf), "Grade1#grade_%.2d", iGradeLevel ); //Except Extracting Hangeul
		return g_UIImageSetMgr.CreateImageByFullName( szBuf );
	}
	else if( COMPARE( iGradeLevel, 35, 55 ) )
	{
		char szBuf[MAX_PATH] = "";
		StringCbPrintf( szBuf, sizeof(szBuf), "Grade2#grade_%.2d", iGradeLevel ); //Except Extracting Hangeul
		return g_UIImageSetMgr.CreateImageByFullName( szBuf );
	}
	return NULL;
}

void ioMyLevelMgr::GetGradeName( int iGradeLevel, char *szBuf, int iBufSize, bool bStepGap, bool bStep /*= true */ ) const
{
	if( COMPARE( iGradeLevel, 0, 4 + 1 ) )
	{
		static char *szLowGradeText[10] = { STR(1), STR(2), STR(3), STR(4), STR(5) };
		strcpy( szBuf, szLowGradeText[iGradeLevel] );
	}
	else if( COMPARE( iGradeLevel, 5, 49 + 1 ) )
	{
		int iMajorLevel = iGradeLevel / 5 - 1;
		int iMinorLevel = iGradeLevel % 5;

		static char *szMajorGrade[10] = { STR(6), STR(7), STR(8), STR(9), STR(10), STR(11), STR(12), STR(13), STR(14) };

		if( bStepGap )
		{
			if( bStep )
				SafeSprintf( szBuf, iBufSize, STR(15), szMajorGrade[iMajorLevel], iMinorLevel+1 );
			else
				SafeSprintf( szBuf, iBufSize, STR(23), szMajorGrade[iMajorLevel], iMinorLevel+1 );
		}
		else
		{
			if( bStep )
				SafeSprintf( szBuf, iBufSize, STR(16), szMajorGrade[iMajorLevel], iMinorLevel+1 );
			else
				SafeSprintf( szBuf, iBufSize, STR(24), szMajorGrade[iMajorLevel], iMinorLevel+1 );
		}
	}
	else if( COMPARE( iGradeLevel, 50, 54 + 1 ) )
	{
		int iGrade = iGradeLevel - 50;

		static char *szGeneral[10] = { STR(17), STR(18), STR(19), STR(20), STR(21) };
		StringCbCopy( szBuf, iBufSize, szGeneral[iGrade] );
	}
	else
	{
		SafeSprintf( szBuf, iBufSize, STR(22) );
	}
}

int ioMyLevelMgr::GetNumHaveMortmainChar( int iGradeLv )
{
	for(vMortmainCharInfo::iterator iter = m_vMortmainCharInfoVector.begin(); iter != m_vMortmainCharInfoVector.end(); ++iter)
	{
		MortmainCharInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( iGradeLv <= pInfo->m_iBelowGradeLevel )
			return pInfo->m_iHaveMortmainChar;
	}

	return 0;
}

int ioMyLevelMgr::GetNextNumHaveMortmainChar( int iGradeLv )
{
	for(vMortmainCharInfo::iterator iter = m_vMortmainCharInfoVector.begin(); iter != m_vMortmainCharInfoVector.end(); ++iter)
	{
		MortmainCharInfo *pInfo = *iter;
		if( !pInfo )
			continue;

		if( iGradeLv <= pInfo->m_iBelowGradeLevel )
			return pInfo->m_iBelowGradeLevel+1;
	}

	return 54; // 에러시 최고 레벨 원수
}

int ioMyLevelMgr::GetNextFishingLevelUpExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vFishingLevelExp.size() ) )
		return MAX_INT_VALUE;

	return m_vFishingLevelExp[iCurLv];
}

int ioMyLevelMgr::GetMaxFishingLevel()
{
	return m_iFishingMaxLevel;
}

int ioMyLevelMgr::GetNextExcavationLevelUpExp( int iCurLv )
{
	// 구간 경험치
	if( !COMPARE( iCurLv, 0, (int)m_vExcavationLevelExp.size() ) )
		return MAX_INT_VALUE;

	return m_vExcavationLevelExp[iCurLv];
}

int ioMyLevelMgr::GetMaxExcavationLevel()
{
	return m_iExcavationMaxLevel;
}



int ioMyLevelMgr::GetNextAOSLevelUpExp(int iCurLv)
{
	if( !COMPARE( iCurLv, 0, (int)m_vecTowerDefLvlExp.size() ) )
		return MAX_INT_VALUE;

	return m_vecTowerDefLvlExp[iCurLv];
}

int ioMyLevelMgr::GetMaxAOSLevel()
{
	return m_nTowerDefMaxLvl;
}
