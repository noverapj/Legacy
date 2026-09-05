#include "stdafx.h"

#include "ioNpcChar.h"
#include "ioNpcDeveloperK.h"
#include "ioNpcBigTower.h"
#include "ioAI.h"
#include "ioChampionAI.h"

#include "ioPlayStage.h"
#include "ioNpcMgr.h"
#include "ioAISymbolTable.h"
#include "ioAIVersions.h"
#include "ioAIXMLDoc.h"

bool	isXMLFile( const char* szBuf )
{
	for( int i = strlen( szBuf ) - 1; i > 3; i-- )
	{
		if(	(	szBuf[i]	== 'l' ||	szBuf[i]	== 'L' ) && 
			(	szBuf[i-1]	== 'm' ||	szBuf[i-1]	== 'M' ) &&
			(	szBuf[i-2]	== 'x' ||	szBuf[i-2]	== 'X' ) )
		{
			return true;
		}
	}

	return false;
}

template<> ioNpcMgr* Singleton< ioNpcMgr >::ms_Singleton = 0;
ioNpcMgr::ioNpcMgr()
{
	m_dwGradeUpTime = 0;
}

ioNpcMgr::~ioNpcMgr()
{
	ClearFeidNPC();
}

ioNpcMgr& ioNpcMgr::GetSingleton()
{
	return Singleton< ioNpcMgr >::GetSingleton();
}

void ioNpcMgr::LoadFieldNPC()
{
	ClearFeidNPC();

	ioINILoader_e kLoader( "config/sp2_npc_info.ini" );
	kLoader.SetTitle_e( "Common" );

	int iMaxNPC = kLoader.LoadInt_e( "MaxNpc", 0 );
	for(int i = 0;i < iMaxNPC;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "FieldNpc%d", i + 1 );
		kLoader.SetTitle( szTitle );


		char szBuf[MAX_PATH] = "";
		kLoader.LoadString_e( "NpcViewName", "", szBuf, MAX_PATH );
		if( std::string(szBuf).size() > 0 )	
		{	
			FieldNPCData *pNpcData = new FieldNPCData;
			pNpcData->m_szNpcViewName = szBuf;
		
			kLoader.LoadString_e( "NpcInfo", "", szBuf, MAX_PATH );
			pNpcData->m_szNpcInfo_ini = szBuf;

			kLoader.LoadString_e( "NpcGradeTitle", "", szBuf, MAX_PATH );
			pNpcData->m_szGradeTitle  = szBuf;

			kLoader.LoadString_e( "NpcRaceInfo", "", szBuf, MAX_PATH );
			pNpcData->m_szNpcRaceInfo_ini = szBuf;

			kLoader.LoadString_e( "NpcAI", "", szBuf, MAX_PATH );
			if( isXMLFile( szBuf ) )
				pNpcData->m_szNpcAI_xml   = szBuf;
			else
				pNpcData->m_szNpcAI_ini   = szBuf;

			pNpcData->m_iNpcType = kLoader.LoadInt_e( "NpcType", 0 );
			pNpcData->m_iRandomAppearance = kLoader.LoadInt_e( "RandomAppreance", 0 );

			int iMaxEquip = kLoader.LoadInt_e( "MaxEquip", 0 );
			for(int j = 0;j < iMaxEquip;j++)
			{
				char szKey[MAX_PATH] = "";
				sprintf_e( szKey, "Equip%d_Code", j + 1 );
				pNpcData->m_vEquipItemList.push_back( kLoader.LoadInt( szKey, 0 ) );
			}

			m_FieldNPCMap.insert( FieldNPCDataMap::value_type( i + 1, pNpcData ) );
		}
	}

	// additional NPC list
	for( ioAISymbolTable::ConfigList::iterator  
				it = g_AISymbolTable.GetConfigList().begin();
				it != g_AISymbolTable.GetConfigList().end();
				it++ )
	{
		if( (*it)->GetConfigType() == ioAIConfigData::AI_CONFIG_NPCLIST )
		{
			ioAIXMLDoc	npcXML;
		
			if( npcXML.Load( (*it)->GetFile().c_str(), (*it)->GetVersion(), true ) )
			{
				BuildAdditionalNPC( &(npcXML.GetRootElement()) );
			}
			else LOG.PrintTimeAndLog( 0, "failed to load xml file named %s.", (*it)->GetFile().c_str() );
		}
	}

	LoadGradeTable();
}

