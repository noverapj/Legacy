
#include "stdafx.h"

#include "ioTreasureCardMgr.h"

template<> ioTreasureCardMgr* Singleton< ioTreasureCardMgr >::ms_Singleton = 0;

ioTreasureCardMgr::ioTreasureCardMgr()
{
}

ioTreasureCardMgr::~ioTreasureCardMgr()
{
	TreasureRandTable::iterator iCreate = m_TreasureRandTable.begin();
	for(;iCreate != m_TreasureRandTable.end();iCreate++)
	{
		TreasureRand &rkRand = iCreate->second;
		rkRand.m_TreasureCardVec.clear();
	}
	m_TreasureRandTable.clear();
}

ioTreasureCardMgr& ioTreasureCardMgr::GetSingleton()
{
	return Singleton< ioTreasureCardMgr >::GetSingleton();
}

void ioTreasureCardMgr::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_treasure_card.ini" );

	kLoader.SetTitle_e( "Common" );

	int iMaxTable = kLoader.LoadInt_e( "max_table", 0 );
	for(int i = 0;i < iMaxTable;i++)
	{
		char szTitle[MAX_PATH] = "";
		sprintf_e( szTitle, "Table%d", i + 1 );
		kLoader.SetTitle( szTitle );

		TreasureRand kRandTable;
		int iMaxRandCard = kLoader.LoadInt_e( "max_card", 0 );
		for(int k = 0;k < iMaxRandCard;k++)
		{
			TreasureCard kCard;

			char szKey[MAX_PATH] = "";
			sprintf_e( szKey, "rand%d_value", k + 1 );
			kCard.m_dwRandValue = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "present%d_type", k + 1 );
			kCard.m_iPresentType = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "present%d_value1", k + 1 );
			kCard.m_iPresentValue1 = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "present%d_value2", k + 1 );
			kCard.m_iPresentValue2 = kLoader.LoadInt( szKey, 0 );

			kRandTable.m_dwRandSeed += kCard.m_dwRandValue;
			kRandTable.m_TreasureCardVec.push_back( kCard );
		}
		m_TreasureRandTable.insert( TreasureRandTable::value_type( i + 1, kRandTable ) );
	}
}

void ioTreasureCardMgr::TreasureCardRandSeed( DWORD dwSeed )
{
	m_TreasureCardRandom.SetRandomSeed( dwSeed );
}

void ioTreasureCardMgr::TreasureCardRandom( DWORD dwTable, short &rPresentType, int &rPresentValue1, int &rPresentValue2 )
{
	TreasureRandTable::iterator iter = m_TreasureRandTable.find( dwTable );
	if( iter != m_TreasureRandTable.end() )
	{
		TreasureRand &rkTable = iter->second;

		DWORD dwRandValue = 0;
		DWORD dwRand = m_TreasureCardRandom.Random( rkTable.m_dwRandSeed );
		for(int i = 0;i < (int)rkTable.m_TreasureCardVec.size();i++)
		{
			TreasureCard &rkCard = rkTable.m_TreasureCardVec[i];
			if( COMPARE( dwRand, dwRandValue, dwRandValue + rkCard.m_dwRandValue ) )
			{
				rPresentType	= rkCard.m_iPresentType;
				rPresentValue1	= rkCard.m_iPresentValue1;
				rPresentValue2	= rkCard.m_iPresentValue2;
				return;
			}
			dwRandValue += rkCard.m_dwRandValue;
		}
	}

	// 없으면 임시로 세팅
	rPresentType = PRESENT_PESO;
	rPresentValue1 = 1000 +  ( 1000 * ( rand() % 10 ) );
}