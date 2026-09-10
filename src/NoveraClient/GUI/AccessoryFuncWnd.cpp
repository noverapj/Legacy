
#include "StdAfx.h"
#include "AccessoryFuncWnd.h"
#include "UISoldierSelectWnd.h"
#include "MyInventoryInfoWnd.h"
#include "../ioAccessoryInfoManager.h"
#include "../ioDateHelp.h"

AccessoryFuncWnd::AccessoryFuncWnd()
{
	m_iTabState = ID_INFO_TAB_BTN;

	m_CharInfo.Init();
	m_CharInfo.m_class_type = 1;

	m_pIconBack = NULL;
	m_pIconBG = NULL;
	m_pFinalBack = NULL;
	m_pStripDark = NULL;
	m_pStripLight = NULL;
	m_pEffect = NULL;
	m_pItemSlot = NULL;
	m_pBlackStrip = NULL;
}

AccessoryFuncWnd::~AccessoryFuncWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconBG );
	SAFEDELETE( m_pFinalBack );
	SAFEDELETE( m_pStripDark );
	SAFEDELETE( m_pStripLight );
	SAFEDELETE( m_pEffect );
	SAFEDELETE( m_pItemSlot );
	SAFEDELETE( m_pBlackStrip );
}

void AccessoryFuncWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "FinalBack" )
	{
		SAFEDELETE( m_pFinalBack );
		m_pFinalBack = pImage;
	}
	else if( szType == "IconBG" )
	{
		SAFEDELETE( m_pIconBG );
		m_pIconBG = pImage;
	}
	else if( szType == "strip_gray_dark" )
	{
		SAFEDELETE( m_pStripDark );
		m_pStripDark = pImage;
		m_pStripDark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pStripDark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pStripDark->SetSize( 205, 21 );
	}
	else if( szType == "strip_gray_light" )
	{
		SAFEDELETE( m_pStripLight );
		m_pStripLight = pImage;
		m_pStripLight->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pStripLight->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pStripLight->SetSize( 205, 21 );
	}
	else if( szType == "BlackStrip" )
	{
		SAFEDELETE( m_pBlackStrip );
		m_pBlackStrip = pImage;
		m_pBlackStrip->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBlackStrip->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pBlackStrip->SetSize( 105, 2 );
		m_pBlackStrip->SetAlpha( (float)MAX_ALPHA_RATE * 0.2f );
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void AccessoryFuncWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ItemSlot" )
	{
		SAFEDELETE( m_pItemSlot );
		m_pItemSlot = pFrame;
		m_pItemSlot->SetAlpha( (float)MAX_ALPHA_RATE * 0.4f );
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void AccessoryFuncWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szReinforceText = xElement.GetStringAttribute_e( "ReinforceText" );
	m_szReinforceAlarm = xElement.GetStringAttribute_e( "ReinforceAlarm" );
	m_szReinforceRollingSound = xElement.GetStringAttribute_e( "ReinforceRollingSound" );
	m_szReinforceResultSound = xElement.GetStringAttribute_e( "ReinforceResultSound" );
	m_szReinforceResultMsg[0] = xElement.GetStringAttribute_e( "ReinforceResultMsg1" );
	m_szReinforceResultMsg[1] = xElement.GetStringAttribute_e( "ReinforceResultMsg2" );
	m_szReinforceResultDesc = xElement.GetStringAttribute_e( "ReinforceResultDesc" );
	m_szAccessoryReinforeTimeOut = xElement.GetStringAttribute_e( "AccessoryReinforeTimeOut" );

	m_dwRollingTime = xElement.GetIntAttribute_e( "RollingTime" );

	char szBuf[MAX_PATH];
	for( int i = 0; i < 5; ++i )
	{
		wsprintf_e( szBuf, "ReinforceStateMsg%d", i + 1 );
		m_szReinforceStateMsg[i] = xElement.GetStringAttribute( szBuf );

		wsprintf_e( szBuf, "AccessoryDesc%d", i + 1 );
		m_szAccessoryDesc[i] = xElement.GetStringAttribute( szBuf );
	}

	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_vRollingEffect.clear();

	int iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
	m_vRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vRollingEffect.push_back( szEffect );
	}

	m_vEndEfffect_S.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vEndEfffect_S.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_S.push_back( szEffect );
	}
}

