
#include "StdAfx.h"

#include "ExpandMedalSlotWnd.h"
//////////////////////////////////////////////////////////////////////////////
SelectSoldierListBtn::SelectSoldierListBtn()
{
	m_iCurMagicCode = 0;
	m_szSelectText.Clear();
}

SelectSoldierListBtn::~SelectSoldierListBtn()
{
}

void SelectSoldierListBtn::iwm_show()
{
	m_iCurMagicCode = 0;
	m_szSelectText.Clear();
}

void SelectSoldierListBtn::InitData()
{
	m_iCurMagicCode = 0;
	m_szSelectText.Clear();
}

void SelectSoldierListBtn::SetListBtnInfo( ioHashString szTitle, int iMagicCode )
{
	m_iCurMagicCode = iMagicCode;
	m_szSelectText = szTitle;
}

void SelectSoldierListBtn::OnRender()
{
	ioButton::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_iCurMagicCode > 0 )
		SetTitleText( m_szSelectText.c_str() );
	else
		SetTitleText( m_szDefaultText.c_str() );
}

void SelectSoldierListBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_szDefaultText = xElement.GetStringAttribute_e( "DescText" );
}
//////////////////////////////////////////////////////////////////////////////
MedalSlotBtn::MedalSlotBtn(void)
{
	m_pIcon = NULL;
	m_pSubIcon = NULL;
	m_pIconBack = NULL;
	m_pIconDisable = NULL;

	m_fIconScale = FLOAT1;
	m_fIconBackScale = FLOAT1;
	m_fIconDisableScale = FLOAT1;
}

MedalSlotBtn::~MedalSlotBtn(void)
{
	SAFEDELETE(m_pIcon);
	SAFEDELETE(m_pSubIcon);
	SAFEDELETE(m_pIconBack);
	SAFEDELETE(m_pIconDisable);

	m_fIconScale = FLOAT1;
	m_fIconBackScale = FLOAT1;
	m_fIconDisableScale = FLOAT1;
}

void MedalSlotBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE(m_pIconBack);
		m_pIconBack = pImage;
	}
	else if( szType == "IconDisable" )
	{
		SAFEDELETE(m_pIconDisable);
		m_pIconDisable = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}
}

void MedalSlotBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void MedalSlotBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void MedalSlotBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );

	RenderBack( iXPos, iYPos );
	RenderIcon( iXPos, iYPos , UI_RENDER_NORMAL );
}

void MedalSlotBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );

	RenderIconDisable( iXPos, iYPos );
}

void MedalSlotBtn::RenderIcon(int iXPos, int iYPos, UIRenderType eType)
{
	if( !m_pIcon )	return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	m_pIcon->SetScale( m_fIconScale );
	if( m_fIconScale != FLOAT1 )
		m_pIcon->Render( iNewXPos, iNewYPos, eType, TFO_BILINEAR );
	else
		m_pIcon->Render( iNewXPos, iNewYPos, eType );

	if( m_pSubIcon )
	{
		m_pSubIcon->SetScale( m_fIconScale );
		m_pSubIcon->Render( iNewXPos, iNewYPos, eType, TFO_BILINEAR );
	}
}

void MedalSlotBtn::RenderBack(int iXPos, int iYPos )
{
	if( !m_pIconBack )	return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	m_pIconBack->SetScale( m_fIconBackScale );
	if( m_fIconBackScale != FLOAT1 )
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
	else
		m_pIconBack->Render( iNewXPos, iNewYPos, UI_RENDER_NORMAL );
}

void MedalSlotBtn::RenderIconDisable( int iXPos, int iYPos )
{
	if( !m_pIconDisable )	return;

	int iNewXPos = iXPos + GetWidth() / 2;
	int iNewYPos = iYPos + GetHeight() / 2;

	m_pIconDisable->SetScale( m_fIconDisableScale );

	if( m_fIconDisableScale != FLOAT1 )
		m_pIconDisable->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	else
		m_pIconDisable->Render( iNewXPos, iNewYPos, UI_RENDER_MULTIPLY );
}

void MedalSlotBtn::SetIcon( ioUIRenderImage* pIcon, float fIconScale, float fIconBackScale, float fIconDisableScale )
{
	SAFEDELETE( m_pIcon );
	m_pIcon = pIcon;

	m_fIconScale = fIconScale;
	m_fIconBackScale = fIconBackScale;
	m_fIconDisableScale = fIconDisableScale;
}

