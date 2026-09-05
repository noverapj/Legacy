
#include "stdafx.h"

#include "ioBingoManager.h"
#include "../Util/IORandom.h"
//#include <algorithm>

template<> ioBingoManager* Singleton< ioBingoManager >::ms_Singleton = 0;

ioBingoManager::ioBingoManager() : m_iBingoMaxNumber( 0 )
{
	Init();
}

ioBingoManager::~ioBingoManager()
{
	Destroy();
}

void ioBingoManager::Init()
{
	m_vecReward.clear();
	m_vecAllBingo.clear();
	
	m_iBingoMaxNumber = 0;
	m_iState = 0;
	m_iPeriod = 0;
	m_iMent = 0;
}

void ioBingoManager::Destroy()
{
	m_vecReward.clear();
	m_vecAllBingo.clear();
	
	m_iBingoMaxNumber = 0;
	m_iState = 0;
	m_iPeriod = 0;
	m_iMent = 0;
}

ioBingoManager& ioBingoManager::GetSingleton()
{
	return Singleton< ioBingoManager >::GetSingleton();
}

BOOL ioBingoManager::LoadINIData( const ioHashString &rkFileName )
{
	ioINILoader kLoader( rkFileName.c_str() );

	// common : number
	kLoader.SetTitle( "common" );
	
	int iBingoNumber = kLoader.LoadInt( "max_bingo_number", 0 );
	if( iBingoNumber == 0 )
		return false;

	m_iBingoType = static_cast<BingoType>( kLoader.LoadInt( "bingo_type", 0 ) );

	// Set Number
	SetMaxNumber( iBingoNumber );

	char szBuf[MAX_PATH]="";

	// Set : alarm
	int iState = kLoader.LoadInt( "reward_state", 0 );
	SetState( iState );

	// Set : period
	int iPeriod = kLoader.LoadInt( "reward_period", 0 );
	SetPeriod( iPeriod );
	
	// Set : ment
	int iMent = kLoader.LoadInt( "reward_ment", 0 );
	SetMent( iMent );

	// reward : present
	int iMaxPresent = kLoader.LoadInt( "reward_count", 0 );
	if( iMaxPresent == 0 )
		return false;

	// all bingo : present
	int iMaxAllBingo = kLoader.LoadInt( "allbingo_count", 0 );
	if( iMaxAllBingo == 0 )
		return false;

	// present
	for( int i = 0 ; i < iMaxPresent ; ++i )
	{
		stRewardData	data;
		char szKeyInfo[MAX_PATH] = "";

		sprintf_s( szKeyInfo, "reward%d", i + 1 );
		kLoader.SetTitle( szKeyInfo);

		int iType    = kLoader.LoadInt( "reward_type", 0 );
		int iValue1  = kLoader.LoadInt( "reward_value1", 0 );
		int iValue2  = kLoader.LoadInt( "reward_value2", 0 );
		int iPercent = kLoader.LoadInt( "reward_percent", 0 );

		if( iPercent <= 0 )
			continue;
		
		// push
		data.index	 = i + 1;	// index
		data.type	 = iType;
		data.value1  = iValue1;
		data.value2	 = iValue2;
		data.percent = iPercent;

		m_vecReward.push_back( data );
	}

	// all bingo
	for( int i = 0 ; i < iMaxAllBingo ; ++i )
	{
		stRewardData	data;
		char szKeyInfo[MAX_PATH] = "";

		sprintf_s( szKeyInfo, "allbingo_%d", i + 1 );
		kLoader.SetTitle( szKeyInfo);

		int iType    = kLoader.LoadInt( "reward_type", 0 );
		int iValue1  = kLoader.LoadInt( "reward_value1", 0 );
		int iValue2  = kLoader.LoadInt( "reward_value2", 0 );
		int iPercent = kLoader.LoadInt( "reward_percent", 0 );
		
		if( iPercent <= 0 )
			continue;

		// push
		data.index	= i + 1;	// index
		data.type	= iType;
		data.value1 = iValue1;
		data.value2	= iValue2;
		data.percent = iPercent;

		m_vecAllBingo.push_back( data );
	}

	//dummy	
	for( int i = 0; i < m_iBingoMaxNumber; ++i )
	{
		sprintf_s( szBuf, "dummy%d", i + 1 );
		kLoader.SetTitle( szBuf );

		int iCode = kLoader.LoadInt( "code", 0 );
		if( 0 < iCode )
			m_DummyInfoVec.push_back( iCode );
	}

	return true;
}

