#include <StdAfx.h>

#include "ExcavationResultWnd.h"
#include "ExcavationWnd.h"
#include "ExcavationLevelUpWnd.h"
#include "../ioExcavationManager.h"
#include "../DataHeaders/LSC_decoitem_info.h"
#include "../DataHeaders/LSC_Jewel_present.h"
#include "../ioDecorationPrice.h"
#include "../ioEventManager.h"

ExcavationResultWnd::ExcavationResultWnd()
{
	m_pIcon = NULL;
	m_pIconBG = NULL;
	m_pIconBGRolling = NULL;
	m_pResultGrade = NULL;
	m_pResultRank = NULL;
	m_pRankEffect = NULL;
	m_pMultiple2 = NULL;
	m_pMultiple3 = NULL;
	m_pMultiple4 = NULL;
	m_pEvent = NULL;

	for( int i=0; i<GRADE_MAX; ++i )
	{
		m_pGrade[i] = NULL;
	}

	for( int i = 0; i < RANK_MAX; ++i )
	{
		m_pRank[i] = NULL;
	}

	m_iState = STATE_IDENTIFING;

	m_bReappraisal = false;

	m_dwRollingSoundID = -1;

	m_bReserve = false;

	m_bShowMultiple = false;
	m_bShow3DEffect = false;

	ClearResultData();
}

ExcavationResultWnd::~ExcavationResultWnd()
{
	SAFEDELETE( m_pIconBG );
	SAFEDELETE( m_pIconBGRolling );
	SAFEDELETE( m_pRankEffect );
	SAFEDELETE( m_pMultiple2 );
	SAFEDELETE( m_pMultiple3 );
	SAFEDELETE( m_pMultiple4 );
	SAFEDELETE( m_pEvent );

	for( int i=0; i<GRADE_MAX; ++i )
	{
		SAFEDELETE( m_pGrade[i] );
	}

	for( int i = 0; i < RANK_MAX; ++i )
	{
		m_pRank[i] = NULL;
	}

	m_pIcon = NULL;
}

void ExcavationResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBG );
		m_pIconBG = pImage;
	}
	else if( szType == "IconBackRolling" )
	{
		SAFEDELETE( m_pIconBGRolling );
		m_pIconBGRolling = pImage;
	}
	else if( szType == "Grade_SSS" )
	{
		SAFEDELETE( m_pGrade[GRADE_SSS] );
		m_pGrade[GRADE_SSS] = pImage;
	}
	else if( szType == "Grade_SS" )
	{
		SAFEDELETE( m_pGrade[GRADE_SS] );
		m_pGrade[GRADE_SS] = pImage;
	}
	else if( szType == "Grade_S" )
	{
		SAFEDELETE( m_pGrade[GRADE_S] );
		m_pGrade[GRADE_S] = pImage;
	}
	else if( szType == "Grade_A" )
	{
		SAFEDELETE( m_pGrade[GRADE_A] );
		m_pGrade[GRADE_A] = pImage;
	}
	else if( szType == "Grade_B" )
	{
		SAFEDELETE( m_pGrade[GRADE_B] );
		m_pGrade[GRADE_B] = pImage;
	}
	else if( szType == "Grade_C" )
	{
		SAFEDELETE( m_pGrade[GRADE_C] );
		m_pGrade[GRADE_C] = pImage;
	}
	else if( szType == "Grade_D" )
	{
		SAFEDELETE( m_pGrade[GRADE_D] );
		m_pGrade[GRADE_D] = pImage;
	}
	else if( szType == "Grade_E" )
	{
		SAFEDELETE( m_pGrade[GRADE_E] );
		m_pGrade[GRADE_E] = pImage;
	}
	else if( szType == "Grade_F" )
	{
		SAFEDELETE( m_pGrade[GRADE_F] );
		m_pGrade[GRADE_F] = pImage;
	}
	else if( szType == "Rank_Blue" )
	{
		SAFEDELETE( m_pRank[RANK_BLUE] );
		m_pRank[RANK_BLUE] = pImage;
	}
	else if( szType == "Rank_Yellow" )
	{
		SAFEDELETE( m_pRank[RANK_YELLOW] );
		m_pRank[RANK_YELLOW] = pImage;
	}
	else if( szType == "Rank_Red" )
	{
		SAFEDELETE( m_pRank[RANK_RED] );
		m_pRank[RANK_RED] = pImage;
	}
	else if( szType == "Multiple2" )
	{
		SAFEDELETE( m_pMultiple2 );
		m_pMultiple2 = pImage;
	}
	else if( szType == "Multiple3" )
	{
		SAFEDELETE( m_pMultiple3 );
		m_pMultiple3 = pImage;
	}
	else if( szType == "Multiple4" )
	{
		SAFEDELETE( m_pMultiple4 );
		m_pMultiple4 = pImage;
	}
	else if( szType == "Event" )
	{
		SAFEDELETE( m_pEvent );
		m_pEvent = pImage;
	}
	else
		ioMovingWnd::AddRenderImage( szType, pImage );
}

