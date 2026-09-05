
#include "StdAfx.h"
#include "ExtraItemFuncWnd.h"
#include "UISoldierSelectWnd.h"
#include "MyInventoryInfoWnd.h"
#include "ItemCompoundWnd.h"
#include "SelectMaterialItemBtn.h"
#include "ExpandMedalSlotWnd.h"
#include "../ioExtendSoundManager.h"

ExtraItemFuncWnd::ExtraItemFuncWnd()
{
	m_nReUseSlot = 0;
	m_iTabState = ID_INFO_TAB_BTN;
	m_iSelectExtraItemIndex = 0;
	m_iClassType = -1;
	m_dwExpandMedalSlotItemType = 0;

	m_pExtraSubBack = NULL;
	m_pManualMark = NULL;
	m_pGaugeMainBack = NULL;
	m_pGaugeBack  = NULL;
	m_pGauge      = NULL;
	m_pNewGauge      = NULL;
	m_pMagicCircle = NULL;
	m_pItemBack = NULL;
	m_pStripDark = NULL;
	m_pManual = NULL;
	m_pEffect = NULL;
	m_pBackEffect = NULL;
	m_pFinalBackEffect = NULL;

	InitData();
}

ExtraItemFuncWnd::~ExtraItemFuncWnd()
{
	SAFEDELETE( m_pExtraSubBack );
	SAFEDELETE( m_pManualMark );
	SAFEDELETE( m_pGaugeMainBack );
	SAFEDELETE( m_pGaugeBack );
	SAFEDELETE( m_pGauge );
	SAFEDELETE( m_pNewGauge );
	SAFEDELETE( m_pMagicCircle );
	SAFEDELETE( m_pItemBack );
	SAFEDELETE( m_pStripDark );
	SAFEDELETE( m_pEffect );
	SAFEDELETE( m_pBackEffect );
	SAFEDELETE( m_pFinalBackEffect );

	m_vRollingEffect.clear();
	m_vExtraRollingEffect.clear();
	m_vEndEfffect_S.clear();
	m_vEndEfffect_ExtraS.clear();
	m_vEndEfffect_F.clear();
}

void ExtraItemFuncWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/MyInventoryHelpWnd.xml", this );
}

void ExtraItemFuncWnd::iwm_show()
{
	for( int i = 0; i < MAX_INVENTORY; i++ )
	{
		ioWnd *pWnd = FindChildWnd( ID_WEAPON_ITEM_BTN + i );
		if( pWnd )
			pWnd->GoToTop();
	}
	ioWnd *pWnd = FindChildWnd( ID_REINFORCE_BTN );
	if( pWnd )
	{
		char szBuf[MAX_PATH]="";
		char szConvertNum[MAX_PATH] = "";
		Help::ConvertNumToStrComma( g_CompoundMaterialMgr.GetSystemReinforceNeedMoney(), szConvertNum, sizeof( szConvertNum ) );
		wsprintf( szBuf, m_szBtnText.c_str(), szConvertNum );
		pWnd->SetTitleText( szBuf );
	}
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SelectTab( m_iTabState );
}

void ExtraItemFuncWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );
}

void ExtraItemFuncWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_INFO_TAB_BTN:
	case ID_FUNC_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			if( m_iFuncState == eSendPacket )
			{
				CheckRadioButton( ID_INFO_TAB_BTN, ID_FUNC_TAB_BTN, m_iTabState );
				return;
			}
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
	case ID_WEAPON_ITEM_BTN:
	case ID_ARMOR_ITEM_BTN:
	case ID_HELMET_ITEM_BTN:
	case ID_CLOAK_ITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[2].c_str() );
			}
			else
			{
				int iSlot = dwID - ID_WEAPON_ITEM_BTN;
				int iSlotIndex = m_CharInfo.m_extra_item[iSlot];			

				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
				if( pInvenWnd )
				{
					if( iSlotIndex > 0 )
					{
						pInvenWnd->SetReleaseExtraItem( iSlotIndex );
					}
				}
			}
		}
		break;
	case ID_MEDAL_ITEM_1_BTN:
	case ID_MEDAL_ITEM_2_BTN:
	case ID_MEDAL_ITEM_3_BTN:
	case ID_MEDAL_ITEM_4_BTN:
	case ID_MEDAL_ITEM_5_BTN:
	case ID_MEDAL_ITEM_6_BTN:
		if( cmd == IOBN_BTNUP )
		{
			int iMedalItemType = 0;
			int iCustomIndex = 0;
			SlotIconBtn *pBtn = dynamic_cast< SlotIconBtn* > ( FindChildWnd( dwID ) );
			if( pBtn )
			{
				iMedalItemType = pBtn->GetMagicCode();
				iCustomIndex = pBtn->GetMagicCodeEx();
			}
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
			if( pInvenWnd )
			{
				if( iMedalItemType > 0 )
				{
					pInvenWnd->SetReleaseMedalItem( iMedalItemType, iCustomIndex );
				}
				else
				{
					SelectExpandMedalWnd( dwID );
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
	case ID_REINFORCE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemReinforce();
		}
		break;
	case ID_MATERIALITEM_BTN:
		if( cmd == IOBN_BTNUP )
		{
			//리스트 설정
			int nItemListSize = m_vTotalItemList.size();
			NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_MATERIALITEM_WND));
			if( pListWnd && pListWnd->IsShow() )
			{
				pListWnd->HideWnd();
			}
			else if( pListWnd && nItemListSize > 0 )
			{
				pListWnd->CreatePullDown( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 25 );
				// 조건이 달라지면 리스트 따로 구하고...
				vNewShopPullDownItem kItemList;
				GetMaterialList( kItemList );

				if( !kItemList.empty() )
				{
					int iSize = kItemList.size();
					for(int i = 0;i < iSize; i++)
					{
						pListWnd->InsertItem( kItemList[i] );
					}
					pListWnd->ShowWnd();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[5].c_str() );
				}
			}
		}
		break;
	case ID_MATERIALITEM_WND:
		if( cmd == IOBN_BTNUP )
		{
			//재료 선택 완료시
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				pPullDownWnd->HideWnd();
				MaterialListBtnUp( pPullDownWnd->GetOpenBtn(), param );
			}
		}
		break;
	case ID_FAIL_MANUAL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideChildWnd( ID_FAIL_TOOL_TIP );
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_nHelpTipIdx );
		}
		else if( cmd == IOWN_OVERED )
		{
			if( m_pManualMark )
				m_pManualMark->SetColor( 206, 78, 0 );

			ShowChildWnd( ID_FAIL_TOOL_TIP );
		}
		else if( cmd == IOWN_LEAVED )
		{
			if( m_pManualMark )
				m_pManualMark->SetColor( 128, 128, 128 );

			HideChildWnd( ID_FAIL_TOOL_TIP );
		}
		break;
	case ID_TARGET_ICON:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_FUNC_TAB_BTN );
		}
		break;
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_INFO_TAB_BTN );
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			SelectTab( ID_FUNC_TAB_BTN, true );
		}
		break;
	case MESSAGE_BOX4:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
				if( pInvenWnd && pInvenWnd->IsShow() )
				{
					if( m_dwExpandMedalSlotItemType > 0 )
					{
						ExpandMedalSlotWnd *pWnd = dynamic_cast<ExpandMedalSlotWnd*>( pInvenWnd->FindChildWnd(MyInventoryWnd::ID_EXPAND_MEDAL_SLOT_OPEN_WND) );
						if( pWnd )
							pWnd->SetClassType( m_iClassType );
						pInvenWnd->ShowExpandMedalSlotOpenWnd( m_dwExpandMedalSlotItemType );
					}
					else if( m_dwExpandMedalSlotItemType == 0 )
					{
						// 이 경우 상점으로 보낸다.
						NewShopWnd *pShopWnd = dynamic_cast<NewShopWnd*>(g_GUIMgr.FindWnd(NEW_SHOP_WND));
						if( pShopWnd )
						{
							pShopWnd->ShowEtcItemTab();
						}
					}
				}
			}
			m_dwExpandMedalSlotItemType = 0;
		}
		break;
	}
}

void ExtraItemFuncWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ExtraSubBack" )
	{
		SAFEDELETE( m_pExtraSubBack );
		m_pExtraSubBack = pImage;
	}
	else if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else if( szType == "MainBack" )
	{
		SAFEDELETE( m_pGaugeMainBack );
		m_pGaugeMainBack = pImage;
	}
	else if( szType == "GaugeBack" )
	{
		SAFEDELETE( m_pGaugeBack );
		m_pGaugeBack = pImage;
	}
	else if( szType == "Gauge" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else if( szType == "NewGauge" )
	{
		SAFEDELETE( m_pNewGauge );
		m_pNewGauge = pImage;
	}
	else if( szType == "magic_circle" )
	{
		SAFEDELETE( m_pMagicCircle );
		m_pMagicCircle = pImage;
	}
	else if( szType == "ItemBack" )
	{
		SAFEDELETE( m_pItemBack );
		m_pItemBack = pImage;
	}
	else if( szType == "strip_gray_dark" )
	{
		SAFEDELETE( m_pStripDark );
		m_pStripDark = pImage;
		m_pStripDark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pStripDark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pStripDark->SetSize( 205, 21 );
	}
	else if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else if( szType == "final_back_effect" )
	{
		SAFEDELETE( m_pFinalBackEffect );
		m_pFinalBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void ExtraItemFuncWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szSelectItemMsg = xElement.GetStringAttribute_e( "SelectItemMsg" );
	m_szLackMaterialMsg = xElement.GetStringAttribute_e( "LackMaterialMsg" );
	m_szRollingSound = xElement.GetStringAttribute_e( "rolling_sound" );
	m_szSuccessSound = xElement.GetStringAttribute_e( "success_sound" );
	m_szExtraSuccessSound = xElement.GetStringAttribute_e( "extra_success_sound" );
	m_szFailSound = xElement.GetStringAttribute_e( "fail_sound" );
	m_szExtraSubManual = xElement.GetStringAttribute_e( "ExtraSubManual" );
	m_szBtnText = xElement.GetStringAttribute_e( "BtnText" );

	m_nExtraSubManualIdx = xElement.GetIntAttribute_e( "extra_sub_manual" );
	m_nHelpTipIdx = xElement.GetIntAttribute_e( "help_tip_idx" );
	m_dwRollingTotalTime = xElement.GetIntAttribute_e( "rolling_duration" );
	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );
	m_dwIconScaleTime = xElement.GetIntAttribute_e( "Icon_Scale_Time" );

	m_fIconScale = xElement.GetFloatAttribute_e( "Icon_Scale" );

	for (int i = 0; i < 9; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "ErrMsg%d", i+1 );
		m_szErrMsg[i] = xElement.GetStringAttribute( szName );
	}
	for (int i = 0; i < 14; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "InfoText%d", i+1 );
		m_szInfoText[i] = xElement.GetStringAttribute( szName );
	}

	int iMax = 0;
	m_vRollingEffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Rolling_Effect" );
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

	m_vExtraRollingEffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Extra_Rolling_Effect" );
	m_vExtraRollingEffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Extra_Rolling_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vExtraRollingEffect.push_back( szEffect );
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

	m_vEndEfffect_ExtraS.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Extra_Success_Effect" );
	m_vEndEfffect_ExtraS.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Extra_Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_ExtraS.push_back( szEffect );
	}

	m_vEndEfffect_F.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Fail_Effect" );
	m_vEndEfffect_F.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Fail_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vEndEfffect_F.push_back( szEffect );
	}
}

void ExtraItemFuncWnd::InitData()
{
	m_vTotalItemList.clear();
	m_nCurReinforce = 0;
	m_fSuccessRate = 0.0f;
	m_bIsSelectItem = false;
	m_bIsSelectMaterial = false;
	m_nMaxFailExp = 0;
	m_fFailExpRate = 0.0f;
	m_nCurFailExp = 0;
	m_dwMagicEffectCreateTime = 0;

	SetChildInActive( ID_REINFORCE_BTN );
}

void ExtraItemFuncWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	if( m_iTabState != ID_FUNC_TAB_BTN )
		return;

	if( m_iFuncState == eReady )
	{
		OnRenderMagicCircle();

		if( m_pItemBack )
			m_pItemBack->Render( iXPos + 96, iYPos + 132 );
		if( m_pStripDark )
			m_pStripDark->Render( iXPos + 27, iYPos + 412, UI_RENDER_MULTIPLY );

		SuccessRateDescRender();
		RenderReadyFailExp( iXPos, iYPos );
		PrintManual( iXPos, iYPos, FONT_SIZE_12 );
	}
	else if( m_iFuncState == eSendPacket )
	{
		if( m_pBackEffect )
			m_pBackEffect->Render( iXPos + 128, iYPos + 165, UI_RENDER_MULTIPLY );

		if( m_pEffect )
		{
			m_pEffect->SetScale( 2.0f );
			m_pEffect->Render( iXPos + 128, iYPos + 165, UI_RENDER_ADD );
		}

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 27, iYPos + 266, FONT_SIZE_12, m_szInfoText[8].c_str() );

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 128, iYPos + 212, FONT_SIZE_17, m_ItemName.c_str() );
	}
	else
	{
		if( m_pFinalBackEffect )
			m_pFinalBackEffect->Render( iXPos + 128, iYPos + 165, UI_RENDER_MULTIPLY );

		if( m_pEffect )
		{
			m_pEffect->SetScale( 2.0f );
			m_pEffect->Render( iXPos + 128, iYPos + 165, UI_RENDER_ADD );
		}

		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos + 128, iYPos + 421, FONT_SIZE_17, m_szInfoText[7].c_str() );

		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 128, iYPos + 212, FONT_SIZE_17, m_ItemName.c_str() );

		char szBuf[MAX_PATH] = "";
		if( m_bSuccess )
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			int iGap = m_nResultReinforce - m_nPreReinforce;
			if ( m_nResultReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				SafeSprintf( szBuf, sizeof( szBuf ), m_szInfoText[9].c_str(), iGap );
			else
				SafeSprintf( szBuf, sizeof( szBuf ), m_szInfoText[10].c_str(), iGap );
		}
		else
		{
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			if ( m_nResultReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
				SafeSprintf( szBuf, sizeof( szBuf ), m_szInfoText[11].c_str() );
			else
				SafeSprintf( szBuf, sizeof( szBuf ), m_szInfoText[12].c_str() );
		}
		g_FontMgr.PrintText( iXPos + 128, iYPos + 235, FONT_SIZE_17, szBuf );
		RenderResultFailExp( iXPos, iYPos );
	}
}

void ExtraItemFuncWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioWnd::OnProcess( fTimePerSec );

	if( m_iTabState != ID_FUNC_TAB_BTN )
		return;

	if( m_iFuncState == eReady )
		ProcessMagicCircle();
	else
	{
		RollingEffect( fTimePerSec );
		UpdateReinforceInfo();
	}
}

void ExtraItemFuncWnd::CreateMagicCircle()
{
	DWORD dwCurTime = FRAMEGETTIME();

	MagicEffectInfo kInfo;
	kInfo.m_dwCreateTime = dwCurTime;

	m_vMagicEffectInfoList.push_back( kInfo );
}

