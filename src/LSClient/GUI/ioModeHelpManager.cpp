

#include "StdAfx.h"

#include "ioSP2GUIManager.h"
#include "ioModeHelpManager.h"

template<> ioModeHelpManager* Singleton< ioModeHelpManager >::ms_Singleton = 0;

ioModeHelpManager::ioModeHelpManager()
{
	m_eCurHelpType   = HT_NONE;
	m_dwHideHelpTime = 0;
	m_vModeHelpList.clear();
	m_dwReserveRevivalEndTime = 0;
}

ioModeHelpManager::~ioModeHelpManager()
{
	m_vModeHelpList.clear();
}

void ioModeHelpManager::LoadModeHelpList()
{
	m_vModeHelpList.clear();

	ioINILoader_e kLoader( "config/sp2_mode_extra_help.ini" );

	kLoader.SetTitle_e( "common" );

	int iMaxInfo = kLoader.LoadInt_e( "extra_help_cnt", 0 );
	m_vModeHelpList.reserve( iMaxInfo );


	char szBuf[MAX_PATH];
	for(int i = 0;i < iMaxInfo;i++)
	{
		sprintf_e( szBuf, "extra_help%d", i+1 );
		kLoader.SetTitle( szBuf );

		ModeHelp MH;
		MH.m_dwID = i + 1;
		MH.m_iType = kLoader.LoadInt_e( "type", 0 );
		MH.m_iDuration  = kLoader.LoadInt_e( "duration", 0 );
		ZeroMemory( szBuf, sizeof( szBuf ) );
		kLoader.LoadString_e( "icon_name", "", szBuf, MAX_PATH );
		MH.m_szIconName = szBuf;

		for( int j=0; j < 10; ++j )
		{
			HelpText HT;
			sprintf_e( szBuf, "text%d", j+1 );
			kLoader.LoadString( szBuf, "", HT.m_szHelp, MAX_PATH );

			if( strcmp( HT.m_szHelp, "" ) == 0 ) break;

			Help::ChangeCharSet( HT.m_szHelp, '#', ' ' );
			sprintf_e( szBuf, "color%d", j+1 );

			char szValue[MAX_PATH];
			kLoader.LoadString( szBuf, "", szValue, MAX_PATH );

			char *pTemp = NULL;
			HT.m_dwColor = strtoul( szValue, &pTemp, 16 );

			// 교체 텍스트.. 없으면 없는거고 있으면 표시할 때 교체하면서 표시한다.
			sprintf_e( szBuf, "swap_text%d", j+1 );
			kLoader.LoadString( szBuf, "", HT.m_szSwapHelp, MAX_PATH );
			sprintf_e( szBuf, "swap_time%d", j+1 );
            HT.m_dwSwapTime = kLoader.LoadInt( szBuf, 0 );

			MH.m_vText.push_back( HT );
		}
		m_vModeHelpList.push_back( MH );
	}

}

void ioModeHelpManager::InitModeHelp()
{
	m_eCurHelpType   = HT_NONE;
	m_dwHideHelpTime = 0;
}

void ioModeHelpManager::ShowHelp( HelpType eHelpType, DWORD dwHideHelpTime /*= 0 */ )
{
	if( m_eCurHelpType == HT_VIEW ) return;
	if( !COMPARE( eHelpType, HT_VIEW, HT_MAX ) ) return;

	if( m_eCurHelpType == eHelpType && eHelpType != HT_SKILL && eHelpType != HT_BUFF ) return;

	if( eHelpType == HT_STOP_GAUGE1 && m_eCurHelpType != HT_NONE )
	{
		if( m_eCurHelpType != HT_DEATH_TIME )
			return;
	}

	if( eHelpType == HT_REVIVING && m_eCurHelpType == HT_DIE )
	{
		m_dwReserveRevivalEndTime = dwHideHelpTime;
		return;
	}

	if( eHelpType == HT_DEATH_TIME && m_eCurHelpType != HT_NONE ) return;

	m_eCurHelpType = eHelpType;
	if( dwHideHelpTime == 0 )
	{
		int iDuration = GetDuration( m_eCurHelpType );
		if( iDuration != 0 )
			m_dwHideHelpTime = FRAMEGETTIME() + iDuration;
		else
			m_dwHideHelpTime = 0;
	}
	else
		m_dwHideHelpTime = dwHideHelpTime;
		
	ShowHelpWnd();
}

void ioModeHelpManager::HideHelp( HelpType eHelpType )
{
	if( m_eCurHelpType != eHelpType ) return;
	if( IsHidingAni() ) return;

	SetHidingAni();
}

void ioModeHelpManager::ShowSkillHelp( const ioHashString &rszDescName,const ioHashString &rszDesc,const ioHashString &rszIconName )
{
	if( Setting::IsHideSkillBuffEmoticon() )
		return;

	if( rszDescName.IsEmpty() ) return;
	if( rszDesc.IsEmpty() ) return;
	if( rszIconName.IsEmpty() ) return;

	SetHelpList( HT_SKILL, rszDescName, rszDesc, rszIconName );
	ShowHelp( HT_SKILL );
}

void ioModeHelpManager::ShowContributePraise( int iContribute )
{
	char szBuf[MAX_PATH];

	memset( szBuf, 0, MAX_PATH );

	SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iContribute );

	SetHelpList( HT_CONTRIBUTE_PRAISE, szBuf );
	ShowHelp( HT_CONTRIBUTE_PRAISE );
}

