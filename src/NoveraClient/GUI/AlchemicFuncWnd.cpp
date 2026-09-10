
#include "StdAfx.h"

#include "AlchemicFuncWnd.h"
#include "MyItemSelecteWnd.h"

#include "../ioUserSpiritManager.h"
#include "../ioUserSpiritInven.h"

SpiritFuncWnd::SpiritFuncWnd(void)
{
	m_pComposeBG = NULL;
	m_pConversionBG = NULL;
	m_pStripDark = NULL;
	m_pNumText  = NULL;
	m_pEffect = NULL;
	m_pBGEffect[0] = NULL;
	m_pBGEffect[1] = NULL;

	m_iSpecialSpiritCode = 0;
	m_iSpecialSpiritCount = 0;
	m_iTabState = 0;
	m_iProcessCount = 0;
}

SpiritFuncWnd::~SpiritFuncWnd(void)
{
	SAFEDELETE( m_pComposeBG );
	SAFEDELETE( m_pConversionBG );
	SAFEDELETE( m_pStripDark );
	SAFEDELETE( m_pNumText );
	SAFEDELETE( m_pBGEffect[0] );
	SAFEDELETE( m_pBGEffect[1] );
}

void SpiritFuncWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ComposeBG" )
	{
		SAFEDELETE( m_pComposeBG );
		m_pComposeBG = pImage;
	}
	else if( szType == "ConversionBG" )
	{
		SAFEDELETE( m_pConversionBG );
		m_pConversionBG = pImage;
	}
	else if( szType == "strip_gray_dark" )
	{
		SAFEDELETE( m_pStripDark );
		m_pStripDark = pImage;
		m_pStripDark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pStripDark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pStripDark->SetSize( 205, 21 );
	}
	else if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else if( szType == "BGEffect1" )
	{
		SAFEDELETE( m_pBGEffect[0] );
		m_pBGEffect[0] = pImage;
	}
	else if( szType == "BGEffect2" )
	{
		SAFEDELETE( m_pBGEffect[1] );
		m_pBGEffect[1] = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void SpiritFuncWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szComposeText = xElement.GetStringAttribute_e( "ComposeText" );
	m_szConversionText = xElement.GetStringAttribute_e( "ConversionText" );
	m_iSpecialSpiritCode = xElement.GetIntAttribute_e( "SpecialSpiritCode" );

	m_szComposeAlarm[0] = xElement.GetStringAttribute_e( "ComposeAlarm1" );
	m_szComposeAlarm[1] = xElement.GetStringAttribute_e( "ComposeAlarm2" );
	m_szSendComposeError[0] = xElement.GetStringAttribute_e( "SendComposeError1" );
	m_szSendComposeError[1] = xElement.GetStringAttribute_e( "SendComposeError2" );
	m_szConversionAlarm = xElement.GetStringAttribute_e( "ConversionAlarm" );
	m_szNoSpiritAlarm = xElement.GetStringAttribute_e( "NoSpiritAlarm" );
	m_szSendConversionError[0] = xElement.GetStringAttribute_e( "SendConversionError1" );
	m_szSendConversionError[1] = xElement.GetStringAttribute_e( "SendConversionError2" );
	m_szSpiritCountText[0] = xElement.GetStringAttribute_e( "SpiritCountText1" );
	m_szSpiritCountText[1] = xElement.GetStringAttribute_e( "SpiritCountText2" );

	m_szComposeRollingSound = xElement.GetStringAttribute_e( "ComposeRollingSound" );
	m_szComposeResultSound = xElement.GetStringAttribute_e( "ComposeResultSound" );
	m_szConversionRollingSound = xElement.GetStringAttribute_e( "ConversionRollingSound" );
	m_szConversionResultSound = xElement.GetStringAttribute_e( "ConversionResultSound" );

	m_dwProcessGapTime = (DWORD)xElement.GetIntAttribute_e( "ProcessGapTime" );
	m_dwProcessFirstGapTime = (DWORD)xElement.GetIntAttribute_e( "ProcessFirstGapTime" );

	m_dwRollingTime = xElement.GetIntAttribute_e( "RollingTime" );

	char szBuf[MAX_PATH];
	for( int i = 0; i < 4; ++i )
	{
		wsprintf_e( szBuf, "ComposeStateMsg%d", i + 1 );
		m_szComposeStateMsg[i] = xElement.GetStringAttribute( szBuf );

		wsprintf_e( szBuf, "ConversionStateMsg%d", i + 1 );
		m_szConversionStateMsg[i] = xElement.GetStringAttribute( szBuf );
	}
}

void SpiritFuncWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iBGEffectY = 202;
	float fBaseScale = 1.f;
	if( m_iTabState != eCOMPOSE )
	{
		fBaseScale = 0.8f;
		iBGEffectY = 173;
	}
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwTimeGap = dwCurTime - m_dwTabChangeTime;
	
	int iEffect1Alpha = MAX_ALPHA_RATE;
	float fEffect1Scale = fBaseScale;
	if( dwTimeGap < BG_EFFECT_1_TIME )
	{
		float fRate = (float)dwTimeGap / BG_EFFECT_1_TIME_GAP;

		fEffect1Scale = fBaseScale - 0.2f + 0.05f * fRate;
		iEffect1Alpha = MAX_ALPHA_RATE * 0.2f * ( 1.f + fRate );
	}
	else
	{
		float fEffect2Scale = fBaseScale;
		int iEffect2Alpha = MAX_ALPHA_RATE;
		if( dwTimeGap < BG_EFFECT_2_TIME )
		{
			float fRate = (float)( dwTimeGap - BG_EFFECT_1_TIME ) / BG_EFFECT_2_TIME_GAP;

			fEffect2Scale = fBaseScale + 0.05f * fRate;
			iEffect2Alpha = MAX_ALPHA_RATE * ( 1.f - 0.2f * fRate );
		}
		else
		{
			m_dwTabChangeTime = dwCurTime - BG_EFFECT_1_TIME;
		}
		if( m_pBGEffect[1] )
		{
			m_pBGEffect[1]->SetScale( fEffect2Scale );
			m_pBGEffect[1]->SetAlpha( iEffect2Alpha );
			m_pBGEffect[1]->Render( iXPos + 128, iYPos + iBGEffectY, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	if( m_pBGEffect[0] )
	{
		m_pBGEffect[0]->SetScale( fEffect1Scale );
		m_pBGEffect[0]->SetAlpha( iEffect1Alpha );
		m_pBGEffect[0]->Render( iXPos + 128, iYPos + iBGEffectY, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	ioWnd::OnRender();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	if( m_iFuncState == eReady || m_iFuncState == eNot )
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );			
	else if( m_iFuncState == eSet || m_iFuncState == eSendPacket )
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

	if( m_iTabState == eCOMPOSE )
	{
		if( m_pComposeBG )
			m_pComposeBG->Render( iXPos + 49, iYPos + 99 );
		if( m_pStripDark )
			m_pStripDark->Render( iXPos + 27, iYPos + 313, UI_RENDER_MULTIPLY );
		if( COMPARE( m_iFuncState, eReady, eSendPacket + 1 ) )
			g_FontMgr.PrintText( iXPos + 129, iYPos + 317, FONT_SIZE_12, m_szComposeStateMsg[m_iFuncState].c_str() );

		if( m_iSpiritCode != 0 )
		{
			char szBuf[MAX_PATH];
			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( m_iSpiritCount, szConvertNum, sizeof( szConvertNum ) );
			wsprintf( szBuf, m_szSpiritCountText[0].c_str(), szConvertNum );

			g_FontMgr.SetAlignType( TAT_RIGHT );	
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 227, iYPos + 107, FONT_SIZE_13, szBuf );

			Help::ConvertNumToStrComma( m_iSpiritMaxCount, szConvertNum, sizeof( szConvertNum ) );
			wsprintf( szBuf, m_szSpiritCountText[1].c_str(), szConvertNum );
			g_FontMgr.PrintText( iXPos + 227, iYPos + 125, FONT_SIZE_11, szBuf );
		}
		IoString str;
		str.Set(0, 0, m_szComposeText.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
		str.OnRender( iXPos + 27, iYPos + 349 );
	}
	else
	{
		if( m_pConversionBG )
			m_pConversionBG->Render( iXPos + 29, iYPos + 98 );
		if( m_pStripDark )
			m_pStripDark->Render( iXPos + 27, iYPos + 259, UI_RENDER_MULTIPLY );
		if( COMPARE( m_iFuncState, eReady, eSendPacket + 1 ) )
			g_FontMgr.PrintText( iXPos + 129, iYPos + 263, FONT_SIZE_12, m_szConversionStateMsg[m_iFuncState].c_str() );
	
		IoString str;
		str.Set(0, 0, m_szConversionText.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
		str.OnRender( iXPos + 27, iYPos + 295 );
	}
}

void SpiritFuncWnd::OnRenderAfterChild()
{
	enum
	{
		NUM_SIZE	= 20,
		NUM_GAP		= 4,
	};
	if( !m_pNumText )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_iTabState == eCOMPOSE )
	{
		if( m_iFuncState == eNot || m_iFuncState == eSet )
		{
			int iCurXPos = iXPos + 97;
			int iTotalSize = 0;
			if( m_iCurSpiritCount < 10 )
				iTotalSize = NUM_SIZE;
			else if( m_iCurSpiritCount < 100 )
				iTotalSize = NUM_SIZE*2 - NUM_GAP;
			else
				iTotalSize = NUM_SIZE*3 - NUM_GAP*2;

			iCurXPos -= (iTotalSize / 2);
			m_pNumText->RenderNum( iCurXPos, iYPos + 164, m_iCurSpiritCount, -NUM_GAP );

			iCurXPos = iXPos + 177;
			iTotalSize = 0;
			int iComposeCount = min( m_iSpiritMaxCount - m_iCurSpiritCount, m_iSpecialSpiritCount );
			if( iComposeCount < 10 )
				iTotalSize = NUM_SIZE;
			else if( iComposeCount < 100 )
				iTotalSize = NUM_SIZE*2 - NUM_GAP;
			else
				iTotalSize = NUM_SIZE*3 - NUM_GAP*2;

			iCurXPos -= (iTotalSize / 2);
			m_pNumText->RenderNum( iCurXPos, iYPos + 210, iComposeCount, -NUM_GAP );
		}
	}
	else
	{
		if( m_iFuncState == eNot || m_iFuncState == eSet )
		{
			int iCurXPos = iXPos + 74;
			int iTotalSize = 0;
			if( m_iCurSpiritCount < 10 )
				iTotalSize = NUM_SIZE;
			else if( m_iCurSpiritCount < 100 )
				iTotalSize = NUM_SIZE*2 - NUM_GAP;
			else
				iTotalSize = NUM_SIZE*3 - NUM_GAP*2;

			iCurXPos -= (iTotalSize / 2);
			m_pNumText->RenderNum( iCurXPos, iYPos + 164, m_iCurSpiritCount, -NUM_GAP );

			if( m_iFuncState == eSet )
			{
				iCurXPos = iXPos + 183;
				iTotalSize = 0;
				int iConversionCount = m_iCurSpiritCount / 3;
				if( iConversionCount < 10 )
					iTotalSize = NUM_SIZE;
				else if( iConversionCount < 100 )
					iTotalSize = NUM_SIZE*2 - NUM_GAP;
				else
					iTotalSize = NUM_SIZE*3 - NUM_GAP*2;

				iCurXPos -= (iTotalSize / 2);
				m_pNumText->RenderNum( iCurXPos, iYPos + 198, iConversionCount, -NUM_GAP );
			}
		}
	}
}

void SpiritFuncWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iProcessCount != 0 )
	{
		if( m_bFirstGap && m_dwProcessTime + m_dwProcessFirstGapTime > dwCurTime )
			return;
		if( !m_bFirstGap && m_dwProcessTime + m_dwProcessGapTime > dwCurTime )
			return;

		m_iCurSpiritCount += m_iProcessCount;
		m_dwProcessTime = dwCurTime;
		m_bFirstGap = false;

		if( m_iCurSpiritCount > m_iSpiritCount )
			m_iCurSpiritCount = m_iSpiritCount;
		else 
		{
			if( m_iTabState == eCOMPOSE )
			{
				if( m_iCurSpiritCount < m_iSpiritMinCount )
					m_iCurSpiritCount = m_iSpiritMinCount;
			}
			else
			{
				if( m_iCurSpiritCount < CONVERSION_MIN )
					m_iCurSpiritCount = CONVERSION_MIN;
			}			
		}
	}

	if( m_iFuncState == eSendPacket && m_dwRollingEndTime < dwCurTime )
	{
		if( m_iTabState == eCOMPOSE )
			SetComposeResult();
		else if( m_iTabState == eCONVERSION )
			SetConversionResult();
	}
}

void SpiritFuncWnd::iwm_show()
{
	m_pEffect = GetEffect("Effect");
	if( m_pEffect )
		m_pEffect->Stop();

	SelectTab( m_iTabState + ID_COMPOSE_TAB_BTN );

	ioUserSpiritInven *pSpiritInven = g_MyInfo.GetSpiritInventory();
	if( !pSpiritInven )
		return;

	m_iSpecialSpiritCount = pSpiritInven->GetSpiritCount( m_iSpecialSpiritCode );
}

void SpiritFuncWnd::iwm_hide()
{
	if( m_pEffect )
		m_pEffect->Stop();
}

void SpiritFuncWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOSE_TAB_BTN:
	case ID_CONVERSION_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			SelectTab( dwID );
		}
		break;
	case ID_COMPOSE_MATERIAL_ICON:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_COMPOSE_TAB_BTN );
		}
		break;
	case ID_CONVERSION_SOURCE_ICON:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_CONVERSION_TAB_BTN );
		}
		break;
	case ID_COMPOSE:
		if( cmd == IOBN_BTNUP )
		{
			SendCompose();
		}
		break;
	case ID_CONVERSION:
		if( cmd == IOBN_BTNUP )
		{
			SendConversion();
		}
		break;
	case ID_CONVERSION_RESULT_ICON:
	case ID_CONVERSION_SELECT:
		if( cmd == IOBN_BTNUP )
		{
			MySpiritSelectWnd *pSelectWnd = dynamic_cast<MySpiritSelectWnd*>( g_GUIMgr.FindWnd( MYSPIRIT_SELECT_WND ) );
			if ( pSelectWnd )
			{
				pSelectWnd->ShowItemSeletWnd( m_iSpiritCode, this );
			}
		}
		break;
	case ID_COMPOSE_MINUS1:
	case ID_COMPOSE_PLUS2:
		if( cmd == IOBN_BTNDOWN )
		{
			m_iProcessCount = -1;
			m_iCurSpiritCount += m_iProcessCount;
			if( m_iCurSpiritCount < m_iSpiritMinCount )
				m_iCurSpiritCount = m_iSpiritMinCount;

			m_dwProcessTime = FRAMEGETTIME();
			m_bFirstGap = true;
		}
		else if( cmd == IOBN_BTNUP )
			m_iProcessCount = 0;
		break;
	case ID_COMPOSE_PLUS1:
	case ID_COMPOSE_MINUS2:
		if( cmd == IOBN_BTNDOWN )
		{
			m_iProcessCount = 1;
			m_iCurSpiritCount += m_iProcessCount;
			if( m_iCurSpiritCount > m_iSpiritCount )
				m_iCurSpiritCount = m_iSpiritCount;

			m_dwProcessTime = FRAMEGETTIME();
			m_bFirstGap = true;
		}
		else if( cmd == IOBN_BTNUP )
			m_iProcessCount = 0;
		break;
	case ID_CONVERSION_MINUS:
		if( cmd == IOBN_BTNDOWN )
		{
			m_iProcessCount = -3;
			m_iCurSpiritCount += m_iProcessCount;
			if( m_iCurSpiritCount < CONVERSION_MIN )
				m_iCurSpiritCount = CONVERSION_MIN;

			m_dwProcessTime = FRAMEGETTIME();
			m_bFirstGap = true;
		}
		else if( cmd == IOBN_BTNUP )
			m_iProcessCount = 0;
		break;
	case ID_CONVERSION_PLUS:
		if( cmd == IOBN_BTNDOWN )
		{
			m_iProcessCount = 3;
			m_iCurSpiritCount += m_iProcessCount;
			if( m_iCurSpiritCount > m_iSpiritCount )
				m_iCurSpiritCount = m_iSpiritCount;

			m_dwProcessTime = FRAMEGETTIME();
			m_bFirstGap = true;
		}
		else if( cmd == IOBN_BTNUP )
			m_iProcessCount = 0;
		break;
	case MYSPIRIT_SELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			if ( param <= 0 )
				break;

			m_iConversionCode = param;
			m_iFuncState = eSet;
			SetChildActive( ID_CONVERSION );

			InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_CONVERSION_RESULT_ICON ) );
			if( pIcon )
				pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( g_SpiritMgr.GetIconName( param ) ), FLOAT1, FLOAT1 );
		}
		break;
	}
}