void AccessoryFuncWnd::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioWnd::OnRender();

	if( m_iTabState == ID_FUNC_TAB_BTN )
	{
		if( m_iFuncState != eEnd && m_iFuncState != eSendPacket )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );	
			if( m_iFuncState == eReady || m_iFuncState == eMax || m_iFuncState == eNot )
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );			
			else if( m_iFuncState == eSet )
				g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

			if( m_pItemSlot )
			{
				m_pItemSlot->Render( iXPos + 29, iYPos + 101 );
				m_pItemSlot->Render( iXPos + 29, iYPos + 193 );
			}
			if( m_pIconBG )
				m_pIconBG->Render( iXPos + 40, iYPos + 111, UI_RENDER_MULTIPLY );
			if( m_pStripDark )
				m_pStripDark->Render( iXPos + 27, iYPos + 295, UI_RENDER_MULTIPLY );
			if( m_pBlackStrip )
			{
				m_pBlackStrip->Render( iXPos + 112, iYPos + 145 );
				m_pBlackStrip->Render( iXPos + 112, iYPos + 237 );
			}
			if( COMPARE( m_iFuncState, eReady, eSendPacket + 1 ) )
				g_FontMgr.PrintText( iXPos + 129, iYPos + 299, FONT_SIZE_12, m_szReinforceStateMsg[m_iFuncState].c_str() );

			IoString str;
			str.Set(0, 0, m_szReinforceText.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
			str.OnRender( iXPos + 27, iYPos + 327 );
		
			g_FontMgr.SetAlignType( TAT_LEFT );	
			if( m_Accessory[0].m_iIndex == 0 )
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 113, FONT_SIZE_12, m_szAccessoryDesc[0].c_str() );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 129, FONT_SIZE_12, m_szAccessoryDesc[1].c_str() );
			
				IoString str2;
				str2.Set(0, 0, m_szAccessoryDesc[4].c_str(), FONT_SIZE_11, 105, TS_NORMAL, TCT_DEFAULT_LIGHTGRAY, 0, TAT_LEFT, TVA_TOP, 1);
				str2.OnRender( iXPos + 112, iYPos + 149 );
			}
			else
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 113, FONT_SIZE_12, m_Accessory[0].GetName().c_str() );
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 129, FONT_SIZE_12, m_szAccessoryTimeText[0].c_str() );
			
				IoString str2;
				str2.Set(0, 0, m_szAccessoryTooltip[0].c_str(), FONT_SIZE_11, 105, TS_NORMAL, TCT_DEFAULT_GRAY, 0, TAT_LEFT, TVA_TOP, 1);
				str2.OnRender( iXPos + 112, iYPos + 149 );
			}
			if( m_Accessory[1].m_iIndex == 0 )
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 205, FONT_SIZE_12, m_szAccessoryDesc[2].c_str() );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 221, FONT_SIZE_12, m_szAccessoryDesc[3].c_str() );
			
				IoString str2;
				str2.Set(0, 0, m_szAccessoryDesc[4].c_str(), FONT_SIZE_11, 105, TS_NORMAL, TCT_DEFAULT_LIGHTGRAY, 0, TAT_LEFT, TVA_TOP, 1);
				str2.OnRender( iXPos + 112, iYPos + 241 );
			}
			else
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 205, FONT_SIZE_12, m_Accessory[1].GetName().c_str() );
				g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
				g_FontMgr.PrintText( iXPos + 112, iYPos + 221, FONT_SIZE_12, m_szAccessoryTimeText[1].c_str() );
			
				IoString str2;
				str2.Set(0, 0, m_szAccessoryTooltip[1].c_str(), FONT_SIZE_11, 105, TS_NORMAL, TCT_DEFAULT_GRAY, 0, TAT_LEFT, TVA_TOP, 1);
				str2.OnRender( iXPos + 112, iYPos + 241 );
			}
		}
		else if( m_iFuncState == eSendPacket )
		{
			if( m_pIconBack )
				m_pIconBack->Render( iXPos + 129, iYPos + 150, UI_RENDER_MULTIPLY );
			if( m_pEffect )
				m_pEffect->Render( iXPos + 129, iYPos + 150, UI_RENDER_ADD );

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 27, iYPos + 256, FONT_SIZE_12, m_szReinforceStateMsg[m_iFuncState].c_str() );

			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 129, iYPos + 202, FONT_SIZE_17, m_Accessory[Type_Accessory].m_Name.c_str() );
		
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos + 129, iYPos + 225, FONT_SIZE_17, m_szAccessoryTimeText[0].c_str() );
		}
		else if( m_iFuncState == eEnd )
		{
			if( m_pFinalBack )
				m_pFinalBack->Render( iXPos + 129, iYPos + 150, UI_RENDER_MULTIPLY );
			if( m_pEffect )
				m_pEffect->Render( iXPos + 129, iYPos + 150, UI_RENDER_ADD );

			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
			g_FontMgr.PrintText( iXPos + 129, iYPos + 202, FONT_SIZE_17, m_Accessory[Type_Accessory].m_Name.c_str() );

			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintText( iXPos + 129, iYPos + 225, FONT_SIZE_17, m_szReinforceResultMsg[0].c_str() );

			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos + 129, iYPos + 421, FONT_SIZE_17, m_szReinforceResultMsg[1].c_str() );

			IoString str;
			str.Set(0, 0, m_szReinforceResultDesc.c_str(), FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
			str.OnRender( iXPos + 27, iYPos + 256 );

			if( m_pStripDark )
			{
				m_pStripDark->Render( iXPos + 27, iYPos + 350, UI_RENDER_MULTIPLY );
				m_pStripDark->Render( iXPos + 27, iYPos + 392, UI_RENDER_MULTIPLY );
			}
			if( m_pStripLight )
				m_pStripLight->Render( iXPos + 27, iYPos + 371, UI_RENDER_MULTIPLY );

			char szText[MAX_BUFFER_SIZE];
			float fValue = m_Accessory[Type_Accessory].m_iRandomNumber / FLOAT1000;
			float fGapValue = m_iAbilityGap / FLOAT1000;
			float fPrevValue = fValue - fGapValue;
			if( m_bAbilitySign )
				fPrevValue = fValue + fGapValue;

			sprintf( szText, m_szReinforceResultText.c_str(), fValue, fPrevValue, fGapValue );

			IoString str2;
			str2.Set(0, 0, szText, FONT_SIZE_12, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 8);
			str2.OnRender( iXPos + 27, iYPos + 354 );
		}
	}
}

void AccessoryFuncWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;

	ioWnd::OnProcess( fTimePerSec );

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_iFuncState == eSendPacket && m_dwRollingEndTime != 0 && m_dwRollingEndTime < dwCurTime )
	{
		if( m_iTabState == ID_FUNC_TAB_BTN )
			SetReinforceResult();
	}
	RollingEffect( fTimePerSec );
}

void AccessoryFuncWnd::RollingEffect( float fTimePerSec )
{
	int iEffectListCnt = 0;
	if( m_iFuncState == eEnd )
		iEffectListCnt = m_vEndEfffect_S.size();
	else if( m_iFuncState == eSendPacket )
		iEffectListCnt = m_vRollingEffect.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}
	if( m_iFuncState == eEnd && m_iCurEffectArray == iEffectListCnt - 1 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;
		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect;
		if( m_iFuncState == eEnd )
			szEffect = m_vEndEfffect_S[m_iCurEffectArray];
		else
			szEffect = m_vRollingEffect[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
			if( m_pEffect )
				m_pEffect->SetScale( 2.0f );
		}
		m_dwRollingEffectCheckTime = dwCurTime;
	}
}

void AccessoryFuncWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/myinventorycostumehelpwnd.xml", this );
}

void AccessoryFuncWnd::iwm_show()
{
	for( int i = 0; i < 3; i++ )
	{
		ioWnd *pWnd = FindChildWnd( ID_INFO_TAB_BTN + i );
		if( pWnd )
			pWnd->GoToTop();
	}

	SelectTab( m_iTabState );
}

