

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioTalisman.h"
#include "ioTalismanMaker.h"

template<> ioTalismanMaker* Singleton< ioTalismanMaker >::ms_Singleton = 0;

ioTalismanMaker::ioTalismanMaker( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
	m_vTalismanList.clear();
}

ioTalismanMaker::~ioTalismanMaker()
{
	ClearTalismanList();
}

ioTalismanMaker& ioTalismanMaker::GetSingleton()
{
	return Singleton< ioTalismanMaker >::GetSingleton();
}

void ioTalismanMaker::LoadTalismanList()
{
	ioINILoader_e kLoader( "config/sp2_talisman.ini" );

	char szTitle[MAX_PATH];

	int iNum = kLoader.LoadInt_e( "common_info", "talisman_cnt", 0 );

	for( int i=0 ; i<iNum ; i++ )
	{
		wsprintf_e( szTitle, "talisman%d", i+1 );
		kLoader.SetTitle( szTitle );

		ioTalisman *pTalisman = new ioTalisman;

		if( pTalisman )
		{
			pTalisman->LoadProperty( kLoader );

			if( FindTalisman( pTalisman->GetTalismanNum() ) )
			{
				LOG.PrintTimeAndLog( 0, "ioTalismanMaker::LoadTalismanList - %d Already Exist",
										 pTalisman->GetTalismanNum() );

				delete pTalisman;
			}
			else
			{
				m_vTalismanList.push_back( pTalisman );
			}
		}
	}
}

ioTalisman* ioTalismanMaker::CreateTalisman( ioBaseChar *pOwner, int iTalismanNum )
{
	ioTalisman *pTalisman = FindTalisman( iTalismanNum );
	if( pTalisman )
	{
		ioTalisman *pNewTalisman = pTalisman->Clone();
		if( pNewTalisman )
		{
			pNewTalisman->InitTalisman( pOwner );
			return pNewTalisman;
		}
	}

	LOG.PrintTimeAndLog( 0, "ioTalismanMaker::CreateTalisman - Not Find Talisman: %d", iTalismanNum );
	return NULL;
}

ioTalisman* ioTalismanMaker::FindTalisman( int iTalismanNum )
{
	TalismanList::iterator iter = m_vTalismanList.begin();

	while( iter != m_vTalismanList.end() )
	{
		if( (*iter)->GetTalismanNum() == iTalismanNum )
			return (*iter);
		else
			++iter;
	}

	return NULL;
}

void ioTalismanMaker::ClearTalismanList()
{
	TalismanList::iterator iter = m_vTalismanList.begin();

	while( iter != m_vTalismanList.end() )
	{
		delete (*iter);
		++iter;
	}
	m_vTalismanList.clear();
}