void SpiritFuncWnd::SelectTab( DWORD dwID )
{
	m_iTabState = dwID - ID_COMPOSE_TAB_BTN;
	m_iFuncState = eReady;
	m_iSpiritCode = 0;
	m_iSpiritCount = 0;
	m_iCurSpiritCount = 0;
	m_iSpiritMinCount = 0;
	m_iSpiritMaxCount = 0;
	m_iConversionCode = 0;
	m_iProcessCount = 0;
	m_dwTabChangeTime = FRAMEGETTIME();

	CheckRadioButton( ID_COMPOSE_TAB_BTN, ID_CONVERSION_TAB_BTN, dwID );
	if( m_iTabState == eCOMPOSE )
	{
		InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_COMPOSE_MATERIAL_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
			pIcon->ShowWnd();
		}
		pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_COMPOSE_SPECIAL_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( g_SpiritMgr.GetIconName( m_iSpecialSpiritCode ) ), 0.85f, 0.85f );
			pIcon->ShowWnd();
			pIcon->SetInActive();
		}
		pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_COMPOSE_RESULT_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
			pIcon->ShowWnd();
		}
		ShowChildWnd( ID_COMPOSE );
		SetChildInActive( ID_COMPOSE );
		HideChildWnd( ID_COMPOSE_MINUS1 );
		HideChildWnd( ID_COMPOSE_PLUS1 );
		HideChildWnd( ID_COMPOSE_MINUS2 );
		HideChildWnd( ID_COMPOSE_PLUS2 );
		HideChildWnd( ID_COMPOSE_ROLLING );

		HideChildWnd( ID_CONVERSION_SOURCE_ICON );
		HideChildWnd( ID_CONVERSION_RESULT_ICON );
		HideChildWnd( ID_CONVERSION_SELECT );
		HideChildWnd( ID_CONVERSION );
		HideChildWnd( ID_CONVERSION_MINUS );
		HideChildWnd( ID_CONVERSION_PLUS );
		HideChildWnd( ID_CONVERSION_ROLLING );
	}
	else
	{
		HideChildWnd( ID_COMPOSE_MATERIAL_ICON );
		HideChildWnd( ID_COMPOSE_SPECIAL_ICON );
		HideChildWnd( ID_COMPOSE_RESULT_ICON );
		HideChildWnd( ID_COMPOSE );
		HideChildWnd( ID_COMPOSE_MINUS1 );
		HideChildWnd( ID_COMPOSE_PLUS1 );
		HideChildWnd( ID_COMPOSE_MINUS2 );
		HideChildWnd( ID_COMPOSE_PLUS2 );
		HideChildWnd( ID_COMPOSE_ROLLING );

		HideChildWnd( ID_CONVERSION_MINUS );
		HideChildWnd( ID_CONVERSION_PLUS );
		HideChildWnd( ID_CONVERSION_ROLLING );

		InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_CONVERSION_SOURCE_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
			pIcon->ShowWnd();
		}
		pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_CONVERSION_RESULT_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
			pIcon->ShowWnd();
		}
		ShowChildWnd( ID_CONVERSION_SELECT );
		ShowChildWnd( ID_CONVERSION );
		SetChildInActive( ID_CONVERSION_SELECT );
		SetChildInActive( ID_CONVERSION_RESULT_ICON );
		SetChildInActive( ID_CONVERSION );
	}
}

