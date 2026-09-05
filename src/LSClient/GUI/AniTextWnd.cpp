#include "StdAfx.h"

#include "PlazaNPCGaugeWnd.h"
#include "AniTextWnd.h"


#define VIB_POS			9
#define VIB_DURATION	33

#define COLORING_DURATION  1000


AniTextWnd::AniTextWnd(void)
{
	m_eState = SAT_NONE;
}


AniTextWnd::~AniTextWnd(void)
{
}

void AniTextWnd::StartAniText( SubWnd eSub )
{
	m_eType = eSub;

	ioWnd* pWnd = FindChildWnd(eSub);

	if( pWnd )
	{
		SetState(SAT_START_ANI);

		pWnd->ShowWnd();
		for( int i = 0; i < pWnd->GetNumChildren(); i++ )
		{
			pWnd->GetChild(i)->ShowWnd();

			AniLetterSubWnd* pAniLetterSubWnd = dynamic_cast<AniLetterSubWnd*>( pWnd->GetChild(i) );
			if ( pAniLetterSubWnd )
				pAniLetterSubWnd->SetState(AniLetterSubWnd::SAT_START_ANI);
		}
	}
}

void AniTextWnd::SetState( ShowAniType eState )
{
	m_eState = eState;

	switch(m_eState)
	{
	case SAT_NONE:
		break;
	case SAT_START_ANI:
		break;
	case SAT_END:
		if( m_eType == ARRIVAL_SUBWND || m_eType == AWAKENING_SUBWND )
		{
			PlazaNPCGaugeWnd *pPlazaNPCGaugeWnd = dynamic_cast<PlazaNPCGaugeWnd *>( g_GUIMgr.FindWnd( PLAZA_NPC_GAUGE_WND ) );
			if( pPlazaNPCGaugeWnd )
			{
				pPlazaNPCGaugeWnd->ChangeShowAniState( ioAnimationState::SAT_START_ROUND );
				pPlazaNPCGaugeWnd->ShowWnd();
			}
		}
		else if( m_eType == DESTROY_SUBWND || m_eType == RUNAWAY_SUBWND )
		{
			
		}
		SetState(SAT_NONE);
		break;
	}
}


AniLetterSubWnd::AniLetterSubWnd(void)
{
	m_pLetter = NULL;
	ZeroMemory( &m_stStart, sizeof(stAniText));
	ZeroMemory( &m_stDest, sizeof(stAniText));
	ZeroMemory( &m_stCurrPos, sizeof(stAniText));
	ZeroMemory( &m_stExFactor, sizeof(stExtraFactor));
	m_szBGM = "";
	m_szEffect = "";
	m_bLeftToRight = false;
	m_fSpeed = 0.f;
	m_fScaleSpeed = 0.f;
	m_dwAniTime = 0.f;
	m_dwZoomTime = 0.f;
	m_eState = SAT_NONE;
}


AniLetterSubWnd::~AniLetterSubWnd(void)
{
	SAFEDELETE(m_pLetter);
}

void AniLetterSubWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Letter" )
	{
		SAFEDELETE(m_pLetter);
		m_pLetter = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}


void AniLetterSubWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_stExFactor.dwStartTime = xElement.GetIntAttribute_e( "StartTime" );
	m_stExFactor.dwStartDuration = xElement.GetIntAttribute_e( "StartDuration" );
	m_stExFactor.fStartScale = xElement.GetFloatAttribute_e( "StartScale" );
	m_stExFactor.nStartXPos = xElement.GetIntAttribute_e( "StartXPos" );
	m_stExFactor.nZoomOutXPos = xElement.GetIntAttribute_e( "ZoomOutXPos" );
	m_stExFactor.fStartYPosRatio = xElement.GetFloatAttribute_e( "StartYPosRatio" );
	m_stExFactor.dwDelayDuration = xElement.GetIntAttribute_e( "DelayDuration" );
	m_stExFactor.dwVibDuration = xElement.GetIntAttribute_e( "VibDuration" );
	m_stExFactor.dwTextClrDuration = xElement.GetIntAttribute_e( "TextClrDuration" );
	m_stExFactor.dwZoomInDuration = xElement.GetIntAttribute_e( "ZoomInDuration" );

	m_szBGM = xElement.GetStringAttribute( "BGM" );
	m_szEffect = xElement.GetStringAttribute_e( "EffectSnd" );
}