void ioNpcMgr::BuildAdditionalNPC( ioXMLElement* npclist )
{
	if( npclist != (ioXMLElement*)0 && !npclist->IsEmpty() )
	{
		ioXMLElement* npc = &(npclist->FirstChild( "NPC" ));
		while( npc != (ioXMLElement*)0 && !npc->IsEmpty() )
		{
			// search npc map if there exist any NPC with same id
			unsigned int	id = (unsigned int)npc->GetIntAttribute_e( "id" );
			if( m_FieldNPCMap.find( id ) == m_FieldNPCMap.end() || 
				m_FieldNPCMap.find( id )->second == (FieldNPCData*)0 )
			{
				FieldNPCData *pNpcData = new FieldNPCData;

				pNpcData->m_iNpcType = npc->GetIntAttribute_e( "NpcType" );
				pNpcData->m_iRandomAppearance = npc->GetIntAttribute_e( "RandomAppreance" );
				BuildAdditionalNPCLabel( pNpcData, &(npc->FirstChild( "LABEL" )) );
				BuildAdditionalNPCRef( pNpcData, &(npc->FirstChild( "REFERENCE" )) );
				BuildAdditionalNPCEquip( pNpcData, &(npc->FirstChild( "EQUIPLIST" )) );

				m_FieldNPCMap.insert( FieldNPCDataMap::value_type( id , pNpcData ) );
			}
			else LOG.PrintTimeAndLog( 0, "Id duplacated : %d was previously defined.", id );
			npc = &(npc->NextSibling( "NPC" ));
		}
	}
	else LOG.PrintTimeAndLog( 0, "xml node npclist is invalid." );
}

void ioNpcMgr::BuildAdditionalNPCLabel( FieldNPCData *pData, ioXMLElement* node )
{
	if( !node->IsEmpty() )
	{
		pData->m_szNpcViewName	= node->GetStringAttribute_e( "NpcViewName" );
		pData->m_szGradeTitle	= node->GetStringAttribute_e( "NpcGradeTitle" );
	}
	else LOG.PrintTimeAndLog( 0, "LABEL node is invalid." );
}

void ioNpcMgr::BuildAdditionalNPCRef( FieldNPCData *pData, ioXMLElement* node )
{
	if( !node->IsEmpty() )
	{
		pData->m_szNpcInfo_ini		= node->GetStringAttribute_e( "NpcInfo" );
		pData->m_szNpcRaceInfo_ini	= node->GetStringAttribute_e( "NpcRaceInfo" );

		std::string		aifile = node->GetStringAttribute_e( "NpcAI" );
		if( isXMLFile( aifile.c_str() ) )
			pData->m_szNpcAI_xml   = aifile.c_str();
		else
			pData->m_szNpcAI_ini   = aifile.c_str();
	}
	else LOG.PrintTimeAndLog( 0, "REFERENCE node is invalid." );
}

void ioNpcMgr::BuildAdditionalNPCEquip( FieldNPCData *pData, ioXMLElement* node )
{
	if( !node->IsEmpty() )
	{
		ioXMLElement* child = &(node->FirstChild( "EQUIP" ));
		while( !child->IsEmpty() )
		{
			pData->m_vEquipItemList.push_back( child->GetIntAttribute_e( "itemcode" ) );
			child = &(child->NextSibling( "EQUIP" ));
		}
	}
	else LOG.PrintTimeAndLog( 0, "EQUIPLIST node is invalid." );
}