void ioBingoManager::GetBingoReward( vector< stRewardData >& rReward )
{
	switch( GetBingoType() )
	{
	case BT_RAND:
		{
			// 선물정보 12개.
			{
				// 임시 vector에 복사.
				vector< stRewardData > vecTemp( m_vecReward.begin(), m_vecReward.end() );

				vector< stRewardData >::iterator	iter	= vecTemp.begin();
				vector< stRewardData >::iterator	iterEnd	= vecTemp.end();

				while( iter != iterEnd )
				{
					(*iter).percent = rand() % (*iter).percent + 1;

					++iter;
				}

				// 정렬.
				sort( vecTemp.begin(), vecTemp.end(), stRewardDataSort() );

				// 12개 
				for( int i = 0 ; i < (int)vecTemp.size() ; ++i )
				{
					if( i >= ioBingo::PRESENT_COUNT - 1 )
						break;

					rReward.push_back( vecTemp[ i ] );
				}

				// 섞기
				io_random_shuffle( rReward.begin(), rReward.end() );
			}

			// 올빙고 선물
			{
/*
				// 임시 vector에 복사.
				vector< stRewardData > vecTemp( m_vecAllBingo.begin(), m_vecAllBingo.end() );

				vector< stRewardData >::iterator	iter	= vecTemp.begin();
				vector< stRewardData >::iterator	iterEnd	= vecTemp.end();

				while( iter != iterEnd )
				{
					(*iter).percent = rand() % (*iter).percent + 1;

					++iter;
				}

				// 정렬.
				sort( vecTemp.begin(), vecTemp.end(), stRewardDataSort() );

				// Set : 13번째에는 올빙고 선물
				rReward.push_back( vecTemp[ 0 ] );
*/

				// 임시 vector에 복사.
				vector< stRewardData > vecTemp( m_vecAllBingo.begin(), m_vecAllBingo.end() );

				DWORD dwTotalRand = 0;
				for( int i = 0; i < (int)vecTemp.size(); i++ )
				{
					dwTotalRand += vecTemp[i].percent;
				}

				IORandom mRand;
				mRand.SetRandomSeed( timeGetTime() );
				DWORD dwRand = mRand.Random( 0, dwTotalRand );	

				DWORD dwCurValue = 0;	
				for( int i = 0; i < (int)vecTemp.size(); ++i )
				{
					if( COMPARE( dwRand, dwCurValue, dwCurValue + vecTemp[i].percent ) )
					{
						rReward.push_back( vecTemp[i] );
						return;
					}
					else
					{
						dwCurValue += vecTemp[i].percent;
					}
				}
			}
		}
		break;

	case BT_SET:
		{
			// 12개 
			for( int i = 0 ; i < ioBingo::PRESENT_COUNT-1 ; ++i )
			{
				if( COMPARE( i, 0, (int)m_vecReward.size() ) )				
					rReward.push_back( m_vecReward[ i ] );				
			}

			//올빙고
			if( !m_vecAllBingo.empty() )
				rReward.push_back( m_vecAllBingo[ 0 ] );
		}
		break;
	}
}

stRewardData& ioBingoManager::GetBingoPresentInfo( const int index )
{
	vector< stRewardData >::iterator	iter	= m_vecReward.begin();
	vector< stRewardData >::iterator	iterEnd	= m_vecReward.end();

	while( iter != iterEnd )
	{
		if( (*iter).index == index )
		{
			return (*iter);
			break;
		}

		++iter;
	}

	static stRewardData NoneData;
	return NoneData;
}

stRewardData& ioBingoManager::GetAllBingoPresentInfo( const int index )
{
	vector< stRewardData >::iterator	iter	= m_vecAllBingo.begin();
	vector< stRewardData >::iterator	iterEnd	= m_vecAllBingo.end();

	while( iter != iterEnd )
	{
		if( (*iter).index == index )
		{
			return (*iter);
			break;
		}

		++iter;
	}

	static stRewardData NoneData;
	return NoneData;
}

void ioBingoManager::GetRegisterRewardPresentInfo( vector< stRewardData >& rPresent )
{
	vector< stRewardData >::iterator	iter	= m_vecReward.begin();
	vector< stRewardData >::iterator	iterEnd	= m_vecReward.end();

	while( iter != iterEnd )
	{
		rPresent.push_back( *iter );

		++iter;
	}
}

void ioBingoManager::GetRegisterAllBingoPresentInfo( vector< stRewardData >& rPresent )
{
	vector< stRewardData >::iterator	iter	= m_vecAllBingo.begin();
	vector< stRewardData >::iterator	iterEnd	= m_vecAllBingo.end();

	while( iter != iterEnd )
	{
		rPresent.push_back( *iter );

		++iter;
	}
}

int ioBingoManager::GetBingoDuumyCode( int iIndex )
{
	if( COMPARE( iIndex, 0, (int)m_DummyInfoVec.size() ) )
		return m_DummyInfoVec[iIndex];

	return 0;
}