void MedalSlotBtn::SetSubIcon( ioUIRenderImage *pIcon )
{
	SAFEDELETE( m_pSubIcon );
	m_pSubIcon = pIcon;
}
//////////////////////////////////////////////////////////////////////////////
ExpandMedalSlotWnd::ExpandMedalSlotWnd(void)
{
	m_iClassType = 0;
	m_dwItemIndex = 0;
	m_iUseLevel = 0;

	m_CharInfo.Init();

	m_pEffect = NULL;
	m_EffectPos.Init();
}

ExpandMedalSlotWnd::~ExpandMedalSlotWnd(void)
{
	SAFEDELETE( m_pEffect );
	m_EffectPos.Init();
}

void ExpandMedalSlotWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/MyInventoryHelpWnd.xml", this );
}

void ExpandMedalSlotWnd::iwm_show()
{
	SelectSoldierListBtn *pClassBtn = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
	{
		pClassBtn->InitData();
		if( m_iClassType != 0 )
		{
			pClassBtn->SetListBtnInfo( g_MyInfo.GetClassName(m_iClassType), m_iClassType );
			pClassBtn->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType ) ), 0 );
		}
	}

	ioButton *pBtn = dynamic_cast<ioButton*>(FindChildWnd(ID_SELECT_BTN));
	if( pBtn )
	{
		char szTitle[MAX_PATH] = "";
		SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_iUseLevel );
		pBtn->SetTitleText( szTitle );
	}

	HideChildWnd( ID_CLASS_LIST_WND );

	UpdateData();
}

void ExpandMedalSlotWnd::iwm_hide()
{
	SelectSoldierListBtn *pClassBtn = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
		pClassBtn->InitData();

	m_iClassType = 0;

	HideChildWnd( ID_CLASS_LIST_WND );
	HideChildWnd( ID_SOLDIER_WND );
}

void ExpandMedalSlotWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHANGE_CLASS_BTN:
		if( cmd == IOBN_BTNUP )
		{
			vNewShopPullDownItem kItemList;
			GetClassList( kItemList );

			NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_CLASS_LIST_WND));
			if( pListWnd && pListWnd->IsShow() )
			{
				pListWnd->HideWnd();
			}
			else if( pListWnd )
			{
				// 일단 생성, ShowWnd 이후 결정된 wndsize로 위치를 수정한다.
				pListWnd->CreatePullDown( pWnd, pWnd->GetXPos(), pWnd->GetYPos() + 24 );

				if( !kItemList.empty() )
				{
					int iSize = kItemList.size();
					for(int i = 0;i < iSize; i++)
					{
						pListWnd->InsertItem( kItemList[i] );
					}
					pListWnd->ShowWnd();

					int iHeight = pListWnd->GetHeight();
					int iEndYPos = pListWnd->GetYPos() + iHeight;
					if( iEndYPos >= GetHeight() )
					{
						int iNewYPos = pWnd->GetYPos() - iHeight - 1;
						pListWnd->SetWndPos( pWnd->GetXPos(), iNewYPos );
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
			}

			UpdateData();
		}
		break;
	case ID_CLASS_LIST_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				SetClassSelect( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
			UpdateData();
		}
		break;
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( CheckEnableSlot() )
			{
				CheckSelectBtn();
			}
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendExpandMedalSlotOpen();
				HideWnd();
			}
		}
		break;
	}
}

void ExpandMedalSlotWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ShowChildWnd( ID_MEDAL_ITEM_1_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_2_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_3_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_4_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_5_BTN );
	ShowChildWnd( ID_MEDAL_ITEM_6_BTN );

	// Effect
	if( m_pEffect && m_EffectPos.x > 0 && m_EffectPos.y > 0 )
	{
		m_pEffect->SetScale( 1.25f );
		m_pEffect->Render( iXPos + m_EffectPos.x - 1, iYPos + m_EffectPos.y - 1, UI_RENDER_ADD );
	}
}

void ExpandMedalSlotWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	SelectSoldierListBtn *pClassBtn = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn && pClassBtn->GetListMagicCode() > 0 )
	{
		RollingEffect( fTimePerSec );
	}
}