void ExtraItemFuncWnd::ProcessMagicCircle()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwMagicEffectCreateTime == 0 ||
		m_dwMagicEffectCreateTime+1000 < dwCurTime )
	{
		CreateMagicCircle();
		m_dwMagicEffectCreateTime = dwCurTime;
	}

	// process
	vMagicEffectInfo::iterator iter = m_vMagicEffectInfoList.begin();
	while( iter != m_vMagicEffectInfoList.end() )
	{
		DWORD dwGap = dwCurTime - (*iter).m_dwCreateTime;
		if( dwGap >= 1500 )
		{
			iter = m_vMagicEffectInfoList.erase( iter );
			continue;
		}

		float fRate = FLOAT1 - ((float)dwGap / 1500.0f);
		float fCurEffectRate = (float)sin(D3DX_PI * FLOAT05 * fRate);

		(*iter).m_iCurAlphaRate = (float)MAX_ALPHA_RATE * (0.5f*fCurEffectRate);
		(*iter).m_fCurScaleRate = 1.25f - (0.25f*fCurEffectRate);

		++iter;
	}
}

void ExtraItemFuncWnd::OnRenderMagicCircle()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pMagicCircle )
	{
		m_pMagicCircle->SetAlpha( MAX_ALPHA_RATE );
		m_pMagicCircle->SetScale( FLOAT1 );
		m_pMagicCircle->Render( iXPos + 129, iYPos + 205 );

		// 퍼지는 이펙트
		vMagicEffectInfo::iterator iter = m_vMagicEffectInfoList.begin();
		while( iter != m_vMagicEffectInfoList.end() )
		{
			m_pMagicCircle->SetAlpha( (*iter).m_iCurAlphaRate );
			m_pMagicCircle->SetScale( (*iter).m_fCurScaleRate );
			m_pMagicCircle->Render( iXPos + 129, iYPos + 205 );

			++iter;
		}
	}
}

void ExtraItemFuncWnd::SuccessRateDescRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum 
	{
		INFO_LEFT_X_OFFSET = 33,
		INFO_LEFT_Y_OFFSET = 415,

		INFO_RIGHT_X_OFFSET = 225,
		INFO_RIGHT_Y_OFFSET = 415,
	};

	if( m_bIsSelectMaterial )
	{
		if ( m_iSelectExtraItemIndex > 0)
		{
			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, m_szInfoText[2].c_str() );

			g_FontMgr.SetAlignType( TAT_RIGHT );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos + INFO_RIGHT_X_OFFSET, iYPos + INFO_RIGHT_Y_OFFSET, FONT_SIZE_12, "%.2f％", m_fSuccessRate );
		}
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_RED );

		if( m_bIsSelectItem )
			g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, m_szLackMaterialMsg.c_str() );
		else
			g_FontMgr.PrintText( iXPos + INFO_LEFT_X_OFFSET, iYPos + INFO_LEFT_Y_OFFSET, FONT_SIZE_12, m_szSelectItemMsg.c_str() );
	}
}

void ExtraItemFuncWnd::RenderReadyFailExp( int iXPos, int iYPos )
{
	if( !m_pManualMark || !m_pGaugeMainBack || !m_pGaugeBack || !m_pGauge || m_nMaxFailExp <= 0 )
		return;

	enum 
	{		
		GAUGE_X_OFFSET = 57,
		GAUGE_Y_OFFSET = 392,
		GAUGE_BACK_OFFSET = 2,

		TEXT_X_OFFSET  = 130,
		TEXT_Y_OFFSET  = 374,

		MANUAL_X_OFFSET = 207,
		MANUAL_Y_OFFSET = 390,
	};
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, m_szInfoText[3].c_str() );	
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );		
	kPrinter.AddTextPiece( FONT_SIZE_12, " %.2f％", m_fFailExpRate * FLOAT100 );	
	kPrinter.PrintFullText( iXPos + TEXT_X_OFFSET, iYPos + TEXT_Y_OFFSET, TAT_CENTER );
	kPrinter.ClearList();

	//게이지
	int nGaugeWidth = m_pGauge->GetWidth() * (float)min( FLOAT1, m_fFailExpRate );
	m_pGaugeMainBack->Render( iXPos + GAUGE_X_OFFSET - GAUGE_BACK_OFFSET, iYPos + GAUGE_Y_OFFSET - GAUGE_BACK_OFFSET );
	m_pGaugeBack->Render( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET );
	m_pGauge->RenderWidthCut( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET, 0, nGaugeWidth );
	m_pManualMark->Render( iXPos + MANUAL_X_OFFSET, iYPos + MANUAL_Y_OFFSET, UI_RENDER_NORMAL_ADD_COLOR );
}

void ExtraItemFuncWnd::RenderResultFailExp( int iXPos, int iYPos )
{
	if( !m_pGaugeMainBack || !m_pGaugeBack || !m_pNewGauge || !m_pGauge )
		return;

	enum 
	{		
		GAUGE_X_OFFSET = 57,
		GAUGE_Y_OFFSET = 277,
		GAUGE_BACK_OFFSET = 2,

		TEXT_X_OFFSET1 = 130,
		TEXT_Y_OFFSET1 = 259,

		TEXT_X_OFFSET2  = 130,
		TEXT_Y_OFFSET2  = 259,
	};
	
	//실패시 문구
	if ( !m_bSuccess )
	{
		if ( m_nMaxFailExp > 0 )
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, m_szInfoText[3].c_str() );	
			kPrinter.AddTextPiece( FONT_SIZE_12, " %.2f％", m_fNewFailExpRate * FLOAT100 );	
			kPrinter.PrintFullText( iXPos + TEXT_X_OFFSET1, iYPos + TEXT_Y_OFFSET1, TAT_CENTER );
			kPrinter.ClearList();	
		
			//게이지
			int nNewGaugeWidth = m_pNewGauge->GetWidth() * (float)min( FLOAT1, m_fNewFailExpRate );
			int nPreGaugeWidth = m_pNewGauge->GetWidth() * (float)min( FLOAT1, m_fFailExpRate );

			//렌더 순서
			m_pGaugeMainBack->Render( iXPos + GAUGE_X_OFFSET - GAUGE_BACK_OFFSET, iYPos + GAUGE_Y_OFFSET - GAUGE_BACK_OFFSET );
			m_pGaugeBack->Render( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET );
			m_pNewGauge->RenderWidthCut( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET, 0, nNewGaugeWidth );
			m_pGauge->RenderWidthCut( iXPos + GAUGE_X_OFFSET, iYPos + GAUGE_Y_OFFSET, 0, nPreGaugeWidth );
		}
		else
		{
			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_RED );
			kPrinter.AddTextPiece( FONT_SIZE_17, m_szInfoText[13].c_str(), g_CompoundMaterialMgr.GetFixedFailReinforce() );
			kPrinter.PrintFullText( iXPos + TEXT_X_OFFSET2, iYPos + TEXT_Y_OFFSET2, TAT_CENTER );
			kPrinter.ClearList();	
		}
	}	
}

void ExtraItemFuncWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	enum
	{
		DEFAULT_MANUAL_X_OFFSET = 28,
		DEFAULT_MANUAL_Y_OFFSET = 319,

		EXTRA_SUB_MANUAL_X_OFFSET = 28,
		EXTRA_SUB_MANUAL_Y_OFFSET = 361,

		EXTRA_SUB_BACK_X_OFFSET = 27,
		EXTRA_SUB_BACK_Y_OFFSET = 360,
	};

	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
	if( !pItem ) 
		return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	int nXpos = a_iXPos + DEFAULT_MANUAL_X_OFFSET;
	int nYpos = a_iYPos + DEFAULT_MANUAL_Y_OFFSET;

	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		nYpos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( nXpos, nYpos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}

	if ( m_bIsSelectItem )
	{
		if ( m_nCurReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
		{
			if( m_pExtraSubBack )
				m_pExtraSubBack->Render( a_iXPos+EXTRA_SUB_BACK_X_OFFSET, a_iYPos+EXTRA_SUB_BACK_Y_OFFSET );

			IoString str;
			str.Set(0, 0, m_szExtraSubManual.c_str(), fScale, 204, TS_NORMAL, TCT_DEFAULT_DARKGRAY, 0, TAT_LEFT, TVA_TOP, 4);
			str.OnRender( a_iXPos + EXTRA_SUB_MANUAL_X_OFFSET, a_iYPos + EXTRA_SUB_MANUAL_Y_OFFSET );
		}
	}
}

