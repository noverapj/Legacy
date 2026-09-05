
#include "StdAfx.h"

#include "RouletteWnd.h"

#include "../ioRouletteManager.h"
#include "../ioRoulette.h"
#include "../io2DRotateQuad.h"

bool RouletteWnd::m_bPosToDefualt = false;

RouletteWnd::RouletteWnd()
{
	m_pRouletteHelpBg = NULL;
	m_pTextBg         = NULL;

	m_pTextTitle      = NULL;
	m_pTextEnableJoin = NULL;
	m_pTextJoin       = NULL;
	m_pTextEnd        = NULL;
	m_pTextEndSec     = NULL;
	m_pTextEndNum     = NULL;
	m_pTextPresent    = NULL;
	
	for( int i=0; i<3; ++i )
	{
		m_pTextReward[i]    = NULL;
		m_pTextRewardBg[i]  = NULL;
	}

	m_pUpperCoupon       = NULL;
	m_pUpperCouponNumber = NULL;

	m_pUnderJoin = NULL;
	m_pUnderCount = NULL;

	m_pArrow = NULL;

	m_pWheelQuad = NULL;

	m_iCurResultArray = 0;
	m_UIEffectList.clear();

	m_iCoin = 0;
	m_iJoinUser = 0;

	m_bSetUIEffect = false;

	m_bMainInfo = true;
	m_iInfoAlphaRate = MAX_ALPHA_RATE;
	m_bFadeIn = true;

	m_dwInfoDelayTime = 0;
	m_bSpinEndEffect = false;

	m_dwRewardStartTime = 0;

	m_State = US_READY;

	m_bJoinEnd = false;

	HideChildWnd( ID_HELP );
}

RouletteWnd::~RouletteWnd()
{
	SAFEDELETE( m_pRouletteHelpBg );
	SAFEDELETE( m_pTextBg );

	SAFEDELETE( m_pTextTitle );
	SAFEDELETE( m_pTextEnableJoin );
	SAFEDELETE( m_pTextJoin );
	SAFEDELETE( m_pTextEnd );
	SAFEDELETE( m_pTextEndSec );
	SAFEDELETE( m_pTextEndNum );
	SAFEDELETE( m_pTextPresent );

	for( int i=0; i<3; ++i )
	{
		SAFEDELETE( m_pTextReward[i] );
		SAFEDELETE( m_pTextRewardBg[i] );
	}

	SAFEDELETE( m_pUpperCoupon );
	SAFEDELETE( m_pUpperCouponNumber );

	SAFEDELETE( m_pUnderJoin );
	SAFEDELETE( m_pUnderCount );

	SAFEDELETE( m_pArrow );

	SAFEDELETE( m_pWheelQuad );
}

void RouletteWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "roulette_help_bg" )
	{
		SAFEDELETE( m_pRouletteHelpBg );
		m_pRouletteHelpBg = pImage;
	}
	else if( szType == "roulette_text_bg" )
	{
		SAFEDELETE( m_pTextBg );
		m_pTextBg = pImage;
	}
	else if( szType == "text_title" )
	{
		SAFEDELETE( m_pTextTitle );
		m_pTextTitle = pImage;
	}
	else if( szType == "text_enable_join" )
	{
		SAFEDELETE( m_pTextEnableJoin );
		m_pTextEnableJoin = pImage;
	}
	else if( szType == "text_join" )
	{
		SAFEDELETE( m_pTextJoin );
		m_pTextJoin = pImage;
	}
	else if( szType == "text_end" )
	{
		SAFEDELETE( m_pTextEnd );
		m_pTextEnd = pImage;
	}
	else if( szType == "text_end_sec" )
	{
		SAFEDELETE( m_pTextEndSec );
		m_pTextEndSec = pImage;
	}
	else if( szType == "text_end_num" )
	{
		SAFEDELETE( m_pTextEndNum );
		m_pTextEndNum = pImage;
	}
	else if( szType == "text_present" )
	{
		SAFEDELETE( m_pTextPresent );
		m_pTextPresent = pImage;
	}
	else if( szType == "text_reward_1" )
	{
		SAFEDELETE( m_pTextReward[0] );
		m_pTextReward[0] = pImage;
	}
	else if( szType == "text_reward_2" )
	{
		SAFEDELETE( m_pTextReward[1] );
		m_pTextReward[1] = pImage;
	}
	else if( szType == "text_reward_3" )
	{
		SAFEDELETE( m_pTextReward[2] );
		m_pTextReward[2] = pImage;
	}
	else if( szType == "text_reward_bg_1" )
	{
		SAFEDELETE( m_pTextRewardBg[0] );
		m_pTextRewardBg[0] = pImage;
	}
	else if( szType == "text_reward_bg_2" )
	{
		SAFEDELETE( m_pTextRewardBg[1] );
		m_pTextRewardBg[1] = pImage;
	}
	else if( szType == "text_reward_bg_3" )
	{
		SAFEDELETE( m_pTextRewardBg[2] );
		m_pTextRewardBg[2] = pImage;
	}
	else if( szType == "upper_coupon" )
	{
		SAFEDELETE( m_pUpperCoupon );
		m_pUpperCoupon = pImage;
	}
	else if( szType == "upper_coupon_num" )
	{
		SAFEDELETE( m_pUpperCouponNumber );
		m_pUpperCouponNumber = pImage;
	}
	else if( szType == "under_coupon" )
	{
		SAFEDELETE( m_pUnderJoin );
		m_pUnderJoin = pImage;
	}
	else if( szType == "under_count" )
	{
		SAFEDELETE( m_pUnderCount );
		m_pUnderCount = pImage;
	}
	else if( szType == "arrow" )
	{
		SAFEDELETE( m_pArrow );
		m_pArrow = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void RouletteWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	int iSize = xElement.GetIntAttribute_e( "Roulette_Count" );
	char szKey[MAX_PATH] = "";
	for( int i=0; i<iSize; ++i )
	{
		wsprintf_e( szKey, "Roulette%d", i+1 );
		ioHashString szRoulette = xElement.GetStringAttribute( szKey );
		m_vRouletteTexName.push_back( szRoulette );
	}
}

void RouletteWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false */ )
{
	float fXGap = ( Setting::Width() - 800.0f ) / 10;
	float fYGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefXPos = m_iDefXPos;
	int iTempDefYPos = m_iDefYPos;

	m_iDefXPos = iTempDefXPos + fXGap;
	m_iDefYPos = iTempDefYPos + fYGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefXPos = iTempDefXPos;
	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void RouletteWnd::iwm_create()
{
	m_pWheelQuad = new io2DRotateQuad;
	if( m_pWheelQuad )
		m_pWheelQuad->InitResource( m_vRouletteTexName );

	if( m_bPosToDefualt )
	{
		m_bPosToDefualt = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void RouletteWnd::iwm_show()
{
	ShowChildWnd( ID_HELP );
}

void RouletteWnd::iwm_hide()
{
	HideChildWnd( ID_HELP );
}

void RouletteWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_HELP:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.ShowWnd( ROULETTE_HELP_WND );
		}
		break;
	}
}

void RouletteWnd::OnProcess( float fTimePerSec )
{
	if( !g_RouletteMgr.IsRouletteEvent() )
		return;

	m_iCurResultArray = max( m_iCurResultArray, g_RouletteMgr.GetCurResultGroupArray() );
	
	if( m_iJoinUser < g_RouletteMgr.GetJoinCount() )
		m_bSetUIEffect = true;

	m_iJoinUser = g_RouletteMgr.GetJoinCount();
	m_iCoin = g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_ROULETTE_COIN );
	m_iCoin = min( 999, m_iCoin );

	if( m_pWheelQuad )
	{
		int iXOffset = m_pWheelQuad->GetWidth() / 2;
		int iYOffset = m_pWheelQuad->GetHeight() / 2;
		m_pWheelQuad->Update( GetDerivedPosX() + iXOffset - 5, GetDerivedPosY() + iYOffset - 5, g_RouletteMgr.GetCurAngle(), m_iCurResultArray );
	}

	ioMovingWnd::OnProcess( fTimePerSec );
}

