
#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"

#include "../DirectInput.h"
#include "../ioKeyManager.h"
#include "../TextColorTable.h"
#include "../ioFishingManager.h"
#include "../NetworkWrappingFunc.h"

#include "../WndID.h"
#include "../Setting.h"
#include "../ioMyInfo.h"
#include "../ioClassPrice.h"

#include "../ioItem.h"
#include "../ioItemMaker.h"
#include "../ioEtcItem.h"
#include "../ioSubscriptionMgr.h"

#include "../ioSetItemInfoManager.h"
#include "../ioSetItemInfo.h"

#include "ioSP2GUIManager.h"

#include "GlobalWnd.h"
#include "MyInventoryWnd.h"

#include "ioMessageBox.h"
#include "RandomDecoSelectWnd.h"

#include <strsafe.h>
#include "../io3DEngine/Safesprintf.h"
#include "../ioPresentMgr.h"
#include "../ioDecorationPrice.h"
#include "../ioEtcItemManager.h"

//////////////////////////////////////////////////////////////////////////

RandomDecoSelectWnd::RandomDecoSelectWnd()
{
	m_pPresentPreBack = NULL;
	m_pPresentBack = NULL;
	m_pPresentIcon = NULL;


	m_dwIndex = 0;    
	m_dwSlotIndex = 0;
	m_iPresentType = 0;   
	m_iPresentMent = 0;   
	m_iPresentValue1 = 0; 
	m_iPresentValue2 = 0;
	m_iPresentValue3 = 0; 
	m_iPresentValue4 = 0;
	m_dwLimitDate = 0;

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

	m_OverCharInfo = m_CharInfo;

	m_bSubscription = false;
	m_iSubscriptionGold = 0;
	m_szSubscriptionID.Clear();
}

RandomDecoSelectWnd::~RandomDecoSelectWnd()
{
	SAFEDELETE( m_pPresentPreBack );
	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentIcon );
}

void RandomDecoSelectWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/RandomDecoSelectHelpWnd.xml", this );
}

void RandomDecoSelectWnd::SetPresent( DWORD dwIndex, DWORD dwSlotIndex )
{
	HideWnd();
	
	m_dwIndex = dwIndex;
	m_dwSlotIndex = dwSlotIndex;
	m_iSelectClassType = 0;

	m_bResult = false;

	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	if( m_dwIndex != 0 || m_dwSlotIndex != 0 )
		ShowWnd();

	if( m_dwIndex == 0 && m_dwSlotIndex == 0 )
	{
		HideWnd();
	}
	else
	{
		UpdateBtn();

		// 3D 
		int iClassType = 0;
		
		if( !m_PackageItemList.empty() )
			iClassType = m_PackageItemList[0].m_iMagicCode;

		int iDecoType  = m_iPresentValue1 % 1000;
		int iSexType   = (m_iPresentValue1 % 100000) / 1000;

		m_CharInfo.Init();
		m_CharInfo.m_class_type = iClassType;	
		m_CharInfo.m_kindred = 1;
		m_CharInfo.m_sex = 1;

		if( iDecoType == UID_KINDRED )
		{
			m_CharInfo.m_kindred = 1;
			m_CharInfo.m_sex = 2;
		}
		else if( iSexType == 1 )
		{
			m_CharInfo.m_kindred = 1;
			m_CharInfo.m_sex = 2;
		}

		CheckSoldierWnd( iClassType, m_CharInfo );
		m_OverCharInfo = m_CharInfo;


		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
			pCharWnd->ShowWnd();
	}

	HideChildWnd( ID_GO );
	ShowChildWnd( ID_RECV );
	ShowChildWnd( ID_SOLDIER_BTN, true );

	SetPresentIcon();
}