void AccessoryFuncWnd::iwm_hide()
{
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
}

void AccessoryFuncWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_INFO_TAB_BTN:
	case ID_FUNC_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			SelectTab( dwID );
		}
		break;
	case ID_MY_SOLDIER_LIST_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*> ( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
			if( pUISoldierWnd )
			{
				pUISoldierWnd->ShowUISoldierSelectWnd( this );
				int iCharArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
				if( iCharArray != -1 )
					pUISoldierWnd->SetSelectCharArray( iCharArray );
			}
		}
		break;
	case ID_MY_SOLDIER_LEFT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSlotIdx = m_CharInfo.m_iSlotIndex;			
			int iCharArray = -1;
			while( iCharArray == -1 )
			{
				if( iSlotIdx > 0 )
					iSlotIdx--;
				else
					return;

				iCharArray = g_MyInfo.GetCharSlotIndexToArray( iSlotIdx );
			}
			UISoldierSelectWndBtnUp( iCharArray );
		}
		break;
	case ID_MY_SOLDIER_RIGHT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iSlotIdx = m_CharInfo.m_iSlotIndex;			
			int iCharArray = -1;
			while( iCharArray == -1 )
			{
				if( iSlotIdx < g_MyInfo.GetCurMaxCharSlot() - 1 )
					iSlotIdx++;
				else
					return;

				iCharArray = g_MyInfo.GetCharSlotIndexToArray( iSlotIdx );
			}
			UISoldierSelectWndBtnUp( iCharArray );
		}
		break;
	case ID_ACCESSORY_RING_BTN:
	case ID_ACCESSORY_NECKLACE_BTN:
	case ID_ACCESSORY_BRACELET_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
			else
			{
				int iSlot = dwID - ID_ACCESSORY_RING_BTN;
				if ( COMPARE( iSlot, 0, MAX_ACC_INVENTORY ) )
				{
					int iSlotIndex = m_CharInfo.m_EquipAccessory[iSlot].m_iAccessoryIdx;			

					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
					if( pInvenWnd )
					{
						if( iSlotIndex > 0 )
							pInvenWnd->SetReleaseAccessory( iSlotIndex );
					}
				}
			}
		}
		break;
	case UI_SOLDIERSELECT_WND:
		if( cmd == IOBN_BTNUP )
		{
			UISoldierSelectWndBtnUp( param );
		}
		break;
	case ID_REINFORCE_ACCESSORY_ICON:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_FUNC_TAB_BTN );
		}
		break;
	case ID_REINFORCE:
		if( cmd == IOBN_BTNUP )
		{
			SendReinforce();
		}
		break;
	case ID_SHOW:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_INFO_TAB_BTN );
		}
		break;
	case ID_AGAIN:
	case ID_REINFORCE_MATERIAL_ICON:
		if( cmd == IOBN_BTNUP )
		{
			int iPrevAccessory = m_Accessory[Type_Accessory].m_iIndex;
			SelectTab( ID_FUNC_TAB_BTN );
			SelectClickAction( iPrevAccessory );
		}
		break;
	}
}

