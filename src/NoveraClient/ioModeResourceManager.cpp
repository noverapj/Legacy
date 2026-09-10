#include "StdAfx.h"
#include "ioModeResourceManager.h"

template<> ioModeResourceManager* Singleton< ioModeResourceManager >::ms_Singleton = 0;

ioModeResourceManager& ioModeResourceManager::GetSingleton()
{
	return Singleton< ioModeResourceManager >::GetSingleton();
}

ioModeResourceManager *ioModeResourceManager::GetSingletonPtr()
{
	return Singleton< ioModeResourceManager >::GetSingletonPtr();
}

ioModeResourceManager::ioModeResourceManager()
{
}

ioModeResourceManager::~ioModeResourceManager()
{
	for( vModeIconList::iterator iter = m_vModeIconList.begin(); iter != m_vModeIconList.end(); ++iter )
	{
		SAFEDELETE( iter->pModeIcon );
		SAFEDELETE( iter->pUserModeIcon );
	}
	m_vModeIconList.clear();
}

void ioModeResourceManager::GetModeTitle( OUT ioHashString& szOut, int eModeType )
{
	// 로딩 배경 로드
	switch( eModeType )
	{
	case MT_SYMBOL:
		szOut = "power";		
		break;
	case MT_TOWER_DEFENSE:
		szOut = "tower_defense";		
		break;
	case MT_DARK_XMAS:
		szOut = "dark_xmas";		
		break;
	case MT_FIRE_TEMPLE:
		szOut = "fire_temple";		
		break;
	case MT_UNDERWEAR:
		szOut = "underwear";		
		break;
	case MT_CBT:
		szOut = "cbt_mode";		
		break;
	case MT_CATCH:
		szOut = "prisoner";		
		break;
	case MT_CATCH_RUNNINGMAN:
		szOut = "prisoner_runningman";		
		break;
	case MT_KING:
		szOut = "king";
		break;
	case MT_SURVIVAL:
		szOut = "survival";		
		break;
	case MT_FIGHT_CLUB:
		szOut = "fight";		
		break;
	case MT_TEAM_SURVIVAL:
		szOut = "teamsurvival";		
		break;
	case MT_BOSS:
		szOut = "boss";		
		break;
	case MT_MONSTER_SURVIVAL:
		szOut = "monstersurvival";		
		break;
	case MT_FOOTBALL:
		szOut = "football";		
		break;
	case MT_HEROMATCH:
		szOut = "heromatch";		
		break;
	case MT_GANGSI:
		szOut = "gangsi";	
		break;
	case MT_DUNGEON_A:
		szOut = "dungeon_a";		
		break;
	case MT_DOUBLE_CROWN:
		szOut = "doublecrown";
		break;
	case MT_HEADQUARTERS:
		szOut = "headquarters";
		break;
	case MT_SHUFFLE_BONUS:
		szOut = "shufflebonus";
		break;
	case MT_FACTORY:
		szOut = "factory";
		break;
	case MT_TEAM_SURVIVAL_AI:
		szOut = "teamsurvivalAI";		
		break;
	case MT_HOUSE:
		szOut = "house";
		break;
	case MT_RAID:
		szOut = "raid";
		break;
	case MT_SUCCESSION:
		szOut = "succession";
		break;
	case MT_PRACTICE:
		szOut = "practice";
		break;
	case MT_FLAG_CAPTURE:
		szOut = "flagcapture";
		break;
	case MT_ARENA:
		szOut = "arena";
		break;
	case MT_BATTLE:
		szOut = "battle";
		break;
	case MT_FARMING:
		szOut = "farming";
		break;
	default:
		szOut = "";
		break;
	}
}

void ioModeResourceManager::GetPlazaTitle( OUT ioHashString& szOut, int ePlazaType )
{
	// 로딩 배경 로드
	switch( ePlazaType )
	{
	case PT_COMMUNITY:
		szOut = "plaza_community";
		break;
	case PT_GUILD:
		szOut = "plaza_guild";
		break;
	default:
		szOut = "plaza_battle";
		break;
	}
}