void RandomDecoSelectWnd::SetSubscription( DWORD dwIndex, const ioHashString szSubscriptionID )
{
	HideWnd();
	
	m_dwIndex = dwIndex;
	m_szSubscriptionID = szSubscriptionID;

	m_iSelectClassType = 0;

	m_bResult = false;
	m_bSubscription = true;

	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	if( m_dwIndex == 0 )
	{
		HideWnd();
	}
	else
	{
		ShowWnd();

		UpdateBtn();

		// 3D 
		int iClassType = 0;
		
		if( !m_PackageItemList.empty() )
			iClassType = m_PackageItemList[0].m_iMagicCode;

		int iDecoType  = m_iPresentValue1 % 1000;
		int iSexType   = (m_iPresentValue1 % 100000) / 1000;

		m_CharInfo.Init();
		m_CharInfo.m_class_type = iClassType;	
		m_CharInfo.m_kindred = 1;
		m_CharInfo.m_sex = 1;

		if( iDecoType == UID_KINDRED )
		{
			m_CharInfo.m_kindred = 1;
			m_CharInfo.m_sex = 2;
		}
		else if( iSexType == 1 )
		{
			m_CharInfo.m_kindred = 1;
			m_CharInfo.m_sex = 2;
		}

		CheckSoldierWnd( iClassType, m_CharInfo );
		m_OverCharInfo = m_CharInfo;


		NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
		if( pCharWnd )
			pCharWnd->ShowWnd();
	}

	HideChildWnd( ID_GO );
	ShowChildWnd( ID_RECV );
	ShowChildWnd( ID_SOLDIER_BTN, true );

	SetPresentIcon();
}

void RandomDecoSelectWnd::OnResultState()
{
	m_bResult = true;

	ShowChildWnd( ID_GO );
	HideChildWnd( ID_RECV );
	HideChildWnd( ID_SOLDIER_BTN );
	HideChildWnd( ID_PULLDOWN_WND );
}

void RandomDecoSelectWnd::PresentRecv()
{
	if( m_bSubscription )
	{
		if( m_dwIndex != 0 && !m_szSubscriptionID.IsEmpty() )
		{
			SP2Packet kPacket( CTPK_SUBSCRIPTION_RECV );
			kPacket << m_dwIndex << m_szSubscriptionID;
			kPacket << m_iSelectClassType;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
	}
	else
	{
		if( m_dwIndex != 0 || m_dwSlotIndex != 0 )
		{
			SP2Packet kPacket( CTPK_PRESENT_RECV );
			kPacket << m_dwIndex << m_dwSlotIndex;
			kPacket << m_iSelectClassType;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
	}
}

void RandomDecoSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GO:
		if( cmd == IOBN_BTNUP )
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->ShowSoldierTabDirect( m_iSelectClassType );
			}
		}
		break;
	case ID_RECV:
		if( cmd == IOBN_BTNUP )
		{
			PresentRecv();
		}
		break;
	case ID_SOLDIER_BTN:
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
			
			UpdateBtn();
		}
		else if( cmd == IOWN_OVERED )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
				PullDownBtnOver( pPullDownWnd->GetOpenBtn(), param );
		}
		break;
	}
}

void RandomDecoSelectWnd::UpdateBtn()
{
	if( m_iSelectClassType == 0 )
		SetChildInActive( ID_RECV );
	else
		SetChildActive( ID_RECV );
}

