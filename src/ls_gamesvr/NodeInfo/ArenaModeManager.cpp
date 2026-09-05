#include "stdafx.h"

#include "ArenaModeManager.h"
#include "../Util/IORandom.h"

template<> ArenaModeManager* Singleton< ArenaModeManager >::ms_Singleton = 0;

ArenaModeManager::ArenaModeManager()
{
	Init();
}

ArenaModeManager::~ArenaModeManager()
{
	Destroy();
}

void ArenaModeManager::Init()
{
}

void ArenaModeManager::Destroy()
{
}

ArenaModeManager& ArenaModeManager::GetSingleton()
{
	return Singleton< ArenaModeManager >::GetSingleton();
}

BOOL ArenaModeManager::LoadINIData( const ioHashString &rkFileName )
{
	ioINILoader kLoader( rkFileName.c_str() );

	// [soldier]
	kLoader.SetTitle( "soldier" );	
	int iCnt  = kLoader.LoadInt( "count", 3 );					// 보상 단계 개수

	char szSoldier[MAX_PATH] = {0,};
	for( int i = 0; i < iCnt; ++i )
	{
		stArenaSoldierInfo stSoldierInfo;		
		sprintf_s( szSoldier, "soldier_number%d", i + 1);
		stSoldierInfo.iSoldier_Number = kLoader.LoadInt( szSoldier, 0 );

		sprintf_s( szSoldier, "soldier_number%d_powerup_on", i + 1);
		stSoldierInfo.iSoldier_PowerUp_On = kLoader.LoadInt( szSoldier, 0 );

		sprintf_s( szSoldier, "soldier_number%d_powerup_code", i + 1);
		stSoldierInfo.iSoldier_PowerUp_Code = kLoader.LoadInt( szSoldier, 0 );

		int temp_class_type = stSoldierInfo.iSoldier_Number; 
		stSoldierInfo.iface = g_DecorationPrice.GetDefaultDecoCode( 0, UID_FACE, temp_class_type + UID_FACE, temp_class_type );
		stSoldierInfo.ihair  = g_DecorationPrice.GetDefaultDecoCode( 0, UID_HAIR, temp_class_type + UID_HAIR, temp_class_type );
		stSoldierInfo.iskin_color = g_DecorationPrice.GetDefaultDecoCode( 0, UID_SKIN_COLOR, temp_class_type + UID_SKIN_COLOR, temp_class_type );
		stSoldierInfo.ihair_color = g_DecorationPrice.GetDefaultDecoCode( 0, UID_HAIR_COLOR, temp_class_type + UID_HAIR_COLOR, temp_class_type );
		stSoldierInfo.iunderwear = g_DecorationPrice.GetDefaultDecoCode( 0, UID_UNDERWEAR, temp_class_type + UID_UNDERWEAR, temp_class_type );

		m_vecvSoldierInfo.push_back(stSoldierInfo);		
	}
	int count = m_vecvSoldierInfo.size();
	for( std::vector<stArenaSoldierInfo>::iterator it = m_vecvSoldierInfo.begin() ; it != m_vecvSoldierInfo.end() ; ++it )
	{
		stArenaSoldierInfo temp_itr  = *it;
	}

	return true;
}