void SpiritFuncWnd::SelectClickAction( int iCode, int iCount, bool bSoldier, bool bCompose )
{
	if( iCode == m_iSpecialSpiritCode )
		return;

	if( iCount == 0 )
	{
		IoString str;
		str.Set(0, 0, m_szNoSpiritAlarm.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);

		g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
		return;
	}
	
	if( m_iTabState == eCOMPOSE )
	{
		if( bSoldier )
		{
			IoString str;
			str.Set(0, 0, m_szComposeAlarm[0].c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);

			g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
			return;
		}
		if( !bCompose )
		{
			IoString str;
			str.Set(0, 0, m_szComposeAlarm[1].c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);

			g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
			return;
		}
		
		m_iSpiritCode = iCode;
		m_iSpiritMaxCount = g_SpiritMgr.GetComposeMax( iCode );
		m_iSpiritMinCount = max( g_SpiritMgr.GetComposeMin( iCode ), m_iSpiritMaxCount - m_iSpecialSpiritCount );
		m_iSpiritCount = min( iCount, m_iSpiritMaxCount );
		m_iCurSpiritCount = m_iSpiritCount;

		if( m_iSpecialSpiritCount + iCount < m_iSpiritMaxCount )
		{
			m_iFuncState = eNot;

			SetChildInActive( ID_COMPOSE );
			HideChildWnd( ID_COMPOSE_MINUS1 );
			HideChildWnd( ID_COMPOSE_PLUS1 );
			HideChildWnd( ID_COMPOSE_MINUS2 );
			HideChildWnd( ID_COMPOSE_PLUS2 );
		}
		else
		{
			m_iFuncState = eSet;

			SetChildActive( ID_COMPOSE );
			ShowChildWnd( ID_COMPOSE_MINUS1 );
			ShowChildWnd( ID_COMPOSE_PLUS1 );
			ShowChildWnd( ID_COMPOSE_MINUS2 );
			ShowChildWnd( ID_COMPOSE_PLUS2 );
			if( m_iSpecialSpiritCount == 0 )
			{
				SetChildInActive( ID_COMPOSE_MINUS1 );
				SetChildInActive( ID_COMPOSE_PLUS1 );
				SetChildInActive( ID_COMPOSE_MINUS2 );
				SetChildInActive( ID_COMPOSE_PLUS2 );
			}
			else
			{
				SetChildActive( ID_COMPOSE_MINUS1 );
				SetChildActive( ID_COMPOSE_PLUS1 );
				SetChildActive( ID_COMPOSE_MINUS2 );
				SetChildActive( ID_COMPOSE_PLUS2 );
			}
		}
		InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_COMPOSE_MATERIAL_ICON ) );
		if( pIcon )
			pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( g_SpiritMgr.GetIconName( iCode ) ), FLOAT1, FLOAT1 );

		pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_COMPOSE_RESULT_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( g_MyInfo.GetMySoldierIcon( iCode ), FLOAT1, FLOAT1 );
			pIcon->SetSubIcon( g_MyInfo.GetSoldierSubIcon( iCode ) );
		}
		SetChildActive( ID_COMPOSE_SPECIAL_ICON );
	}
	else
	{
		if( iCount < CONVERSION_MIN )
		{
			IoString str;
			str.Set(0, 0, m_szConversionAlarm.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);

			g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
			return;
		}
		m_iSpiritCode = iCode;
		m_iSpiritCount = iCount - (iCount % CONVERSION_MIN);		
		m_iCurSpiritCount = m_iSpiritCount;

		InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_CONVERSION_SOURCE_ICON ) );
		if( pIcon )
			pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( g_SpiritMgr.GetIconName( iCode ) ), FLOAT1, FLOAT1 );
	
		int iSpiritType = g_SpiritMgr.GetSpiritType( iCode );
		int iConversionSpiritType = g_SpiritMgr.GetSpiritType( m_iConversionCode );
		if( m_iConversionCode == 0 || iConversionSpiritType != iSpiritType || m_iConversionCode == m_iSpiritCode )
		{
			m_iConversionCode = 0;
			pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_CONVERSION_RESULT_ICON ) );
			if( pIcon )
				pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
		}
		if( m_iConversionCode != 0 )
		{
			m_iFuncState = eSet;
			SetChildActive( ID_CONVERSION );
		}
		else
		{
			m_iFuncState = eNot;
			SetChildInActive( ID_CONVERSION );
		}
		SetChildActive( ID_CONVERSION_SELECT );
		SetChildActive( ID_CONVERSION_RESULT_ICON );
		ShowChildWnd( ID_CONVERSION_MINUS );
		ShowChildWnd( ID_CONVERSION_PLUS );
	}
}