void ExcavationResultWnd::AddEffect( const ioHashString &szType, ioUI3DEffectRender *pUI3DRender )
{
	if( szType == "RankEffect" )
	{
		SAFEDELETE( m_pRankEffect );
		m_pRankEffect = pUI3DRender;
	}
	else
		ioMovingWnd::AddEffect( szType, pUI3DRender );
}

void ExcavationResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_Text[TEXT_PRICE_ROLLING] = xElement.GetStringAttribute_e( "TextPriceRolling" );
	m_Text[TEXT_PRICE]         = xElement.GetStringAttribute_e( "TextPrice" );
	m_Text[TEXT_ETCITEM_DESC]  = xElement.GetStringAttribute_e( "TextEtcItemDesc" );
	m_Text[TEXT_AUTO_CLOSE]    = xElement.GetStringAttribute_e( "TextAutoClose" );
	m_Text[TEXT_PRICE1_BUTTON] = xElement.GetStringAttribute_e( "TextPrice1Button" );
	m_Text[TEXT_PRICE2_BUTTON] = xElement.GetStringAttribute_e( "TextPrice2Button" );
	m_Text[TEXT_PRICE3_BUTTON] = xElement.GetStringAttribute_e( "TextPrice3Button" );
	
	m_dwRollingTime       = xElement.GetIntAttribute_e( "RollingTime" );
	m_dwRollingChangeTime = xElement.GetIntAttribute_e( "RollingChangeTime" );
	m_dwWindowCloseTime   = 30000;

	m_dwNormalGradeEventTime     = xElement.GetIntAttribute_e( "NormalGradeEventTime" );
	m_dwHighGradeEventTime       = xElement.GetIntAttribute_e( "HighGradeEventTime" );
	m_dwEventDelayTimeToMulti    = xElement.GetIntAttribute_e( "EventDelayTimeToMultiple" );
	m_dwEventDelayTimeTo3DEffect = xElement.GetIntAttribute_e( "EventDelayTimeTo3DEffect" );
	m_dwMultipleEventTime        = xElement.GetIntAttribute_e( "MultipleEventTime" );

	m_dwTwinkleTime = xElement.GetIntAttribute_e( "ResultEffectTwinkleTime" );

	m_szRollingSound = xElement.GetStringAttribute_e( "RollingSound" );
	m_szRollingStopSound = xElement.GetStringAttribute_e( "RollingStopSound" );
	m_szMultipleSound = xElement.GetStringAttribute_e( "MultipleSound" );
}

void ExcavationResultWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /* = false */ )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	POINT ptClientSize;
	g_App.GetClientSize( ptClientSize );
	int iXPos = ( ptClientSize.x / 2 ) - GetWidth() / 2;
	int iYPos = ( ptClientSize.y / 2 ) - GetHeight() / 2 - 30;
	SetWndPos( iXPos, iYPos );
}

