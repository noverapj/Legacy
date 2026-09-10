#include <StdAfx.h>

#include "StartingSignalWnd.h"
#include "../GameStage/Mode/Practice/IoPracticeMode.h"
#include "Practice/PracticeInfoWnd.h"

StartingSignalWnd::StartingSignalWnd()
{
	for ( int i = 0 ; i < TEXT_NUM_COUNT ; i++ )
		m_pText_Num[i] = NULL;

	m_pStartText = NULL;
	m_pStartBlur = NULL;
	m_pStartCount = NULL;

	m_dwFadeInTime = 0;
	m_dwFadeOutTime = 0;
	m_dwCountGap = 0;
	m_dwStartTextTime = 0;
	m_dwCurrentStartTextTime = 0;

	m_iCount = 0;
}

StartingSignalWnd::~StartingSignalWnd()
{
	for ( int i = 0 ; i < TEXT_NUM_COUNT ; i++ )
		SAFEDELETE( m_pText_Num[i] );

	SAFEDELETE( m_pStartText );
	SAFEDELETE( m_pStartBlur );
	SAFEDELETE( m_pStartCount );
}

void StartingSignalWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Text_3" )
	{
		SAFEDELETE( m_pText_Num[0] );
		m_pText_Num[0] = pImage;
	}
	else if( szType == "Text_2" )
	{
		SAFEDELETE( m_pText_Num[1] );
		m_pText_Num[1] = pImage;
	}
	else if( szType == "Text_1" )
	{
		SAFEDELETE( m_pText_Num[2] );
		m_pText_Num[2] = pImage;
	}
	else if( szType == "StartText" )
	{
		SAFEDELETE( m_pStartText );
		m_pStartText = pImage;
	}
	else if( szType == "StartBlur" )
	{
		SAFEDELETE( m_pStartBlur );
		m_pStartBlur = pImage;
	}
	else if( szType == "Count" )
	{
		SAFEDELETE( m_pStartCount );
		m_pStartCount = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void StartingSignalWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwFadeInTime		= xElement.GetIntAttribute_e( "FadeInTime" );
	m_dwFadeOutTime		= xElement.GetIntAttribute_e( "FadeOutTime" );
	m_dwCountGap		= xElement.GetIntAttribute_e( "CountGap" );
	m_dwStartTextTime	= xElement.GetIntAttribute_e( "StartTextTime" );

	m_szCountSound[0]		= xElement.GetStringAttribute_e("Sound3Sec");
	m_szCountSound[1]		= xElement.GetStringAttribute_e("Sound2Sec");
	m_szCountSound[2]		= xElement.GetStringAttribute_e("Sound1Sec");
	m_szCountSound[3]		= xElement.GetStringAttribute_e("Sound0Sec");
}

void StartingSignalWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_dwCurrentStartTextTime == 0 )
		return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentStartTextTime;
	//dwGapTime -= m_dwCountGap * TEXT_NUM_COUNT;

	if ( dwGapTime >= m_dwFadeInTime + m_dwStartTextTime + m_dwFadeOutTime + ( m_dwCountGap * TEXT_NUM_COUNT ) )
	{
		m_dwCurrentStartTextTime = 0;

		if( m_pPlayMode && m_pPlayMode->GetModeType() == MT_PRACTICE )
		{
			ioPracticeMode *pPracticeMode = ToPracticeMode( m_pPlayMode );
			if( pPracticeMode )
			{
				SP2Packet kPacket( CTPK_PRACTICE_GAME_START );			
				kPacket << g_PracticeMgr.GetPreIndex();
				TCPNetwork::SendToServer( kPacket );

				pPracticeMode->StartPractice();
			}			
		}

		HideWnd();
	}
}

void StartingSignalWnd::OnRender()
{
	ioWnd::OnRender();	
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	Render_Ready( iXPos, iYPos );
	Render_Start( iXPos, iYPos - 40 );
}