void SpiritFuncWnd::SendCompose()
{
	if( m_iFuncState != eSet )
	{
		IoString str;
		str.Set(0, 0, m_szSendComposeError[m_iFuncState].c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);

		g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
		return;
	}
	TCPNetwork::MouseBusy( true );

	int iComposeCount = min( m_iSpiritMaxCount - m_iCurSpiritCount, m_iSpecialSpiritCount );

	SP2Packet kPacket( CTPK_SPIRIT_COMPOSE );
	kPacket << m_iSpiritCode;
	kPacket << m_iCurSpiritCount;
	kPacket << m_iSpecialSpiritCode;
	kPacket << iComposeCount;
	TCPNetwork::SendToServer( kPacket );
}

void SpiritFuncWnd::SendConversion()
{
	if( m_iFuncState != eSet )
	{
		IoString str;
		str.Set(0, 0, m_szSendConversionError[m_iFuncState].c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_CENTER, TVA_TOP, 4);

		g_GUIMgr.SetMsgBox( MB_OK, NULL, &str );
		return;
	}
	TCPNetwork::MouseBusy( true );

	SP2Packet kPacket( CTPK_SPIRIT_CONVERSION );
	kPacket << m_iSpiritCode;
	kPacket << m_iCurSpiritCount;
	kPacket << m_iConversionCode;
	TCPNetwork::SendToServer( kPacket );
}

