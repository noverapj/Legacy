#include "StdAfx.h"

#include "InvenFourExtraCompoundWnd.h"
#include "ItemCompoundWnd.h"
#include "../ioVoiceChat/Fourier.h"

InvenFourExtraCompoundBuyWnd::InvenFourExtraCompoundBuyWnd(void)
{
	m_dwEtcItemType = 0;

	m_CharInfo.m_class_type = 1;
	m_CharInfo.m_kindred	= 1;
	m_CharInfo.m_sex		= 1;
	m_CharInfo.m_beard	    = 1;
	m_CharInfo.m_face	    = 1;
	m_CharInfo.m_hair	    = 1;
	m_CharInfo.m_skin_color = 1;
	m_CharInfo.m_hair_color = 1;
	m_CharInfo.m_accessories= 1;
	m_CharInfo.m_underwear  = 1;

	m_pManual     = NULL;
	m_pBackEffect = NULL;

	m_dwBackAniTime       = 0;
	m_dwBackAniSizePerGap = 0;
	m_dwBackAniAlphaGap   = 0;

	for (int i = 0; i < MAX_BACK_EFFECT ; i++)
	{
		m_dwBackAniCheckTime[i] = 0;
		m_fBackAniScale[i]      = 0.0f;
		m_byBackAniAlpha[i]     = 0;	
	}
	m_dwStartAniCheckTime = 0;

	m_bReleaseHelmet = false;
	m_bReleaseArmor  = false;
	m_bReleaseCloak  = false;
	m_bReleaseWeapon = false;
}

InvenFourExtraCompoundBuyWnd::~InvenFourExtraCompoundBuyWnd(void)
{
	SAFEDELETE( m_pBackEffect );
}

void InvenFourExtraCompoundBuyWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/FourExtraHelpWnd.xml", this );
}

void InvenFourExtraCompoundBuyWnd::iwm_show()
{
	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	// 버튼 세팅
	vNewShopPullDownItem kItemList;
	GetSoldierList( kItemList );

	if( !kItemList.empty() )
	{
		Help::GetDefaultCharInfo( m_CharInfo, kItemList[0].m_iMagicCode );
		SetBtnTitle( kItemList[0].m_iMagicCode );
	}

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, true, false );
		pCharWnd->SetCharActionAnimation( "public_change_pose.ani" );
		pCharWnd->ShowWnd();
	}

	ShowChildWnd( ID_CHAR_SELECT_BTN, true ); // btn이 뒤로 가는 현상 때문에 

	// init btn
	enum { MAX_ITEM = 4, };
	for (int i = 0; i < MAX_ITEM ; i++)
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_WEAPON_SELECT_BTN + i ));
		if( !pItem )
			continue;
		pItem->InitData();
	}

	for (int i = 0; i < MAX_ITEM ; i++)
	{
		SetIconBtn( ID_WEAPON_ICON_BTN + i, -1 ); // -1 : 아이콘 초기화
	}

	for (int i = 0; i < MAX_BACK_EFFECT ; i++)
	{
		m_dwBackAniCheckTime[i] = 0;
		m_fBackAniScale[i]      = 0.0f;
		m_byBackAniAlpha[i]     = 0;	
	}

	m_dwStartAniCheckTime = FRAMEGETTIME();
}

void InvenFourExtraCompoundBuyWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );
}

bool InvenFourExtraCompoundBuyWnd::IsHaveAllExtra( int iClassType )
{
	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem ) return false;

	enum { MAX_EXTRA_ITEM = 4, };
	IntVec vExtraItemCode;
	vExtraItemCode.reserve( MAX_EXTRA_ITEM );
	vExtraItemCode.push_back(  10000+iClassType );
	vExtraItemCode.push_back( 110000+iClassType );
	vExtraItemCode.push_back( 210000+iClassType );
	vExtraItemCode.push_back( 310000+iClassType );

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; i++ )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;

		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;

		if( kSlot.m_bCharEquip )
			continue;

		int iCodeSize = vExtraItemCode.size();
		if( iCodeSize == 0 )
			return true;

		for (int j = 0; j < iCodeSize ; j++)
		{
			if( kSlot.m_iItemCode == vExtraItemCode[j] )	
			{
				vExtraItemCode.erase( vExtraItemCode.begin() + j );
				break;
			}
		}
	}

	if( !vExtraItemCode.empty() )
		return false;

	return true;
}

