

#include "stdafx.h"

#include "ioUserSpiritManager.h"
#include "ioUserSpiritInven.h"

//////////////////////////////////////////////////////////////////////////////////////////
template<> ioUserSpiritManager* Singleton< ioUserSpiritManager >::ms_Singleton = 0;

ioUserSpiritManager::ioUserSpiritManager()
{
}

ioUserSpiritManager::~ioUserSpiritManager()
{
}

ioUserSpiritManager& ioUserSpiritManager::GetSingleton()
{
	return Singleton< ioUserSpiritManager >::GetSingleton();
}

void ioUserSpiritManager::LoadINIInfo()
{
	ioINILoader_e kLoader( "config/sp2_spirit_info.ini" );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	kLoader.SetTitle_e( "common" );

	kLoader.LoadString_e( "spirit_name", "", szBuf, MAX_PATH );
	m_szSpiritName = szBuf;

	int iTableCount = kLoader.LoadInt_e( "spirit_table_count", 0 );
	for( int i = 0; i < iTableCount; i++ )
	{
		sprintf_e( szBuf, "spirit_table%d", i + 1 );
		kLoader.SetTitle( szBuf );

		SpiritTable cTableInfo;
		cTableInfo.m_iSellPeso = kLoader.LoadInt_e( "spirit_sell_peso", 0 );
		cTableInfo.m_iComposeMin = kLoader.LoadInt_e( "spirit_compose_need_min", 0 );
		cTableInfo.m_iComposeMax = kLoader.LoadInt_e( "spirit_compose_need_max", 0 );
		cTableInfo.m_iDecomposeType = kLoader.LoadInt_e( "spirit_decompose_type", DT_SPIRIT );
		cTableInfo.m_iDecomposeQuantity = kLoader.LoadInt_e( "spirit_decompose_quantity", 0 );
		cTableInfo.m_fDecomposeCriticalRate = kLoader.LoadFloat_e( "spirit_decompose_critical_rate", 0.f );
		cTableInfo.m_iDecomposeCriticalQuantity = kLoader.LoadInt_e( "spirit_decompose_critical_quantity", 0 );
		cTableInfo.m_fConversionCriticalRate = kLoader.LoadFloat_e( "spirit_conversion_critical_rate", 0.f );
		cTableInfo.m_iConversionCriticalQuantity = kLoader.LoadInt_e( "spirit_conversion_critical_output", 0 );
	
		m_SpiritTableMap.insert( SpiritTableMap::value_type( i + 1, cTableInfo ) );

		int iSpiritCount = kLoader.LoadInt_e( "spirit_count", 0 );
		for( int j = 0; j < iSpiritCount; j++ )
		{
			SpiritInfo cSpiritInfo;
			cSpiritInfo.m_iTable = i + 1;
			cSpiritInfo.m_iDecomposeType = cTableInfo.m_iDecomposeType;

			sprintf_e( szKey, "spirit_code%d", j + 1 );
			cSpiritInfo.m_iCode = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "spirit_icon%d", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			cSpiritInfo.m_szIcon = szBuf;

			sprintf_e( szKey, "spirit_name%d", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			cSpiritInfo.m_szName = szBuf;

			if( cSpiritInfo.m_szName.IsEmpty() )
			{
				wsprintf_e( szBuf, "%s %s", g_MyInfo.GetClassName( cSpiritInfo.m_iCode ), m_szSpiritName.c_str() );
				cSpiritInfo.m_szName = szBuf;
			}
			m_SpiritInfoMap.insert( SpiritInfoMap::value_type( cSpiritInfo.m_iCode, cSpiritInfo ) );
		}
	}

	kLoader.SetTitle_e( "special_spirit" );

	SpiritTable cTableInfo;
	cTableInfo.m_iSellPeso = kLoader.LoadInt_e( "spirit_sell_peso", 0 );
	m_SpiritTableMap.insert( SpiritTableMap::value_type( SPT_SPECIAL, cTableInfo ) );

	int iSpiritCount = kLoader.LoadInt_e( "spirit_count", 0 );
	for( int j = 0; j < iSpiritCount; j++ )
	{
		SpiritInfo cSpiritInfo;
		cSpiritInfo.m_iTable = SPT_SPECIAL;

		sprintf_e( szKey, "spirit_code%d", j + 1 );
		cSpiritInfo.m_iCode = kLoader.LoadInt( szKey, 0 );

		sprintf_e( szKey, "spirit_icon%d", j + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		cSpiritInfo.m_szIcon = szBuf;

		sprintf_e( szKey, "spirit_name%d", j + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		cSpiritInfo.m_szName = szBuf;

		m_SpiritInfoMap.insert( SpiritInfoMap::value_type( cSpiritInfo.m_iCode, cSpiritInfo ) );

		m_iSpecialSpiritCode = cSpiritInfo.m_iCode;
	}

	kLoader.SetTitle_e( "error_msg" );
	for( int j = 0; j < 8; j++ )
	{
		sprintf_e( szKey, "error_msg%d", j + 1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		m_szPacketError[j] = szBuf;
	}
}

int ioUserSpiritManager::GetDecomposeQuantity( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return 0;

	int iTableType = m_SpiritInfoMap.find( iSpiritCode )->second.m_iTable;
	if( m_SpiritTableMap.find( iTableType ) == m_SpiritTableMap.end() )
		return 0;

	return m_SpiritTableMap.find( iTableType )->second.m_iDecomposeQuantity;
}

int ioUserSpiritManager::GetComposeMax( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return 0;

	int iTableType = m_SpiritInfoMap.find( iSpiritCode )->second.m_iTable;
	if( m_SpiritTableMap.find( iTableType ) == m_SpiritTableMap.end() )
		return 0;

	return m_SpiritTableMap.find( iTableType )->second.m_iComposeMax;
}

int ioUserSpiritManager::GetComposeMin( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return 0;

	int iTableType = m_SpiritInfoMap.find( iSpiritCode )->second.m_iTable;
	if( m_SpiritTableMap.find( iTableType ) == m_SpiritTableMap.end() )
		return 0;

	return m_SpiritTableMap.find( iTableType )->second.m_iComposeMin;
}

int ioUserSpiritManager::GetSpiritType( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return SPT_NORMAL;

	return m_SpiritInfoMap.find( iSpiritCode )->second.m_iTable;
}

int ioUserSpiritManager::GetSellConst( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return 0;

	int iTableType = m_SpiritInfoMap.find( iSpiritCode )->second.m_iTable;
	if( m_SpiritTableMap.find( iTableType ) == m_SpiritTableMap.end() )
		return 0;

	return m_SpiritTableMap.find( iTableType )->second.m_iSellPeso;
}

ioHashString ioUserSpiritManager::GetIconName( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return ioHashString();

	return m_SpiritInfoMap.find( iSpiritCode )->second.m_szIcon;
}

ioHashString ioUserSpiritManager::GetName( int iSpiritCode )
{
	if( m_SpiritInfoMap.find( iSpiritCode ) == m_SpiritInfoMap.end() )
		return ioHashString();

	return m_SpiritInfoMap.find( iSpiritCode )->second.m_szName;
}

ioHashString ioUserSpiritManager::GetDecomposeSpiritName( int iClassType )
{
	if( m_SpiritInfoMap.find( iClassType ) == m_SpiritInfoMap.end() )
		return ioHashString();

	if( m_SpiritInfoMap.find( iClassType )->second.m_iDecomposeType == DT_SPIRIT )
		return m_SpiritInfoMap.find( iClassType )->second.m_szName;
	else
	{
		return m_SpiritInfoMap.find( m_iSpecialSpiritCode )->second.m_szName;
	}
}

void ioUserSpiritManager::GetSpiritInfo( vAlchemicItemInfoList &rkInfoList, int iSpiritType )
{
	rkInfoList.clear();

	ioUserSpiritInven *pSpiritInven = g_MyInfo.GetSpiritInventory();
	if( !pSpiritInven )
		return;

	SpiritInfoMap::iterator iter, iEnd;
	iEnd = m_SpiritInfoMap.end();
	for(iter = m_SpiritInfoMap.begin();iter != iEnd;iter++)
	{
		if( iSpiritType != 0 && iSpiritType != (*iter).second.m_iTable )
			continue;
		if( (*iter).second.m_iDecomposeType == DT_SPECIAL )
			continue;

		SpiritInvenInfo cSpiritInvenInfo;
		if( pSpiritInven )
			pSpiritInven->FindSpiritInfo( (*iter).second.m_iCode, cSpiritInvenInfo );

		AlchemicItemInfo kInfo;
		kInfo.m_iCode = (*iter).second.m_iCode;
		kInfo.m_iCount = cSpiritInvenInfo.m_iCount;
		kInfo.m_bNewAdd = cSpiritInvenInfo.m_bNewAdd;
		kInfo.m_iTabType = (*iter).second.m_iTable;
		kInfo.m_szIcon = (*iter).second.m_szIcon;
		kInfo.m_szName = (*iter).second.m_szName;

		rkInfoList.push_back( kInfo );
	}
}

bool ioUserSpiritManager::CheckPacketResult( int iResult )
{
	if( iResult == SPIRIT_RESULT_SUCCESS || iResult == SPIRIT_RESULT_PRESENT_SUCCESS )
		return true;
	if( COMPARE( iResult, SPIRIT_RESULT_NOT_ENOUGH_SPIRIT, SPIRIT_RESULT_EXCEPTION + 1 ) )
	{
		IoString str;
		str.Set(0, 0, m_szPacketError[iResult - 2].c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);
		g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
	}
	return false;
}