void AccessoryFuncWnd::SelectTab( DWORD dwID )
{
	if( m_iTabState == ID_FUNC_TAB_BTN && m_iFuncState == eSendPacket )
		return;

	m_iTabState = dwID;
	m_dwRollingEndTime = 0;
	m_iFuncState = eReady;
	m_Accessory[Type_Accessory].Init();
	m_Accessory[Type_Material].Init();

	CheckRadioButton( ID_INFO_TAB_BTN, ID_FUNC_TAB_BTN, dwID );
	if( m_iTabState == ID_INFO_TAB_BTN )
	{
		SetAccessoryInfo( m_CharInfo.m_class_type );

		ShowChildWnd( ID_MY_SOLDIER_LIST_BTN );
		ShowChildWnd( ID_MY_SOLDIER_LEFT_BTN );
		ShowChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
		ShowChildWnd( ID_COSTUME_SOLDIER_WND );
		ShowChildWnd( ID_ACCESSORY_RING_BTN );
		ShowChildWnd( ID_ACCESSORY_NECKLACE_BTN );
		ShowChildWnd( ID_ACCESSORY_BRACELET_BTN );

		HideChildWnd( ID_REINFORCE_ACCESSORY_ICON );
		HideChildWnd( ID_REINFORCE_MATERIAL_ICON );
		HideChildWnd( ID_REINFORCE );
		HideChildWnd( ID_REINFORCE_ROLLING );
		HideChildWnd( ID_SHOW );
		HideChildWnd( ID_AGAIN );
	}
	else
	{
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
		HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
		HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
		HideChildWnd( ID_COSTUME_SOLDIER_WND );
		HideChildWnd( ID_ACCESSORY_RING_BTN );
		HideChildWnd( ID_ACCESSORY_NECKLACE_BTN );
		HideChildWnd( ID_ACCESSORY_BRACELET_BTN );

		ShowChildWnd( ID_REINFORCE );
		SetChildInActive( ID_REINFORCE );
		HideChildWnd( ID_REINFORCE_ROLLING );
		HideChildWnd( ID_SHOW );
		HideChildWnd( ID_AGAIN );

		InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_REINFORCE_ACCESSORY_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
			pIcon->ShowWnd();
			pIcon->SetInActive();
		}
		pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_REINFORCE_MATERIAL_ICON ) );
		if( pIcon )
		{
			pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
			pIcon->ShowWnd();
			pIcon->SetInActive();
		}
	}
}

void AccessoryFuncWnd::SetAccessoryInfo( int iClassType )
{
	int iCharArray = g_MyInfo.GetClassArray( iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( m_CharInfo, iClassType );

	// 코스튬 관련 (장비 장착시 캐릭터)
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_COSTUME_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetWndPos( 54, 155 );
		pCharWnd->SetZPosition( 470.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
		pCharWnd->SetUserInfoToolTip( true );
	}

	for ( int i=0; i<MAX_INVENTORY-1; i++ )
		SetAccessorySubInfo( i+ID_ACCESSORY_RING_BTN, i, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE );
}

void AccessoryFuncWnd::SetAccessorySubInfo( DWORD dwWndID, int iSlotNum, float fScale, float fBackScale, float fDisableScale )
{
	ioUserAccessory  *pAccessory = g_MyInfo.GetUserAccessory();
	if ( !pAccessory )
		return;

	SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*>(FindChildWnd(dwWndID));
	if( !pBtn )
		return;

	if ( !COMPARE( iSlotNum, 0, MAX_INVENTORY ) )
		return;

	int nSlotIndex = m_CharInfo.m_EquipAccessory[iSlotNum].m_iAccessoryIdx;
	ioUIRenderImage *pImg = NULL;
	
	AccessorySlot kSlot;	
	if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( m_CharInfo.m_class_type );
		ioCharRentalData *pCharRentalData = g_MyInfo.GetCharRentalData();
		if( pCharRentalData && pCharRentalData->GetEquipAccessory( dwCharIndex, kSlot, iSlotNum ) )
		{
			pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );

			pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType );
			pBtn->SetMagicCode( -(iSlotNum+1) );
			pBtn->SetMagicCodeEx( kSlot.m_iAccessoryCode );
		}
		else
		{
			pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType );
			pBtn->SetMagicCode( 0 );
		}
	}
	else if( nSlotIndex > 0 && pAccessory->GetAccessorySlot( nSlotIndex, kSlot ) )
	{
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType );
		pBtn->SetMagicCode( nSlotIndex );

		bool bGray = false;
		time_t kLimitTime = DateHelp::ConvertSecondTime( kSlot.GetYear(), kSlot.GetMonth(), kSlot.GetDay(), kSlot.GetHour(), kSlot.GetMinute(), 0 );
		kLimitTime += 30;
		time_t kCurServerTime = g_MyInfo.GetServerDate();
		kCurServerTime += ( REALGETTIME()/1000 );
		int iGapSec = kLimitTime - kCurServerTime;
		if( iGapSec <= 0 )
			bGray = true;

		pBtn->SetIconGray( bGray );
	}
	else
	{
		pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType );
		pBtn->SetMagicCode( 0 );
	}
}