void ExtraItemFuncWnd::SetExtraItemInfo( int iClassType )
{
	if( iClassType == -1 )
		m_iClassType = g_MyInfo.GetClassType();
	else
		m_iClassType = iClassType;

	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );
	else
		Help::GetDefaultCharInfo( m_CharInfo, m_iClassType );

	for( int i = 0; i < MAX_INVENTORY; i++ )
		m_CharInfo.m_EquipCostume[i].m_CostumeCode = 0;

	// 코스튬 관련 (장비 장착시 캐릭터)
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetWndPos( 54, 72 );
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, false, true );
		pCharWnd->SetUserInfoToolTip( true );
	}

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
		return;
		
	for( int i = 0; i < MAX_INVENTORY; i++ )
	{
		SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*>( FindChildWnd( ID_WEAPON_ITEM_BTN + i ) );
		if( !pBtn )
			continue;

		int iSlotIndex = m_CharInfo.m_extra_item[i];
		int iReinforce = 0;
		EXTRAITEMSLOT kSlot;
		ioUIRenderImage *pImg = NULL;
		if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
		{
			DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( m_CharInfo.m_class_type );
			ioCharRentalData *pCharRentalData = g_MyInfo.GetCharRentalData();
			if( pCharRentalData && pCharRentalData->GetEquipExtraItem( dwCharIndex, kSlot, 0 ) )
			{
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				iReinforce = kSlot.m_iReinforce;

				pBtn->SetIcon( pImg, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, kSlot.m_PeriodType, iReinforce, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
				pBtn->SetMagicCode( -1 - i );
				pBtn->SetMagicCodeEx( kSlot.m_iItemCode );
				pBtn->SetMagicCodeEx2( kSlot.m_iReinforce );
			}
			else
			{
				pBtn->SetIcon( pImg, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, kSlot.m_PeriodType, iReinforce, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
				pBtn->SetMagicCode( 0 );
			}
		}
		else if( pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
		{
			pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
			iReinforce = kSlot.m_iReinforce;

			pBtn->SetIcon( pImg, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, kSlot.m_PeriodType, iReinforce, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
			pBtn->SetMagicCode( iSlotIndex );
		}
		else
		{
			pBtn->SetIcon( pImg, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, kSlot.m_PeriodType, iReinforce, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
			pBtn->SetMagicCode( 0 );
		}

		const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
		if( pItem )
			pBtn->SetGradeType( pItem->GetGradeType() );
	}

	// medal
	MedalSyncInfoVec vItemTypeVec;
	g_MyInfo.GetMedalItemTypeVec( vItemTypeVec, m_iClassType );

	int iSlotNum = g_MedalItemMgr.GetSlotNum( g_MyInfo.GetClassLevel( m_iClassType, false ) );
	int iExpandMedalSlotNum = g_ExpandMedalSlotMgr.GetExpandMedalSlotNum( m_iClassType );
	int iCurMedal = 0;
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*> ( FindChildWnd( ID_MEDAL_ITEM_1_BTN + i ) );
		if( !pBtn )
			continue;

		if( i < iSlotNum  )
		{
			ioUIRenderImage *pImg = NULL;
			ioUIRenderImage *pSubImg = NULL;
			pBtn->SetMagicCode( 0 ); // 초기화
			pBtn->SetMagicCodeEx( 0 );

			char szTitle[MAX_PATH]="";
			if( COMPARE( i, 0, (int) vItemTypeVec.size() ) )
			{
				const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vItemTypeVec[iCurMedal].m_iItemType );
				if( !pItemInfo )
					continue;
				pImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
				pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );
				pBtn->SetMagicCode( vItemTypeVec[iCurMedal].m_iItemType );
				pBtn->SetMagicCodeEx( vItemTypeVec[iCurMedal].m_iCustomIndex );
				StringCbCopy( szTitle, sizeof( szTitle ), pItemInfo->m_sName.c_str() );
				iCurMedal++;
			}
			else
			{
				SafeSprintf( szTitle, sizeof( szTitle ), m_szInfoText[0].c_str(), i + 1 );
			}

			pBtn->SetIcon( pImg, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, 0 );
			pBtn->SetSubIcon( pSubImg );
			pBtn->SetActive();
			pBtn->SetLockIcon( false );
			pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
			pBtn->SetTitleText( szTitle );
			ioUITitle *pTitle = pBtn->GetTitle();
			if( pTitle )
			{
				pTitle->SetWidthCutSize(59.0f);
				pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
			}
		}
		else if( iExpandMedalSlotNum > 0 )
		{
			ioUIRenderImage *pImg = NULL;
			ioUIRenderImage *pSubImg = NULL;
			pBtn->SetMagicCode( 0 ); // 초기화

			ioExpandMedalSlotManager::ExpandMedalSlotVec vExpandMedalSlot;
			vExpandMedalSlot.clear();
			int iExpandMedalCount = g_ExpandMedalSlotMgr.GetExpandMedalSlot( m_iClassType, vExpandMedalSlot );
			bool bExpand = false;
			char szTitle[MAX_PATH]="";
			for( int j=0; j<iExpandMedalCount; ++j )
			{
				bool bUpdateText = false;
				if( vExpandMedalSlot[j].m_iSlotNumber == i )
				{
					if( iCurMedal < (int)vItemTypeVec.size() )
					{
						const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vItemTypeVec[iCurMedal].m_iItemType );
						if( !pItemInfo )
							continue;

						pImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
						pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );
						pBtn->SetMagicCode( vItemTypeVec[iCurMedal].m_iItemType );
						pBtn->SetMagicCodeEx( vItemTypeVec[iCurMedal].m_iCustomIndex );
						StringCbCopy( szTitle, sizeof( szTitle ), pItemInfo->m_sName.c_str() );
						iCurMedal++;
					}
					else
					{
						pBtn->SetMagicCode( 0 );
						pBtn->SetMagicCodeEx( 0 );
						SafeSprintf( szTitle, sizeof( szTitle ), m_szInfoText[0].c_str(), i + 1 );
					}
					bExpand = true;
					bUpdateText = true;
				}

				if( bUpdateText )
				{
					if( vExpandMedalSlot[j].IsMortmain() )
					{
						pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
					}
					else
					{
						pBtn->SetTitleColor( 0xFFFF0000, 0 );
						g_ExpandMedalSlotMgr.CheckRemainTime( vExpandMedalSlot[j].m_iClassType, vExpandMedalSlot[j].m_iSlotNumber, szTitle, sizeof(szTitle) );
					}
				}
			}

			if( bExpand )
			{
				// 오픈되어 있으나 메달은 없는 슬롯.
				pBtn->SetIcon( pImg, MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, 0 );
				pBtn->SetSubIcon( pSubImg );
				pBtn->SetActive();
				pBtn->SetLockIcon( false );
				pBtn->SetTitleText( szTitle );
				ioUITitle *pTitle = pBtn->GetTitle();
				if( pTitle )
				{
					pTitle->SetWidthCutSize(59.0f);
					pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
				}
			}
			else
			{
				// 오픈 되지 않은 슬롯.
				pBtn->SetMagicCode( 0 );
				pBtn->SetIcon( NULL , MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, 0 );
				pBtn->SetActive();
				pBtn->SetLockIcon( true );
				char szTitle[MAX_PATH]="";
				SafeSprintf( szTitle, sizeof( szTitle ), m_szInfoText[1].c_str(), g_MedalItemMgr.GetLevelOver( i + 1 ) );
				pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
				pBtn->SetTitleText( szTitle );
			}
		}
		else
		{
			pBtn->SetMagicCode( 0 );
			pBtn->SetIcon( NULL , MY_INVENTORY_INFO_EXTRA_SCALE, MY_INVENTORY_INFO_BACK_SCALE, MY_INVENTORY_INFO_DISABLE_SCALE, 0 );
			pBtn->SetActive();
			pBtn->SetLockIcon( true );
			char szTitle[MAX_PATH]="";
			SafeSprintf( szTitle, sizeof( szTitle ), m_szInfoText[1].c_str(), g_MedalItemMgr.GetLevelOver( i + 1 ) );
			pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
			pBtn->SetTitleText( szTitle );
		}
	}

	// UI코스튬 관련 (스몰 코스튬 버튼)
	for( int i=0; i<MAX_INVENTORY; i++ )
	{
		if( m_CharInfo.m_EquipCostume[i].m_CostumeIdx > 0 )
		{
			if( m_iTabState == ID_INFO_TAB_BTN )
				ShowChildWnd( i+ID_WEAPON_COSTUME_SMALL_BTN );

			SetCostumeSubInfo( i+ID_WEAPON_COSTUME_SMALL_BTN, i, MY_INVENTORY_INFO_SMALL_COSTUME_SCALE, MY_INVENTORY_INFO_SMALL_COSTUME_BACK_SCALE, MY_INVENTORY_INFO_SMALL_COSTUME_DISABLE_SCALE );
		}
		else
			HideChildWnd( i+ID_WEAPON_COSTUME_SMALL_BTN );
	}
}