void AniLetterSubWnd::SetState( ShowAniType eState )
{
	m_eState = eState;

	switch(m_eState)
	{
	case SAT_NONE:
		break;

	case SAT_START_ANI:
		if( !m_szBGM.IsEmpty() )
		{
			g_SoundMgr.StopStream();

			g_SoundMgr.OpenOggStream( m_szBGM.c_str() );
			g_SoundMgr.PlayStream( true );
		}

		m_dwAniTime = FRAMEGETTIME() + m_stExFactor.dwStartTime;
		m_stStart.fX = (Setting::Width() / 2) + m_stExFactor.nStartXPos;
		m_stStart.fY = (Setting::Height() * m_stExFactor.fStartYPosRatio);
		m_stStart.fScale = m_stExFactor.fStartScale;

		m_stDest.fX = (Setting::Width() / 2) + m_stExFactor.nZoomOutXPos;
		m_stDest.fY = (Setting::Height() * m_stExFactor.fStartYPosRatio);
		m_stDest.fScale = FLOAT1;

		if( m_stDest.fX >= m_stStart.fX)
			m_bLeftToRight = true;

		m_stCurrPos.fScale = m_stStart.fScale;
		m_stCurrPos.fX	= m_stStart.fX;
		m_stCurrPos.fY	= m_stStart.fY;
		break;

	case SAT_ZOOM_OUT:
		{
			m_dwZoomTime = m_dwAniTime;
			m_dwAniTime += m_stExFactor.dwStartDuration;

			float fDest = (float) ((m_stStart.fScale - m_stDest.fScale) * 2);
			float fTime = (float) (m_stExFactor.dwStartDuration * m_stExFactor.dwStartDuration);
			m_fScaleSpeed = fDest / fTime; 

			if( m_bLeftToRight)
				fDest = (float) ( (m_stDest.fX - m_stStart.fX) * 2);
			else
				fDest = (float) ( (m_stStart.fX - m_stDest.fX) * 2);

			fTime = (float) (m_stExFactor.dwStartDuration * m_stExFactor.dwStartDuration);
			m_fSpeed = fDest / fTime;

			m_fAlphaRate = 0.f;
		}
		break;

	case SAT_DELAY:
		if( !m_szEffect.IsEmpty() )
			g_SoundMgr.PlaySound(m_szEffect);

		m_dwAniTime += m_stExFactor.dwDelayDuration;
		m_stCurrPos.fScale = m_stDest.fScale;
		m_stCurrPos.fX = m_stDest.fX;
		m_stCurrPos.fY = m_stDest.fY;
		break;

	case SAT_VIB:
		m_dwZoomTime = m_dwAniTime + VIB_DURATION;
		m_dwAniTime += m_stExFactor.dwVibDuration;
		m_stCurrPos.fX = m_stDest.fX;
		m_stCurrPos.fY = m_stDest.fY;
		m_stCurrPos.fScale = m_stDest.fScale;
		m_nOffSet = 0;
		break;

	case SAT_ANI_COLOR:
		m_dwZoomTime = m_dwAniTime + COLORING_DURATION;
		m_dwAniTime += m_stExFactor.dwTextClrDuration;
		m_stCurrPos.fX = m_stDest.fX;
		m_stCurrPos.fY = m_stDest.fY;
		m_stCurrPos.fScale = m_stDest.fScale;
		m_nOffSet = 0;
		break;

	case SAT_ZOOM_IN:
		{
			m_dwZoomTime = m_dwAniTime;
			m_dwAniTime += m_stExFactor.dwZoomInDuration;

			float fDest = (float) ((m_stStart.fScale - m_stDest.fScale) * 2);
			float fTime = (float) (m_stExFactor.dwZoomInDuration * m_stExFactor.dwZoomInDuration);
			m_fScaleSpeed = fDest / fTime; 

			if( m_bLeftToRight)
				fDest = (float) ( (m_stDest.fX - m_stStart.fX) * 2);
			else
				fDest = (float) ( (m_stStart.fX - m_stDest.fX) * 2);

			fTime = (float) (m_stExFactor.dwZoomInDuration * m_stExFactor.dwZoomInDuration);
			m_fSpeed = fDest / fTime;

			m_fAlphaRate = FLOAT1;
		}
		break;

	case SAT_END:
		{
			HideWnd();
			SetState( SAT_NONE );

			ioWnd* pWnd = GetParent();
			if( pWnd )
			{
				AniTextWnd* pAniWnd = dynamic_cast<AniTextWnd*>( pWnd->GetParent() );
				if( pAniWnd )
					pAniWnd->SetState(AniTextWnd::SAT_END);
			}
		}
		break;

	default:
		break;
	}
}