void InvenFourExtraCompoundBuyWnd::GetSoldierList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	int iMaxClass = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i <  iMaxClass; i++)
	{
		//if( !g_ClassPrice.IsActiveClass( i ) ) continue;

		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo ) continue;

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;

		if( g_MyInfo.IsCharExerciseStyleToClassType( iClassType, EXERCISE_RENTAL ) == false )  // 대여 용병은 선택 가능
		{
			if( g_MyInfo.IsCharMortmain( iClassType ) ) continue;      // 영구 용병은 리스트에서 제외
		}

		ioEtcItemFourExtraCompound *pEtcItem = dynamic_cast< ioEtcItemFourExtraCompound* > (g_EtcItemMgr.FindEtcItem( m_dwEtcItemType ));
		if( pEtcItem )
		{
			if( !pEtcItem->IsRightSoldierCode( iClassType ) ) continue;
		}
		else
		{
			continue;
		}

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = iClassType;
		kItem.m_szName     = pInfo->GetName();
		if( IsHaveAllExtra( iClassType ) )
			kItem.m_dwTextColor= g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( kItem.m_iMagicCode ) );
		else
			kItem.m_dwTextColor= TCT_DEFAULT_GRAY;
		rkList.push_back( kItem );
	}

	if( rkList.empty() ) return;

	std::sort( rkList.begin() , rkList.end(), SoldierSort() );
}