void ExtraItemFuncWnd::SetCostumeSubInfo( DWORD dwWndID, int nSlotNum, float fScale, float fBackScale, float fDisableScale )
{
	ioUserCostume *pCostume = g_MyInfo.GetUserCostume();
	if ( !pCostume )
		return;

	SlotIconBtn *pBtn = dynamic_cast<SlotIconBtn*>(FindChildWnd(dwWndID));
	if( !pBtn )
		return;

	if ( !COMPARE( nSlotNum, 0, MAX_INVENTORY ) )
		return;

	int nSlotIndex = m_CharInfo.m_EquipCostume[nSlotNum].m_CostumeIdx;
	ioUIRenderImage *pImg = NULL;

	CostumeSlot kSlot;	
	if( m_CharInfo.m_chExerciseStyle == EXERCISE_RENTAL )
	{
		DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( m_CharInfo.m_class_type );
		ioCharRentalData *pCharRentalData = g_MyInfo.GetCharRentalData();
		if( pCharRentalData && pCharRentalData->GetEquipCostume( dwCharIndex, kSlot, nSlotNum ) )
		{
			pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );

			pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType, 0, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom, true );
			pBtn->SetMagicCode( -(nSlotNum+1) );
			pBtn->SetMagicCodeEx( kSlot.m_nCostumeCode );
		}
		else
		{
			pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType, 0, kSlot.m_dwMaleCustom, kSlot.m_dwFemaleCustom );
			pBtn->SetMagicCode( 0 );
		}
	}
	else if( nSlotIndex > 0 && pCostume->GetCostumeSlot( nSlotIndex, kSlot ) )
	{
		pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
		pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType, 0, 0, 0, true );
		pBtn->SetMagicCode( nSlotIndex );
	}
	else
	{
		pBtn->SetIcon( pImg, fScale, fBackScale, fDisableScale, kSlot.m_PeriodType );
		pBtn->SetMagicCode( 0 );
	}

	pBtn->SetGradeType( g_CostumeInfoMgr.GetGradeType( kSlot.m_nCostumeCode ) );
}

void ExtraItemFuncWnd::SelectExpandMedalWnd( DWORD dwId )
{
	if( !COMPARE( dwId, ID_MEDAL_ITEM_1_BTN, ID_MEDAL_ITEM_6_BTN+1 ) )
		return;

	int iSlotNumber = 0;
	switch( dwId )
	{
	case ID_MEDAL_ITEM_1_BTN:
		return;
	case ID_MEDAL_ITEM_2_BTN:
	case ID_MEDAL_ITEM_3_BTN:
	case ID_MEDAL_ITEM_4_BTN:
	case ID_MEDAL_ITEM_5_BTN:
	case ID_MEDAL_ITEM_6_BTN:
		iSlotNumber = dwId - ID_MEDAL_ITEM_1_BTN;
	}

	if( iSlotNumber < g_MedalItemMgr.GetSlotNum( g_MyInfo.GetClassLevel( m_iClassType, true ) ) )
	{
		return;
	}

	if( !g_MyInfo.IsCharMortmain( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[0].c_str() );
		return;
	}
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( g_GUIMgr.FindWnd(MY_INVENTORY_WND) );
	if( pInvenWnd )
	{
		ioPlayStage* pStage = pInvenWnd->GetPlayStage();
		if( pStage )
		{
			bool bError = false;
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				if( pStage->GetModeType() != MT_MYROOM )
					bError = true;
			}
			else
			{
				if( pStage->GetModeType() != MT_TRAINING && pStage->GetModeType() != MT_HEADQUARTERS && pStage->GetModeType() != MT_HOUSE )
					bError = true;				
			}
						
			if( bError )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[1].c_str() );
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[3].c_str() );
				}
			}
		}
	}

	if( g_ExpandMedalSlotMgr.CheckExpandMedalSlot( m_iClassType, iSlotNumber ) )
		return;

	g_GUIMgr.SetExpandMedalSlotMsgBox( NULL, this, iSlotNumber );
}

void ExtraItemFuncWnd::SetExpandMedalSlotItemData( DWORD dwItemIdx )
{
	m_dwExpandMedalSlotItemType = dwItemIdx;
}

void ExtraItemFuncWnd::UISoldierSelectWndBtnUp( int iSoldierArray )
{
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( GetParent() );
	if( !pInvenWnd )
		return;
	
	int iClassType = g_MyInfo.GetClassType( iSoldierArray );
	if( iClassType == -1 )
		return;
	
	if( g_MyInfo.IsCharExerciseStyle( iSoldierArray, EXERCISE_PCROOM ) && pInvenWnd->GetCurTabID() == MyInventoryWnd::ID_DECO_TAB_BTN )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[4].c_str() );
		return;
	}

	pInvenWnd->ChangeClassType( iClassType );
	SetExtraItemInfo( iClassType );
}