void AniLetterSubWnd::OnProcess( float fTimePerSec )
{
	switch(m_eState)
	{
	case SAT_NONE:
		break;

	case SAT_START_ANI:
		if( FRAMEGETTIME() > m_dwAniTime )
		{
			ShowWnd();
			SetState( SAT_ZOOM_OUT );
		}
		break;

	case SAT_ZOOM_OUT:
		if( FRAMEGETTIME() > m_dwAniTime )
		{
			SetState( SAT_DELAY );
		}
		else
		{
			float fTime = (float)(FRAMEGETTIME() - m_dwZoomTime);
			m_stCurrPos.fScale = m_stStart.fScale - (0.5f * m_fScaleSpeed * fTime * fTime);

			if( m_bLeftToRight)
			{
				m_stCurrPos.fX = m_stStart.fX + (0.5f * m_fSpeed * fTime * fTime);
				m_stCurrPos.fY = m_stStart.fY;// - (0.5f * m_fSpeed * fTime);
			}
			else
			{
				m_stCurrPos.fX = m_stStart.fX - (0.5f * m_fSpeed * fTime * fTime);
				m_stCurrPos.fY = m_stStart.fY;// - (0.5f * m_fSpeed * fTime);
			}

			m_fAlphaRate = ( fTime / (float)m_stExFactor.dwStartDuration );
		}
		break;

	case SAT_DELAY:
		if( FRAMEGETTIME() > m_dwAniTime )
		{
			SetState( SAT_VIB );
		}
		break;

	case SAT_VIB:
		if( FRAMEGETTIME() > m_dwAniTime )
		{
			SetState( SAT_ANI_COLOR );
		}
		else
		{
			if( FRAMEGETTIME() > m_dwZoomTime )
			{
				m_dwZoomTime += VIB_DURATION;
				m_nOffSet++;
				if(m_nOffSet >= VIB_POS )
					m_nOffSet = 0;
			}
		}
		break;

	case SAT_ANI_COLOR:
		if( FRAMEGETTIME() > m_dwAniTime )
		{
			SetState( SAT_ZOOM_IN );
		}
		else
		{
			float fTime = (float)( m_dwZoomTime - FRAMEGETTIME() );
			m_fAlphaRate = ( fTime / (float)(COLORING_DURATION) );

			if( fTime > COLORING_DURATION )
			{
				m_dwZoomTime += COLORING_DURATION;
				m_nOffSet++;
//				m_fAlphaRate = 0.f;
			}
		}
		break;

	case SAT_ZOOM_IN:
		if( FRAMEGETTIME() > m_dwAniTime )
		{
			SetState( SAT_END );
		}
		else
		{
			float fTime = (float)(FRAMEGETTIME() - m_dwZoomTime);
			m_stCurrPos.fScale = m_stDest.fScale + (0.5f * m_fScaleSpeed * fTime * fTime);

			if( m_bLeftToRight)
			{
				m_stCurrPos.fX = m_stDest.fX - (0.5f * m_fSpeed * fTime * fTime);
				m_stCurrPos.fY = m_stDest.fY;// - (0.5f * m_fSpeed * fTime);
			}
			else
			{
				m_stCurrPos.fX = m_stDest.fX + (0.5f * m_fSpeed * fTime * fTime);
				m_stCurrPos.fY = m_stDest.fY;// - (0.5f * m_fSpeed * fTime);
			}

			m_fAlphaRate = ( fTime / (float)m_stExFactor.dwZoomInDuration );
		}
		break;

	case SAT_END:
		break;

	default:
		break;
	}
}