void InvenFourExtraCompoundBuyWnd::GetExtraList( DWORD dwID, int iClassType, vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
	if( !pItem )
		return;

	int iExtraCode = 0;
	if( dwID == ID_WEAPON_SELECT_BTN )
		iExtraCode = 10000+iClassType;
	else if( dwID == ID_ARMOR_SELECT_BTN )
		iExtraCode = 110000+iClassType;
	else if( dwID == ID_HELMET_SELECT_BTN )
		iExtraCode = 210000+iClassType;
	else if( dwID == ID_CLOAK_SELECT_BTN )
		iExtraCode = 310000+iClassType;
	else
		return;

	int iSize = pItem->GetExtraItemCnt();
	for( int i=0; i < iSize; i++ )
	{
		EXTRAITEMSLOT kSlot;
		if( !pItem->GetExtraItemArray( i, kSlot ) )
			continue;
		if( kSlot.m_bCharEquip )
			continue;
		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;
		if( kSlot.m_PeriodType != EPT_MORTMAIN )
			continue;
		if( kSlot.m_iItemCode != iExtraCode )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = kSlot.m_iIndex;

		char szName[MAX_PATH]="";

		if( kSlot.m_iTradeState == EET_ENABLE )
		{
			SafeSprintf( szName, sizeof( szName ), STR(1), kSlot.m_Name.c_str(), kSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else if( kSlot.m_iTradeState == EET_NORMAL )
		{
			SafeSprintf( szName, sizeof( szName ), STR(2), kSlot.m_Name.c_str(), kSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", kSlot.m_Name.c_str(), kSlot.m_iReinforce );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void InvenFourExtraCompoundBuyWnd::SetPullDownMenu( ioWnd *pOpenBtn )
{
	if( !pOpenBtn ) return;

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(FindChildWnd( ID_PULLDOWN_WND ));
	if( pPullDownWnd )
	{
		if( pPullDownWnd->IsShow() )
			pPullDownWnd->HideWnd();
		else
		{
			pPullDownWnd->CreatePullDown( pOpenBtn, pOpenBtn->GetXPos(), pOpenBtn->GetYPos() + 24 );

			vNewShopPullDownItem kItemList;
			if( pOpenBtn->GetID() == ID_CHAR_SELECT_BTN )
				GetSoldierList( kItemList );
			else
				GetExtraList( pOpenBtn->GetID(), m_CharInfo.m_class_type, kItemList );

			for(int i = 0;i < (int)kItemList.size();i++)
			{
				pPullDownWnd->InsertItem( kItemList[i] );
			}
			pPullDownWnd->ShowWnd();

			int iHeight = pPullDownWnd->GetHeight();
			int iEndYPos = pPullDownWnd->GetYPos() + iHeight;
			if( iEndYPos >= GetHeight() )
			{
				int iNewYPos = pOpenBtn->GetYPos() - iHeight - 1;
				pPullDownWnd->SetWndPos( pOpenBtn->GetXPos(), iNewYPos );
			}

			if( pOpenBtn->GetID() != ID_CHAR_SELECT_BTN )
			{
				if( kItemList.empty() )
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
		}
	}
}

void InvenFourExtraCompoundBuyWnd::SetBtnTitle( int iClassType )
{
	ioWnd *pWnd = FindChildWnd( ID_CHAR_SELECT_BTN );
	if( !pWnd )
		return;

	pWnd->SetTitleText( g_MyInfo.GetClassName( iClassType ) );	
	pWnd->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) ), 0 );
}

void InvenFourExtraCompoundBuyWnd::SetIconBtn( DWORD dwID, int iSlotIdx  )
{
	if( !COMPARE( dwID, ID_WEAPON_ICON_BTN, ID_CLOAK_ICON_BTN + 1 ) ) 
		return;

	EXTRAITEMSLOT kSlot;
	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( pExtraItem )
	{
		if( pExtraItem->GetExtraItem( iSlotIdx, kSlot ) )
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( dwID ));
			if( pIconBtn )
			{
				ioUIRenderImage *pImg = NULL;
				pImg = g_UIImageSetMgr.CreateImageByFullName( kSlot.m_IconName );
				pIconBtn->SetIcon( pImg, 0.80f, 0.80f, kSlot.m_iReinforce );
				pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
				if( kSlot.m_dwFemaleCustom > 0 || kSlot.m_dwMaleCustom > 0 )
					pIconBtn->SetExtraItemCustom( true );
				else
					pIconBtn->SetExtraItemCustom( false );
			}
		}
		else
		{
			InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd( dwID ));
			if( pIconBtn )
				pIconBtn->SetIcon( NULL, 0.80f, 0.80f );
		}
	}
}

void InvenFourExtraCompoundBuyWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_BUY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendItemCompound();
		}
		break;
	case ID_CHAR_SELECT_BTN:
	case ID_WEAPON_SELECT_BTN:
	case ID_ARMOR_SELECT_BTN:
	case ID_HELMET_SELECT_BTN:
	case ID_CLOAK_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SetPullDownMenu( pWnd );
		}
		break;
	case ID_PULLDOWN_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				PullDownBtnUP( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
		}
		break;
	}
}