void ExcavationResultWnd::iwm_show()
{
	if( g_ExcavationMgr.GetArtifactCount() == 0 )
	{
		HideWnd();
		return;
	}

	if( !m_bReserve )
	{
		HideWnd();
		return;
	}

	Reset();

	m_dwRollingSoundID = g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	m_bReserve = false;
}

void ExcavationResultWnd::iwm_hide()
{
	if( m_dwRollingSoundID != -1 )
	{
		g_SoundMgr.StopSound( m_szRollingSound, m_dwRollingSoundID );
	}

	ClearResultData();
}

bool ExcavationResultWnd::iwm_spacebar()
{
	if( g_App.IsMouseBusy() )
		return false;

	ioWnd *pWnd = FindChildWnd( ID_OK );
	if( pWnd )
	{
		if( !pWnd->HasWndStyle( IWS_INACTIVE ) )
		{
			if( m_iResultItemType > 0 )
			{
				g_ExcavationMgr.SendIdentifyComplete();
			}
			return true;
		}
	}
	
	return false;
}

void ExcavationResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if(!pWnd) return;

	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_IDENTIFY:
		if( cmd == IOBN_BTNUP )
		{
			g_ExcavationMgr.SendIdentify();
		}
		break;
	case ID_OK:
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			if( m_iResultItemType > 0 )
			{
				g_ExcavationMgr.SendIdentifyComplete();
			}
		}
		break;
	}
}

void ExcavationResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	char szBuf[MAX_PATH] = "";

	int iCloseTime = 0;

	if( m_iState == STATE_IDENTIFING )
	{
		if( m_dwRollingEndTime < REALGETTIME() )
		{
			g_SoundMgr.StopSound( m_szRollingSound, m_dwRollingSoundID );
			g_SoundMgr.PlaySound( m_szRollingStopSound );
			m_dwRollingSoundID = -1;

			m_iState = STATE_IDENTIFY_COMPLETE;

			CheckButton();

			SetArtifactInfo();

			m_dwCloseWndTime = REALGETTIME() + m_dwWindowCloseTime;
			m_dwEventCheckTime = FRAMEGETTIME();

			int iGrade = m_iResultGradeIdx-1;
			if( COMPARE( iGrade, GRADE_SSS, GRADE_MAX ) )
				m_pResultGrade = m_pGrade[iGrade];
			else
				m_pResultGrade = NULL;

			SetRankIcon( iGrade );

			ExcavationWnd* pExcavationWnd = dynamic_cast<ExcavationWnd*>( g_GUIMgr.FindWnd( EXCAVATION_WND ) );
			if( pExcavationWnd )
			{
				if( pExcavationWnd->IsProcessDelay() )
				{
					pExcavationWnd->SetProcessDelay( false );
				}

				if( pExcavationWnd->IsLevelUpDelay() )
				{
					pExcavationWnd->SetLevelUpDelay( false );

					g_GUIMgr.ShowWnd( EXCAVATION_LEVELUP_WND );
				}
			}

			return;
		}

		if( m_dwRollingCheckTime < REALGETTIME() )
		{
			m_dwRollingCheckTime = REALGETTIME() + m_dwRollingChangeTime;

			SetArtifactInfo();
			
			m_iRollingIndex++;
			if( m_iRollingIndex >= g_ExcavationMgr.GetArtifactCount() )
				m_iRollingIndex = 0;

			m_pResultGrade = m_pGrade[m_iRollingGrade];
			m_iRollingGrade++;
			if( m_iRollingGrade >= GRADE_MAX )
				m_iRollingGrade = GRADE_SSS;

			SetRankIcon( m_iRollingGrade );
		}
	}
	else if( m_iState == STATE_IDENTIFY_COMPLETE )
	{
		iCloseTime = ( m_dwCloseWndTime - REALGETTIME() ) / 1000;
		if( m_dwCloseWndTime < REALGETTIME() )
		{
			if( m_iResultItemType > 0 )
			{
				g_ExcavationMgr.SendIdentifyComplete();
			}

			iCloseTime = 0;
		}

		ProcessGradeEvent();
		ProcessEventDelay();
		ProcessMultipleEvent();
		Process3DEffectEvent( fTimePerSec );
	}

	/*iCloseTime = max( 0, iCloseTime );
	wsprintf( szBuf, m_Text[TEXT_AUTO_CLOSE].c_str(), iCloseTime );
	m_pCloseTime.Set( iXPos + 177, iYPos + 239, szBuf, FONT_SIZE_13, -1, TS_NORMAL, TCT_DEFAULT_WHITE, TCT_DEFAULT_BLACK, TAT_CENTER, TVA_TOP );*/
	iCloseTime = max( 0, iCloseTime );
	m_PopupClosePrinter.ClearList();

	m_PopupClosePrinter.SetTextStyle( TS_NORMAL );
	m_PopupClosePrinter.SetTextColor( TCT_DEFAULT_GRAY );
	m_PopupClosePrinter.AddTextPiece( FONT_SIZE_13, m_Text[TEXT_AUTO_CLOSE].c_str() );
	m_PopupClosePrinter.SetTextColor( TCT_DEFAULT_BLUE );
	m_PopupClosePrinter.AddTextPiece( FONT_SIZE_13, "%d", iCloseTime );
}