void AniLetterSubWnd::OnRender()
{
	if( m_eState == SAT_NONE || m_eState == SAT_START_ANI || m_eState == SAT_END)
		return;

	if( !m_pLetter )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();	

	const POINT ptOffSet[9] = { 0, 0, 2, 0, -1, 0, -1, 2, -1, -1, 0, 1, -2, 0, 1, -1, -2, 1};

	switch(m_eState)
	{
	case SAT_DELAY:
		{
			float fXPos = (float)iXPos + m_stCurrPos.fX;
			float fYPos = (float)iYPos + m_stCurrPos.fY;
			m_pLetter->SetScale(m_stCurrPos.fScale, m_stCurrPos.fScale);
			m_pLetter->Render( fXPos, fYPos );
		}
		break;

	case SAT_ZOOM_OUT:
		{
			float fXPos = (float)iXPos + m_stCurrPos.fX;
			float fYPos = (float)iYPos + m_stCurrPos.fY;

			m_pLetter->SetScale(m_stCurrPos.fScale, m_stCurrPos.fScale);
			m_pLetter->SetAlpha(255.f);
			m_pLetter->Render( fXPos, fYPos );

 			m_pLetter->SetColor(255, 255, 255);
 			m_pLetter->SetAlpha( int( 255.0f * (1.f - m_fAlphaRate)) );
 			m_pLetter->Render( fXPos, fYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
		}
		break;

	case SAT_VIB:
		{
			float fXPos = (float)iXPos + m_stCurrPos.fX;
			float fYPos = (float)iYPos + m_stCurrPos.fY;
			m_pLetter->SetAlpha(255.f);
			if( m_nOffSet == 0 )
			{
				m_pLetter->Render(fXPos + ptOffSet[m_nOffSet].x, fYPos + ptOffSet[m_nOffSet].y);
				m_pLetter->Render(fXPos + ptOffSet[m_nOffSet].x, fYPos + ptOffSet[m_nOffSet].y, UI_RENDER_SCREEN);
			}
			else if( m_nOffSet % 3 == 0)
			{
				m_pLetter->Render(fXPos + ptOffSet[m_nOffSet].x, fYPos + ptOffSet[m_nOffSet].y);
				m_pLetter->Render(fXPos + ptOffSet[m_nOffSet].x, fYPos + ptOffSet[m_nOffSet].y, UI_RENDER_SCREEN);
			}
			else
			{
				m_pLetter->Render(fXPos + ptOffSet[m_nOffSet].x, fYPos + ptOffSet[m_nOffSet].y);
			}
		}
		break;

	case SAT_ANI_COLOR:
		{
			float fXPos = (float)iXPos + m_stCurrPos.fX;
			float fYPos = (float)iYPos + m_stCurrPos.fY;

			m_pLetter->SetScale(m_stCurrPos.fScale, m_stCurrPos.fScale);
			m_pLetter->SetAlpha(255.f);
			m_pLetter->Render( fXPos, fYPos );

			if( m_nOffSet == 0 )
			{
				m_pLetter->SetColor(255, 255, 255);
				m_pLetter->SetAlpha( int( 255.0f * 0.75f * (m_fAlphaRate)) );
				m_pLetter->Render( fXPos, fYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
			}
			else if( m_nOffSet % 2 == 0)
			{
				m_pLetter->SetColor(255, 255, 255);
				m_pLetter->SetAlpha( int( 255.0f * 0.75f * (m_fAlphaRate)) );
				m_pLetter->Render( fXPos, fYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
			}
			else
			{
				m_pLetter->SetColor(255, 255, 255);
				m_pLetter->SetAlpha( int( 255.0f * 0.75f * (1.f - m_fAlphaRate)) );
				m_pLetter->Render( fXPos, fYPos, UI_RENDER_NORMAL_ADD_COLOR, TFO_BILINEAR );
			}
		}
		break;

	case SAT_ZOOM_IN:
		{
			float fXPos = (float)iXPos + m_stCurrPos.fX;
			float fYPos = (float)iYPos + m_stCurrPos.fY;
			m_pLetter->SetScale(m_stCurrPos.fScale, m_stCurrPos.fScale);
			m_pLetter->SetAlpha( int( 255.0f * (1.f - m_fAlphaRate)) );
			m_pLetter->Render( fXPos, fYPos );
		}
		break;

	default:
		break;
	}
}

void AniLetterSubWnd::iwm_create()
{
	m_dwStyle |= IWS_PROCESS_ALWAYS;
}