void InvenFourExtraCompoundBuyWnd::PullDownBtnUP( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) return;
	
	vNewShopPullDownItem kItemList;
	if( pOpenBtn->GetID() == ID_CHAR_SELECT_BTN )
		GetSoldierList( kItemList );
	else
		GetExtraList( pOpenBtn->GetID(), m_CharInfo.m_class_type, kItemList );

	if( !COMPARE( iListArray, 0, (int)kItemList.size() ) ) return;		
	

	if( pOpenBtn->GetID() == ID_CHAR_SELECT_BTN )
	{
		// btn
		if( m_iCurBtnUpClassType == kItemList[iListArray].m_iMagicCode ) return;

		m_iCurBtnUpClassType = kItemList[iListArray].m_iMagicCode;
		SetBtnTitle( kItemList[iListArray].m_iMagicCode );

		enum { MAX_ITEM = 4, };
		for (int i = 0; i < MAX_ITEM ; i++)
		{
			SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_WEAPON_SELECT_BTN + i ));
			if( !pItem )
				continue;
			pItem->InitData();
		}

		for (int i = 0; i < MAX_ITEM ; i++)
		{
			SetIconBtn( ID_WEAPON_ICON_BTN + i, -1 ); // -1 : 아이콘 초기화
		}
	}
	else
	{
		// btn
		SelectExtraItemBtn *pItemBtn = dynamic_cast<SelectExtraItemBtn*>( pOpenBtn );
		if( !pItemBtn ) return;
		
		if( pItemBtn->GetSelectExtraItemSlot() == kItemList[iListArray].m_iMagicCode ) return;

		pItemBtn->SetSelectExtraItemSlot( kItemList[iListArray].m_iMagicCode );
		SetIconBtn( pOpenBtn->GetID() + ADD_SELECT_TO_ICON, kItemList[iListArray].m_iMagicCode );
	}
	
	// char
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( !pCharWnd ) return;
	
	if( pOpenBtn->GetID() == ID_CHAR_SELECT_BTN )
	{
		Help::GetDefaultCharInfo( m_CharInfo, kItemList[iListArray].m_iMagicCode );
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, true, false );
	}
	else if( pOpenBtn->GetID() == ID_WEAPON_SELECT_BTN )
	{
		pCharWnd->ReleaseItem( ioCreateChar::CCS_WEAPON );
		pCharWnd->EquipItem( 10000+m_CharInfo.m_class_type, 0, 0 );
	}
	else if( pOpenBtn->GetID() == ID_ARMOR_SELECT_BTN )
	{
		pCharWnd->ReleaseItem( ioCreateChar::CCS_ARMOR );
		pCharWnd->EquipItem( 110000+m_CharInfo.m_class_type, 0, 0 );
	}
	else if( pOpenBtn->GetID() == ID_HELMET_SELECT_BTN )
	{
		pCharWnd->ReleaseItem( ioCreateChar::CCS_HELMET );
		pCharWnd->EquipItem( 210000+m_CharInfo.m_class_type, 0, 0 );
	}
	else if( pOpenBtn->GetID() == ID_CLOAK_SELECT_BTN )
	{
		pCharWnd->ReleaseItem( ioCreateChar::CCS_CLOAK );
		pCharWnd->EquipItem( 310000+m_CharInfo.m_class_type, 0, 0 );
	}
}

void InvenFourExtraCompoundBuyWnd::SetEtcItemType( DWORD dwEtcItemType )
{
	m_dwEtcItemType = dwEtcItemType;
}

void InvenFourExtraCompoundBuyWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	// back ani
	enum { SECOND_BACK_ANI_DELAY = 1000, };
	if( m_dwBackAniCheckTime[0] == 0 && m_dwBackAniCheckTime[1] == 0 )
	{
		m_dwBackAniCheckTime[0] = dwCurTime;
	}
	else if( m_dwBackAniCheckTime[0] != 0 && dwCurTime - m_dwBackAniCheckTime[0] > m_dwBackAniTime )
	{
		m_dwBackAniCheckTime[0] = 0;
	}
	else if(  m_dwBackAniCheckTime[0] != 0 && m_dwBackAniCheckTime[1] == 0 && dwCurTime - m_dwBackAniCheckTime[0] > SECOND_BACK_ANI_DELAY )
	{
		m_dwBackAniCheckTime[1] = dwCurTime;
	}
	else if( m_dwBackAniCheckTime[1] != 0 && dwCurTime - m_dwBackAniCheckTime[1] > m_dwBackAniTime )
	{
		m_dwBackAniCheckTime[1] = 0;
	}	
	else if( m_dwBackAniCheckTime[1] != 0 && m_dwBackAniCheckTime[0] == 0 && dwCurTime - m_dwBackAniCheckTime[1] > SECOND_BACK_ANI_DELAY  )
	{
		m_dwBackAniCheckTime[0] = dwCurTime;
	}
	
	if( m_dwBackAniTime != 0 )
	{
		for (int i = 0; i < MAX_BACK_EFFECT ; i++)
		{
			if( m_dwBackAniCheckTime[i] == 0 )
				continue;
			m_fBackAniScale[i]  = FLOAT1 + ( ( sin( (float)( PI/2*(dwCurTime - m_dwBackAniCheckTime[i])) / (float)m_dwBackAniTime ) * (float)m_dwBackAniSizePerGap ) * 0.01f ); // FLOAT1->1.25f 사이즈
			m_byBackAniAlpha[i] = m_dwBackAniAlphaGap - ( sin( (float)(PI/2*(dwCurTime - m_dwBackAniCheckTime[i])) / (float)m_dwBackAniTime) * (float)m_dwBackAniAlphaGap );// 127 -> 255 알파	
		}
	}
	
	// start ani
	enum { MAX_START_ANI_WAIT_TIME = 600, };
	if( m_dwStartAniCheckTime != 0 && ( dwCurTime - m_dwStartAniCheckTime ) > MAX_START_ANI_WAIT_TIME )
	{
		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
		{
			pCharWnd->SetEquip( false, true, false );
		}

		m_dwStartAniCheckTime = 0;
	}
}

void InvenFourExtraCompoundBuyWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TEXT_X_OFFSET  = 33,
		TEXT_Y_OFFSET  = 399,

		BACK_X          = 129,
		BACK_Y          = 162,
	};

	PrintManual( iXPos+TEXT_X_OFFSET, iYPos+TEXT_Y_OFFSET, FONT_SIZE_12 );

	// back
	if( m_pBackEffect )
	{
		m_pBackEffect->SetAlpha( 255 );
		m_pBackEffect->SetScale( FLOAT1 );
		m_pBackEffect->Render( iXPos+BACK_X, iYPos+BACK_Y );

		for (int i = 0; i < MAX_BACK_EFFECT ; i++)
		{
			m_pBackEffect->SetAlpha( m_byBackAniAlpha[i] );
			m_pBackEffect->SetScale( m_fBackAniScale[i] );
			m_pBackEffect->Render( iXPos+BACK_X, iYPos+BACK_Y );
		}
	}
}

void InvenFourExtraCompoundBuyWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );
	if( !pItem ) return;

	int iSubManual = pItem->GetInventorySubManual() - 1;
	ioComplexStringPrinter kPrinter;
	int iMaxLine = m_pManual->GetMaxLine( iSubManual );
	for(int iLine = 1; iLine < iMaxLine; iLine++)
	{
		m_pManual->SetPrinter( iSubManual, iLine, fScale, kPrinter);
		a_iYPos += m_pManual->GetSpaceLine( iSubManual, iLine);
		kPrinter.PrintFullTextWidthCut( a_iXPos, a_iYPos, TAT_LEFT, DESC_WIDTH );
		kPrinter.ClearList();
	}
}

void InvenFourExtraCompoundBuyWnd::SendItemCompound()
{
	enum { MAX_ITEM = 4, };
	int iItemIndexList[MAX_ITEM]={ 0, 0, 0, 0 };
	for (int i = 0; i < MAX_ITEM ; i++)
	{
		SelectExtraItemBtn *pItem = dynamic_cast<SelectExtraItemBtn*>(FindChildWnd( ID_WEAPON_SELECT_BTN + i ));
		if( !pItem )
			continue;
		iItemIndexList[i] = pItem->GetSelectExtraItemSlot();
	}

	ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
	if( !pExtraItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pExtraItem == NULL.", __FUNCTION__ );
		return;
	}

	for (int i = 0; i < MAX_ITEM ; i++)
	{
		EXTRAITEMSLOT kSlot;
		if( !pExtraItem->GetExtraItem( iItemIndexList[i], kSlot ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}	
	}
		
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)m_dwEtcItemType;
	kPacket << (int)m_CharInfo.m_class_type;
	for (int i = 0; i < MAX_ITEM ; i++)
		kPacket << iItemIndexList[i];
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void InvenFourExtraCompoundBuyWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "back_effect" )
	{
		SAFEDELETE( m_pBackEffect );
		m_pBackEffect = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void InvenFourExtraCompoundBuyWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName, sizeof( szKeyName ), "back_ani_gap" );
	m_dwBackAniTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName, sizeof( szKeyName ), "back_ani_size_per_gap" );
	m_dwBackAniSizePerGap = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf( szKeyName, sizeof( szKeyName ), "back_ani_alpha_gap" );
	m_dwBackAniAlphaGap = xElement.GetIntAttribute( szKeyName );
}
//////////////////////////////////////////////////////////////////////////
InvenFourExtraCompoundBuyResultWnd::InvenFourExtraCompoundBuyResultWnd()
{
	m_pEffect     = NULL;

	m_bResult = false;
	m_bSuccess = false;

	m_dwEtcItemType = 0;
}