void ExtraItemFuncWnd::SelectTab( DWORD dwID, bool bReUse )
{
	m_iTabState = dwID;
	m_iFuncState = eReady;
	if( !bReUse )
		m_nReUseSlot = 0;

	HideChildWnd( ID_MATERIALITEM_WND );
	HideChildWnd( ID_FAIL_MANUAL_BTN );
	HideChildWnd( ID_FAIL_TOOL_TIP );
	HideChildWnd( ID_REINFORCE_ROLLING );
	HideChildWnd( ID_USE );
	HideChildWnd( ID_GO );
	for( int i = 0; i < MAX_INVENTORY; i++ )
		HideChildWnd( ID_WEAPON_COSTUME_SMALL_BTN + i );

	CheckRadioButton( ID_INFO_TAB_BTN, ID_FUNC_TAB_BTN, dwID );
	if( m_iTabState == ID_INFO_TAB_BTN )
	{
		SetExtraItemInfo( m_iClassType );

		ShowChildWnd( ID_MY_SOLDIER_LIST_BTN );
		ShowChildWnd( ID_MY_SOLDIER_LEFT_BTN );
		ShowChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
		ShowChildWnd( ID_SOLDIER_WND );

		HideChildWnd( ID_REINFORCE_BTN );
		HideChildWnd( ID_TARGET_ICON );
		HideChildWnd( ID_MATERIALITEM_BTN );
		HideChildWnd( ID_MATERIALITEM_ICON );

		for( int i = 0; i < MAX_INVENTORY; i++ )
			ShowChildWnd( ID_WEAPON_ITEM_BTN + i );
		for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
			ShowChildWnd( ID_MEDAL_ITEM_1_BTN + i );
	}
	else
	{
		HideChildWnd( ID_MY_SOLDIER_LIST_BTN );
		HideChildWnd( ID_MY_SOLDIER_LEFT_BTN );
		HideChildWnd( ID_MY_SOLDIER_RIGHT_BTN );
		HideChildWnd( ID_SOLDIER_WND );

		ShowChildWnd( ID_REINFORCE_BTN );
		ShowChildWnd( ID_TARGET_ICON );

		for( int i = 0; i < MAX_INVENTORY; i++ )
			HideChildWnd( ID_WEAPON_ITEM_BTN + i );
		for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
			HideChildWnd( ID_MEDAL_ITEM_1_BTN + i );

		//강화 가능 아이템 얻기	
		InitData();
		LoadTotalItemList();

		//재료 초기화
		SelectMaterialItemBtn *pMaterialItemIcon = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(ID_MATERIALITEM_ICON));
		if( pMaterialItemIcon )
		{
			pMaterialItemIcon->InitData();
			pMaterialItemIcon->ShowWnd();
		}

		SelectExtraItemBtn *pMaterialItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_MATERIALITEM_BTN));
		if( pMaterialItemBtn )
		{
			pMaterialItemBtn->SetSelectMaterial( 0, "" );
			pMaterialItemBtn->SetCurTitleColor( TCT_DEFAULT_DARKGRAY );
			pMaterialItemBtn->ShowWnd();
		}

		//다시 사용 가능하다면 슬롯에 재설정
		if( m_nReUseSlot > 0 )
		{
			bool bMaxReinforce = false;
			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kSlot;
				if( pExtraItem->GetExtraItem( m_nReUseSlot, kSlot ) )
				{
					if( kSlot.m_iReinforce >= g_CompoundMaterialMgr.GetMaxReinforce() )
						bMaxReinforce = true;
				}
			}

			if( SlotReload() && !bMaxReinforce )
				UpdateItemData( bReUse );
			else 
				TargetItemInit();
		}
		else 
			TargetItemInit();	
	}
}

void ExtraItemFuncWnd::LoadTotalItemList()
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
	{
		HideWnd();
		return;
	}

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		MaterialCompoundTargetItemInfo kInfo;
		kInfo.m_iSlotIndex = kSlot.m_iIndex;
		kInfo.m_iItemCode = kSlot.m_iItemCode;
		kInfo.m_iReinforce = kSlot.m_iReinforce;

		m_vTotalItemList.push_back( kInfo );
	}

	std::sort( m_vTotalItemList.begin(), m_vTotalItemList.end(), MaterialCompoundItemInfoSort() );
}

bool ExtraItemFuncWnd::SlotReload()
{
	vNewShopPullDownItem kItemList;
	GetTargetList( kItemList );	

	vNewShopPullDownItem::iterator iter = kItemList.begin();
	for( ; iter != kItemList.end(); ++iter )
	{
		const NewShopPullDownItem& rkNewShopItem = *iter;
		if( rkNewShopItem.m_iMagicCode == m_nReUseSlot )
		{			
			m_iSelectExtraItemIndex = m_nReUseSlot;
			m_nReUseSlot = 0;
			return true;
		}
	}

	return false;
}

void ExtraItemFuncWnd::UpdateItemData( bool bReUse )
{
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iSelectExtraItemIndex, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pIconBtn )
			{
				m_ItemName = kSlot.m_Name;	
				m_nItemCode = kSlot.m_iItemCode;

				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1, kSlot.m_iReinforce );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				pIconBtn->SetActive();

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					pIconBtn->SetGradeType( pItem->GetGradeType() );
			}

			//현재 강화 레벨 설정
			m_nCurReinforce = kSlot.m_iReinforce;	

			bool bPrevAdditive = m_bSelectAdditive;
			m_bIsSelectItem = true;
			m_bIsSelectMaterial = false;
			m_bSelectAdditive = false;

			//실패 경험치 얻기
			m_nMaxFailExp = g_CompoundMaterialMgr.GetMaxFailExp( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
			m_nCurFailExp = (int)kSlot.m_failExp;

			if ( m_nMaxFailExp > 0 )
			{
				m_fFailExpRate = ( (float)m_nCurFailExp / m_nMaxFailExp );
				ShowChildWnd( ID_FAIL_MANUAL_BTN );
			}
			else
				HideChildWnd( ID_FAIL_MANUAL_BTN );

			m_fFailExpRate = min( FLOAT1, m_fFailExpRate );

			SelectMaterialItemBtn *pMaterialItemIcon = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(ID_MATERIALITEM_ICON));
			if( !pMaterialItemIcon )
				return;

			SelectExtraItemBtn *pMaterialItemBtn = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_MATERIALITEM_BTN));
			if( pMaterialItemBtn )
			{
				pMaterialItemBtn->SetInActive();
				pMaterialItemBtn->SetSelectMaterial( 0, "" );
				pMaterialItemBtn->SetCurTitleColor( TCT_DEFAULT_DARKGRAY );
			}

			int iHavePeso = g_MyInfo.GetMoney();
			int iNeedPeso = g_CompoundMaterialMgr.GetNeedPeso( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
			int iSysNeedMoney = g_CompoundMaterialMgr.GetSystemReinforceNeedMoney();

			ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
			if( !pUserEtc )
				return;

			int iHaveCount = 0;
			ioUserEtcItem::ETCITEMSLOT rkEtcItem;
			if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, rkEtcItem ) )
				iHaveCount = rkEtcItem.m_iValue1;

			int iNeedCount = g_CompoundMaterialMgr.GetNeedMaterialCount( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
			if( !bPrevAdditive && bReUse && iHavePeso >= iNeedPeso + iSysNeedMoney && iNeedPeso > 0 )
			{
				MaterialListBtnUp( pMaterialItemBtn, 1 );
				pMaterialItemIcon->SetData( 0, iNeedPeso );
				m_bIsSelectMaterial = true;
				ApplySuccessRate();
				SetChildActive( ID_MATERIALITEM_BTN );
			}
			else if( iHaveCount >= iNeedCount && iHavePeso >= iSysNeedMoney )
			{
				MaterialListBtnUp( pMaterialItemBtn, 0 );
				pMaterialItemIcon->SetData( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iNeedCount );
				m_bIsSelectMaterial = true;
				m_bSelectAdditive = true;
				ApplySuccessRate();
				SetChildActive( ID_MATERIALITEM_BTN );
			}
			else if( m_nCurReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
				pMaterialItemIcon->SetRecommand( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iNeedCount );
			else if( iHavePeso >= iNeedPeso + iSysNeedMoney )
			{
				MaterialListBtnUp( pMaterialItemBtn, 1 );
				pMaterialItemIcon->SetData( 0, iNeedPeso );
				m_bIsSelectMaterial = true;
				ApplySuccessRate();
				SetChildActive( ID_MATERIALITEM_BTN );
			}
			else
				pMaterialItemIcon->SetRecommand( 0, iNeedPeso );

			//경고 문구 출력
			int nFixedReinforce = g_CompoundMaterialMgr.GetFixedFailReinforce();
			if ( m_nCurReinforce >= nFixedReinforce )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szInfoText[4].c_str(), nFixedReinforce, nFixedReinforce );
		}
	}
	if( m_bIsSelectMaterial )
		SetChildActive( ID_REINFORCE_BTN );
	else
		SetChildInActive( ID_REINFORCE_BTN );
}

void ExtraItemFuncWnd::TargetItemInit()
{
	InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
	if( pTargetIconBtn )
		pTargetIconBtn->SetIcon( NULL, FLOAT1, FLOAT1 );

	SetChildInActive( ID_MATERIALITEM_BTN );
}

