

#include "stdafx.h"

#include "ioAnnounce.h"
#include "ioAnnounceManager.h"

template<> ioAnnounceManager* Singleton< ioAnnounceManager >::ms_Singleton = 0;

ioAnnounceManager::ioAnnounceManager()
{
	m_dwAnnounceInterval = 4000;

	char szBuf[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_sound.ini" );
	kLoader.SetTitle_e( "sound_file_name" );

	kLoader.LoadString_e( "reload", "", szBuf, MAX_PATH );
	m_ReLoadSound = szBuf;

	kLoader.LoadString_e( "need_mp", "", szBuf, MAX_PATH );
	m_NeedMPSound = szBuf;

	kLoader.LoadString_e( "time_limit", "", szBuf, MAX_PATH );
	m_TimeLimitSound = szBuf;

	kLoader.LoadString_e( "item_supply", "", szBuf, MAX_PATH );
	m_ItemSupplySound = szBuf;

	kLoader.LoadString_e( "symbol_defense_my_team", "", szBuf, MAX_PATH );
	m_SymbolDefenseMySound = szBuf;

	kLoader.LoadString_e( "symbol_defense_other_team", "", szBuf, MAX_PATH );
	m_SymbolDefenseOtherSound = szBuf;
}

ioAnnounceManager::~ioAnnounceManager()
{
	ClearAnnounceList();
}

void ioAnnounceManager::SetAnnounceInterval( DWORD dwInterval )
{
	m_dwAnnounceInterval = dwInterval;
}

void ioAnnounceManager::UpdateAnnouce()
{
	ioAnnounce *pAnnounce;
	AnnounceList::iterator iter = m_AnnounceList.begin();
	while( iter != m_AnnounceList.end() )
	{
		pAnnounce = *iter;

		if( pAnnounce->IsLive() )
		{
			++iter;
		}
		else
		{
			iter = m_AnnounceList.erase( iter );
			delete pAnnounce;
		}
	}
}

void ioAnnounceManager::RenderAnnounce()
{
	if( m_AnnounceList.empty() )	return;

	ioAnnounce *pFront = m_AnnounceList.front();
	if( pFront )
	{
		pFront->Render();
	}
}

void ioAnnounceManager::ClearAnnounceList()
{
	AnnounceList::iterator iter;
	for( iter=m_AnnounceList.begin() ; iter!=m_AnnounceList.end() ; ++iter )
	{
		delete *iter;
	}
	
	m_AnnounceList.clear();
}

bool ioAnnounceManager::IsNowAnnouncing() const
{
	return !m_AnnounceList.empty();
}

void ioAnnounceManager::DeleteAnnounce( int iLevel )
{
	if( m_AnnounceList.empty() )
		return;

	ioAnnounce *pAnnounce;

	AnnounceList::iterator iter;
	for( iter=m_AnnounceList.begin() ; iter!=m_AnnounceList.end() ; ++iter )
	{
		pAnnounce = *iter;
		if( pAnnounce->GetLevel() == iLevel )
		{
			m_AnnounceList.erase( iter );
			delete pAnnounce;
			return;
		}
	}
}

bool ioAnnounceManager::AnnounceGreater::operator()( const ioAnnounce *a,
													 const ioAnnounce *b ) const
{
	int iLevelA = a->GetLevel();
	int iLevelB = b->GetLevel();

	if( iLevelA > iLevelB )
		return true;
	else if( iLevelA == iLevelB )
	{
		return ( a->GetStartTime() > b->GetStartTime() );
	}

	return false;
}

void ioAnnounceManager::AddNewAnnounce( ioAnnounce *pNewAnnounce, DWORD dwLiveTime )
{
	pNewAnnounce->SetLiveTime( dwLiveTime );
	m_AnnounceList.push_front( pNewAnnounce );

	m_AnnounceList.sort( AnnounceGreater() );
}

//-------------------------------------------------------------------

void ioAnnounceManager::GameReadyAnnounce( DWORD dwStartGapTime,
										   TeamType eOwnerTeam,
										   int iCurRound,
										   int iHalfRound )
{
	ClearAnnounceList();

	ioGameReadyAnnounce *pNewAnnouce = new ioGameReadyAnnounce;
	pNewAnnouce->SetAnnounce( eOwnerTeam, iCurRound, iHalfRound );
	AddNewAnnounce( pNewAnnouce, dwStartGapTime );
}

void ioAnnounceManager::NormalAnnounce( const char *szTitle,
									    const char *szAnnounce,
										DWORD dwTitleColor,
										int iLevel )
{
	ioNormalAnnounce *pNewAnnounce = new ioNormalAnnounce;
	pNewAnnounce->SetAnnounce( szTitle, szAnnounce, dwTitleColor );
	pNewAnnounce->SetLevel( iLevel );

	AddNewAnnounce( pNewAnnounce, m_dwAnnounceInterval );
}

void ioAnnounceManager::LevelupAnnounce( int iStr, int iDex, int iInt, int iVit )
{
	ioLevelupAnnounce *pNewAnnounce = new ioLevelupAnnounce;
	pNewAnnounce->SetAnnounce( iStr, iDex, iInt, iVit );
	AddNewAnnounce( pNewAnnounce, m_dwAnnounceInterval );
}

void ioAnnounceManager::GetItemAnnounce( const ioItem *pItem )
{

	ioGetItemAnnounce *pNewAnnounce = new ioGetItemAnnounce;
	pNewAnnounce->SetAnnounce( pItem );
	AddNewAnnounce( pNewAnnounce, m_dwAnnounceInterval );
}

void ioAnnounceManager::RevivalAnnounce( DWORD dwRevivalGapTime )
{
	ioRevivalAnnounce *pNewAnnounce = new ioRevivalAnnounce;
	AddNewAnnounce( pNewAnnounce, dwRevivalGapTime );
}

void ioAnnounceManager::SymbolDefenseAnnounce( DWORD dwDefenseTime, bool bDefenseMode )
{
	ioDefenseSymbolAnnounce *pNewAnnounce = new ioDefenseSymbolAnnounce;
	pNewAnnounce->SetAnnounce( bDefenseMode );
	AddNewAnnounce( pNewAnnounce, dwDefenseTime );

	if( bDefenseMode )
	{
		g_SoundMgr.PlaySound( m_SymbolDefenseMySound );
	}
	else
	{
		g_SoundMgr.PlaySound( m_SymbolDefenseOtherSound );
	}
}

void ioAnnounceManager::ExitPrisonerModeAnnounce( DWORD dwExitPrisonerModeTime, bool bExitPrisonerMode )
{
	ioExitPrisonerMode *pNewAnnounce = new ioExitPrisonerMode;
	pNewAnnounce->SetAnnounce( bExitPrisonerMode );
	AddNewAnnounce( pNewAnnounce, dwExitPrisonerModeTime );

	if( bExitPrisonerMode )
	{
		g_SoundMgr.PlaySound( m_SymbolDefenseMySound );
	}
	else
	{
		g_SoundMgr.PlaySound( m_SymbolDefenseOtherSound );
	}
}

void ioAnnounceManager::TimeLimitAnnounce( DWORD dwTimeLimit )
{
	ioTimeLimitAnnounce *pNewAnnounce = new ioTimeLimitAnnounce;
	AddNewAnnounce( pNewAnnounce, dwTimeLimit );

	g_SoundMgr.PlaySound( m_TimeLimitSound );
}

void ioAnnounceManager::ItemSupplyAnnounce( const ioHashStringVec &vNameList )
{
	if( vNameList.empty() )	return;

	int iItemCnt = vNameList.size();

	char szBuf[MAX_PATH];
	memset( szBuf, 0, MAX_PATH );

	for( int i=0 ; i<iItemCnt ; i++ )
	{
		strcat( szBuf, vNameList[i].c_str() );

		if( i+1 < iItemCnt )
		{
			strcat( szBuf, ", " );
		}
	}

	NormalAnnounce( STR(1), szBuf, TCT_DEFAULT_GRAY );

	g_SoundMgr.PlaySound( m_ItemSupplySound );
}

void ioAnnounceManager::NeedMoreMPAnnouce()
{
	NormalAnnounce( STR(1), STR(2), TCT_DEFAULT_GRAY );

	g_SoundMgr.PlaySound( m_NeedMPSound );
}

void ioAnnounceManager::ReLoadAmmoAnnounce()
{
	NormalAnnounce( STR(1),
					STR(2),
					TCT_DEFAULT_GRAY );

	g_SoundMgr.PlaySound( m_ReLoadSound );
}

void ioAnnounceManager::NeedMoreZenAnnouce()
{
	NormalAnnounce( STR(1),
					STR(2),
					TCT_DEFAULT_GRAY );

	g_SoundMgr.PlaySound( m_ReLoadSound );
}

ioAnnounceManager& ioAnnounceManager::GetSingleton()
{
	return Singleton<ioAnnounceManager>::GetSingleton();
}