void ioNpcMgr::LoadGradeTable()
{
	int i = 0;
	char szKey[MAX_PATH] = "";	
	char szBuf[MAX_PATH] = "";
	ioINILoader_e kLoader( "config/sp2_npc_grade_info.ini" );

	kLoader.SetTitle_e( "Common" );
	m_dwGradeUpTime = kLoader.LoadInt_e( "GradeUpTime", 0 );

	kLoader.LoadString_e( "DefaultViewName", "", szBuf, MAX_PATH );
	m_GradeZeroEquip.m_szViewName = szBuf;

	m_GradeZeroEquip.m_iEquipItem[ES_WEAPON] = kLoader.LoadInt_e( "DefaultEquip1", 0 );
	m_GradeZeroEquip.m_iEquipItem[ES_ARMOR]  = kLoader.LoadInt_e( "DefaultEquip2", 0 );
	m_GradeZeroEquip.m_iEquipItem[ES_HELMET] = kLoader.LoadInt_e( "DefaultEquip3", 0 );
	m_GradeZeroEquip.m_iEquipItem[ES_CLOAK]  = kLoader.LoadInt_e( "DefaultEquip4", 0 );

	kLoader.LoadString_e( "DefaultNpcAI", "", szBuf, MAX_PATH );
	m_GradeZeroEquip.m_szAI = szBuf;

	
	{
		kLoader.SetTitle_e( "RandomGrade" );
		int iMaxTable = kLoader.LoadInt_e( "MaxRandom", 0 );
		for(i = 0;i < iMaxTable;i++)
		{
			GradeTable kTable;

			sprintf_e( szKey, "LimitGrade_%d", i + 1 );
			kTable.m_iLimitGrade = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "MinimumRand_%d", i + 1 );
			kTable.m_iMinimumRand = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "MaximumRand_%d", i + 1 );
			kTable.m_iMaximumRand = kLoader.LoadInt( szKey, 0 );

			m_GradeTable.push_back( kTable );
		}
	}

	{
		kLoader.SetTitle_e( "GradeAbility" );
		int iMaxAbility = kLoader.LoadInt_e( "MaxAbility", 0 );
		for(i = 0;i < iMaxAbility;i++)
		{
			char szTitle[MAX_PATH] = "";
			sprintf_e( szTitle, "GradeAbility%d", i + 1 );
			kLoader.SetTitle( szTitle );

			GradeAbility kAbility;
			kAbility.m_iGradeLevel = kLoader.LoadInt_e( "GradeLevel", 0 );

			kAbility.m_bEquipItem  = kLoader.LoadBool_e( "EquipItem", false );
			kAbility.m_dwProtected = kLoader.LoadInt_e( "Protected", 0 );
			kAbility.m_bLevelUpMotion = kLoader.LoadBool_e( "LevelUpMotion", false );
			kAbility.m_fRecoveryHpRate = kLoader.LoadFloat_e( "RecoveryHpRate", 0.0f );
			kLoader.LoadString_e( "Skeleton", "", szBuf, MAX_PATH );
			kAbility.m_szSkeleton = szBuf;

			kAbility.m_fArmorClass = kLoader.LoadFloat_e( "ArmorClass", 0.0f );
			kAbility.m_fMaxArmorClass = kLoader.LoadFloat_e( "MaxArmorClass", 0.0f );
			kAbility.m_fSpeedClass = kLoader.LoadFloat_e( "SpeedClass", 0.0f );
			kAbility.m_fMaxSpeedClass = kLoader.LoadFloat_e( "MaxSpeedClass", 0.0f );
			m_GradeAbility.push_back( kAbility );
		}
	}

	{
		kLoader.SetTitle_e( "EquipRandTable" );
		int iMaxTable = kLoader.LoadInt_e( "MaxTable", 0 );
		for(i = 0;i < iMaxTable;i++)
		{
			char szTitle[MAX_PATH] = "";
			sprintf_e( szTitle, "EquipRandTable%d", i + 1 );
			kLoader.SetTitle( szTitle );

			GradeEquipRandTable kEquip;
			kLoader.LoadString_e( "NpcViewName", "", szBuf, MAX_PATH );
			kEquip.m_szViewName = szBuf;

			kEquip.m_iEquipItem[ES_WEAPON] = kLoader.LoadInt_e( "Equip1_Code", 0 );
			kEquip.m_iEquipItem[ES_ARMOR]  = kLoader.LoadInt_e( "Equip2_Code", 0 );
			kEquip.m_iEquipItem[ES_HELMET] = kLoader.LoadInt_e( "Equip3_Code", 0 );
			kEquip.m_iEquipItem[ES_CLOAK]  = kLoader.LoadInt_e( "Equip4_Code", 0 );
			
			kLoader.LoadString_e( "NpcAI", "", szBuf, MAX_PATH );
			kEquip.m_szAI = szBuf;
			m_GradeEquipRandTable.push_back( kEquip );
		}
	}
}

const ioNpcMgr::FieldNPCData *ioNpcMgr::FindFieldNPC( DWORD dwCode )
{
	FieldNPCDataMap::iterator iter = m_FieldNPCMap.find( dwCode );
	if( iter != m_FieldNPCMap.end() )
		return iter->second;
	return NULL;
}