void RouletteWnd::OnRender()
{
	if( !g_RouletteMgr.IsRouletteEvent() )
		return;

	if( !m_pRouletteHelpBg || !m_pTextBg )
		return;

	enum { CENTER_X = 59, CENTER_Y = 47 };

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	int iXOffset = 0;
	int iYOffset = 0;

	if( m_pWheelQuad && m_pWheelQuad->IsNeedRender() )
	{
		// 소멸은 g_UIRenderer가 해준다.
		ioRotateQuad *pQuad = new ioRotateQuad( m_pWheelQuad );
		if( pQuad )
		{
			g_UIRenderer.AddUIQuad( pQuad );
		}
	}

	m_pRouletteHelpBg->Render( iXPos, iYPos );
	m_pTextBg->Render( iXPos, iYPos );

	if( m_State == US_END && g_RouletteMgr.IsJoinRoulette( g_MyInfo.GetPublicID() ) )
	{
		RenderReward( iXPos, iYPos );
	}
	else
	{
		RenderCenterInfo( iXPos, iYPos );
		RenderUpperInfo( iXPos, iYPos );
		RenderUnderInfo( iXPos, iYPos );
	}

	if( m_pArrow )
		m_pArrow->Render( iXPos + 53, iYPos + 1 );

	RenderUIEffect();
}