InvenFourExtraCompoundBuyResultWnd::~InvenFourExtraCompoundBuyResultWnd()
{
	SAFEDELETE( m_pEffect );

	m_vSuccessEfffect.clear();
	m_vFailEfffect.clear();
}

void InvenFourExtraCompoundBuyResultWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );

	if( !m_szDestroySound.IsEmpty() )
		g_SoundMgr.StopSound( m_szDestroySound, 0 );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szRollingSound, 0 );

	if( !m_szSuccessSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szSuccessSound, 0 );

	if( !m_szFailSound.IsEmpty() )
		g_SoundMgr.StopSound( m_szFailSound, 0 );
}

void InvenFourExtraCompoundBuyResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );
}

void InvenFourExtraCompoundBuyResultWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_duration" );
	m_dwRollingTotalTime = xElement.GetIntAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "destroy_sound" );
	m_szDestroySound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "rolling_sound" );
	m_szRollingSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "success_sound" );
	m_szSuccessSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "fail_sound" );
	m_szFailSound = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "start_animation" );
	m_szStartAni = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "sucess_animation" );
	m_szSuccessAni = xElement.GetStringAttribute( szKeyName );

	ZeroMemory( szKeyName, sizeof( szKeyName ) );
	StringCbPrintf_e( szKeyName, sizeof( szKeyName ), "fail_animation" );
	m_szFailAni = xElement.GetStringAttribute( szKeyName );


	m_dwRollingEffectTime = xElement.GetIntAttribute_e( "Effect_Gap_Time" );

	m_vSuccessEfffect.clear();
	int iMax = xElement.GetIntAttribute_e( "Max_Success_Effect" );
	m_vSuccessEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Success_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vSuccessEfffect.push_back( szEffect );
	}

	m_vFailEfffect.clear();
	iMax = xElement.GetIntAttribute_e( "Max_Fail_Effect" );
	m_vFailEfffect.reserve( iMax );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "Fail_Effect_%d", i+1 );
		ioHashString szEffect = xElement.GetStringAttribute( szName );
		if( szEffect == "" )
			break;
		m_vFailEfffect.push_back( szEffect );
	}

	m_dwBackEffectTime = xElement.GetIntAttribute_e( "Back_Gap_Time" );
}

void InvenFourExtraCompoundBuyResultWnd::ShowResult( int iClassType, int iClassTime, DWORD dwEtcItemType, bool bSuccess )
{
	m_dwEtcItemType = dwEtcItemType;
	m_bResult       = false;
	m_bSuccess      = bSuccess;

	char szBuf[MAX_PATH] = "";
	SafeSprintf( szBuf, sizeof( szBuf ), g_MyInfo.GetClassName( iClassType ) );
	m_sTitleName  = szBuf;
	m_sClassTime = g_PresentMgr.GetPresentValue2Text( PRESENT_SOLDIER, iClassType, iClassTime );

	SAFEDELETE(m_pEffect);

	ShowWnd();

	DWORD dwCurTime = FRAMEGETTIME();
	m_dwRollingEndTime = dwCurTime + m_dwRollingTotalTime;
	m_dwRollingEffectCheckTime = 0;
	m_dwBackEffectCheckTime = 0;

	m_iCurEffectArray = -1;

	ReBatchButton( ROLLING_BTN );

	if( !m_szDestroySound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szDestroySound, DSBVOLUME_MAX, PSM_THREAD );

	if( !m_szRollingSound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szRollingSound, DSBVOLUME_MAX, PSM_LOOP | PSM_THREAD );

	Help::GetDefaultCharInfo( m_CharInfo, iClassType );
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( false, true, false );
		pCharWnd->EquipItem( 10000+m_CharInfo.m_class_type, 0, 0 );
		pCharWnd->EquipItem( 110000+m_CharInfo.m_class_type, 0, 0 );
		pCharWnd->EquipItem( 210000+m_CharInfo.m_class_type, 0, 0 );
		pCharWnd->EquipItem( 310000+m_CharInfo.m_class_type, 0, 0 );
		if( !m_szStartAni.IsEmpty() )
			pCharWnd->SetCharActionAnimation( m_szStartAni ); // SetEquip 이후에
		pCharWnd->ShowWnd();
	}
}