void ioModeHelpManager::Process( const ioPlayMode *pPlayMode )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if(	m_dwHideHelpTime > 0 &&	dwCurTime > m_dwHideHelpTime )
	{
		m_dwHideHelpTime = 0;
		SetHidingAni();	
	}

	if( IsHidedAni() )
	{
		if( !ShowHelpWhenHided( dwCurTime, pPlayMode ) )
		{
			InitModeHelp();
			HideHelpWnd();
		}
	}
}

void ioModeHelpManager::ProcessHelpText()
{
	int iSize = m_vModeHelpList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == m_eCurHelpType )
		{
			vHelpText &rvText = m_vModeHelpList[i].m_vText;
			int iTextSize = rvText.size();
			for (int j = 0; j < iTextSize ; j++)
			{
				rvText[j].SwapProcess();
			}
			break;
		}
	}
}

const vHelpText * ioModeHelpManager::GetCurHelpText() const
{
	int iSize = m_vModeHelpList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == m_eCurHelpType )
		{
			return &m_vModeHelpList[i].m_vText;
		}
	}

	return NULL;
}

const vHelpText * ioModeHelpManager::GetHelpText( HelpType eType ) const
{
	int iSize = m_vModeHelpList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == eType )
		{
			return &m_vModeHelpList[i].m_vText;
		}
	}

	return NULL;
}

const ioHashString * ioModeHelpManager::GetCurIconName() const
{
	if( m_eCurHelpType == HT_NONE )
		return NULL;

	int iSize = m_vModeHelpList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == m_eCurHelpType )
		{
			return &m_vModeHelpList[i].m_szIconName;
		}
	}

	return NULL;
}

const ioHashString * ioModeHelpManager::GetIconName( HelpType eHelpType ) const
{
	if( eHelpType == HT_NONE )
		return NULL;

	int iSize = m_vModeHelpList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == eHelpType )
		{
			return &m_vModeHelpList[i].m_szIconName;
		}
	}

	return NULL;
}

int ioModeHelpManager::GetDuration( HelpType eHelpType )
{
	int iSize = m_vModeHelpList.size();
	int iDuration = 0;
	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == eHelpType )
		{
			iDuration = m_vModeHelpList[i].m_iDuration;
			break;
		}
	}

	return iDuration;
}

void ioModeHelpManager::SetHelpList( HelpType eHelpType,
									 const ioHashString &rszText1, const ioHashString &rszText2, const ioHashString &rszIconName )
{
	int iSize = m_vModeHelpList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == eHelpType )
		{
			m_vModeHelpList[i].m_szIconName = rszIconName;

			vHelpText &rvText = m_vModeHelpList[i].m_vText;
			int iTextSize = rvText.size();
			for (int j = 0; j < iTextSize ; j++)
			{
				if( j == 0)
					StringCbCopy( rvText[j].m_szHelp, sizeof(rvText[j].m_szHelp), rszText1.c_str() );
				else if( j == 1 )
					StringCbCopy( rvText[j].m_szHelp, sizeof(rvText[j].m_szHelp), rszText2.c_str() );
			}
			return;
		}
	}
}

void ioModeHelpManager::SetHelpList( HelpType eHelpType, const ioHashString &rszText2 )
{
	int iSize = m_vModeHelpList.size();
	for( int i=0; i < iSize; ++i )
	{
		if( m_vModeHelpList[i].m_iType == eHelpType )
		{
			vHelpText &rvText = m_vModeHelpList[i].m_vText;
			int iTextSize = rvText.size();
			for (int j = 0; j < iTextSize ; j++)
			{
				if( j == 1 )
					StringCbCopy( rvText[j].m_szHelp, sizeof(rvText[j].m_szHelp), rszText2.c_str() );
			}
			return;
		}
	}
}

ioModeHelpManager& ioModeHelpManager::GetSingleton()
{
	return Singleton< ioModeHelpManager >::GetSingleton();
}

ioModeHelpManager *ioModeHelpManager::GetSingletonPtr()
{
	return Singleton< ioModeHelpManager >::GetSingletonPtr();
}


void ioModeHelpManager::ShowHelpWnd()
{
}

void ioModeHelpManager::HideHelpWnd()
{
}

void ioModeHelpManager::SetHidingAni()
{	
}

bool ioModeHelpManager::IsHidedAni()
{
	return false;
}

bool ioModeHelpManager::IsHidingAni()
{
	return false;
}

ioModeHelpManager::HelpType ioModeHelpManager::GetCurHelpType() const
{
	return m_eCurHelpType;
}

void ioModeHelpManager::HideHelpDirectly()
{
	InitModeHelp();
	HideHelpWnd();
}

bool ioModeHelpManager::ShowHelpWhenHided( DWORD dwCurTime, const ioPlayMode *pPlayMode )
{
	bool bShowHelp = false;
	if( m_eCurHelpType == HT_DIE )
	{
		if( m_dwReserveRevivalEndTime != 0  )
		{
			if( m_dwReserveRevivalEndTime > dwCurTime )
			{
				m_eCurHelpType = HT_NONE;
				ShowHelp( HT_REVIVING, m_dwReserveRevivalEndTime );
				bShowHelp = true;
			}
			m_dwReserveRevivalEndTime = 0;
		}
		else if( pPlayMode && ( pPlayMode->GetModeType() == MT_CATCH || pPlayMode->GetModeType() == MT_CATCH_RUNNINGMAN || 
			pPlayMode->GetModeType() == MT_CBT || pPlayMode->GetModeType() == MT_UNDERWEAR || pPlayMode->GetModeType() == MT_FARMING ) )
		{
			ShowHelp( HT_VIEW );
			bShowHelp = true;
		}
	}

	return bShowHelp;
}