void AccessoryFuncWnd::UISoldierSelectWndBtnUp( int iSoldierArray )
{
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( !pInvenWnd )
		return;
	
	int iClassType = g_MyInfo.GetClassType( iSoldierArray );
	if( iClassType == -1 )
		return;
	
	if( g_MyInfo.IsCharExerciseStyle( iSoldierArray, EXERCISE_PCROOM ) && pInvenWnd->GetCurTabID() == MyInventoryWnd::ID_DECO_TAB_BTN )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "PC방 용병은 선택 할 수 없습니다." );
		return;
	}

	pInvenWnd->ChangeClassType( iClassType );
	SetAccessoryInfo( iClassType );
}

void AccessoryFuncWnd::SelectClickAction( int iIndex )
{
	if( m_iTabState == ID_INFO_TAB_BTN )
		return;
	if( m_iFuncState == eSendPacket || m_iFuncState == eEnd )
		return;
	if( iIndex == m_Accessory[Type_Accessory].m_iIndex )
		return;
	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return;

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
		return;

	int iType = Type_Accessory;
	if( m_Accessory[iType].m_iIndex != 0 )
		iType = Type_Material;

	if( !pAccessory->GetAccessorySlot( iIndex, m_Accessory[iType] ) )
		return;

	time_t kLimitTime = DateHelp::ConvertSecondTime( m_Accessory[iType].GetYear(), m_Accessory[iType].GetMonth(), m_Accessory[iType].GetDay(), m_Accessory[iType].GetHour(), m_Accessory[iType].GetMinute(), 0 );
	kLimitTime += 30;
	time_t kCurServerTime = g_MyInfo.GetServerDate();
	kCurServerTime += ( REALGETTIME()/1000 );
	int iGapSec = kLimitTime - kCurServerTime;
	if( iGapSec <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szAccessoryReinforeTimeOut.c_str() );
		m_Accessory[iType].Init();
		return;
	}

	char szText[MAX_PATH]="";
	Help::GetRemainTime( m_Accessory[iType].GetYear(), m_Accessory[iType].GetMonth(), m_Accessory[iType].GetDay(), m_Accessory[iType].GetHour(), m_Accessory[iType].GetMinute(), szText, sizeof( szText ), true );
	m_szAccessoryTimeText[iType] = szText;

	const ioItem* pItem = g_ItemMaker.GetItemConst( m_Accessory[iType].m_iAccessoryCode, __FUNCTION__ );
	if ( !pItem )
		return;

	if( Type_Accessory == iType )
	{
		m_iAbilityMax = 0;
		m_iAbilityGap = 0;
		m_bAbilitySign = false;
	}

	char szTooltip[MAX_PATH] = "";
	int iSlot = ES_RING + m_Accessory[iType].m_iAccessoryCode/DEFAULT_BASIC_ACCESSORY_CODE - 1;
	float fRanNum = m_Accessory[iType].m_iRandomNumber/FLOAT1000;
	if ( iSlot == ES_RING )
	{
		const ioRingItem* pRing = ToRingItemConst( pItem );
		if ( !pRing )
			return;

		float fMAxValue = pRing->GetAbilityMax() / FLOAT1000;
		sprintf( szTooltip, pRing->GetReinforceText().c_str(), fRanNum, fMAxValue );
		if( Type_Accessory == iType )
		{
			m_iAbilityMax = pRing->GetAbilityMax();
			m_iAbilityGap = pRing->GetAbilityGap();
			m_bAbilitySign = pRing->GetAbilitySign();
			m_szReinforceResultText = pRing->GetReinforceResultText();
		}
	}
	else if ( iSlot == ES_NECKLACE )
	{
		const ioNecklaceItem* pNeck = ToNecklaceItemConst( pItem );
		if ( !pNeck )
			return;

		float fMAxValue = pNeck->GetAbilityMax() / FLOAT1000;
		sprintf( szTooltip, pNeck->GetReinforceText().c_str(), fRanNum, fMAxValue );
		if( Type_Accessory == iType )
		{
			m_iAbilityMax = pNeck->GetAbilityMax();
			m_iAbilityGap = pNeck->GetAbilityGap();
			m_bAbilitySign = pNeck->GetAbilitySign();
			m_szReinforceResultText = pNeck->GetReinforceResultText();
		}
	}
	else if ( iSlot == ES_BRACELET )
	{
		const ioBraceletItem* pBracelet = ToBraceletItemConst( pItem );
		if ( !pBracelet )
			return;

		float fMAxValue = pBracelet->GetAbilityMax() / FLOAT1000;
		sprintf( szTooltip, pBracelet->GetReinforceText().c_str(), fRanNum, fMAxValue );
		if( Type_Accessory == iType )
		{
			m_iAbilityMax = pBracelet->GetAbilityMax();
			m_iAbilityGap = pBracelet->GetAbilityGap();
			m_bAbilitySign = pBracelet->GetAbilitySign();
			m_szReinforceResultText = pBracelet->GetReinforceResultText();
		}
	}
	m_szAccessoryTooltip[iType] = szTooltip;
	
	for( int i = 0; i < 2; i++ )
	{
		InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_REINFORCE_ACCESSORY_ICON + i ) );
		if( pIcon )
		{
			if( iType == i )
			{
				pIcon->SetIcon( g_UIImageSetMgr.CreateImageByFullName( m_Accessory[iType].m_IconName ), 0.9f, 0.9f );
				pIcon->SetActive();
			}
			pIcon->SetWndPos( 48, 119 + 92 * i );
			pIcon->ShowWnd();
		}
	}
	ShowChildWnd( ID_REINFORCE_MATERIAL_ICON );
	ShowChildWnd( ID_REINFORCE );
	HideChildWnd( ID_SHOW );
	HideChildWnd( ID_AGAIN );

	enum { DAY_SEC = 86400 };

	m_iFuncState = eSet;
	if( ( !m_bAbilitySign && m_iAbilityMax <= m_Accessory[Type_Accessory].m_iRandomNumber ) ||
		( m_bAbilitySign && m_iAbilityMax >= m_Accessory[Type_Accessory].m_iRandomNumber ) )
		m_iFuncState = eMax;
	else if( m_Accessory[Type_Material].m_iIndex == 0 )
		m_iFuncState = eNot;

	if( m_iFuncState == eSet )
		SetChildActive( ID_REINFORCE );
	else
		SetChildInActive( ID_REINFORCE );
}