void ExcavationResultWnd::StartEventDelay()
{
	m_dwEventCheckTime = FRAMEGETTIME();
}

void ExcavationResultWnd::StartMultipleEvent()
{
	if( m_iResultCount <= 1 )
		return;

	m_bShowMultiple = true;
	m_dwEventCheckTime = FRAMEGETTIME();

	//g_SoundMgr.PlaySound( m_szMultipleSound );
}

void ExcavationResultWnd::Start3DEffectEvent()
{
	if( m_iResultGradeIdx > g_ExcavationMgr.GetEffectGrade() )
		return;

	if( m_iResultItemType == ioExcavationManager::RESULT_ITEM )
		return;

	m_bShow3DEffect = true;

	if( m_pRankEffect )
		m_pRankEffect->Start();
}

void ExcavationResultWnd::OnRender()
{
	ioMovingWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	switch( m_iState )
	{
	case STATE_IDENTIFING:
		{
			if( m_pIconBGRolling )
				m_pIconBGRolling->Render( iXPos + 178, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		break;
		
	case STATE_IDENTIFY_COMPLETE:
		{
			if( m_pIconBG )
				m_pIconBG->Render( iXPos + 178, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		break;
	}	

	if( m_pIcon )
	{
		m_pIcon->SetAlpha( MAX_ALPHA_RATE );
		m_pIcon->Render( iXPos + 178, iYPos + 98 );
	}

	RenderGradeEvent( iXPos + 177, iYPos + 121 );
	RenderMultipleEvent( iXPos + 249, iYPos + 102 );
	Render3DEffectEvent( iXPos + 178, iYPos + 115 );

	m_PopupClosePrinter.PrintFullText( iXPos + 177, iYPos + 239, TAT_CENTER );
}

void ExcavationResultWnd::ProcessGradeEvent()
{
	if( m_iState == STATE_IDENTIFING )
		return;

	if( m_bEndGradeEvent )
	{
		m_fGradeEventRate = 0.0f;

		if( m_dwTwinkleCheckTime + m_dwTwinkleTime < FRAMEGETTIME() )
		{
			m_bTwinkle = !m_bTwinkle;
			m_dwTwinkleCheckTime = FRAMEGETTIME();
		}
	}
	else
	{
		bool bEnd = false;
		if( m_iResultGradeIdx > g_ExcavationMgr.GetEffectGrade() )
		{
			m_fGradeEventRate = Help::GetAniRateByTime( FRAMEGETTIME() - m_dwEventCheckTime, m_dwNormalGradeEventTime, Help::RATE_REVERSE );
			if( m_dwEventCheckTime + m_dwNormalGradeEventTime < FRAMEGETTIME() )
			{
				bEnd = true;
			}
		}
		else
		{
			m_fGradeEventRate = Help::GetAniRateByTime( FRAMEGETTIME() - m_dwEventCheckTime, m_dwHighGradeEventTime, Help::RATE_REVERSE );
			if( m_dwEventCheckTime + m_dwHighGradeEventTime < FRAMEGETTIME() )
			{
				bEnd = true;
			}
		}

		if( bEnd )
		{
			StartEventDelay();
			m_bEndGradeEvent = true;
			m_dwTwinkleCheckTime = FRAMEGETTIME();
		}
	}
}

void ExcavationResultWnd::ProcessEventDelay()
{
	if( !m_bEndGradeEvent )
		return;

	if( !m_bReappraisal && !m_bShowMultiple && m_dwEventCheckTime + m_dwEventDelayTimeToMulti < FRAMEGETTIME() )
	{
		StartMultipleEvent();
	}

	if( m_iResultGradeIdx <= g_ExcavationMgr.GetEffectGrade() )
	{
		if( !m_bShow3DEffect && m_dwEventCheckTime + m_dwEventDelayTimeTo3DEffect < FRAMEGETTIME() )
		{
			Start3DEffectEvent();
		}
	}
}

void ExcavationResultWnd::ProcessMultipleEvent()
{
	if( !m_bShowMultiple )
		return;

	if( m_iResultCount <= 1 )
		return;

	if( m_bReappraisal )
	{
		m_fMultipleEventRate = 0.0f;
		return;
	}

	if( m_dwEventCheckTime + m_dwMultipleEventTime < FRAMEGETTIME() )
	{
		m_fMultipleEventRate = 0.0f;
		return;
	}

	m_fMultipleEventRate = Help::GetAniRateByTime( FRAMEGETTIME() - m_dwEventCheckTime, m_dwMultipleEventTime, Help::RATE_REVERSE );
}

void ExcavationResultWnd::Process3DEffectEvent( float fTimePerSec )
{
	if( !m_bShow3DEffect )
		return;

	if( m_iResultItemType == ioExcavationManager::RESULT_ITEM )
		return;

	if( m_iResultGradeIdx > g_ExcavationMgr.GetEffectGrade() )
		return;

	if( m_pRankEffect )
		m_pRankEffect->UpdateEffectList( fTimePerSec * 1000 );
}

void ExcavationResultWnd::RenderGradeEvent( int iXPos, int iYPos )
{
	if( m_iResultItemType == ioExcavationManager::RESULT_ITEM && m_iState == STATE_IDENTIFY_COMPLETE )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetVertAlign( TVA_TOP );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos, iYPos + 98, FONT_SIZE_13, m_Text[TEXT_ETCITEM_DESC].c_str() );
		return;
	}
	
	if( m_iResultItemType != ioExcavationManager::RESULT_ARTIFACT )
		return;

	if( !m_pResultGrade || !m_pResultRank )
		return;

	if( m_iState == STATE_IDENTIFING )
	{
		m_pResultGrade->SetAlpha( MAX_ALPHA_RATE );
		m_pResultGrade->SetScale( 1.0f );
		m_pResultGrade->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		m_pResultRank->SetAlpha( MAX_ALPHA_RATE );
		m_pResultRank->SetScale( 1.0f );
		m_pResultRank->Render( iXPos, iYPos + 31, UI_RENDER_NORMAL );
		return;
	}

	if( m_iResultGradeIdx <= g_ExcavationMgr.GetEffectGrade() )
	{
		float fScale = m_fGradeEventRate * 4.0f + 1.0f;
		float fAlpha = 1.0f - m_fGradeEventRate;

		m_pResultGrade->SetAlpha( MAX_ALPHA_RATE * fAlpha );
		m_pResultGrade->SetScale( fScale );
		m_pResultGrade->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		m_pResultRank->SetAlpha( MAX_ALPHA_RATE * fAlpha );
		m_pResultRank->SetScale( fScale );
		m_pResultRank->Render( iXPos, iYPos + 31, UI_RENDER_NORMAL );

		if( m_bEndGradeEvent && m_bTwinkle )
		{
			m_pResultGrade->SetAlpha( MAX_ALPHA_RATE * fAlpha );
			m_pResultGrade->SetScale( fScale );
			m_pResultGrade->Render( iXPos, iYPos, UI_RENDER_SCREEN );
			m_pResultRank->SetAlpha( MAX_ALPHA_RATE * fAlpha );
			m_pResultRank->SetScale( fScale );
			m_pResultRank->Render( iXPos, iYPos + 31, UI_RENDER_SCREEN );
		}
	}
	else
	{
		m_pResultGrade->SetAlpha( MAX_ALPHA_RATE );
		m_pResultGrade->SetScale( 1.0f );
		m_pResultGrade->Render( iXPos, iYPos, UI_RENDER_NORMAL );
		m_pResultRank->SetAlpha( MAX_ALPHA_RATE );
		m_pResultRank->SetScale( 1.0f );
		m_pResultRank->Render( iXPos, iYPos + 31, UI_RENDER_NORMAL );

		if( m_fGradeEventRate > 0.0f )
		{
			m_pResultGrade->SetColor( TCT_DEFAULT_WHITE );
			m_pResultGrade->SetAlpha( MAX_ALPHA_RATE * m_fGradeEventRate );
			m_pResultGrade->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
			m_pResultRank->SetColor( TCT_DEFAULT_WHITE );
			m_pResultRank->SetAlpha( MAX_ALPHA_RATE * m_fGradeEventRate );
			m_pResultRank->Render( iXPos, iYPos + 31, UI_RENDER_NORMAL );
		}
	}
}

void ExcavationResultWnd::RenderMultipleEvent( int iXPos, int iYPos )
{
	if( !m_bShowMultiple )
		return;

	if( m_iResultCount <= 1 )
		return;

	float fScale = m_fMultipleEventRate * 4.0f + 1.0f;
	float fAlpha = 1.0f - m_fMultipleEventRate;

	ioUIRenderImage *pMultiple = NULL;
	if( m_iResultCount == 2 )
	{
		pMultiple = m_pMultiple2;
	}
	else if( m_iResultCount == 3 )
	{
		pMultiple = m_pMultiple3;
	}
	else if( m_iResultCount == 4 )
	{
		pMultiple = m_pMultiple4;
	}

	if( pMultiple )
	{
		pMultiple->SetAlpha( MAX_ALPHA_RATE * fAlpha );
		pMultiple->SetScale( fScale );
		pMultiple->Render( iXPos, iYPos, UI_RENDER_NORMAL );

		if( m_fMultipleEventRate <= 0.0f && m_bTwinkle )
		{
			pMultiple->Render( iXPos, iYPos, UI_RENDER_SCREEN );
		}
	}
}

void ExcavationResultWnd::Render3DEffectEvent( int iXPos, int iYPos )
{
	if( !m_bShow3DEffect )
		return;

	if( m_iResultItemType == ioExcavationManager::RESULT_ITEM )
		return;

	if( m_iResultGradeIdx > g_ExcavationMgr.GetEffectGrade() )
		return;

	if( m_pRankEffect )
		m_pRankEffect->Render( iXPos, iYPos );
}

void ExcavationResultWnd::SetArtifactInfo()
{
	LSC_Excavation_info *pInfo = NULL;
	LSC_Excavation_grade *pGrade = NULL;

	if( m_iState == STATE_IDENTIFING )
	{
		if( m_bReappraisal )
			pInfo  = g_ExcavationMgr.GetArtifactInfo( m_iResultArtifactIdx );
		else
			pInfo  = g_ExcavationMgr.GetArtifactInfoByArray( m_iRollingIndex );
	}
	else if( m_iState == STATE_IDENTIFY_COMPLETE )
	{
		pInfo  = g_ExcavationMgr.GetArtifactInfo( m_iResultArtifactIdx );
		pGrade = g_ExcavationMgr.GetGradeInfo( m_iResultGradeIdx );
	}

	if( pInfo )
	{
		m_pIcon = g_ExcavationMgr.GetRewardIcon( pInfo->ItemIndex );

		ioHashString szItemName;
		g_ExcavationMgr.GetResultItemName( pInfo, szItemName );

		IoUIText *pText = GetText( "item_name" );
		if( pText )
			pText->SetText( szItemName.c_str() );

		if( pInfo->ItemType == ioExcavationManager::RESULT_ARTIFACT )
		{
			pText->SetOffsetY( 167 );

			pText = GetText( "item_count" );
			if( pText )
				pText->SetVisible( false );
		}
		else
		{
			pText->SetOffsetY( 144 );

			pText = GetText( "item_count" );
			if( pText )
			{
				pText->SetVisible( true );

				ioHashString szItemValue;
				g_ExcavationMgr.GetResultItemValue( pInfo, szItemValue, m_iResultPrice );

				pText->SetText( szItemValue.c_str() );
			}
		}

		pText = GetText( "desc" );
		if( pText )
			pText->SetText( GET_STRING(pInfo->ItemInfo) );

		char szBuf[MAX_PATH] = "";
		if( m_iState == STATE_IDENTIFING )
		{
			pText = GetText( "price" );
			if( pText )
				pText->SetText( m_Text[TEXT_PRICE_ROLLING].c_str() );
		}
		else if( m_iState == STATE_IDENTIFY_COMPLETE )
		{
			if( m_iResultItemType == ioExcavationManager::RESULT_ITEM )
			{
				pText = GetText( "price" );
				if( pText )
					pText->SetVisible( false );
			}
			else
			{
				pText = GetText( "price" );
				if( pText )
				{
					char szConvertNum[MAX_PATH] = "";
					Help::ConvertNumToStrComma( m_iResultPrice, szConvertNum, MAX_PATH );
					wsprintf( szBuf, m_Text[TEXT_PRICE].c_str(), szConvertNum );
					pText->SetVisible( true );
					pText->SetText( szBuf );
				}
			}
		}
	}
}

void ExcavationResultWnd::SetReserveResultItem( int iItemType, int iArtifactIdx, int iGradeIdx, int iPrice, int iIdentifyPrice, int iGetCnt )
{
	m_bReserve = true;

	m_iResultItemType      = iItemType;
	m_iResultArtifactIdx   = iArtifactIdx;
	m_iResultGradeIdx      = iGradeIdx;
	m_iResultPrice         = iPrice;
	m_iResultIdentifyPrice = iIdentifyPrice;
	m_iResultCount         = iGetCnt;

	if( m_iResultItemType == ioExcavationManager::RESULT_ITEM )
	{
		m_iResultGradeIdx      = 0;
		//m_iResultPrice         = 0;
		m_iResultIdentifyPrice = 0;
	}
	SetButtonPrice();
}

void ExcavationResultWnd::SetReappraisal( int iItemType, int iArtifactIdx, int iGradeIdx, int iPrice, int iIdentifyPrice, int iGetCnt )
{
	if( m_dwRollingSoundID != -1 )
		g_SoundMgr.StopSound( m_szRollingSound, m_dwRollingSoundID );

	m_dwRollingSoundID = g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	SetReserveResultItem( iItemType, iArtifactIdx, iGradeIdx, iPrice, iIdentifyPrice, iGetCnt );
	Reset();
	
	m_bReappraisal = true;

	if( m_iResultCount > 1 )
		m_bShowMultiple = true;

	if( !IsShow() )
	{
		ShowWnd();
	}
}

void ExcavationResultWnd::Reset()
{
	m_iState = STATE_IDENTIFING;
	m_iRollingIndex = 0;
	m_iRollingGrade = GRADE_SSS;
	m_dwRollingEndTime = REALGETTIME() + m_dwRollingTime;
	m_dwRollingCheckTime = REALGETTIME() + m_dwRollingChangeTime;
	m_dwCloseWndTime = 0;

	m_bEndGradeEvent = false;
	m_bEndEventDelay = false;
	m_bShowMultiple = false;
	m_bShow3DEffect = false;

	m_bReappraisal = false;

	m_dwEventCheckTime = 0;;

	SetChildInActive( ID_IDENTIFY );
	SetChildInActive( ID_OK );

	POINT ptClientSize;
	g_App.GetClientSize( ptClientSize );
	int iXPos = ( ptClientSize.x / 2 ) - GetWidth() / 2;
	int iYPos = ( ptClientSize.y / 2 ) - GetHeight() / 2 - 30;
	SetWndPos( iXPos, iYPos );
}

void ExcavationResultWnd::ClearResultData()
{
	m_iResultItemType = 0;
	m_iResultGradeIdx = 0;
	m_iResultArtifactIdx = 0;
	m_iResultPrice = 0;
	m_iResultIdentifyPrice = 0;
	m_iResultCount = 0;
}
void ExcavationResultWnd::CheckButton()
{
	if( m_iResultItemType == ioExcavationManager::RESULT_ITEM )
	{
		SetChildInActive( ID_IDENTIFY );
		SetChildActive( ID_OK );
	}
	else if( m_iResultIdentifyPrice > g_MyInfo.GetMoney() )
	{
		SetChildInActive( ID_IDENTIFY );
		SetChildActive( ID_OK );
	}
	else if( m_iResultGradeIdx-1 == GRADE_SSS )
	{
		SetChildInActive( ID_IDENTIFY );
		SetChildActive( ID_OK );
	}
	else
	{
		SetChildActive( ID_IDENTIFY );
		SetChildActive( ID_OK );
	}
}

void ExcavationResultWnd::SetButtonPrice()
{
	ioButton *pBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_IDENTIFY ) );
	if( pBtn )
	{
		char szBuf[MAX_PATH] = "";
		if( m_iResultGradeIdx-1 == GRADE_SSS )
		{
			wsprintf( szBuf, "%s", m_Text[TEXT_PRICE2_BUTTON].c_str() );
		}
		else
		{
			if( m_iResultItemType == ioExcavationManager::RESULT_ARTIFACT )
			{
				if( m_iResultIdentifyPrice <= g_MyInfo.GetMoney() )
				{
					char szConvertNum[MAX_PATH] = "";
					Help::ConvertNumToStrComma( m_iResultIdentifyPrice, szConvertNum, MAX_PATH );
					wsprintf( szBuf, m_Text[TEXT_PRICE1_BUTTON].c_str(), szConvertNum );
				}
				else
				{
					wsprintf( szBuf, "%s", m_Text[TEXT_PRICE3_BUTTON].c_str() );
				}
			}
			else
			{
				wsprintf( szBuf, "%s", m_Text[TEXT_PRICE2_BUTTON].c_str() );
			}
		}

		pBtn->SetTitleText( szBuf );
	}
}

void ExcavationResultWnd::SetRankIcon( int iGradeIndex )
{
	if( COMPARE( iGradeIndex, GRADE_SSS, GRADE_A ) )
	{
		m_pResultRank = m_pRank[RANK_BLUE];
	}
	else if( COMPARE( iGradeIndex, GRADE_A, GRADE_E ) )
	{
		m_pResultRank = m_pRank[RANK_YELLOW];
	}
	else if( COMPARE( iGradeIndex, GRADE_E, GRADE_MAX ) )
	{
		m_pResultRank = m_pRank[RANK_RED];
	}
	else
	{
		m_pResultRank = NULL;
	}
}