
#include "StdAfx.h"
#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"

#include "ShuffleRankWnd.h"

ShuffleRankWnd::ShuffleRankWnd()
{
	m_dwState		= STATE_DELAY;
	m_dwCurrentTime = 0;
	m_dwFadeTime	= 0;
	m_bSort			= false;
}

ShuffleRankWnd::~ShuffleRankWnd()
{
}

void ShuffleRankWnd::ParseExtraInfo( ioXMLElement &xElement )
{	
	m_dwFadeTime = xElement.GetIntAttribute_e( "FadeTime" );
	m_iHieght    = xElement.GetIntAttribute_e( "Height" );
}

void ShuffleRankWnd::ClearRank()
{
	m_vRank.clear();
}

void ShuffleRankWnd::AddRankUser( const ioHashString& szName, int iTeamType, int iCurrStar )
{
	Rank sRank;
	sRank.szName    = szName;
	sRank.iTeamType = iTeamType;
	sRank.iCurrStar = iCurrStar;

	m_vRank.push_back( sRank );

	m_bSort = true;
	SetSize( GetWidth(), m_iHieght * (int)m_vRank.size() );
}

void ShuffleRankWnd::Show()
{
	if( !IsShow() )
	{
		SetState( STATE_FADEIN );
		ShowWnd();
	}
}

void ShuffleRankWnd::Hide()
{
	if( m_dwState != STATE_FADEOUT )
		SetState( STATE_FADEOUT );
}

void ShuffleRankWnd::SetState( DWORD dwState )
{
	m_dwState       = dwState;
	m_dwCurrentTime = FRAMEGETTIME();
	m_dwFadeTime    = max( 100, m_dwFadeTime );

	switch( m_dwState )
	{
	case STATE_DELAY:
	case STATE_FADEIN:
		m_iAlphaRate = 0;
		break;
	case STATE_VIEW:
	case STATE_FADEOUT:
		m_iAlphaRate = MAX_ALPHA_RATE;
		break;
	}
}

D3DCOLOR ShuffleRankWnd::GetRankColor( int iRank )
{
	switch( iRank )
	{
	case 1:		
		return TCT_EX_YEELOW;	
	case 2:
		return TCT_EX_RICE;
	case 3:
		return TCT_EX_BROWN;
	default:
		return TCT_DEFAULT_WHITE;		
	}	
}

void ShuffleRankWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	DWORD dwGapTime  = FRAMEGETTIME() - m_dwCurrentTime;
	switch( m_dwState )
	{
	case STATE_DELAY:
		{
		}
		break;
	case STATE_FADEIN:
		if( dwGapTime < m_dwFadeTime )
		{
			m_iAlphaRate = (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / m_dwFadeTime );
		}
		else
		{
			SetState( STATE_VIEW );
		}
		break;
	case STATE_VIEW:		
		{
			if( m_bSort && !m_vRank.empty() )
			{				
				std::sort( m_vRank.begin(), m_vRank.end(), RankOrder() );
				m_bSort = false;
			}
		}	
		break;
	case STATE_FADEOUT:
		if( dwGapTime < m_dwFadeTime )
		{
			m_iAlphaRate = MAX_ALPHA_RATE - ( (float)MAX_ALPHA_RATE * (float)( (float)dwGapTime / m_dwFadeTime ) );
		}
		else
		{
			HideWnd();
		}
		break;
	}
}

void ShuffleRankWnd::OnRender()
{
	ioWnd::OnRender();

	switch( m_dwState )
	{
	case STATE_FADEIN:
	case STATE_VIEW:
	case STATE_FADEOUT:
		{
			int iXPos = GetDerivedPosX();
			int iYPos = GetDerivedPosY();
			int iSize = (int)m_vRank.size();
			
			if( m_vRank.empty() )
				return;

			for( int iRank = 0; iRank < iSize;  ++iRank )
			{
				if( iRank < iSize-1 )
				{
					OnRankRender( iXPos, iYPos, m_vRank[iRank].iTeamType, m_vRank[iRank].szName, iRank, m_vRank[iRank].iCurrStar, false );
					iYPos += m_iHieght;
				}
				else
				{
					OnRankRender( iXPos, iYPos, m_vRank[iRank].iTeamType, m_vRank[iRank].szName, iRank, m_vRank[iRank].iCurrStar, true );
					iYPos += m_iHieght;
				}
			}
		}
		break;
	}
}

void ShuffleRankWnd::OnRankRender( int iXPos, int iYPos, int iTeamType, ioHashString szName, int iRank, int iStar, bool bBottom )
{
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );	
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	if( bBottom )
	{
		g_FontMgr.SetTextColor( 145, 145, 145 );
		g_FontMgr.PrintTextAlpha( iXPos + 40, iYPos, FONT_SIZE_12, m_iAlphaRate, "º°°ÅÁö" );
	}
	else
	{
		g_FontMgr.SetTextColor( GetRankColor( iRank ) );
		g_FontMgr.PrintTextAlpha( iXPos + 40, iYPos, FONT_SIZE_12, m_iAlphaRate, "%dÀ§", iRank + 1 );
	}

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_OUTLINE_FULL_2X );
	kPrinter.SetBkColor( 0, 0, 0 );

	if( iTeamType == TEAM_BLUE )
		kPrinter.SetTextColor( TCT_EX_BLUE );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_RED );

	kPrinter.AddTextPiece( FONT_SIZE_12, "%s", szName.c_str() );

	kPrinter.SetTextColor( TCT_DEFAULT_WHITE );
	kPrinter.AddTextPiece( FONT_SIZE_12, "´Ô " );
	kPrinter.SetTextColor( TCT_EX_YEELOW );
	kPrinter.AddTextPiece( FONT_SIZE_12, "¡Ú%d", iStar );
	kPrinter.PrintFullText( iXPos + 46, iYPos, TAT_LEFT, m_iAlphaRate );
}