void RandomDecoSelectWnd::SetPresentIcon()
{
	SAFEDELETE( m_pPresentIcon );

	int iSexType   = (m_iPresentValue1 % 100000) / 1000;
	int iDecoType  = m_iPresentValue1 % 1000;
	const DecoData &kData = g_DecorationPrice.GetDecoData( iSexType, iDecoType, m_iPresentValue2 );
	m_pPresentIcon = g_UIImageSetMgr.CreateImageByFullName( kData.m_szIcon );


	m_PresentInfo[0].ClearList();
	m_PresentInfo[1].ClearList();

	// 첫 라인 : 이름
	m_PresentInfo[0].SetTextStyle( TS_NORMAL );
	m_PresentInfo[0].SetBkColor( 0, 0, 0 );	
	m_PresentInfo[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_PresentInfo[0].AddTextPiece( FONT_SIZE_17, g_PresentMgr.GetPresentValue1Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );

	// 두번째 라인 : 기간, 갯수 등등 추가 정보 
	m_PresentInfo[1].SetTextStyle( TS_NORMAL );
	m_PresentInfo[1].SetBkColor( 0, 0, 0 );
	m_PresentInfo[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_PresentInfo[1].AddTextPiece( FONT_SIZE_17,  g_PresentMgr.GetPresentValue2Text( m_iPresentType, m_iPresentValue1, m_iPresentValue2 ).c_str() );
}

void RandomDecoSelectWnd::iwm_show()
{
	if( m_bSubscription )
	{
		ioSubscriptionMgr::SubscriptionData kData;
		g_SubscriptionMgr.GetSubscriptionDataToIndex( m_dwIndex, m_szSubscriptionID, kData );

		m_dwSlotIndex = 0;
		m_szSendID.Clear();

		m_iPresentType = kData.m_iPresentType;
		m_iPresentValue1 = kData.m_iPresentValue1;
		m_iPresentValue2 = kData.m_iPresentValue2;
		m_iPresentValue3 = 0;
		m_iPresentValue4 = 0;

		m_iPresentMent = kData.m_iMentType;
		m_dwLimitDate = kData.m_dwLimitDate;
	}
	else
	{
		g_PresentMgr.GetPresentDataToIndex( m_dwIndex, m_dwSlotIndex, m_szSendID, m_iPresentType, m_iPresentMent, m_iPresentValue1,
											m_iPresentValue2, m_iPresentValue3,
											m_iPresentValue4, m_dwLimitDate );
	}

	// 초기화
	m_bPullDownShow = false;
	m_PackageItemList.clear();
	GetClassList( m_PackageItemList );

	// 가능용병 없음
	if( m_PackageItemList.empty() )
	{
		HideWnd();

		MyInventoryWnd *pInven = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInven && pInven->IsShow() )
			pInven->UpdateTab( false, true );

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	m_iSelectClassType = m_PackageItemList[0].m_iMagicCode;

	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof( szTitle ), STR(2), g_MyInfo.GetClassName( m_iSelectClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iSelectClassType ) ) );

	ioButton *pBtn = (ioButton*)(FindChildWnd(ID_SOLDIER_BTN));
	if( pBtn )
	{
		pBtn->SetTitleText( szTitle );
		pBtn->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( m_iSelectClassType ) ), 0 );
	}

	CheckSoldierWnd( m_iSelectClassType, m_CharInfo );
}

void RandomDecoSelectWnd::iwm_hide()
{
	HideChildWnd( ID_SOLDIER_WND );
}

void RandomDecoSelectWnd::DefaultDecoration( CHARACTER &rkCharInfo )
{
	// 얼굴       UID_FACE
	rkCharInfo.m_face = g_MyInfo.GetEquipItemCode((rkCharInfo.m_class_type*100000) + ((rkCharInfo.m_sex - 1) * 1000) + UID_FACE);
	if( rkCharInfo.m_face == -1 )
		rkCharInfo.m_face = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_FACE, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_FACE );

	// 머리       UID_HAIR  
	rkCharInfo.m_hair = g_MyInfo.GetEquipItemCode((rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000) + UID_HAIR);
	if( rkCharInfo.m_hair == -1 )
		rkCharInfo.m_hair = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_HAIR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_HAIR );

	// 피부색     UID_SKIN_COLOR
	rkCharInfo.m_skin_color = g_MyInfo.GetEquipItemCode((rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000) + UID_SKIN_COLOR);
	if( rkCharInfo.m_skin_color == -1 )
		rkCharInfo.m_skin_color = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_SKIN_COLOR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_SKIN_COLOR );

	// 머리색     UID_HAIR_COLOR
	rkCharInfo.m_hair_color = g_MyInfo.GetEquipItemCode((rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000) + UID_HAIR_COLOR);
	if( rkCharInfo.m_hair_color == -1 )
		rkCharInfo.m_hair_color = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_HAIR_COLOR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_HAIR_COLOR );

	// 속옷       UID_UNDERWEAR
	rkCharInfo.m_underwear = g_MyInfo.GetEquipItemCode((rkCharInfo.m_class_type * 100000) + ((rkCharInfo.m_sex - 1) * 1000) + UID_UNDERWEAR);
	if( rkCharInfo.m_underwear == -1 )
		rkCharInfo.m_underwear = g_DecorationPrice.GetDefaultDecoCode( rkCharInfo.m_sex - 1, UID_UNDERWEAR, g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + UID_UNDERWEAR );
}

void RandomDecoSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentPreBack" )
	{
		SAFEDELETE( m_pPresentPreBack );
		m_pPresentPreBack = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void RandomDecoSelectWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>( FindChildWnd( ID_PULLDOWN_WND ));
	if( !pPullDownWnd ) return;
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( !pCharWnd ) return;

	bool bReleseHelm = false;
	bool bReleseAll = false;
	bool bShopMotion = false;

	int iDecoType  = m_iPresentValue1 % 1000;
	switch( iDecoType )
	{
	case UID_KINDRED:
		{
			bReleseHelm = false;
			bReleseAll = false;
			bShopMotion = true;
		}
		break;
	case UID_FACE:
		{
			bReleseHelm = true;
			bReleseAll = false;
			bShopMotion = false;
		}
		break;
	case UID_HAIR:
		{
			bReleseHelm = true;
			bReleseAll = false;
			bShopMotion = false;
		}
		break;
	case UID_SKIN_COLOR:
		{
			bReleseHelm = false;
			bReleseAll = false;
			bShopMotion = false;
		}
		break;
	case UID_HAIR_COLOR:
		{
			bReleseHelm = true;
			bReleseAll = false;
			bShopMotion = false;
		}
		break;
	case UID_UNDERWEAR:
		{
			bReleseHelm = false;
			bReleseAll = true;
			bShopMotion = false;
		}
		break;
	}

	// PullDown 열렸다 닫혔을때..
	if( m_bPullDownShow && !pPullDownWnd->IsShow() )
	{
		m_bPullDownShow = false;

		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( bReleseHelm, bReleseAll, bShopMotion );
	}
}

void RandomDecoSelectWnd::OnRenderPresent( int iXPos, int iYPos )
{
	enum 
	{
		TITLE_X_OFFSET	= 129,
		TITLE_Y_OFFSET	= 152,

		RESULT_DESC_X	= 129,
		RESULT_DESC_Y	= 421,
	};

	if( !m_PresentInfo[0].IsEmpty() )
		m_PresentInfo[0].PrintFullText( iXPos+TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET, TAT_CENTER );

	if( !m_PresentInfo[1].IsEmpty() )
		m_PresentInfo[1].PrintFullText( iXPos+TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET+23, TAT_CENTER );

	if( m_bResult )
	{
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );
		g_FontMgr.PrintText( iXPos+RESULT_DESC_X, iYPos+RESULT_DESC_Y, FONT_SIZE_17, STR(1) );
	}
}

void RandomDecoSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		ICON_X	= 129,
		ICON_Y	= 105,
	};

	if( m_bResult )
	{
		if( m_pPresentBack )
			m_pPresentBack->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}
	else
	{
		if( m_pPresentPreBack )
			m_pPresentPreBack->Render( iXPos+ICON_X, iYPos+ICON_Y, UI_RENDER_MULTIPLY );
	}

	if( m_pPresentIcon )
		m_pPresentIcon->Render( iXPos+ICON_X, iYPos+ICON_Y );

	OnRenderPresent( iXPos, iYPos );
}