void AccessoryFuncWnd::SetReinforceResult()
{
	m_iFuncState = eEnd;
	m_iCurEffectArray = 0;
	m_dwRollingEffectCheckTime = 0;

	HideChildWnd( ID_REINFORCE_ROLLING );
	ShowChildWnd( ID_SHOW );
	ShowChildWnd( ID_AGAIN );

	ioUserAccessory *pAccessory = g_MyInfo.GetUserAccessory();
	if( !pAccessory )
		return;

	if( !pAccessory->GetAccessorySlot( m_Accessory[Type_Accessory].m_iIndex, m_Accessory[Type_Accessory] ) )
		return;

	m_Accessory[Type_Material].Init();
	if( !m_szReinforceRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szReinforceRollingSound, 0 );
	if( !m_szReinforceResultSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szReinforceResultSound );
}

void AccessoryFuncWnd::OnAccessoryReinforceResult( int iIndex )
{
	m_iFuncState = eSendPacket;
	m_dwRollingEndTime = FRAMEGETTIME() + m_dwRollingTime;
	m_iCurEffectArray = 0;
	m_dwRollingEffectCheckTime = 0;

	HideChildWnd( ID_REINFORCE );
	HideChildWnd( ID_REINFORCE_MATERIAL_ICON );
	ShowChildWnd( ID_REINFORCE_ROLLING );

	if( !m_szReinforceRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szReinforceRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );
}