void ioNpcMgr::ClearFeidNPC()
{
	FieldNPCDataMap::iterator iCreate = m_FieldNPCMap.begin();
	for(;iCreate != m_FieldNPCMap.end();iCreate++)
	{
		SAFEDELETE( iCreate->second );
	}
	m_FieldNPCMap.clear();
}

ioNpcChar* ioNpcMgr::CreateNpcCharByCharacter( SP2Packet &rkPacket, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode )
{
	int iClassLevel;
	DWORD dwCharIndex;
	rkPacket >> dwCharIndex >> iClassLevel;

	CHARACTER kCharInfo;
	rkPacket >> kCharInfo;

	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );

	char szFileName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szFileName, MAX_PATH );	

	ioEntityGroup *pGrp = pPlayStage->CreateEntityGrpAndSceneNode( szFileName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcCharByCharacter - %s Create Failed", szFileName );
		return NULL;
	}

	ioNpcChar *pNpcChar = CreateNpcChar( pGrp, pPlayStage, pPlayMode, NT_DEFAULT );
	if( !pNpcChar )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcCharByCharacter - %s Create Failed 2", szFileName );
		return NULL;
	}

	pNpcChar->SetShadowCastEnable( true );
	pPlayStage->AddGameEntity( pNpcChar );
	pPlayStage->AddBaseChar( pNpcChar );
	pPlayStage->AddAniEventHandler( pNpcChar, pGrp );

	pNpcChar->InitINIValue( rkLoader );	
	pNpcChar->InitRaceInfo( kCharInfo );

	pNpcChar->SetNPCIndex( NPC_NOT_INDEX );

	pNpcChar->Initialize( NULL );

	pNpcChar->SetIndex( dwCharIndex );
	pNpcChar->SetClassLevel( iClassLevel );

	pNpcChar->SetState( CS_LOADING );	

	return pNpcChar;
}

ioNpcChar* ioNpcMgr::CreateNpcCharByMyCharacter( SP2Packet &rkPacket, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode )
{
	DWORD dwCharIndex;
	rkPacket >> dwCharIndex;

	int iCharArray = g_MyInfo.GetCharArray( dwCharIndex );
	const CHARACTER kCharInfo = g_MyInfo.GetCharacter( iCharArray );
	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );

	char szFileName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szFileName, MAX_PATH );	

	ioEntityGroup *pGrp = pPlayStage->CreateEntityGrpAndSceneNode( szFileName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcCharByCharacter - %s Create Failed", szFileName );
		return NULL;
	}

	ioNpcChar *pNpcChar = CreateNpcChar( pGrp, pPlayStage, pPlayMode, NT_DEFAULT );
	if( !pNpcChar )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcCharByCharacter - %s Create Failed 2", szFileName );
		return NULL;
	}

	pNpcChar->SetShadowCastEnable( true );
	pPlayStage->AddGameEntity( pNpcChar );
	pPlayStage->AddBaseChar( pNpcChar );
	pPlayStage->AddAniEventHandler( pNpcChar, pGrp );

	pNpcChar->InitINIValue( rkLoader );	
	pNpcChar->InitRaceInfo( kCharInfo );	

	pNpcChar->Initialize( NULL );

	pNpcChar->SetIndex( dwCharIndex );
	pNpcChar->SetClassLevel( g_MyInfo.GetClassLevel( kCharInfo.m_class_type, false ) );


	pNpcChar->SetState( CS_LOADING );

	return pNpcChar;
}