void ioModeResourceManager::GetModeRoomTitle( OUT ioHashString& szOut, int eMyRoomType, bool bCampHeroBattle )
{
	switch( eMyRoomType )
	{
	case MRT_LOBBY:
		szOut = "myroom_lobby";
		break;
	case MRT_BATTLE:
	case MRT_SHUFFLE:
		szOut = "myroom_battle";
		break;
	case MRT_FIND_MODE:
		szOut = "myroom_find_mode";
		break;
	case MRT_LADDER:
		if( bCampHeroBattle )
		{
			szOut = "myroom_ladder_hero";
		}
		else
		{
			szOut = "myroom_ladder";
		}
		break;
	case MRT_RANKBATTLE:
		szOut = "myroom_rankbattle";
		break;
	default:
		szOut = "myroom_practice";
		break;
	}
}
void ioModeResourceManager::InitCommonModeIcon()
{
	ioINILoader_e kLoader( "config/sp2_mode_loading.ini" );

	// 로딩 리소스 로드
	for( int eModeType = MT_NONE; eModeType < MAX_MODE_TYPE; ++eModeType )
	{
		ioHashString szTitle;		
		if( eModeType == MT_TRAINING )
		{
			//광장 리소스
			for( int ePlaza = PT_BATTLE; ePlaza <= PT_GUILD; ++ePlaza )
			{
				GetPlazaTitle( szTitle, ePlaza );
				AddModeIcon( kLoader, szTitle, eModeType, ePlaza, 0 );
			}
		}
		else if( eModeType == MT_NONE || eModeType == MT_MYROOM )
		{
			//룸 타입별 리소스( 로비, 광장, 오늘의 모드 등)
			for( int eRoom = MRT_LOBBY; eRoom <= MRT_RANKBATTLE; ++eRoom )
			{
				GetModeRoomTitle( szTitle, eRoom, false );
				AddModeIcon( kLoader, szTitle, eModeType, eRoom, FALSE );

				//래더 - 진영전
				if( eRoom == MRT_LADDER )
				{
					GetModeRoomTitle( szTitle, eRoom, true );
					AddModeIcon( kLoader, szTitle, eModeType, eRoom, TRUE );
				}
			}
		}
		else
		{
			//일반 모드
			GetModeTitle( szTitle, eModeType );
			AddModeIcon( kLoader, szTitle, eModeType, 0, 0 );
		}
	}
}

void ioModeResourceManager::AddModeIcon( ioINILoader& rkLoader, const ioHashString& szTitle, int eType, int eKindValue1, int eKindValue2 )
{
	rkLoader.SetTitle( szTitle.c_str() );
	char szBuf[MAX_PATH];

	ModeIcon Icon;
	Icon.eType			= eType;
	Icon.eKindValue1	= eKindValue1;
	Icon.eKindValue2	= eKindValue2;

	rkLoader.LoadString( "icon_image_user_mode", "", szBuf, MAX_PATH );
	Icon.pUserModeIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );

	rkLoader.LoadString( "icon_image", "", szBuf, MAX_PATH );
	Icon.pModeIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf, true );
	
	m_vModeIconList.push_back( Icon );
}

ioUIRenderImage* ioModeResourceManager::GetModeIcon( int eType )
{
	vModeIconList::iterator iter = m_vModeIconList.begin();
	for( ; iter != m_vModeIconList.end(); ++iter )
	{
		const ModeIcon& rkIcon = *iter;

		//일반 모드의 경우 eKindValue1/2는 초기값으로 세팅되어있음
		if( rkIcon.eType == eType && rkIcon.eKindValue1 == 0 && rkIcon.eKindValue2 == 0 )
		{
			if( rkIcon.pModeIcon )
				rkIcon.pModeIcon->SetScale( FLOAT1, FLOAT1 );

			return rkIcon.pModeIcon;
		}
	}

	return NULL;
}

ioUIRenderImage* ioModeResourceManager::GetModeIconByUserMode( int eType )
{
	vModeIconList::iterator iter = m_vModeIconList.begin();
	for( ; iter != m_vModeIconList.end(); ++iter )
	{
		const ModeIcon& rkIcon = *iter;

		//일반 모드의 경우 eKindValue1/2는 초기값으로 세팅되어있음
		if( rkIcon.eType == eType && rkIcon.eKindValue1 == 0 && rkIcon.eKindValue2 == 0 )
		{
			if( rkIcon.pUserModeIcon )
				rkIcon.pUserModeIcon->SetScale( FLOAT1, FLOAT1 );

			return rkIcon.pUserModeIcon;
		}
	}

	return NULL;
}

ioUIRenderImage* ioModeResourceManager::GetPlazaIcon( int ePlazaType )
{
	vModeIconList::iterator iter = m_vModeIconList.begin();
	for( ; iter != m_vModeIconList.end(); ++iter )
	{
		const ModeIcon& rkIcon = *iter;
				
		if( rkIcon.eType == MT_TRAINING && rkIcon.eKindValue1 == ePlazaType && rkIcon.eKindValue2 == 0 )
		{
			if( rkIcon.pModeIcon )
				rkIcon.pModeIcon->SetScale( FLOAT1, FLOAT1 );

			return rkIcon.pModeIcon;
		}
	}

	return NULL;
}

ioUIRenderImage* ioModeResourceManager::GetModeRoomIcon( int eRoomType, bool bCampHeroBattle )
{
	vModeIconList::iterator iter = m_vModeIconList.begin();
	for( ; iter != m_vModeIconList.end(); ++iter )
	{
		const ModeIcon& rkIcon = *iter;
		if( rkIcon.eType == MT_NONE || rkIcon.eType == MT_MYROOM || eRoomType == MRT_FIND_MODE )
		{
			if( rkIcon.eKindValue1 == eRoomType && rkIcon.eKindValue2 == static_cast<int>( bCampHeroBattle ) )
			{
				if( rkIcon.pModeIcon )
					rkIcon.pModeIcon->SetScale( FLOAT1, FLOAT1 );

				return rkIcon.pModeIcon;
			}
		}
	}

	return NULL;
}