void AccessoryFuncWnd::SendReinforce()
{
	if( m_iFuncState != eSet )
		return;

	m_iFuncState = eSendPacket;
	m_dwRollingEndTime = 0;
	m_iCurEffectArray = 0;
	m_dwRollingEffectCheckTime = 0;

	HideChildWnd( ID_REINFORCE );
	HideChildWnd( ID_REINFORCE_MATERIAL_ICON );
	ShowChildWnd( ID_REINFORCE_ROLLING );
	InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_REINFORCE_ACCESSORY_ICON ) );
	if( pIcon )
		pIcon->SetWndPos( 103, 124 );

	TCPNetwork::MouseBusy( true );

	SP2Packet kPacket( CTPK_ACCESSORY_REINFORCE );
	kPacket << m_Accessory[Type_Accessory].m_iIndex;
	kPacket << m_Accessory[Type_Material].m_iIndex;
	TCPNetwork::SendToServer( kPacket );
}

void AccessoryFuncWnd::ResetAccessoryFuncTab()
{
	m_iTabState = ID_FUNC_TAB_BTN;
	m_dwRollingEndTime = 0;
	m_iFuncState = eReady;
	m_Accessory[Type_Accessory].Init();
	m_Accessory[Type_Material].Init();

	CheckRadioButton( ID_INFO_TAB_BTN, ID_FUNC_TAB_BTN, ID_FUNC_TAB_BTN );

	HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
	HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
	HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
	HideChildWnd( ID_COSTUME_SOLDIER_WND );
	HideChildWnd( ID_ACCESSORY_RING_BTN );
	HideChildWnd( ID_ACCESSORY_NECKLACE_BTN );
	HideChildWnd( ID_ACCESSORY_BRACELET_BTN );

	ShowChildWnd( ID_REINFORCE );
	SetChildInActive( ID_REINFORCE );
	HideChildWnd( ID_REINFORCE_ROLLING );
	HideChildWnd( ID_SHOW );
	HideChildWnd( ID_AGAIN );

	InventoryIconBtn *pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_REINFORCE_ACCESSORY_ICON ) );
	if( pIcon )
	{
		pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
		pIcon->ShowWnd();
		pIcon->SetInActive();
	}
	pIcon = dynamic_cast<InventoryIconBtn*>( FindChildWnd( ID_REINFORCE_MATERIAL_ICON ) );
	if( pIcon )
	{
		pIcon->SetIcon( NULL, FLOAT1, FLOAT1 );
		pIcon->ShowWnd();
		pIcon->SetInActive();
	}
}