void ExpandMedalSlotWnd::UpdateData()
{
	SelectSoldierListBtn *pClassBtn = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
	{
		if( m_iClassType > 0 && pClassBtn->GetListMagicCode() <= 0 )
		{
			pClassBtn->SetListBtnInfo( g_MyInfo.GetClassName(m_iClassType), m_iClassType );
			pClassBtn->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType ) ), 0 );
		}
		else if( m_iClassType == 0 )
		{
			vNewShopPullDownItem kItemList;
			GetClassList( kItemList );
			if( !kItemList.empty() )
			{
				NewShopPullDownItem &rkItem = kItemList[0];
				m_iClassType = rkItem.m_iMagicCode;
				pClassBtn->SetListBtnInfo( g_MyInfo.GetClassName(m_iClassType), m_iClassType );
				pClassBtn->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType ) ), 0 );
			}
		}
		else
			m_iClassType = pClassBtn->GetListMagicCode();
	}

	int iCharArray = g_MyInfo.GetClassArray( m_iClassType );
	if( COMPARE( iCharArray, 0, g_MyInfo.GetCharCount() ) )
		m_CharInfo = g_MyInfo.GetCharacter( iCharArray );

	if( iCharArray > -1 )
	{
		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
		{
			pCharWnd->SetWndPos( 47, 6 );
			pCharWnd->SetZPosition( 600.0f );
			pCharWnd->SetChar( m_CharInfo );
			pCharWnd->SetEquip( false, false, false );
			pCharWnd->SetUserInfoToolTip( true );
			pCharWnd->ShowWnd();
		}
	}

	// 3D 캐릭터 창이 버튼을 가리므로 따로 처리한다.
	MyInventoryWnd *pParent = dynamic_cast<MyInventoryWnd*>( GetParent() );
	if( pParent )
	{
		ioButton *pBtn = dynamic_cast<ioButton*>( pParent->FindChildWnd(MyInventoryWnd::ID_CHARGE_GOLD_BTN) );
		if( pBtn && pBtn->IsShow() )
			pBtn->GoToTop();

		pBtn = dynamic_cast<ioButton*>( pParent->FindChildWnd(MyInventoryWnd::ID_SHOP_BTN) );
		if( pBtn && pBtn->IsShow() )
			pBtn->GoToTop();
	}

	// 메달 정보 셋팅
	MedalSyncInfoVec vItemTypeVec;
	g_MyInfo.GetMedalItemTypeVec( vItemTypeVec, m_iClassType );

	if( m_iClassType <= 0 )
		vItemTypeVec.clear();
	
	ioExpandMedalSlotManager::ExpandMedalSlotVec vExpandMedalSlot;
	vExpandMedalSlot.clear();
	int iExpandMedalSlotNum = g_ExpandMedalSlotMgr.GetExpandMedalSlot( m_iClassType, vExpandMedalSlot );
	int iSlotNum = g_MedalItemMgr.GetSlotNum( g_MyInfo.GetClassLevel(m_iClassType,false) );

	int iTotalSlotNum = iSlotNum + iExpandMedalSlotNum;
	int iCurMedal = 0;
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		MedalSlotBtn *pBtn = dynamic_cast<MedalSlotBtn*> ( FindChildWnd( ID_MEDAL_ITEM_1_BTN + i ) );
		if( !pBtn )
			continue;

		if( i < iSlotNum  )
		{
			ioUIRenderImage *pIcon = NULL;
			ioUIRenderImage *pSubIcon = NULL;
			char szTitle[MAX_PATH]="";
			if( COMPARE( i, 0, (int) vItemTypeVec.size() ) )
			{
				const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vItemTypeVec[iCurMedal].m_iItemType );
				if( !pItemInfo )
					continue;
				pIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
				pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );
				StringCbCopy( szTitle, sizeof( szTitle ), pItemInfo->m_sName.c_str() );
				iCurMedal++;
			}
			else
			{
				SafeSprintf( szTitle, sizeof( szTitle ), STR(1), i+1 );
			}

			pBtn->SetIcon( pIcon, MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
			pBtn->SetSubIcon( pSubIcon );
			pBtn->SetActive();
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
			ioUIRenderImage *pIcon = NULL;
			ioUIRenderImage *pSubIcon = NULL;
			char szTitle[MAX_PATH]="";
			bool bExpand = false;
			for( int j=0; j<iExpandMedalSlotNum; ++j )
			{
				if( vExpandMedalSlot[j].m_iSlotNumber == i )
				{
					if( COMPARE( iCurMedal, 0, (int) vItemTypeVec.size() ) )
					{
						const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( vItemTypeVec[iCurMedal].m_iItemType );
						if( !pItemInfo )	continue;
						pIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sIcon );
						pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pItemInfo->m_sSubIcon );

						if( vExpandMedalSlot[j].IsMortmain() )
						{
							pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
							StringCbCopy( szTitle, sizeof( szTitle ), pItemInfo->m_sName.c_str() );
						}
						else
						{
							pBtn->SetTitleColor( 0xFFFF0000, 0 );
							g_ExpandMedalSlotMgr.CheckRemainTime( vExpandMedalSlot[j].m_iClassType, vExpandMedalSlot[j].m_iSlotNumber, szTitle, sizeof(szTitle) );
						}

						iCurMedal++;
					}
					else
					{
						if( vExpandMedalSlot[j].IsMortmain() )
						{
							pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
							SafeSprintf( szTitle, sizeof( szTitle ), STR(2), i+1 );
						}
						else
						{
							pBtn->SetTitleColor( 0xFFFF0000, 0 );
							g_ExpandMedalSlotMgr.CheckRemainTime( vExpandMedalSlot[j].m_iClassType, vExpandMedalSlot[j].m_iSlotNumber, szTitle, sizeof(szTitle) );
						}
					}
					bExpand = true;
				}
			}

			if( bExpand )
			{
				pBtn->SetIcon( pIcon, MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
				pBtn->SetSubIcon( pSubIcon );
				pBtn->SetActive();
			}
			else
			{
				pBtn->SetIcon( NULL , MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
				pBtn->SetInActive();
				SafeSprintf( szTitle, sizeof( szTitle ), STR(3), g_MedalItemMgr.GetLevelOver( i + 1 ) );
				pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
			}

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
			pBtn->SetIcon( NULL , MEDAL_ICON_SCALE, MEDAL_ICON_BACK_SCALE, MEDAL_ICON_DISABLE_SCALE );
			pBtn->SetInActive();
			char szTitle[MAX_PATH]="";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(4), g_MedalItemMgr.GetLevelOver( i + 1 ) );
			pBtn->SetTitleColor( 0xFF6E6E6E, 0 );
			pBtn->SetTitleText( szTitle );
		}
	}

	SAFEDELETE( m_pEffect );
}

void ExpandMedalSlotWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szRollingSound = xElement.GetStringAttribute_e( "rolling_sound" );

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
}

bool ExpandMedalSlotWnd::CheckEnableSlot()
{
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(GetParent());
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
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "메달슬롯 오픈은 광장/훈련소/개인본부에서만 가능합니다." );
				}
				return false;
			}
		}
	}

	SelectSoldierListBtn *pClassBtn = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	int iClassType = pClassBtn->GetListMagicCode();
	if( !pClassBtn || pClassBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return false;
	}

	if( g_MyInfo.IsCharRentalToClassType( iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return false;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return false;
	}	

	if( g_MyInfo.GetClassLevel( iClassType, true ) >= m_iUseLevel )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), m_iUseLevel );
		return false;
	}

	ioExpandMedalSlotManager::ExpandMedalSlotVec vExpandMedalSlotVec;
	bool bMortmain = false;
	int iCnt = g_ExpandMedalSlotMgr.GetExpandMedalSlot( iClassType, vExpandMedalSlotVec );
	for( int j=0; j<iCnt; ++j )
	{
		if( vExpandMedalSlotVec[j].m_iSlotNumber == m_iSlotNumber &&
			vExpandMedalSlotVec[j].IsMortmain() )
			bMortmain = true;
	}
	if( bMortmain )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return false;
	}

	return true;
}
void ExpandMedalSlotWnd::CheckSelectBtn()
{
	ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*>( g_EtcItemMgr.FindEtcItem( m_dwItemIndex ) );
	if( !pItem )
		return;

	int iUseLevel = pItem->GetLimitLevel();
	int iSlotNumber = pItem->GetUseSlotNumber();

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof(szTitle), STR(1), pItem->GetName().c_str() );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, szTitle );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(3), "", STR(4), STR(5) );
}