void ExtraItemFuncWnd::GetTargetList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iSize = m_vTotalItemList.size();
	rkList.reserve( iSize );

	std::sort( m_vTotalItemList.begin(), m_vTotalItemList.end(), MaterialCompoundItemInfoSort() );

	for( int i=0; i < iSize; ++i )
	{
		EXTRAITEMSLOT kTargetSlot;
		if( !pItem->GetExtraItem( m_vTotalItemList[i].m_iSlotIndex, kTargetSlot ) )
			continue;

		if( kTargetSlot.m_iReinforce >= g_CompoundMaterialMgr.GetMaxReinforce() )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = kTargetSlot.m_iIndex;

		char szName[MAX_PATH]="";
		if( kTargetSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), m_szInfoText[5].c_str(), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kTargetSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), m_szInfoText[6].c_str(), kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kTargetSlot.m_Name.c_str(), kTargetSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName = szName;
		rkList.push_back( kItem );
	}
}

void ExtraItemFuncWnd::MaterialListBtnUp( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetMaterialList( kItemList );	

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		iListArray = kItemList.size() - 1;		

	ioUITitle *pTitle = pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	int nSelectMaterialCode = rkItem.m_iMagicCode;
	SetMaterialCode( nSelectMaterialCode );

	SelectExtraItemBtn *pTarget = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd(ID_MATERIALITEM_BTN));
	if( pTarget )
	{
		pTarget->SetSelectMaterial( nSelectMaterialCode, rkItem.m_szName );
		pTarget->SetCurTitleColor( rkItem.m_dwTextColor );
	}
}

void ExtraItemFuncWnd::ApplySuccessRate()
{	
	float fSuccessRate = 0.0f;

	//최대 실패경험치가 존재할 때
	if ( m_nMaxFailExp > 0 )
	{
		//성공율 얻기
		fSuccessRate = g_CompoundMaterialMgr.GetSuccessConst( ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
		fSuccessRate = ((float)( m_nCurFailExp + fSuccessRate ) / m_nMaxFailExp * FLOAT100 );
	}
	else if ( m_nMaxFailExp == 0 )
		fSuccessRate = g_CompoundMaterialMgr.GetReinforceSuccessRate( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );

	fSuccessRate += g_CompoundMaterialMgr.GetPCRoomBonusReinforceSuccessRate( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );

	m_fSuccessRate = fSuccessRate;

	m_fSuccessRate = min( FLOAT100, m_fSuccessRate );
	m_fSuccessRate = max( 0.0f, m_fSuccessRate );
}

void ExtraItemFuncWnd::GetMaterialList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return;

	char szCnt[MAX_PATH]="";	

	int iHaveCount = 0;
	ioUserEtcItem::ETCITEMSLOT rkEtcItem;
	if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, rkEtcItem ) )
		iHaveCount = rkEtcItem.m_iValue1;

	int iHavePeso = g_MyInfo.GetMoney();
	int iSysNeedMoney = g_CompoundMaterialMgr.GetSystemReinforceNeedMoney();
	int iNeedCount = g_CompoundMaterialMgr.GetNeedMaterialCount( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
	if( iHaveCount >= iNeedCount && iHavePeso >= iSysNeedMoney )
	{
		ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE );
		if( !pItem ) 
			return;

		NewShopPullDownItem kItem;
		wsprintf_e( szCnt, "%d개", iHaveCount );

		kItem.m_dwTextColor = TCT_DEFAULT_GREEN;
		kItem.m_dwCntTextColor = TCT_DEFAULT_GREEN;
		kItem.m_szName      = pItem->GetName();
		kItem.m_iMagicCode	= ioEtcItem::EIT_ETC_ADDICTIVE_PIECE;
		kItem.m_bCount = true;
		kItem.m_szCnt = szCnt;
		rkList.push_back( kItem );
	}

	if( m_nCurReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
		return;
		
	int iNeedPeso = g_CompoundMaterialMgr.GetNeedPeso( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
	if( iHavePeso >= iNeedPeso + iSysNeedMoney && iNeedPeso > 0 )
	{
		NewShopPullDownItem kItem;
		Help::ConvertNumToStrComma( iNeedPeso, szCnt, sizeof(szCnt) );

		kItem.m_dwTextColor = TCT_DEFAULT_GREEN;
		kItem.m_dwCntTextColor = TCT_DEFAULT_GREEN;
		kItem.m_szName      = "페소";
		kItem.m_iMagicCode	= 0;
		kItem.m_bCount = true;
		kItem.m_szCnt = szCnt;
		rkList.push_back( kItem );
	}
}

void ExtraItemFuncWnd::SetMaterialCode( int nMaterialCode )
{
	if ( !m_bIsSelectItem )
		return;

	SelectMaterialItemBtn *pMaterialItemIcon = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(ID_MATERIALITEM_ICON));
	if( !pMaterialItemIcon )
		return;

	if( nMaterialCode == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE )
	{
		ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
		if( !pUserEtc )
			return;

		int iHaveCount = 0;
		ioUserEtcItem::ETCITEMSLOT rkEtcItem;
		if( pUserEtc->GetEtcItem( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, rkEtcItem ) )
			iHaveCount = rkEtcItem.m_iValue1;

		int iNeedCount = g_CompoundMaterialMgr.GetNeedMaterialCount( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );
		if( iHaveCount >= iNeedCount )
		{
			pMaterialItemIcon->SetData( ioEtcItem::EIT_ETC_ADDICTIVE_PIECE, iNeedCount );
			m_bIsSelectMaterial = true;
			m_bSelectAdditive = true;
			ApplySuccessRate();
		}
	}
	else
	{
		int iHavePeso = g_MyInfo.GetMoney();
		int iNeedPeso = g_CompoundMaterialMgr.GetNeedPeso( m_nCurReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );

		if( iHavePeso >= iNeedPeso )
		{
			pMaterialItemIcon->SetData( 0, iNeedPeso );
			m_bIsSelectMaterial = true;
			m_bSelectAdditive = false;
			ApplySuccessRate();
		}
	}
	if( m_bIsSelectMaterial )
		SetChildActive( ID_REINFORCE_BTN );
	else
		SetChildInActive( ID_REINFORCE_BTN );
}

void ExtraItemFuncWnd::SelectClickAction( int iIndex )
{
	if( m_iTabState == ID_INFO_TAB_BTN )
		return;
	if( m_iFuncState == eSendPacket || m_iFuncState == eEnd )
		return;
	
	if( iIndex > 0 )
	{
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( pExtraItem )
		{
			EXTRAITEMSLOT kSlot;
			if( pExtraItem->GetExtraItem( iIndex, kSlot ) )
			{
				if( kSlot.m_PeriodType != EPT_MORTMAIN )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[8].c_str() );
				else if( kSlot.m_iReinforce >= g_CompoundMaterialMgr.GetMaxReinforce() )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 강화수치가 최대입니다." );
				else
				{
					m_iSelectExtraItemIndex = iIndex;
					UpdateItemData();
				}
			}
		}
	}
}

bool ExtraItemFuncWnd::GetCurAlchemicItemInfo( OUT AlchemicToolTipInfo &rkInfo, IN DWORD dwItemInfoID )
{
	if( COMPARE(dwItemInfoID, ID_MATERIALITEM_ICON, ID_MATERIALITEM_ICON+1) )
	{
		SelectMaterialItemBtn *pItem = dynamic_cast<SelectMaterialItemBtn*>(FindChildWnd(dwItemInfoID));
		if( pItem )
		{
			int nCode = pItem->GetCode();
			if( nCode <= 0 )
				return false;

			rkInfo.m_iCode = nCode;
			rkInfo.m_iType = AlchemicItemToolTip::ATT_ETCITEM;
			return true;
		}
	}
	return false;
}