void StartingSignalWnd::Render_Ready( int iXPos, int iYPos )
{
	if( m_dwCurrentStartTextTime == 0 ) return;
	if( m_iCount >= TEXT_NUM_COUNT ) return;

	if(m_pStartCount)
		m_pStartCount->Render(iXPos, iYPos);

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentStartTextTime;

	dwGapTime -= m_iCount * m_dwCountGap;

	if ( dwGapTime < m_dwCountGap )
	{
		int iAlphaRate = MAX_ALPHA_RATE;
		float fRate = Help::GetAniRateByTime( dwGapTime, m_dwFadeInTime, Help::RATE_NORMAL );
		float fScaleRate = 5.f - ( 4.f * fRate );

		if( COMPARE( m_iCount, 0, TEXT_NUM_COUNT ) )
		{
			if( m_pText_Num[m_iCount] )
			{
				m_pText_Num[m_iCount]->SetScale( fScaleRate );
				m_pText_Num[m_iCount]->SetAlpha( MAX_ALPHA_RATE * fRate );
				m_pText_Num[m_iCount]->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
	else
	{
		if( COMPARE( m_iCount + 1, 0, 4 ) )
			g_SoundMgr.PlaySound(m_szCountSound[m_iCount + 1], DSBVOLUME_MAX, PSM_NONE );
		
		++m_iCount;
	}
}

void StartingSignalWnd::Render_Start( int iXPos, int iYPos )
{
	if( m_dwCurrentStartTextTime == 0 ) return;
	if( m_iCount < TEXT_NUM_COUNT ) return;

	DWORD dwGapTime = FRAMEGETTIME() - m_dwCurrentStartTextTime;

	dwGapTime -= m_dwCountGap * TEXT_NUM_COUNT;
	
	if ( dwGapTime < m_dwFadeInTime ) // 시작
	{
		int iAlphaRate = MAX_ALPHA_RATE;
		float fRate = Help::GetAniRateByTime( dwGapTime, m_dwFadeInTime, Help::RATE_NORMAL );
		float fScaleRate = 5.f - ( 4.f * fRate );
		if( !m_pStartText )
			return;
		m_pStartText->SetScale( fScaleRate );
		m_pStartText->SetAlpha( MAX_ALPHA_RATE * fRate );
		m_pStartText->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if ( dwGapTime < m_dwFadeInTime + m_dwStartTextTime ) // 유지
	{
		if( !m_pStartText )
			return;
		m_pStartText->SetScale( 1.f );
		m_pStartText->SetAlpha( MAX_ALPHA_RATE );

		m_pStartText->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	else if ( dwGapTime < m_dwFadeInTime + m_dwStartTextTime + m_dwFadeOutTime ) // 퇴장
	{
		int iAlphaRate = MAX_ALPHA_RATE;
		float fRate = Help::GetAniRateByTime( dwGapTime - ( m_dwFadeInTime + m_dwStartTextTime ), m_dwFadeOutTime, Help::RATE_NORMAL );
		float fScaleRate = 2.f + ( 4.f * fRate );

		if( !m_pStartBlur )
			return;
		m_pStartBlur->SetScale( fScaleRate, 2.f );
		m_pStartBlur->SetAlpha( MAX_ALPHA_RATE * ( 1.f - fRate ) );
		m_pStartBlur->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	/*else
	{
		m_dwCurrentStartTextTime = 0;

		if( m_pPlayMode && m_pPlayMode->GetModeType() == MT_PRACTICE )
		{
			ioPracticeMode *pPracticeMode = ToPracticeMode( m_pPlayMode );
			if( pPracticeMode )
			{
				SP2Packet kPacket( CTPK_PRACTICE_GAME_START );			
				kPacket << g_PracticeMgr.GetPreIndex();
				TCPNetwork::SendToServer( kPacket );

				pPracticeMode->StartPractice();
			}			
			

		}

		HideWnd();
	}*/
}

void StartingSignalWnd::SetStart()
{
	m_iCount = 0;
	m_dwCurrentStartTextTime = FRAMEGETTIME();

	g_SoundMgr.PlaySound(m_szCountSound[0], DSBVOLUME_MAX, PSM_NONE );
}