void InvenFourExtraCompoundBuyResultWnd::OnProcess( float fTimePerSec )
{
	ioWnd::OnProcess( fTimePerSec );

	if( !IsShow() ) return;

	RollingEffect( fTimePerSec );

	UpdateCompoundInfo();
}

void InvenFourExtraCompoundBuyResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowPresentTabDirect();
			}
		}
		break;
	case ID_USE:
		if( cmd == IOBN_BTNUP )
		{
			ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
			if( pUserEtcItem )
			{
				ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( m_dwEtcItemType );
				if( !pEtcItem )
				{
					LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL : %d", __FUNCTION__, m_dwEtcItemType );
					return;
				}
				HideWnd();

				pEtcItem->OnUseBtnPush( m_dwEtcItemType, pUserEtcItem );
			}
		}
		break;
	}
}

void InvenFourExtraCompoundBuyResultWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		TITLE_X		    = 129,
		TITLE_Y		    = 270,

		TITLE_Y_GAP     = 23,

		DESC_X		    = 33,
		DESC_Y		    = 399,

		ROLLING_EFFECT_WEAPON_X = 55,
		ROLLING_EFFECT_WEAPON_Y = 120,

		ROLLING_EFFECT_HELMET_X = 203,
		ROLLING_EFFECT_HELMET_Y = 120,

		ROLLING_EFFECT_ARMOR_X = 55,
		ROLLING_EFFECT_ARMOR_Y = 204,

		ROLLING_EFFECT_CLOAK_X = 203,
		ROLLING_EFFECT_CLOAK_Y = 204,
	};

	// Title
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y, FONT_SIZE_17, m_sTitleName.c_str() );

	if( m_bResult )
	{
		if( m_bSuccess )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y+TITLE_Y_GAP, FONT_SIZE_17, m_sClassTime.c_str() );	

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_BLUE );
			g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y+(TITLE_Y_GAP*2), FONT_SIZE_17, STR(1) );	

			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
			g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y+(TITLE_Y_GAP*3), FONT_SIZE_17, STR(2) );	
		}
		else
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_RED );
			g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y+TITLE_Y_GAP, FONT_SIZE_17, STR(3) );
		}
	}
	else
	{
		// rolling
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos+TITLE_X, iYPos+TITLE_Y+TITLE_Y_GAP, FONT_SIZE_17, STR(4) );
	}
}

void InvenFourExtraCompoundBuyResultWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		FINAL_EFFECT_X  = 129,
		FINAL_EFFECT_Y  = 162,

		ROLLING_EFFECT_WEAPON_X = 55,
		ROLLING_EFFECT_WEAPON_Y = 120,

		ROLLING_EFFECT_HELMET_X = 203,
		ROLLING_EFFECT_HELMET_Y = 120,

		ROLLING_EFFECT_ARMOR_X = 55,
		ROLLING_EFFECT_ARMOR_Y = 204,

		ROLLING_EFFECT_CLOAK_X = 203,
		ROLLING_EFFECT_CLOAK_Y = 204,
	};

	// Effect
	if( m_pEffect )
	{
		if( m_bResult )
		{
			m_pEffect->SetScale( 3.50f );
			m_pEffect->Render( iXPos+FINAL_EFFECT_X, iYPos+FINAL_EFFECT_Y, UI_RENDER_ADD, TFO_BILINEAR );
		}
		else
		{
			enum { MAX_EFFECT = 4, };
			int iXPosList[MAX_EFFECT]={ ROLLING_EFFECT_WEAPON_X, ROLLING_EFFECT_HELMET_X, ROLLING_EFFECT_ARMOR_X, ROLLING_EFFECT_CLOAK_X };
			int iYPosList[MAX_EFFECT]={ ROLLING_EFFECT_WEAPON_Y, ROLLING_EFFECT_HELMET_Y, ROLLING_EFFECT_ARMOR_Y, ROLLING_EFFECT_CLOAK_Y };
			for (int i = 0; i < MAX_EFFECT ; i++)
			{
				m_pEffect->SetScale( 1.60f );
				m_pEffect->Render( iXPos+iXPosList[i], iYPos+iYPosList[i], UI_RENDER_ADD, TFO_BILINEAR );
			}
		}
	}
}