void RandomDecoSelectWnd::SetPullDownMenu( ioWnd *pOpenBtn )
{
	if( !pOpenBtn ) return;

	NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(FindChildWnd( ID_PULLDOWN_WND ));
	if( pPullDownWnd )
	{
		if( pPullDownWnd->IsShow() )
		{
			pPullDownWnd->HideWnd();
		}
		else
		{
			m_OverCharInfo = m_CharInfo;
			pPullDownWnd->CreatePullDown( pOpenBtn, pOpenBtn->GetXPos(), pOpenBtn->GetYPos() + 24 );

			vNewShopPullDownItem kItemList;
			ConvertItemList( m_PackageItemList, kItemList );

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
			
			m_bPullDownShow = true;
		}
	}
}

void RandomDecoSelectWnd::GetClassList( vPackageItem &rkList )
{
	rkList.clear();

	// 보유중인 용병 : 체험(PC방) 용병 제외
	int iMaxClass =  g_ClassPrice.MaxClassPrice();
	for (int i = 0; i <  iMaxClass; i++)
	{
		int iCharArray = g_MyInfo.GetCharSlotIndexToArray( i );
		if( iCharArray == -1 ) continue;
		if( g_MyInfo.IsCharExercise( iCharArray ) ) continue;
		if( !g_MyInfo.IsCharActive( iCharArray ) ) continue;

		int iSetIdx = g_MyInfo.GetClassType( iCharArray ) - 1;
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo ) continue;

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;


		int iPresentValue1NoClassType = ( m_iPresentValue1 % 100000 ); // m_iPresentValue1에 class 타입이 들어갈 수 있으므로 % 100000 삭제한다. 
		// 선택된 용병의 치장으로 변신!
		int iItemType = iClassType * 100000 + iPresentValue1NoClassType;
		int iItemCode = m_iPresentValue2;

		ITEMSLOT kSlot;
		kSlot.m_item_type = iItemType;
		kSlot.m_item_code = iItemCode;

		// 이미 있는 용병
		if( g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) )
			continue;

		// 템 성별 구분...
		int iItemSexType = (iPresentValue1NoClassType % 100000) / 1000;
		if( iItemSexType == 1 )		// 여캐용 템이면 여캐 있는지 체크 필요
		{
			ITEMSLOT kSexSlot;
			const int iSexType = 0;
			kSexSlot.m_item_type = ( iClassType*100000 ) + ( iSexType * 1000 ) + UID_KINDRED; // 해당 종족에 여자 치장
			kSexSlot.m_item_code = RDT_HUMAN_WOMAN;

			if( !g_MyInfo.IsSlotItem( kSexSlot.m_item_type, kSexSlot.m_item_code ) )
			{
				continue;
			}
		}
		//


		PackageItem kItem;
		kItem.m_iMagicCode = iClassType;
		kItem.m_szName     = pInfo->GetName();			
		kItem.m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );
		rkList.push_back( kItem );
	}

	if( rkList.empty() ) return;

	//std::sort( rkList.begin() , rkList.end(), PackageItemSort() );
}

void RandomDecoSelectWnd::PullDownBtnUP( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) return;	
	if( pOpenBtn->GetID() != ID_SOLDIER_BTN ) return;

	if( !COMPARE( iListArray, 0, (int)m_PackageItemList.size() ) ) return;
	if( m_PackageItemList[iListArray].m_iMagicCode == 0 ) return;
	if( m_iSelectClassType == m_PackageItemList[iListArray].m_iMagicCode ) return;

	m_iSelectClassType = m_PackageItemList[iListArray].m_iMagicCode;

	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_MyInfo.GetClassName( m_iSelectClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iSelectClassType ) ) );
	pOpenBtn->SetTitleText( szTitle );
	pOpenBtn->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( m_PackageItemList[iListArray].m_nGradeType ), 0 );

	CheckSoldierWnd( m_iSelectClassType, m_CharInfo );
}