void SpiritFuncWnd::OnSpiritComposeResult( int iClassType )
{
	m_iFuncState = eSendPacket;
	m_iComposeResultClassType = iClassType;
	m_dwRollingEndTime = FRAMEGETTIME() + m_dwRollingTime;

	HideChildWnd( ID_COMPOSE );
	HideChildWnd( ID_COMPOSE_MINUS1 );
	HideChildWnd( ID_COMPOSE_PLUS1 );
	HideChildWnd( ID_COMPOSE_MINUS2 );
	HideChildWnd( ID_COMPOSE_PLUS2 );
	ShowChildWnd( ID_COMPOSE_ROLLING );

	if( m_pEffect )
	{
		m_pEffect->Start();
		m_pEffect->SetPosX( 127 - m_pEffect->GetWidth() / 2 );
		m_pEffect->SetPosY( 196 - m_pEffect->GetHeight() / 2 );
	}
	if( !m_szComposeRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szComposeRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void SpiritFuncWnd::OnSpiritConversionResult( int iSpiritCode, int iCount, bool bCritical )
{
	m_iFuncState = eSendPacket;
	m_iConversionResultSpiritCode = iSpiritCode;
	m_iConversionResultCount = iCount;
	m_bConversionResultCritical = bCritical;
	m_dwRollingEndTime = FRAMEGETTIME() + m_dwRollingTime;

	HideChildWnd( ID_CONVERSION );
	HideChildWnd( ID_CONVERSION_MINUS );
	HideChildWnd( ID_CONVERSION_PLUS );
	ShowChildWnd( ID_CONVERSION_ROLLING );
	SetChildInActive( ID_CONVERSION_SELECT );
	SetChildInActive( ID_CONVERSION_RESULT_ICON );

	if( m_pEffect )
	{
		m_pEffect->Start();
		m_pEffect->SetPosX( 127 - m_pEffect->GetWidth() / 2 );
		m_pEffect->SetPosY( 166 - m_pEffect->GetHeight() / 2 );
	}
	if( !m_szConversionRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szConversionRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void SpiritFuncWnd::SetComposeResult()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	SelectTab( ID_COMPOSE_TAB_BTN );
	SpiritComposeResultWnd *pWnd = dynamic_cast<SpiritComposeResultWnd*>(g_GUIMgr.FindWnd(SPIRIT_COMPOSE_RESULT_WND));
	if( pWnd )
	{
		pWnd->ShowWnd();
		pWnd->SetWndPos( iXPos, iYPos );
		pWnd->SetSoldierInfo( m_iComposeResultClassType );
	}
	if( m_pEffect )
		m_pEffect->Stop();

	if( !m_szComposeRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szComposeRollingSound, 0 );
	if( !m_szComposeResultSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szComposeResultSound );
}

void SpiritFuncWnd::SetConversionResult()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	SelectTab( ID_CONVERSION_TAB_BTN );
	SpiritConversionResultWnd *pWnd = dynamic_cast<SpiritConversionResultWnd*>(g_GUIMgr.FindWnd(SPIRIT_CONVERSION_RESULT_WND));
	if( pWnd )
	{
		pWnd->ShowWnd();
		pWnd->SetWndPos( iXPos, iYPos );
		pWnd->SetSpiritInfo( m_iConversionResultSpiritCode, m_iConversionResultCount, m_bConversionResultCritical );
	}
	if( m_pEffect )
		m_pEffect->Stop();

	if( !m_szConversionRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szConversionRollingSound, 0 );
	if( !m_szConversionResultSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szConversionResultSound );
}
//////////////////////////////////////////////////////////////////////////////
SpiritComposeResultWnd::SpiritComposeResultWnd()
{
	m_CharInfo.Init();

	m_pEffect = NULL;
}

SpiritComposeResultWnd::~SpiritComposeResultWnd()
{
}

void SpiritComposeResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szMortmain = xElement.GetStringAttribute_e( "mortmain" );
	m_szDesc = xElement.GetStringAttribute_e( "composedesc" );
	m_dwShowTime = xElement.GetIntAttribute_e( "showtime" );
}

void SpiritComposeResultWnd::SetSoldierInfo( int iClassType )
{
	m_CharInfo.Init();
	Help::GetDefaultCharInfo( m_CharInfo, iClassType );
	m_szName = g_MyInfo.GetClassName( iClassType );

	if( m_pEffect )
	{
		m_pEffect->Start();
		m_pEffect->SetPosX( 127 - m_pEffect->GetWidth() / 2 );
		m_pEffect->SetPosY( 191 - m_pEffect->GetHeight() / 2 );
	}

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_3D_INFO ) );
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( FLOAT500 );
		pCharWnd->SetChar( m_CharInfo, false );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->SetUserInfoToolTip( false );
		pCharWnd->SetCharScale( 0.85f );
	}
}