void InvenFourExtraCompoundBuyResultWnd::UpdateCompoundInfo()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwRollingEndTime < dwCurTime )			// RollingEnd
	{
		if( !m_bResult )
		{
			m_bResult = true;
			ReBatchButton( RESULT_BTN );

			if( !m_szRollingSound.IsEmpty() )
				g_SoundMgr.StopSound( m_szRollingSound, 0 );

			NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
			if( m_bSuccess )
			{
				if( pCharWnd && !m_szSuccessAni.IsEmpty() )
					pCharWnd->SetCharMotionItemAnimation( m_szSuccessAni );

				if( !m_szSuccessSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szSuccessSound, DSBVOLUME_MAX, PSM_THREAD );
			}
			else
			{
				if( pCharWnd && !m_szFailAni.IsEmpty() )
					pCharWnd->SetCharMotionItemAnimation( m_szFailAni );

				if( !m_szFailSound.IsEmpty() )
					g_SoundMgr.PlaySound( m_szFailSound, DSBVOLUME_MAX, PSM_THREAD );
			}

			m_iCurEffectArray = -1;
			m_dwRollingEffectCheckTime = 0;
		}
	}
}

void InvenFourExtraCompoundBuyResultWnd::RollingEffect( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	int iEffectListCnt = 0;
	if( m_bResult && m_bSuccess )
		iEffectListCnt = m_vSuccessEfffect.size();
	else
		iEffectListCnt = m_vFailEfffect.size();

	if( iEffectListCnt == 0 )
	{
		SAFEDELETE( m_pEffect );
		return;
	}

	if( m_iCurEffectArray == iEffectListCnt-1 )
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

		ioHashString szEffect;
		if( m_bResult && m_bSuccess )
			szEffect = m_vSuccessEfffect[m_iCurEffectArray];
		else
			szEffect = m_vFailEfffect[m_iCurEffectArray];


		if( !szEffect.IsEmpty() )
		{
			m_pEffect = g_UIImageSetMgr.CreateImageByFullName( szEffect );
		}

		m_dwRollingEffectCheckTime = dwCurTime;
	}
}

void InvenFourExtraCompoundBuyResultWnd::ReBatchButton( int iType )
{
	enum
	{
		ONE_BTN_SIZE	= 204,
		TWO_BTN_SIZE	= 102,

		ONE_BTN_X		= 27,
		TWO_BTN_X		= 131,

		ONE_TEXT_X		= 102,
		TWO_TEXT_X		= 51,
	};

	switch( iType )
	{
	case ROLLING_BTN:
		ShowChildWnd(ID_ROLLING);
		SetChildInActive(ID_ROLLING);

		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);
		break;
	case RESULT_BTN:
		HideChildWnd(ID_ROLLING);
		HideChildWnd(ID_USE);
		HideChildWnd(ID_GO);

		// 예외처리
		ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
		if( !pUserEtcItem )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
			break;
		}

		ioUserEtcItem::ETCITEMSLOT kSlot;
		if( !pUserEtcItem->GetEtcItem( m_dwEtcItemType, kSlot) )
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( ONE_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( ONE_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		else
		{
			ioButton *pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_USE));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( ONE_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}

			pButton = dynamic_cast<ioButton*>(FindChildWnd(ID_GO));
			if( pButton )
			{
				pButton->SetSize( TWO_BTN_SIZE, pButton->GetHeight() );
				pButton->SetWndPos( TWO_BTN_X, pButton->GetYPos() );

				ioUITitle *pTitle =  pButton->GetTitle();
				if( pTitle )
				{
					pTitle->SetOffset( TWO_TEXT_X, pTitle->GetTextYOffSet() );
				}

				pButton->ShowWnd();
			}
		}
		break;
	}
}

void InvenFourExtraCompoundBuyResultWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/FourExtraHelpWnd.xml", this );
}