ioNpcChar* ioNpcMgr::CreateNpcChar( DWORD dwCode, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode )
{
	const FieldNPCData *pNpcData = FindFieldNPC( dwCode );
	if( !pNpcData )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar : NpcData NULL!! :%d - %d", dwCode, m_FieldNPCMap.size() );
		return NULL;
	}

	if( !pPlayStage )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar : PlayStage NULL!! :%d - %d", dwCode, m_FieldNPCMap.size() );
		return NULL;
	}

	ioEntityGroup		*pGrp = (ioEntityGroup*)0;
	ioNpcChar			*pNpcChar = (ioNpcChar*)0;
	ioNpcAppearance		*npcappr = (ioNpcAppearance*)0;

	ioINILoader kInfoLoader( pNpcData->m_szNpcInfo_ini.c_str() );
	kInfoLoader.SetTitle_e( "developer" );

	// try loading appearance information from source
	// this time, we'll determine it with id, but will change ai file
	if( pNpcData->m_iRandomAppearance == 1 )
	{
		// load npc_ai_appearance.xml
		npcappr = g_AIDB.GetRandomAppearanceData();

		if( !npcappr )	
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar : npc appearance data NULL!! :%d - %d", dwCode );
			return NULL;
		}

		/*
		LOG.PrintTimeAndLog( 0,
			"race = %d, sex = %d, beard = %d, accessory = %d, face = %d, hair = %d, skincolor = %d, haircolor = %d, underwear = %d, model = %s, raceinfo = %s ", 
						npcappr->m_race,	
						npcappr->m_sex, 
						npcappr->m_beard,	
						npcappr->m_accessory, 
						npcappr->m_face, 
						npcappr->m_hair, 
						npcappr->m_skincolor, 
						npcappr->m_haircolor, 
						npcappr->m_underwear,
						npcappr->m_model.c_str(), 
						npcappr->m_raceinfo.c_str() );
		*/

		pGrp = pPlayStage->CreateEntityGrpAndSceneNode( npcappr->m_model.c_str(), true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 1", npcappr->m_model.c_str() );
			return NULL;
		}

		// NPC 생성
		pNpcChar = CreateNpcChar( pGrp, pPlayStage, pPlayMode, pNpcData->m_iNpcType );
		if( !pNpcChar )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 2", npcappr->m_model.c_str() );
			return NULL;
		}
	}
	else
	{
		// NPC 모델 정보	
		char szModelName[MAX_PATH] = "";
		kInfoLoader.LoadString_e( "model_file", "", szModelName, MAX_PATH );

		pGrp = pPlayStage->CreateEntityGrpAndSceneNode( szModelName, true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 1", szModelName );
			return NULL;
		}

		// NPC 생성
		pNpcChar = CreateNpcChar( pGrp, pPlayStage, pPlayMode, pNpcData->m_iNpcType );
		if( !pNpcChar )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 2", szModelName );
			return NULL;
		}
	}

	pNpcChar->SetNpcCode( dwCode );
	pNpcChar->SetShadowCastEnable( true );
	pPlayStage->AddGameEntity( pNpcChar );
	pPlayStage->AddBaseChar( pNpcChar );
	pPlayStage->AddAniEventHandler( pNpcChar, pGrp );

	// NPC 세팅
	pNpcChar->SetNpcViewName( pNpcData->m_szNpcViewName );
	pNpcChar->SetNpcGradeTitle( pNpcData->m_szGradeTitle );
	pNpcChar->InitINIValue( kInfoLoader );

	// NPC 레이스 정보 INI
	if( pNpcData->m_iRandomAppearance == 1 )
	{
		pNpcChar->InitRaceInfo( npcappr );
	}
	else
	{
		ioINILoader kRaceLoader( pNpcData->m_szNpcRaceInfo_ini.c_str() );
		pNpcChar->InitRaceInfo( kRaceLoader );
	}

	// NPC 전용 정보
	pNpcChar->InitCustomInfo( kInfoLoader );
	pNpcChar->InitCustomEffect( kInfoLoader );
	pNpcChar->InitHeadMeshInfo( kInfoLoader );

	// 장착 아이템
	ITEM_DATA kEquipItem[MAX_INVENTORY];
	int iMaxItem = pNpcData->m_vEquipItemList.size();;
	for(int i = 0;i < iMaxItem;i++)
	{
		kEquipItem[i].m_item_code = pNpcData->m_vEquipItemList[i];
	}
	pNpcChar->Initialize( (const ITEM_DATA *)&kEquipItem );

	// NPC AI 정보 INI
	CreateAI( pNpcChar, pPlayStage, pNpcData->m_szNpcAI_ini, pNpcData->m_szNpcAI_xml );

	pNpcChar->SetState( CS_LOADING );
	return pNpcChar;
}