void RouletteWnd::RenderReward( int iXPos, int iYPos )
{
	enum { CENTER_X = 59 };

	int iRank = g_RouletteMgr.GetRewardRank();
	if( iRank == 0 )
		return;

	ioUIRenderImage *pTextImage = m_pTextReward[iRank-1];
	ioUIRenderImage *pBgImage = m_pTextRewardBg[iRank-1];

	int iXOffset = 0, iYOffset = 0;
	if( pBgImage )
	{
		iXOffset = pBgImage->GetWidth() / 2;
		iYOffset = pBgImage->GetHeight() / 2;
		pBgImage->Render( iXPos + iXOffset, iYPos + iYOffset );
	}

	if( m_pTextPresent )
	{
		iXOffset = CENTER_X - ( m_pTextPresent->GetWidth() / 2 );
		m_pTextPresent->Render( iXPos + iXOffset, iYPos + 74 );
	}

	if( pTextImage )
	{
		if( m_dwRewardStartTime > 0 && m_dwRewardStartTime < FRAMEGETTIME() )
		{
			iXOffset = CENTER_X - pTextImage->GetWidth() / 2;
			pTextImage->SetScale( FLOAT1 );
			pTextImage->Render( iXPos + iXOffset, iYPos + 37 );

			if( !m_bSpinEndEffect )
			{
				SetUIEffect( pTextImage, iXPos + iXOffset, iYPos + 37 );
				m_bSpinEndEffect = true;
			}
		}
		else
		{
			iXOffset = CENTER_X - pTextImage->GetWidth() / 2;
			pTextImage->SetScale( FLOAT05 );
			pTextImage->Render( iXPos + iXOffset, iYPos + 37, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void RouletteWnd::RenderCenterInfo( int iXPos, int iYPos )
{
	enum { CENTER_X = 59, CENTER_Y = 47 };

	if( !m_pTextTitle || !m_pTextJoin || !m_pTextEnableJoin || !m_pTextEnd || !m_pTextEndNum || !m_pTextEndSec )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();
	float fRate = (float)( (fTimePerSec*1000) / 150.0f );

	switch( m_State )
	{
	case US_READY:
		{
			if( m_iCoin <= 0 )
			{
				int iXOffset = m_pTextJoin->GetWidth() / 2;
				m_pTextTitle->Render( iXPos + CENTER_X - iXOffset - 3, iYPos + CENTER_Y + 1 );
			}
			else
			{
				int iXOffset = m_pTextEnableJoin->GetWidth() / 2;
				m_pTextEnableJoin->SetAlpha( MAX_ALPHA_RATE );
				m_pTextEnableJoin->Render( iXPos + CENTER_X - iXOffset, iYPos + CENTER_Y );
			}
		}
		break;
	case US_SPIN:
		{
			if( m_bMainInfo )
			{
				if( g_RouletteMgr.IsJoinRoulette( g_MyInfo.GetPublicID() ) )
				{
					int iXOffset = m_pTextJoin->GetWidth() / 2;
					m_pTextJoin->SetAlpha( m_iInfoAlphaRate );
					m_pTextJoin->Render( iXPos + CENTER_X - iXOffset, iYPos + CENTER_Y );
				}
				else if( g_RouletteMgr.EnableRouletteSpin( g_MyInfo.GetPublicID() ) )
				{
					int iXOffset = m_pTextEnableJoin->GetWidth() / 2;
					m_pTextEnableJoin->SetAlpha( m_iInfoAlphaRate );
					m_pTextEnableJoin->Render( iXPos + CENTER_X - iXOffset, iYPos + CENTER_Y );
				}
				else
				{
					int iXOffset = m_pTextJoin->GetWidth() / 2;
					m_pTextTitle->SetAlpha( m_iInfoAlphaRate );
					m_pTextTitle->Render( iXPos + CENTER_X - iXOffset - 3, iYPos + CENTER_Y + 1 );
				}

				if( g_RouletteMgr.GetState() == ioRouletteManager::RS_SPIN )
				{
					if( m_bFadeIn )
					{
						m_iInfoAlphaRate += MAX_ALPHA_RATE*fRate;
						m_iInfoAlphaRate = min( m_iInfoAlphaRate, MAX_ALPHA_RATE );
						if( m_iInfoAlphaRate >= MAX_ALPHA_RATE )
						{
							if( m_dwInfoDelayTime == 0 )
							{
								m_dwInfoDelayTime = FRAMEGETTIME() + 300;
							}
							else if( m_dwInfoDelayTime > 0 && m_dwInfoDelayTime < FRAMEGETTIME() )
							{
								m_dwInfoDelayTime = 0;
								m_bFadeIn = false;
							}
						}
					}
					else
					{
						m_iInfoAlphaRate -= MAX_ALPHA_RATE*fRate;
						m_iInfoAlphaRate = max( m_iInfoAlphaRate, 0 );
						if( m_iInfoAlphaRate <= 0 )
						{
							m_bFadeIn = true;
							m_bMainInfo = false;
							if( m_bJoinEnd )
								m_State = US_JOIN_END;
						}
					}
				}
			}
			else
			{
				if( dwCurTime <= g_RouletteMgr.GetJoinEndTime() )
				{
					int iRemainSec = g_RouletteMgr.GetJoinEndTime() - dwCurTime;
					iRemainSec /= 1000;

					int iCount = 1;
					if( iRemainSec >= 10 )
						iCount = 2;

					int iNumWidth = m_pTextEndNum->GetWidth() / 10;
					int iWidth = m_pTextEnd->GetWidth() + 1 +
						( iNumWidth * iCount ) - ( 5 * iCount ) - 5 +
						m_pTextEndSec->GetWidth();
					int iOffset = CENTER_X - iWidth/2;

					m_pTextEnd->SetAlpha( m_iInfoAlphaRate );
					m_pTextEnd->Render( iXPos + iOffset, iYPos + CENTER_Y );
					iOffset += m_pTextEnd->GetWidth();

					int iNumber = iRemainSec/10;
					if( iNumber > 0 )
					{
						m_pTextEndNum->SetAlpha( m_iInfoAlphaRate );
						m_pTextEndNum->RenderNum( iXPos + iOffset, iYPos + CENTER_Y - 1, iNumber );
						iOffset += iNumWidth - 5;
					}

					iNumber = iRemainSec%10;
					m_pTextEndNum->SetAlpha( m_iInfoAlphaRate );
					m_pTextEndNum->RenderNum( iXPos + iOffset, iYPos + CENTER_Y - 1, iNumber );
					iOffset += iNumWidth - 5;

					m_pTextEndSec->SetAlpha( m_iInfoAlphaRate );
					m_pTextEndSec->Render( iXPos + iOffset, iYPos + CENTER_Y - 1 );
				}

				if( m_bFadeIn )
				{
					m_iInfoAlphaRate += MAX_ALPHA_RATE*fRate;
					m_iInfoAlphaRate = min( m_iInfoAlphaRate, MAX_ALPHA_RATE );
					if( m_iInfoAlphaRate >= MAX_ALPHA_RATE )
					{
						if( m_dwInfoDelayTime == 0 )
						{
							m_dwInfoDelayTime = FRAMEGETTIME() + 300;
						}
						else if( m_dwInfoDelayTime > 0 && m_dwInfoDelayTime < FRAMEGETTIME() )
						{
							m_dwInfoDelayTime = 0;
							m_bFadeIn = false;
						}
					}
				}
				else
				{
					m_iInfoAlphaRate -= MAX_ALPHA_RATE*fRate;
					m_iInfoAlphaRate = max( m_iInfoAlphaRate, 0 );
					if( m_iInfoAlphaRate <= 0 )
					{
						m_bFadeIn = true;
						m_bMainInfo = true;
					}
				}
			}
		}
		break;
	case US_JOIN_END:
		if( m_pTextEnd )
		{
			if( m_iInfoAlphaRate < MAX_ALPHA_RATE )
			{
				m_iInfoAlphaRate += MAX_ALPHA_RATE*fRate;
				m_iInfoAlphaRate = min( m_iInfoAlphaRate, MAX_ALPHA_RATE );
			}

			int iXOffset = m_pTextEnd->GetWidth() / 2;
			m_pTextEnd->SetAlpha( m_iInfoAlphaRate );
			m_pTextEnd->Render( iXPos + CENTER_X - iXOffset, iYPos + CENTER_Y );
		}
		break;
	case US_END:
		if( m_pTextEnd )
		{
			if( m_iInfoAlphaRate < MAX_ALPHA_RATE )
			{
				m_iInfoAlphaRate += MAX_ALPHA_RATE*fRate;
				m_iInfoAlphaRate = min( m_iInfoAlphaRate, MAX_ALPHA_RATE );
			}

			int iXOffset = m_pTextEnd->GetWidth() / 2;
			m_pTextEnd->SetAlpha( m_iInfoAlphaRate );
			m_pTextEnd->Render( iXPos + CENTER_X - iXOffset, iYPos + CENTER_Y );
		}
		break;
	}
}

void RouletteWnd::RenderUpperInfo( int iXPos, int iYPos )
{
	if( !m_pUpperCoupon || !m_pUpperCouponNumber )
		return;
	
	enum { CENTER_X = 59, TEXT_Y = 23, NUMBER_Y = 22,
		   TEXT_GAP = -2, NUMBER_GAP = -3 };
	
	int iCount = 1;
	if( m_iCoin >= 100 )
		iCount = 3;
	else if( m_iCoin >= 10 )
		iCount = 2;

	int iNumWidth = m_pUpperCouponNumber->GetWidth() / 10;
	int iWidth = m_pUpperCoupon->GetWidth() + ( iNumWidth * iCount );
	
	int iOffset = CENTER_X - iWidth/2;

	m_pUpperCoupon->Render( iXPos + iOffset, iYPos + TEXT_Y );
	iOffset += m_pUpperCoupon->GetWidth() + TEXT_GAP;

	int iNumber = m_iCoin/100;
	if( iNumber > 0 )
	{
		m_pUpperCouponNumber->RenderNum( iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
		iOffset += iNumWidth + NUMBER_GAP;
	}
	
	iNumber = (m_iCoin%100)/10;
	if( iNumber > 0 || iCount == 3  )
	{
		m_pUpperCouponNumber->RenderNum( iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
		iOffset += iNumWidth + NUMBER_GAP;
	}
	
	iNumber = m_iCoin%10;
	m_pUpperCouponNumber->RenderNum( iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
}

void RouletteWnd::RenderUnderInfo( int iXPos, int iYPos )
{
	if( !m_pUnderJoin || !m_pUpperCouponNumber || !m_pUnderCount )
		return;

	enum { CENTER_X = 59, TEXT_Y = 79, NUMBER_Y = 79,
		   TEXT_GAP = 3, NUMBER_GAP = -3 };

	int iCount = 1;
	if( m_iJoinUser >= 10 )
		iCount = 2;

	int iNumWidth = m_pUpperCouponNumber->GetWidth() / 10;
	int iWidth = m_pUnderJoin->GetWidth() + ( iNumWidth * iCount ) + m_pUnderCount->GetWidth();
	int iOffset = CENTER_X - iWidth/2;

	m_pUnderJoin->Render( iXPos + iOffset, iYPos + TEXT_Y );
	iOffset += m_pUnderJoin->GetWidth() + TEXT_GAP;

	int iNumber = m_iJoinUser/10;
	if( iNumber > 0 )
	{
		m_pUpperCouponNumber->RenderNum( iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
		
		if( m_bSetUIEffect )
		{
			SetUIEffect( m_pUpperCouponNumber, iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
		}

		iOffset += iNumWidth + NUMBER_GAP;
	}

	iNumber = m_iJoinUser%10;
	m_pUpperCouponNumber->RenderNum( iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
	if( m_bSetUIEffect )
	{
		SetUIEffect( m_pUpperCouponNumber, iXPos + iOffset, iYPos + NUMBER_Y, iNumber );
	}
	iOffset += iNumWidth;

	m_pUnderCount->Render( iXPos + iOffset, iYPos + NUMBER_Y );

	if( m_bSetUIEffect )
		m_bSetUIEffect = false;
}

void RouletteWnd::RenderUIEffect()
{
	if( m_UIEffectList.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	UIEffectAniInfoList::iterator iter = m_UIEffectList.begin();
	while( iter != m_UIEffectList.end() )
	{
		UIEffectAniInfo &kInfo = *iter;

		if( kInfo.m_dwEndTime < dwCurTime )
		{
			iter = m_UIEffectList.erase( iter );
			continue;
		}

		ioUIRenderImage *pImage = kInfo.GetImage();
		if( pImage )
		{
			float fRate = float( ( kInfo.m_dwEndTime - dwCurTime ) / FLOAT500 );
			kInfo.m_iAlpha = MAX_ALPHA_RATE*fRate;
			kInfo.m_fScale = FLOAT1 + ( FLOAT1 - fRate );

			pImage->SetAlpha( kInfo.m_iAlpha );

			int iXPos, iYPos;
			if( kInfo.m_iNum != -1 )
			{
				float fScale = kInfo.m_fScale;
				iXPos = kInfo.m_iXPos - ( pImage->GetWidth()/10 * fScale ) / 2;
				iYPos = kInfo.m_iYPos - ( pImage->GetHeight() * fScale ) / 2;
				pImage->RenderNum( iXPos, iYPos, kInfo.m_iNum, 0.0f, kInfo.m_fScale, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			else
			{
				float fScale = kInfo.m_fScale;
				pImage->SetScale( kInfo.m_fScale );
				pImage->Render( kInfo.m_iXPos, kInfo.m_iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}

			pImage->SetScale( FLOAT1 );
			pImage->SetAlpha( MAX_ALPHA_RATE );
		}

		++iter;
	}
}

void RouletteWnd::SetUIEffect( ioUIRenderImage *pImage, int x, int y, int iNum /* = -1 */ )
{
	if( !pImage )
		return;

	int iXPos = 0;
	int iYPos = 0;
	if( iNum != -1 )
	{
		iXPos = pImage->GetWidth() / 10 / 2;
		iYPos = pImage->GetHeight() / 2;
	}

	UIEffectAniInfo kInfo( pImage, x + iXPos, y + iYPos, iNum );
	m_UIEffectList.push_back( kInfo );
}

void RouletteWnd::SetState( UIState eState )
{
	if( m_State == eState )
		return;

	switch( eState )
	{
	case US_READY:
		{
			m_State = US_READY;
			m_bSetUIEffect = false;
			m_bMainInfo = true;
			m_iInfoAlphaRate = MAX_ALPHA_RATE;
			m_bFadeIn = true;
			m_dwInfoDelayTime = 0;
			m_bSpinEndEffect = false;
			m_dwRewardStartTime = 0;
		}
		break;
	case US_SPIN:
		{
			m_iCurResultArray = g_RouletteMgr.GetCurResultGroupArray();
			m_State = US_SPIN;
		}
		break;
	case US_JOIN_END:
		{
			m_bJoinEnd = true;
// 			m_State = US_JOIN_END;
// 			m_iInfoAlphaRate = MAX_ALPHA_RATE;
		}
		break;
	case US_END:
		{
			m_State = US_END;
			m_dwRewardStartTime = FRAMEGETTIME() + 300;
			m_bJoinEnd = false;
		}
		break;
	}
}