void ExpandMedalSlotWnd::GetClassList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ClassTypeInfoList vClassList;
	vClassList.clear();
	vClassList.reserve( g_ClassPrice.MaxActiveClassPrice() );

	int iMax = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i < iMax; i++)
	{
		int iCharArray = g_MyInfo.GetCharSlotIndexToArray( i );
		if( iCharArray == -1 )
			continue;

		// 체험 용병
		if( g_MyInfo.IsCharExercise( iCharArray ) )
			continue;

		int iSetIdx = g_MyInfo.GetClassType( iCharArray ) - 1;
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo )
			continue;

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
		if( iClassType <= 0 )
			continue;

		ClassTypeInfo kInfo;
		kInfo.m_iClassType = iClassType;
		kInfo.m_iCurLevel  = g_MyInfo.GetClassLevel( kInfo.m_iClassType, true );

		// 레벨 체크
		if( kInfo.m_iCurLevel >= m_iUseLevel )
			continue;

		// 영구로 열려 있는 확장 메달 슬롯
		ioExpandMedalSlotManager::ExpandMedalSlotVec vExpandMedalSlotVec;
		bool bMortmain = false;
		int iCnt = g_ExpandMedalSlotMgr.GetExpandMedalSlot( iClassType, vExpandMedalSlotVec );
		for( int j=0; j<iCnt; ++j )
		{
			if( vExpandMedalSlotVec[j].m_iSlotNumber == m_iSlotNumber &&
				vExpandMedalSlotVec[j].IsMortmain() )
				bMortmain = true;
		}
		if( bMortmain )
			continue;

		// 전투 체험 설정 용병.
		if( g_MyInfo.IsCharRentalToClassType( iClassType ) )
			continue;

		// 기간제 용병
		if( !g_MyInfo.IsCharMortmain( iClassType ) )
			continue;

		vClassList.push_back( kInfo );
	}

	std::sort( vClassList.begin(), vClassList.end(), ClassTypeInfoSort() );

	int iClassSize = vClassList.size();
	rkList.reserve( iClassSize );

	for( int i=0; i < iClassSize; ++i )
	{
		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = vClassList[i].m_iClassType;

		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "Lv%d %s", vClassList[i].m_iCurLevel, g_MyInfo.GetClassName(kItem.m_iMagicCode, g_MyInfo.GetCharPowerUpGradeByClassType( kItem.m_iMagicCode ) ) );
		kItem.m_dwTextColor = g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( kItem.m_iMagicCode ) );
		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void ExpandMedalSlotWnd::SetClassSelect( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetClassList( kItemList );

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) 
		return;			

	ioUITitle *pTitle =  pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[iListArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int iMagicCode = rkItem.m_iMagicCode;

	SelectSoldierListBtn *pTarget = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pTarget && iMagicCode > 0 )
	{
		pTarget->SetListBtnInfo( rkItem.m_szName, iMagicCode );
		pTarget->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iMagicCode ) ), 0 );
	}
}

void ExpandMedalSlotWnd::SetItemInfo( DWORD dwItemIndex )
{
	ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*>( g_EtcItemMgr.FindEtcItem( dwItemIndex ) );
	if( !pItem )
		return;

	m_dwItemIndex = dwItemIndex;
	m_iUseLevel = pItem->GetLimitLevel();
	m_iSlotNumber = pItem->GetUseSlotNumber();

	m_iCurEffectArray = -1;

	if( !COMPARE( m_iSlotNumber, 0, 6) )
		return;

	MedalSlotBtn *pBtn = dynamic_cast<MedalSlotBtn*> ( FindChildWnd( m_iSlotNumber + ID_MEDAL_ITEM_1_BTN ) );
	if( pBtn )
		m_EffectPos.SetPosition( pBtn->GetXPos() + pBtn->GetWidth()/2 , pBtn->GetYPos() + pBtn->GetHeight()/2 );
}

void ExpandMedalSlotWnd::SendExpandMedalSlotOpen()
{
	SelectSoldierListBtn *pClassBtn = dynamic_cast<SelectSoldierListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if ( !pClassBtn )
		return;

	m_iClassType = pClassBtn->GetListMagicCode();

	ioEtcItemExpandMedalSlot *pItem = dynamic_cast<ioEtcItemExpandMedalSlot*> ( g_EtcItemMgr.FindEtcItem( m_dwItemIndex ) );
	if( !pItem )	
		return;

	// 해당 아이템의 기간을 받아온다.
	DWORD dwLimitTime = pItem->GetLimitTime();

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwItemIndex;
	kPacket << m_iClassType;
	kPacket << m_iSlotNumber;
	kPacket << dwLimitTime;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

void ExpandMedalSlotWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = m_vRollingEffect.size();
	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_dwRollingEffectCheckTime == 0 || dwCurTime - m_dwRollingEffectCheckTime > m_dwRollingEffectTime )		// 교체
	{
		SAFEDELETE( m_pEffect );

		m_iCurEffectArray++;

		if( !COMPARE( m_iCurEffectArray, 0, iEffectListCnt ) )
			m_iCurEffectArray = 0;

		ioHashString szEffect = m_vRollingEffect[m_iCurEffectArray];

		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRollingEffectCheckTime = dwCurTime;
	}
}