void ExtraItemFuncWnd::SendItemReinforce()
{
	if( m_iFuncState != eReady )
		return;
	if( !m_bIsSelectMaterial )
		return;

	if ( m_iSelectExtraItemIndex <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[6].c_str() );
		return;
	}

	bool bTarget = false;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		EXTRAITEMSLOT kSlot;
		if( pExtraItem->GetExtraItem( m_iSelectExtraItemIndex, kSlot ) )
			bTarget = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, m_szErrMsg[7].c_str() );
		return;
	}

	m_nReUseSlot = m_iSelectExtraItemIndex;

	int nFixedReinforce = g_CompoundMaterialMgr.GetFixedFailReinforce();
	SP2Packet kPacket( CTPK_EXTRAITEM_REINFORCE );
	kPacket << m_iSelectExtraItemIndex;
	if ( m_nCurReinforce >= nFixedReinforce )
		kPacket << (int)MATERIAL_SPIRIT;
	else
	{
		if( m_bSelectAdditive )
			kPacket << (int)MATERIAL_SPIRIT;
		else
			kPacket << (int)MATERIAL_PESO;
	}

	TCPNetwork::MouseBusy( true );
	TCPNetwork::SendToServer( kPacket );
}

void ExtraItemFuncWnd::SetReinforceResultInfo( bool bSuccess, int nTargetSlot, int nReinforce )
{
	m_iFuncState = eSendPacket;
	m_bSuccess = bSuccess;
	m_iSelectExtraItemIndex = nTargetSlot;
	m_nPreReinforce = m_nCurReinforce;
	m_nResultReinforce = nReinforce;	

	SAFEDELETE(m_pEffect);

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingEffectCheckTime = 0;
	m_iCurEffectArray = -1;

	HideChildWnd( ID_REINFORCE_BTN );
	HideChildWnd( ID_MATERIALITEM_BTN );
	HideChildWnd( ID_MATERIALITEM_WND );
	HideChildWnd( ID_MATERIALITEM_ICON );
	HideChildWnd( ID_FAIL_MANUAL_BTN );
	HideChildWnd( ID_FAIL_TOOL_TIP );
	ShowChildWnd( ID_REINFORCE_ROLLING );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	g_QuestMgr.QuestCompleteTermReserve( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_nResultReinforce, m_nItemCode );

#if defined( USE_GA )
	char chLabel[32] = {0,};

	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chLabel, "%d", m_nItemCode );
	else
		SafeSprintf( chLabel, sizeof(chLabel), "%1", m_nItemCode );

	// ITEM_ENHANCE_MATERIAL
	g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
		, "Item"
		, "Material"
		, chLabel
		, 1
		, "%2FITEM%2FENHANCE%2FMATERIAL" );
#endif
}

void ExtraItemFuncWnd::SetFailExp( int nPreFailExp, int nCurFailExp )
{
	//최대 실패 경험치 얻기
	m_nMaxFailExp = g_CompoundMaterialMgr.GetMaxFailExp( m_nResultReinforce, ioEtcItem::EIT_ETC_MATERIAL_COMPOUND_001 );

	if( m_nMaxFailExp > 0 )
	{
		m_fNewFailExpRate = ( (float)nCurFailExp / m_nMaxFailExp );
		m_fFailExpRate = ( (float)nPreFailExp / m_nMaxFailExp );
	}

	m_fNewFailExpRate = min( FLOAT1, m_fNewFailExpRate );
	m_fFailExpRate = min( FLOAT1, m_fFailExpRate );
}

void ExtraItemFuncWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = 0;
	if( m_iFuncState == eEnd )
	{
		if( m_bSuccess )
		{
			if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
				iEffectListCnt = m_vEndEfffect_S.size();
			else 
				iEffectListCnt = m_vEndEfffect_ExtraS.size();
		}
		else
			iEffectListCnt = m_vEndEfffect_F.size();
	}
	else
	{
		if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
			iEffectListCnt = m_vRollingEffect.size();
		else
			iEffectListCnt = m_vExtraRollingEffect.size();
	}
	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_iFuncState == eEnd )
	{
		//최대가 되면 이펙트 삭제
		if( m_iCurEffectArray == iEffectListCnt - 1 )
		{
			SAFEDELETE( m_pEffect );
			return;
		}

		if( m_dwRollingEffectCheckTime == 0 || m_dwRollingEffectCheckTime + m_dwRollingEffectTime < dwCurTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect;
			if( m_bSuccess )
			{
				if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
					szEffect = m_vEndEfffect_S[m_iCurEffectArray];
				else 
					szEffect = m_vEndEfffect_ExtraS[m_iCurEffectArray];
			}
			else
				szEffect = m_vEndEfffect_F[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
	else
	{
		if( m_dwRollingEffectCheckTime == 0 || m_dwRollingEffectCheckTime + m_dwRollingEffectTime < dwCurTime )		// 교체
		{
			SAFEDELETE( m_pEffect );

			m_iCurEffectArray++;

			if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
				m_iCurEffectArray = 0;

			ioHashString szEffect = "";
			if ( m_nPreReinforce < g_CompoundMaterialMgr.GetFixedFailReinforce() )
				szEffect= m_vRollingEffect[m_iCurEffectArray];
			else
				szEffect= m_vExtraRollingEffect[m_iCurEffectArray];

			if( !szEffect.IsEmpty() )
				m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );

			m_dwRollingEffectCheckTime = dwCurTime;
		}
	}
}

void ExtraItemFuncWnd::UpdateReinforceInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();
	
	// RollingEnd
	if( m_dwRollingEndTime < dwCurTime )			
	{
		if( m_iFuncState == eSendPacket )
		{
			m_dwScaleStartTime = dwCurTime;

			HideChildWnd( ID_REINFORCE_ROLLING );
			ShowChildWnd( ID_USE );
			ShowChildWnd( ID_GO );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			if( m_bSuccess )
			{
				if( g_ExSoundMgr.IsSound( ExSound::EST_REINFORCE_SUCCESS ) )
				{
					g_ExSoundMgr.PlaySound( ExSound::EST_REINFORCE_SUCCESS );
				}
				else
				{
					if( !m_szSuccessSound.IsEmpty() && m_nResultReinforce <= g_CompoundMaterialMgr.GetFixedFailReinforce() )
						g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
					else if( !m_szExtraSuccessSound.IsEmpty() && m_nResultReinforce > g_CompoundMaterialMgr.GetFixedFailReinforce() )
						g_SoundMgr.PlaySound( m_szExtraSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
				}
			}
			else
			{
				if( g_ExSoundMgr.IsSound( ExSound::EST_REINFORCE_FAIL ) )
				{
					g_ExSoundMgr.PlaySound( ExSound::EST_REINFORCE_FAIL );
				}
				else
				{
					if( !m_szFailSound.IsEmpty() )
						g_SoundMgr.PlaySound( m_szFailSound, DSBVOLUME_MAX, PSM_THREAD );
				}
			}
			InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pTargetIconBtn )
				pTargetIconBtn->SetNum( m_nResultReinforce );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>( GetParent() );
			if( pInvenWnd && pInvenWnd->IsShow() )
				pInvenWnd->UpdateTab( false, false );

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
			m_iFuncState = eEnd;
			g_QuestMgr.QuestCompleteTerm( QC_EXTRA_ITEM_REINFORCE_SUCCESS, m_nResultReinforce, m_nItemCode );
		}
		else
		{
			float fScale = FLOAT1;
			DWORD dwGapTime = dwCurTime - m_dwScaleStartTime;
			if( dwGapTime < m_dwIconScaleTime )
			{
				float fScaleRate = (float)dwGapTime / (float)m_dwIconScaleTime;
				fScale = m_fIconScale * ( FLOAT1 - fScaleRate ) + FLOAT1 * fScaleRate;
			}		
			InventoryIconBtn *pTargetIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_TARGET_ICON));
			if( pTargetIconBtn )
				pTargetIconBtn->SetScale( fScale );
		}
	}
}


	// 2019-04-30 
void ExtraItemFuncWnd::SetRollingStopSound()
{
	g_SoundMgr.StopSound(m_szRollingSound,0);
}
