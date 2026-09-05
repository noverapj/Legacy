#include "stdafx.h"
#include ".\iofirstsoldiermanager.h"
#include "ioDecorationPrice.h"
#include <strsafe.h>

template<> ioFirstSoldierManager* Singleton< ioFirstSoldierManager >::ms_Singleton = 0;

ioFirstSoldierManager::ioFirstSoldierManager(void)
{
}

ioFirstSoldierManager::~ioFirstSoldierManager(void)
{
	Clear();
}

ioFirstSoldierManager& ioFirstSoldierManager::GetSingleton()
{
	return Singleton<ioFirstSoldierManager>::GetSingleton();
}

void ioFirstSoldierManager::LoadINI()
{
	enum { MAX_LOAD = 11, };
	int  iDecoTypeArray[MAX_LOAD] = { UID_KINDRED, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR, UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	int  iSexTypeArray[MAX_LOAD]  = { -1, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_MAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN, RDT_HUMAN_WOMAN };
	char szMaxArray[MAX_LOAD][MAX_PATH] ={ "MaxKindred", "MaxManHair", "MaxManHairColor", "MaxManFace", "MaxManSkinColor", "MaxManUnderwear",  "MaxWomanHair", "MaxWomanHairColor", "MaxWomanFace", "MaxWomanSkinColor", "MaxWomanUnderwear"};
	char szNameArray[MAX_LOAD][MAX_PATH]={ "Kindred%d",  "ManHair%d",  "ManHairColor%d",  "ManFace%d",  "ManSkinColor%d",  "ManUnderwear%d",   "WomanHair%d",  "WomanHairColor%d",  "WomanFace%d",  "WomanSkinColor%d",  "WomanUnderwear%d" };

	char szKeyName[MAX_PATH]="";
	char szBuf[MAX_PATH]="";

	Clear();

	ioINILoader kLoader( "config/sp2_first_soldier.ini" );
	kLoader.SetTitle( "Common" );
	int iMaxSoldier = kLoader.LoadInt( "MaxSoldier", 0 );
	for (int i = 0; i < iMaxSoldier ; i++)
	{
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "Soldier%d", i+1 );
		kLoader.SetTitle( szKeyName );

		int iClassType = kLoader.LoadInt( "ClassType", 0 );
		if( iClassType == 0 )
			continue;

		for (int j = 0; j < MAX_LOAD ; j++)
		{
			DecoVectorInfo *pInfo = new DecoVectorInfo;
			if( !pInfo )
				continue;
			pInfo->m_iClassType= iClassType;
			pInfo->m_iDecoType = iDecoTypeArray[j];
			pInfo->m_iSexType  = iSexTypeArray[j];
			ZeroMemory( szKeyName, sizeof( szKeyName ) );
			StringCbCopy( szKeyName, sizeof( szKeyName ), szMaxArray[j] );
			int iMaxDeco = kLoader.LoadInt( szKeyName, 0 );
			for (int k = 0; k < iMaxDeco ; k++)
			{
				ZeroMemory( szKeyName, sizeof( szKeyName ) );
				StringCbPrintf( szKeyName, sizeof( szKeyName ), szNameArray[j], k+1 );
				int iMagicCode = kLoader.LoadInt( szKeyName, 0 );
				pInfo->m_vDecoCode.push_back( iMagicCode );
			}	
			m_vDecoVectorInfo.push_back( pInfo );
		}	
	}	
}

void ioFirstSoldierManager::Clear()
{
	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		pInfo->m_vDecoCode.clear();
		SAFEDELETE( pInfo );
	}
	m_vDecoVectorInfo.clear();
}


bool ioFirstSoldierManager::IsExistDecoAll( const CHARACTER &rkCharInfo )
{
	if( !IsExistDeco( rkCharInfo.m_class_type, rkCharInfo.m_sex-1, UID_KINDRED, GetDecoKindredCode( rkCharInfo ) ) )
		return false;
	if( !IsExistDeco( rkCharInfo.m_class_type, rkCharInfo.m_sex-1, UID_HAIR, rkCharInfo.m_hair ) )
		return false;
	if( !IsExistDeco( rkCharInfo.m_class_type, rkCharInfo.m_sex-1, UID_HAIR_COLOR, rkCharInfo.m_hair_color ) )
		return false;
	if( !IsExistDeco( rkCharInfo.m_class_type, rkCharInfo.m_sex-1, UID_FACE, rkCharInfo.m_face ) )
		return false;
	if( !IsExistDeco( rkCharInfo.m_class_type, rkCharInfo.m_sex-1, UID_SKIN_COLOR, rkCharInfo.m_skin_color ) )
		return false;
	if( !IsExistDeco( rkCharInfo.m_class_type, rkCharInfo.m_sex-1, UID_UNDERWEAR, rkCharInfo.m_underwear ) )
		return false;

	return true;
}

bool ioFirstSoldierManager::IsExistDeco( int iClassType, int iSexType, int iDecoType, int iDecoCode )
{
	for(vDecoVectorInfo::iterator iter = m_vDecoVectorInfo.begin(); iter != m_vDecoVectorInfo.end(); ++iter)
	{
		DecoVectorInfo *pInfo = (*iter);
		if( !pInfo )
			continue;
		if( pInfo->m_iClassType != iClassType )
			continue;
		if( pInfo->m_iDecoType != UID_KINDRED )
		{
			if( pInfo->m_iSexType != iSexType )
				continue;
		}
		if( pInfo->m_iDecoType != iDecoType )
			continue;
		if( pInfo->m_vDecoCode.empty() )
			continue;
		
		int iSize = pInfo->m_vDecoCode.size();
		for (int i = 0; i < iSize ; i++)
		{
			if( iDecoCode == pInfo->m_vDecoCode[i] )
				return true;
		}
	}

	return false;
}

int ioFirstSoldierManager::GetDecoKindredCode( const CHARACTER &rkCharInfo )
{
	if( rkCharInfo.m_kindred == 1 && rkCharInfo.m_sex == 1 )
	{
		return RDT_HUMAN_MAN;
	}
	else if( rkCharInfo.m_kindred == 1 && rkCharInfo.m_sex == 2 )
	{
		return RDT_HUMAN_WOMAN;
	}
	else if( rkCharInfo.m_kindred == 2 && rkCharInfo.m_sex == 1 ) 
	{
		return RDT_ELF_MAN;
	}	
	else if( rkCharInfo.m_kindred == 2 && rkCharInfo.m_sex == 2 )
	{
		return RDT_ELF_WOMAN;
	}
	else if( rkCharInfo.m_kindred == 3 && rkCharInfo.m_sex == 1 )
	{
		return RDT_DWARF_MAN;
	}
	else if( rkCharInfo.m_kindred == 3 && rkCharInfo.m_sex == 2 )
	{
		return RDT_DWARF_WOMAN;
	}

	return -1;
}