ioNpcChar* ioNpcMgr::CreateNpcChar( DWORD dwNPCIndex, DWORD dwCode, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode )
{
	const FieldNPCData *pNpcData = FindFieldNPC( dwCode );
 	if( !pNpcData )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar : NpcData NULL!! :%d - %d", dwCode, m_FieldNPCMap.size() );
		return NULL;
	}

	if( !pPlayStage )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar : PlayStage NULL!! :%d - %d", dwCode, m_FieldNPCMap.size() );
		return NULL;
	}

	ioEntityGroup		*pGrp = (ioEntityGroup*)0;
	ioNpcChar			*pNpcChar = (ioNpcChar*)0;
	ioNpcAppearance		*npcappr = (ioNpcAppearance*)0;

	ioINILoader kInfoLoader( pNpcData->m_szNpcInfo_ini.c_str() );
	kInfoLoader.SetTitle_e( "developer" );

	// try loading appearance information from source
	// this time, we'll determine it with id, but will change ai file
	if( pNpcData->m_iRandomAppearance == 1 )
	{
		// load npc_ai_appearance.xml
		npcappr = g_AIDB.GetRandomAppearanceData();

		if( !npcappr )	
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar : npc appearance data NULL!! :%d - %d", dwCode );
			return NULL;
		}

		/*
		LOG.PrintTimeAndLog( 0,
			"race = %d, sex = %d, beard = %d, accessory = %d, face = %d, hair = %d, skincolor = %d, haircolor = %d, underwear = %d, model = %s, raceinfo = %s ", 
						npcappr->m_race,	
						npcappr->m_sex, 
						npcappr->m_beard,	
						npcappr->m_accessory, 
						npcappr->m_face, 
						npcappr->m_hair, 
						npcappr->m_skincolor, 
						npcappr->m_haircolor, 
						npcappr->m_underwear,
						npcappr->m_model.c_str(), 
						npcappr->m_raceinfo.c_str() );
		*/

		pGrp = pPlayStage->CreateEntityGrpAndSceneNode( npcappr->m_model.c_str(), true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 1", npcappr->m_model.c_str() );
			return NULL;
		}

		// NPC 생성
		pNpcChar = CreateNpcChar( pGrp, pPlayStage, pPlayMode, pNpcData->m_iNpcType );
		if( !pNpcChar )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 2", npcappr->m_model.c_str() );
			return NULL;
		}
	}
	else
	{
		// NPC 모델 정보	
		char szModelName[MAX_PATH] = "";
		kInfoLoader.LoadString_e( "model_file", "", szModelName, MAX_PATH );

		pGrp = pPlayStage->CreateEntityGrpAndSceneNode( szModelName, true );
		if( !pGrp )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 1", szModelName );
			return NULL;
		}

		// NPC 생성
		pNpcChar = CreateNpcChar( pGrp, pPlayStage, pPlayMode, pNpcData->m_iNpcType );
		if( !pNpcChar )
		{
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar - %s Create Failed 2", szModelName );
			return NULL;
		}
	}

	pNpcChar->SetNpcCode( dwCode );
	pNpcChar->SetShadowCastEnable( true );
	pPlayStage->AddGameEntity( pNpcChar );
	pPlayStage->AddBaseChar( pNpcChar );
	pPlayStage->AddAniEventHandler( pNpcChar, pGrp );

	// NPC 세팅
	pNpcChar->SetNpcViewName( pNpcData->m_szNpcViewName );
	pNpcChar->SetNpcGradeTitle( pNpcData->m_szGradeTitle );
	pNpcChar->InitINIValue( kInfoLoader );

	// NPC 레이스 정보 INI
	if( pNpcData->m_iRandomAppearance == 1 )
	{
		pNpcChar->InitRaceInfo( npcappr );
	}
	else
	{
		ioINILoader kRaceLoader( pNpcData->m_szNpcRaceInfo_ini.c_str() );
		pNpcChar->InitRaceInfo( kRaceLoader );
	}

	// NPC 전용 정보
	pNpcChar->InitCustomInfo( kInfoLoader );
	pNpcChar->InitCustomEffect( kInfoLoader );
	pNpcChar->InitHeadMeshInfo( kInfoLoader );
	
	pNpcChar->SetNPCIndex( dwNPCIndex );

	// 장착 아이템
	ITEM_DATA kEquipItem[MAX_INVENTORY];
	int iMaxItem = pNpcData->m_vEquipItemList.size();;
	for(int i = 0;i < iMaxItem;i++)
	{
		kEquipItem[i].m_item_code = pNpcData->m_vEquipItemList[i];
	}
	pNpcChar->Initialize( (const ITEM_DATA *)&kEquipItem );

	// NPC AI 정보 INI
	CreateAI( pNpcChar, pPlayStage, pNpcData->m_szNpcAI_ini, pNpcData->m_szNpcAI_xml );

	pNpcChar->SetState( CS_LOADING );
	return pNpcChar;
}