void SpiritComposeResultWnd::iwm_show()
{
	m_pEffect = GetEffect("Effect");
	if( m_pEffect )
		m_pEffect->Stop();

	m_dwShowEndTime = FRAMEGETTIME() + m_dwShowTime;
}

void SpiritComposeResultWnd::iwm_hide()
{
	if( m_pEffect )
		m_pEffect->Stop();
}

void SpiritComposeResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 128, iYPos + 380, FONT_SIZE_17, m_szName.c_str() );
	g_FontMgr.PrintText( iXPos + 128, iYPos + 402, FONT_SIZE_13, m_szMortmain.c_str() );

	IoString str;
	str.Set(0, 0, m_szDesc.c_str(), FONT_SIZE_13, 204, TS_NORMAL, TCT_DEFAULT_LIGHTGRAY, 0, TAT_CENTER, TVA_TOP, 4);
	str.OnRender( iXPos + 128, iYPos + 423 );
}

void SpiritComposeResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwShowEndTime < dwCurTime )
		HideWnd();
}
//////////////////////////////////////////////////////////////////////////////
SpiritConversionResultWnd::SpiritConversionResultWnd()
{
	m_pEffect = NULL;
	m_pNumText = NULL;
}

SpiritConversionResultWnd::~SpiritConversionResultWnd()
{
	SAFEDELETE( m_pNumText );
}

void SpiritConversionResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwShowTime = xElement.GetIntAttribute_e( "showtime" );
	m_dwEffectStartTime = xElement.GetIntAttribute_e( "effectstarttime" );
	m_dwEffectTime = xElement.GetIntAttribute_e( "effecttime" );
}

void SpiritConversionResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "NumText" )
	{
		SAFEDELETE( m_pNumText );
		m_pNumText = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void SpiritConversionResultWnd::SetSpiritInfo( int iSpiritCode, int iCount, bool bCritical )
{
	m_iCount = iCount;
	m_bCritical = bCritical;
	m_bEffect = false;
	m_szName = g_SpiritMgr.GetName( iSpiritCode );

	InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_ICON ) );
	if( pIcon )
		pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( g_SpiritMgr.GetIconName( iSpiritCode ) ), FLOAT1, FLOAT1 );

	if( m_pEffect )
	{
		m_pEffect->Start();
		m_pEffect->SetPosX( 127 - m_pEffect->GetWidth() / 2 );
		m_pEffect->SetPosY( 166 - m_pEffect->GetHeight() / 2 );
	}
}

void SpiritConversionResultWnd::iwm_show()
{
	m_pEffect = GetEffect("Effect");
	if( m_pEffect )
		m_pEffect->Stop();

	m_dwStartTime = FRAMEGETTIME();
}

void SpiritConversionResultWnd::iwm_hide()
{
	if( m_pEffect )
		m_pEffect->Stop();
}

void SpiritConversionResultWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_bCritical && !m_bEffect && m_dwStartTime + m_dwEffectStartTime < dwCurTime )
		m_bEffect = true;
	if( m_dwStartTime + m_dwShowTime < dwCurTime )
		HideWnd();
}

void SpiritConversionResultWnd::OnRenderAfterChild()
{
	enum
	{
		NUM_SIZE	= 20,
		NUM_GAP		= 4,
	};
	if( !m_pNumText )
		return;

	int iCount = m_iCount;
	if( m_bCritical && !m_bEffect )
		iCount = m_iCount / 3;

	float fScale = 1.f;
	if( m_bEffect )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		DWORD dwEffectStartTime = m_dwStartTime + m_dwEffectStartTime;
		DWORD dwEffectGapTime = dwCurTime - dwEffectStartTime;
		float fRate = (float)dwEffectGapTime / (float)m_dwEffectTime;
		if( fRate > 1.f )
			fRate = 1.f;

		fScale = 1.2f - 0.2f * fRate;
	}

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	int iCurXPos = iXPos + 127;
	int iTotalSize = 0;
	if( iCount < 10 )
		iTotalSize = NUM_SIZE;
	else if( iCount < 100 )
		iTotalSize = NUM_SIZE*2 - NUM_GAP;
	else
		iTotalSize = NUM_SIZE*3 - NUM_GAP*2;

	iTotalSize *= fScale;
	iCurXPos -= (iTotalSize / 2);
	m_pNumText->RenderNum( iCurXPos, iYPos + 187, iCount, -NUM_GAP, fScale, UI_RENDER_NORMAL, TFO_BILINEAR );
	
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );	
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 128, iYPos + 215, FONT_SIZE_17, m_szName.c_str() );
}