void RandomDecoSelectWnd::PullDownBtnOver( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) return;	
	if( pOpenBtn->GetID() != ID_SOLDIER_BTN ) return;

	if( m_PackageItemList[iListArray].m_iMagicCode == 0 ) return;
	if( !COMPARE( iListArray, 0, (int)m_PackageItemList.size() ) ) return;

	int iClassType = m_PackageItemList[iListArray].m_iMagicCode;
	if( m_OverCharInfo.m_class_type == iClassType ) return;

	CheckSoldierWnd( iClassType, m_OverCharInfo );
}

void RandomDecoSelectWnd::CheckSoldierWnd( int iClassType, CHARACTER &rkCharInfo )
{
	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		bool bReleseHelm = false;
		bool bReleseAll = false;
		bool bShopMotion = false;

		rkCharInfo.m_class_type = iClassType;

		int iDecoType  = m_iPresentValue1 % 1000;

		// 종족/성별은 먼저 처리한다.
		if( iDecoType == UID_KINDRED )
		{
			switch( m_iPresentValue2 )
			{
			case RDT_HUMAN_MAN:
				rkCharInfo.m_kindred = 1;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_HUMAN_WOMAN:
				rkCharInfo.m_kindred = 1;
				rkCharInfo.m_sex = 2;
				break;
			case RDT_ELF_MAN:
				rkCharInfo.m_kindred = 2;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_ELF_WOMAN:
				rkCharInfo.m_kindred = 2;
				rkCharInfo.m_sex = 2;
				break;
			case RDT_DWARF_MAN:
				rkCharInfo.m_kindred = 3;
				rkCharInfo.m_sex = 1;
				break;
			case RDT_DWARF_WOMAN:
				rkCharInfo.m_kindred = 3;
				rkCharInfo.m_sex = 2;
				break;
			}

			bReleseHelm = false;
			bReleseAll = false;
			bShopMotion = true;
		}

		// 클레스, 종족, 성별에 따라 현재착용중인 치장으로 교체
		DefaultDecoration( rkCharInfo );

		// 변경할 치장 적용
		switch( iDecoType )
		{
		case UID_FACE:
			{
				rkCharInfo.m_face = m_iPresentValue2;

				bReleseHelm = true;
				bReleseAll = false;
				bShopMotion = false;
			}
			break;
		case UID_HAIR:
			{
				rkCharInfo.m_hair = m_iPresentValue2;

				bReleseHelm = true;
				bReleseAll = false;
				bShopMotion = false;
			}
			break;
		case UID_SKIN_COLOR:
			{
				rkCharInfo.m_skin_color = m_iPresentValue2;

				bReleseHelm = false;
				bReleseAll = false;
				bShopMotion = false;
			}
			break;
		case UID_HAIR_COLOR:
			{
				rkCharInfo.m_hair_color = m_iPresentValue2;

				bReleseHelm = true;
				bReleseAll = false;
				bShopMotion = false;
			}
			break;
		case UID_UNDERWEAR:
			{
				rkCharInfo.m_underwear = m_iPresentValue2;

				bReleseHelm = false;
				bReleseAll = true;
				bShopMotion = false;
			}
			break;
		}

		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( rkCharInfo );
		pCharWnd->SetEquip( bReleseHelm, bReleseAll, bShopMotion );
	}
}

void RandomDecoSelectWnd::ConvertItemList( IN vPackageItem &rkPackageList, OUT vNewShopPullDownItem &rkList )
{
	for(int i=0; i < (int)rkPackageList.size(); i++)
	{
		PackageItem &rkItem = rkPackageList[i];

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = rkItem.m_iMagicCode;

		char szName[MAX_PATH]="";
		StringCbPrintf( szName, sizeof( szName ), "%s", rkItem.m_szName.c_str() );	

		kItem.m_dwTextColor = g_UIImageRenderMgr.GetGradeTextColor( rkItem.m_nGradeType );
		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