ioNpcChar* ioNpcMgr::CreateNpcChar( ioEntityGroup *pGrp, ioPlayStage *pPlayStage, ioPlayMode *pPlayMode, int iNpcType )
{	
	ioNpcChar *pNpcChar = NULL;
	switch( iNpcType )
	{
	case NT_DEFAULT:
		pNpcChar = new ioNpcChar( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_DEVELOPER_K:
		pNpcChar = new ioNpcDeveloperK( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_BIGTOWER:
		pNpcChar = new ioNpcBigTower( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_TOWERGUARD:
		pNpcChar = new ioNpcTwGuard( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_LAIR:
		pNpcChar = new ioNpcLair( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_LAIRGUARD:
		pNpcChar = new ioNpcLairGuard( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_NEXUS:
		pNpcChar = new ioNpcNexus( pGrp, pPlayStage, pPlayMode );
		break;
	case NT_NEXUSGUARD:
		pNpcChar = new ioNpcNexusGuard( pGrp, pPlayStage, pPlayMode );
		break;

	default:
		{
			pNpcChar = new ioNpcChar( pGrp, pPlayStage, pPlayMode );
			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateNpcChar Create None Type : %d", iNpcType );
		}
		break;
	}
	return pNpcChar;
}

ioAI* ioNpcMgr::CreateAI(	ioNpcChar *pCreator, 
							ioPlayStage *pPlayStage, 
							const ioHashString &rkINIFileName,
							const ioHashString &rkXMLFileName )
{

	if( pCreator == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateAI Create None NPC : %s", rkINIFileName );
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateAI Create None NPC : %s", rkXMLFileName );
		return NULL;
	}

	if( rkXMLFileName.IsEmpty() )
		return CreateAI( pCreator, pPlayStage, rkINIFileName );

	ioChampionAI *pAI = NULL;

	// return null if aitype is not 2
	//ioXMLDocument	xmlDoc;
	ioAIXMLDoc	xmlDoc;
	
//	std::ifstream	ifile( rkXMLFileName.c_str() );

//	if( ifile )
//	{
	// just show warning if the version mismatches
	if( xmlDoc.Load( rkXMLFileName.c_str(), g_szAIFileVersion ) ) 
	{
		pAI = new ioChampionAI( pCreator, pPlayStage );
//		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateAI Create Champion AI Type : %s", rkXMLFileName );
	
		if( pAI )
		{
			// kaseton
			pAI->LoadAI( xmlDoc );
			pCreator->SetAI( pAI );
		}
	}
	else
	{
		//LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateAI AI Creation failed with : %s", rkXMLFileName );
	}
//	}
//	else LOG.PrintTimeAndLog( 0, "npc ai file named as %s is not on path", rkXMLFileName.c_str() );

	return pAI;
}
	
ioAI* ioNpcMgr::CreateAI(	ioNpcChar *pCreator, 
							ioPlayStage *pPlayStage, 
							const ioHashString &rkINIFileName )
{

	if( pCreator == NULL )
	{
		LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateAI Create None NPC : %s", rkINIFileName );
		return NULL;
	}

	if( rkINIFileName.IsEmpty() )
	{
		return NULL;
	}
	
	ioINILoader kLoader( rkINIFileName.c_str() );
	kLoader.SetTitle_e( "Info" );
	ioAI *pAI = NULL;
	switch( kLoader.LoadInt_e( "AIType", 0 ) )
	{
	case AI_NORMAL:
		if( Help::IsMonsterDungeonMode(pPlayStage->GetModeType()) ) 
		{
			if(COMPARE(pCreator->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC))
				pAI = new ioAI( pCreator, pPlayStage );
			else
				pAI = new ioAIPathFind( pCreator, pPlayStage );
		}
		else
		{
			pAI = new ioAI( pCreator, pPlayStage );
		}
		break;
	case AI_CHAMPION:
		pAI = new ioChampionAI( pCreator, pPlayStage );
		break;
	default:
		{
			if( Help::IsMonsterDungeonMode(pPlayStage->GetModeType()) ) 
			{
				if(COMPARE(pCreator->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC))
					pAI = new ioAI( pCreator, pPlayStage );
				else
					pAI = new ioAIPathFind( pCreator, pPlayStage );
			}
			else
			{
				pAI = new ioAI( pCreator, pPlayStage );
			}

			LOG.PrintTimeAndLog( 0, "ioNpcMgr::CreateAI Create None AI Type : %s", rkINIFileName );
		}
		break;
	}
	
	if( pAI )
	{
		// kaseton
		pAI->LoadAI( kLoader );
		pCreator->SetAI( pAI );
	}

	return pAI;
}

void ioNpcMgr::ChangeRandomEquipItem( ioNpcChar *pCreator, bool bEquipItem )
{
	if( !pCreator ) return;
	if( m_GradeEquipRandTable.empty() ) return;

	if( pCreator->GetLevel() == 0 )
	{
		// 장착 아이템
		ITEM_DATA kEquipItem[MAX_INVENTORY];
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			kEquipItem[i].m_item_code = m_GradeZeroEquip.m_iEquipItem[i];
		}
		pCreator->Initialize( (const ITEM_DATA *)&kEquipItem );
		pCreator->SetAI( m_GradeZeroEquip.m_szAI );
		pCreator->SetNpcViewName( m_GradeZeroEquip.m_szViewName );
	}
	else if( bEquipItem )
	{
		int iTableSize = m_GradeEquipRandTable.size();
		int iSelect    = pCreator->GetGradeUpRandom( iTableSize );

		GradeEquipRandTable &rkTable = m_GradeEquipRandTable[iSelect];
		
		// 장착 아이템
		ITEM_DATA kEquipItem[MAX_INVENTORY];
		for(int i = 0;i < MAX_INVENTORY;i++)
		{
			kEquipItem[i].m_item_code = rkTable.m_iEquipItem[i];
		}
		pCreator->Initialize( (const ITEM_DATA *)&kEquipItem );
		pCreator->SetAI( rkTable.m_szAI );
		pCreator->SetNpcViewName( rkTable.m_szViewName );
	}
}

void ioNpcMgr::GetRandomGradeTable( int iGradeLevel, int &rMinimumRand, int &rMaximumRand )
{
	rMinimumRand = rMaximumRand = 0;
	for(int i = 0;i < (int)m_GradeTable.size();i++)
	{
		GradeTable &rkTable = m_GradeTable[i];
		if( iGradeLevel <= rkTable.m_iLimitGrade )
		{
			rMinimumRand = rkTable.m_iMinimumRand;
			rMaximumRand = rkTable.m_iMaximumRand;
			break;
		}
	}
}

void ioNpcMgr::GetGradeAbilityValue( int iGradeLevel, bool &rEquipItem, DWORD &rProtected, bool &rLevelUpMotion, 
									 float &rRecoveryHpRate, ioHashString &rSkeleton )
{
	rProtected = 0;
	rEquipItem = rLevelUpMotion = false;
	rRecoveryHpRate = 0.0f;
	for(int i = 0;i < (int)m_GradeAbility.size();i++)
	{
		GradeAbility &rkAbility = m_GradeAbility[i];
		if( iGradeLevel == rkAbility.m_iGradeLevel )
		{
			rEquipItem = rkAbility.m_bEquipItem;
			rProtected = rkAbility.m_dwProtected;
			rLevelUpMotion  = rkAbility.m_bLevelUpMotion;
			rRecoveryHpRate = rkAbility.m_fRecoveryHpRate;
			rSkeleton  = rkAbility.m_szSkeleton;
			break;
		}
	}
}

void ioNpcMgr::GetGradeAbilityArmor( int iGradeLevel, float &rArmor, float &rMaxArmor )
{
	rArmor = rMaxArmor = 0.0f;
	for(int i = 0;i < (int)m_GradeAbility.size();i++)
	{
		GradeAbility &rkAbility = m_GradeAbility[i];
		if( iGradeLevel == rkAbility.m_iGradeLevel )
		{
			rArmor = rkAbility.m_fArmorClass;
			rMaxArmor = rkAbility.m_fMaxArmorClass;
			break;
		}
	}
}

void ioNpcMgr::GetGradeAbilitySpeed( int iGradeLevel, float &rSpeed, float &rMaxSpeed )
{
	rSpeed = rMaxSpeed = 0.0f;
	for(int i = 0;i < (int)m_GradeAbility.size();i++)
	{
		GradeAbility &rkAbility = m_GradeAbility[i];
		if( iGradeLevel == rkAbility.m_iGradeLevel )
		{
			rSpeed = rkAbility.m_fSpeedClass;
			rMaxSpeed = rkAbility.m_fMaxSpeedClass;
			break;
		}
	}
}