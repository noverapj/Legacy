
#include "StdAfx.h"

#include "GrowthDownSelectWnd.h"

////////////////////////// HARDCODE ////////////////////////////
// 육성정보 관련해서 1~8 값을 사용중.
// 1~4까지는 스킬 육성정보. 5~8까지는 캐릭터 기본 육성정보.
// 스킬정보인지 캐릭터 기본 정보인지 구분하기 위해서
// 1, 4, 5등의 숫자를 그냥 사용중.
////////////////////////////////////////////////////////////////
SelectListBtn::SelectListBtn()
{
	m_iCurMagicCode = 0;
	m_szSelectText.Clear();
}

SelectListBtn::~SelectListBtn()
{
}

void SelectListBtn::iwm_show()
{
	m_iCurMagicCode = 0;
	m_szSelectText.Clear();
}

void SelectListBtn::InitData()
{
	m_iCurMagicCode = 0;
	m_szSelectText.Clear();
}

void SelectListBtn::SetListBtnInfo( ioHashString szTitle, int iMagicCode )
{
	m_iCurMagicCode = iMagicCode;
	m_szSelectText = szTitle;
}

void SelectListBtn::OnRender()
{
	ioButton::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_iCurMagicCode > 0 )
		SetTitleText( m_szSelectText.c_str() );
	else
		SetTitleText( m_szDefaultText.c_str() );
}

void SelectListBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_szDefaultText = xElement.GetStringAttribute_e( "DescText" );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrowthDownSelectWnd::GrowthDownSelectWnd(void)
{
	m_iClassType		= 0;
	m_iReUseTargetClass = 0;

	m_EtcInfo.ClearList();

	m_pPImg = NULL;
	m_pManual = NULL;
}

GrowthDownSelectWnd::~GrowthDownSelectWnd(void)
{
	m_Desc[0].ClearList();
	m_Desc[1].ClearList();

	m_EtcInfo.ClearList();

	SAFEDELETE(m_pPImg);
}

void GrowthDownSelectWnd::iwm_create()
{
}

void GrowthDownSelectWnd::iwm_show()
{
	m_iClassType = 0;
	m_Desc[0].ClearList();
	m_Desc[1].ClearList();

	m_EtcInfo.ClearList();

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_GROWTH_DOWN );
	if( pItem )
	{
		InventoryIconBtn *pIconBtn = dynamic_cast<InventoryIconBtn*>(FindChildWnd(ID_ITEM_ICON));
		if( pIconBtn )
		{
			ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() );
			ioUIRenderImage *pSubImg = g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() );
			pIconBtn->SetIcon( pImg, FLOAT1, FLOAT1 );
			pIconBtn->SetSubIcon( pSubImg );
			pIconBtn->SetMarkType( INVENTORY_MARK_TYPE_NONE );
			pIconBtn->SetActive();
		}

		m_EtcInfo.SetTextStyle( TS_NORMAL );
		m_EtcInfo.SetBkColor( 0, 0, 0 );
		m_EtcInfo.SetTextColor( TCT_DEFAULT_DARKGRAY );
		m_EtcInfo.AddTextPiece( FONT_SIZE_17, pItem->GetName().c_str() );
	}

	m_pManual = ioEtcInvenSubManualMgr::GetSingletonPtr();

	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
		pClassBtn->InitData();

	SelectListBtn *pGrowthBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
	if( pGrowthBtn )
		pGrowthBtn->InitData();

	HideChildWnd( ID_CLASS_LIST_WND );
	HideChildWnd( ID_GROWTH_LIST_WND );
	
	SetReUseTargetClassSelect( m_iReUseTargetClass );
	UpdateData();
}

void GrowthDownSelectWnd::iwm_hide()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
		pClassBtn->InitData();

	SelectListBtn *pGrowthBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
	if( pGrowthBtn )
		pGrowthBtn->InitData();

	HideChildWnd( ID_CLASS_LIST_WND );
	HideChildWnd( ID_GROWTH_LIST_WND );
}

void GrowthDownSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHANGE_CLASS_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SelectListBtn *pGrowthBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
			if( pGrowthBtn )
				pGrowthBtn->InitData();

			SetChildInActive( ID_CHANGE_GROWTH_BTN );
			HideChildWnd( ID_GROWTH_LIST_WND );

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
					return;
				}
			}

			UpdateData();
		}
		break;
	case ID_CHANGE_GROWTH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			HideChildWnd( ID_CLASS_LIST_WND );

			int iClassType = 0;
			SelectListBtn *pGrowthBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
			if( pGrowthBtn )
				iClassType = pGrowthBtn->GetListMagicCode();

			vNewShopPullDownItem kItemList;
			GetGrowthList( kItemList );

			NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_GROWTH_LIST_WND));
			if( pListWnd && pListWnd->IsShow() )
			{
				pListWnd->HideWnd();
			}
			else if( pListWnd && iClassType > 0 )
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
	case ID_GROWTH_LIST_WND:
		if( cmd == IOBN_BTNUP )
		{
			NewShopPullDownWnd *pPullDownWnd = dynamic_cast<NewShopPullDownWnd *>(pWnd);
			if( pPullDownWnd )
			{
				SetGrowthSelect( pPullDownWnd->GetOpenBtn(), param );
				pPullDownWnd->HideWnd();
			}
			UpdateData();
		}
		break;
	case ID_SELECT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			CheckSelectBtn();
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendGrowthDown();
			}
		}
		break;
	}
}

void GrowthDownSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	PrintManual( iXPos+27, iYPos+187, FONT_SIZE_12 );

	enum
	{
		TITLE_X_OFFSET	= 128,
		TITLE_Y_OFFSET	= 152,

		X_OFFSET	= 128,
		Y_OFFSET	= 371,
	};

	if( !m_EtcInfo.IsEmpty() )
		m_EtcInfo.PrintFullText( iXPos+TITLE_X_OFFSET, iYPos+TITLE_Y_OFFSET, TAT_CENTER );

	// 복구 포인트 관련 출력
	float fDescWidth1 = m_Desc[0].GetFullWidth();
	float fDescWidth2 = m_Desc[1].GetFullWidth();
	float fImgWidth = 12;

	float fTotalWidth = fDescWidth1 + fImgWidth + 2 + fDescWidth2;
	float fHalfWidth = fTotalWidth * FLOAT05;

	float fCurXPos = iXPos + X_OFFSET - fHalfWidth;
	float fCurYPos = iYPos + Y_OFFSET;
	m_Desc[0].PrintFullText( fCurXPos, fCurYPos, TAT_LEFT );

	fCurXPos += fDescWidth1;
	if( m_pPImg )
		m_pPImg->Render( fCurXPos, fCurYPos+1 );

	fCurXPos += (fImgWidth+2);
	m_Desc[1].PrintFullText( fCurXPos, fCurYPos, TAT_LEFT );
}

void GrowthDownSelectWnd::UpdateData()
{
	m_iClassType = 0;
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
	{
		m_iClassType = pClassBtn->GetListMagicCode();
	}

	int iIndex = 0;
	SelectListBtn *pBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
	if( pBtn )
	{
		if( m_iClassType > 0 )
			pBtn->SetActive();
		else
			pBtn->SetInActive();

		iIndex = pBtn->GetListMagicCode();
	}

	bool bChar = false;
	if( iIndex > 4 )
		bChar = true;

	char szBuf[MAX_PATH] = "";
	int iCurLevel = 0;
	int iNextLevel = 0;

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		if( bChar )
		{
			int iSlot = iIndex-5;
			iCurLevel = pLevel->GetCharGrowthLevel( m_iClassType, iSlot, true );
		}
		else
		{
			int iSlot = iIndex - 1;
			iCurLevel = pLevel->GetItemGrowthLevel( m_iClassType, iSlot, true );
		}

		if( iCurLevel > 0 )
			iNextLevel = iCurLevel - 1;
	}

	int iGetPoint = 0;
	if( iCurLevel > 0 ) 
		iGetPoint = g_GrowthInfoMgr.GetGrowthUpNeedPoint( bChar );

	// 육성복구 포인트 정보
	m_Desc[0].ClearList();
	m_Desc[1].ClearList();

	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( TCT_DEFAULT_WHITE );
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_13, STR(1), iGetPoint );

	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( TCT_DEFAULT_WHITE );
	m_Desc[1].SetTextColor( TCT_DEFAULT_RED );
	m_Desc[1].AddTextPiece( FONT_SIZE_13, STR(2) );
}

void GrowthDownSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "P_Img" )
	{
		SAFEDELETE( m_pPImg );
		m_pPImg = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );	
	}	
}

void GrowthDownSelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	char szKeyName[MAX_PATH]="";
	ioHashString szTemp;

	for (int i = 0; i < 8 ; i++)
	{
		ZeroMemory( szKeyName, sizeof( szKeyName ) );
		StringCbPrintf( szKeyName, sizeof( szKeyName ), "GrowthBtn%d", i+1 );
		szTemp  = xElement.GetStringAttribute( szKeyName );
		m_TitleText[i] = szTemp;
	}

	m_vExtraClassVec.clear();
	m_vExtraClassVec.reserve(10);
	int iMax = xElement.GetIntAttribute_e( "MaxClass" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "ClassType%d", i+1 );
		int iClassType = xElement.GetIntAttribute( szName );
		if( iClassType  <= 0 )
			continue;
		m_vExtraClassVec.push_back( iClassType );
	}
}

void GrowthDownSelectWnd::SendGrowthDown()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( !pClassBtn || pClassBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	SelectListBtn *pBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
	if( !pBtn || pBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}	

	m_iClassType = pClassBtn->GetListMagicCode();
	int iIndex = pBtn->GetListMagicCode();

	int iSlot = 0;
	bool bItem = true;
	if( iIndex > 4 )
	{
		bItem = false;
		iSlot = iIndex-5;
	}
	else
	{
		iSlot = iIndex - 1;
	}

	m_iReUseTargetClass = m_iClassType;
	
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_GROWTH_DOWN;
	kPacket << m_iClassType;
	kPacket << bItem;
	kPacket << iSlot;
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void GrowthDownSelectWnd::CheckSelectBtn()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( !pClassBtn || pClassBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		return;
	}

	SelectListBtn *pBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
	if( !pBtn || pBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		return;
	}

	if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		return;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		return;
	}	

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(2) );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, STR(3), "", STR(4), STR(5) );
}

void GrowthDownSelectWnd::GetClassList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ClassTypeInfoList vClassList;
	vClassList.clear();

	int iExtraSize = (int) m_vExtraClassVec.size();
	vClassList.reserve( g_ClassPrice.MaxActiveClassPrice() + iExtraSize );
	/*
	int iMax = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i < iMax; i++)
	{
		if( !g_ClassPrice.IsActiveClass( i ) )
		{
			// inactive 용병이지만 표시해야 하는 경우가 있다 ( 해외 가챠 캡슐용병등 )
			bool bContinue = true;
			int iClassType = g_ClassPrice.GetClassType( i );
			int iSize = m_vExtraClassVec.size();
			for (int j = 0; j < iSize ; j++)
			{
				int &rClassType = m_vExtraClassVec[j];
				if( iClassType == rClassType )
				{
					bContinue = false;
					break;
				}
			}

			if( bContinue )
				continue;
		}

		ClassTypeInfo kInfo;
		kInfo.m_iClassType = g_ClassPrice.GetClassType( i );
		kInfo.m_iCurLevel  = g_MyInfo.GetClassLevel( kInfo.m_iClassType, true );

		vClassList.push_back( kInfo );
	}
	*/
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( !pLevel )
		return;

	for ( int i=0; i< g_MyInfo.GetCharCount(); i++ )
	{
		ClassTypeInfo kInfo;
		kInfo.m_iClassType = g_MyInfo.GetClassType( i );
		kInfo.m_iCurLevel  = g_MyInfo.GetClassLevel( kInfo.m_iClassType, true );
		
		if ( kInfo.m_iClassType <= 0 || kInfo.m_iCurLevel <= 0 )
			continue;		
		
		int nGrowthLevel = 0;
		for( int i=0; i < 8; ++i )
		{			
			int iSlot = i;
			int iItemCode = 0;

			bool bItem = true;
			if( iSlot >= 4 )
			{
				bItem = false;
				iSlot = iSlot-4;
			}

			int iLevel = 0;
			if( bItem )
				iLevel = pLevel->GetItemGrowthLevel( kInfo.m_iClassType, iSlot, true );
			else
				iLevel = pLevel->GetCharGrowthLevel( kInfo.m_iClassType, iSlot, true );

			nGrowthLevel += iLevel;
		}

		if ( nGrowthLevel <= 0 ) 
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
		StringCbPrintf( szName, sizeof( szName ), "%s", g_MyInfo.GetClassName( kItem.m_iMagicCode, g_MyInfo.GetCharPowerUpGradeByClassType( kItem.m_iMagicCode ) ) );
		kItem.m_dwTextColor = g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( kItem.m_iMagicCode ) );
		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}

void GrowthDownSelectWnd::GetGrowthList( vNewShopPullDownItem &rkList )
{
	rkList.clear();
	rkList.reserve( 8 );

	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		for( int i=0; i < 8; ++i )
		{
			int iIndex = i+1;


			int iSlot = 0;
			int iItemCode = 0;


			bool bItem = true;
			if( iIndex > 4 )
			{
				bItem = false;
				iSlot = iIndex-5;
			}
			else
			{
				iSlot = iIndex-1;
			}

			int iLevel = 0;
			if( bItem )
				iLevel = pLevel->GetItemGrowthLevel( m_iClassType, iSlot, true );
			else
				iLevel = pLevel->GetCharGrowthLevel( m_iClassType, iSlot, true );

			NewShopPullDownItem kItem;
			kItem.m_iMagicCode = iIndex;

			char szName[MAX_PATH]="";
			StringCbPrintf( szName, sizeof( szName ), "%s +%d", m_TitleText[i].c_str(), iLevel );
			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
			kItem.m_szName      = szName;
			rkList.push_back( kItem );
		}
	}
}

void GrowthDownSelectWnd::SetClassSelect( ioWnd *pOpenBtn, int iListArray )
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

	SelectListBtn *pTarget = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pTarget && iMagicCode > 0 )
	{
		pTarget->SetListBtnInfo( rkItem.m_szName, iMagicCode );
		pTarget->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iMagicCode ) ), 0 );
	}
}

void GrowthDownSelectWnd::SetReUseTargetClassSelect( int iClassType )
{
	if( iClassType == 0 )
		return;

	SelectListBtn *pTarget = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( !pTarget )
		return;

	vNewShopPullDownItem kItemList;
	GetClassList( kItemList );

	vNewShopPullDownItem::iterator iter = kItemList.begin();
	for( ; iter != kItemList.end(); ++iter )
	{
		const NewShopPullDownItem& rkItem = *iter;
		if( rkItem.m_iMagicCode == iClassType )
		{
			pTarget->SetListBtnInfo( rkItem.m_szName, iClassType );
			pTarget->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iClassType ) ), 0 );
			break;
		}
	}
}

void GrowthDownSelectWnd::SetGrowthSelect( ioWnd *pOpenBtn, int iListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetGrowthList( kItemList );

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

	SelectListBtn *pTarget = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_GROWTH_BTN));
	if( pTarget && iMagicCode > 0 )
		pTarget->SetListBtnInfo( rkItem.m_szName, iMagicCode );
}

void GrowthDownSelectWnd::PrintManual( int a_iXPos, int a_iYPos , float fScale )
{
	if( !m_pManual )
		return;

	ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_GROWTH_DOWN );
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GrowthAllDownSelectWnd::GrowthAllDownSelectWnd(void)
{
	m_iClassType = 0;

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
}

GrowthAllDownSelectWnd::~GrowthAllDownSelectWnd(void)
{
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_Desc[i].ClearList();
	}
	
	for (int i = 0; i < MAX_TITLE ; i++)
	{
		m_sTitles[i].ClearList();
	}
}

void GrowthAllDownSelectWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/GrowthHelpWnd.xml", this );
}

void GrowthAllDownSelectWnd::iwm_show()
{	
	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_Desc[i].ClearList();
	}

	for (int i = 0; i < MAX_TITLE ; i++)
	{
		m_sTitles[i].ClearList();
	}

	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
		pClassBtn->InitData();

	HideChildWnd( ID_CLASS_LIST_WND );

	UpdateData();
}

void GrowthAllDownSelectWnd::iwm_hide()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
		pClassBtn->InitData();

	HideChildWnd( ID_CLASS_LIST_WND );
	HideChildWnd( ID_SOLDIER_WND );
}

void GrowthAllDownSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
						int iNewYPos = pWnd->GetYPos() - iHeight - 2;
						pListWnd->SetWndPos( pWnd->GetXPos(), iNewYPos );
					}
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					return;
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
			CheckSelectBtn();
		}
		break;
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendGrowthDown();
			}
		}
		break;
	}
}

void GrowthAllDownSelectWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		X_OFFSET_TITLE= 33,
		Y_OFFSET_TITLE= 353,
		TITLE_LINE_GAP= 21,
		TITLE_X_GAP   = 103,
	};

	for (int i = 0; i < MAX_TITLE/2 ; i++)
	{
		m_sTitles[i].PrintFullText( iXPos + X_OFFSET_TITLE, iYPos + Y_OFFSET_TITLE + (i*TITLE_LINE_GAP), TAT_LEFT );
		m_sTitles[i+(MAX_TITLE/2)].PrintFullText( iXPos + X_OFFSET_TITLE + TITLE_X_GAP, iYPos + Y_OFFSET_TITLE + (i*TITLE_LINE_GAP), TAT_LEFT );
	}
}

void GrowthAllDownSelectWnd::OnRenderAfterChild()
{
	ioWnd *pWnd = FindChildWnd( ID_CLASS_LIST_WND );
	if( pWnd && pWnd->IsShow() ) return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum
	{
		X_OFFSET_DESC = 129,
		Y_OFFSET_DESC = 276,
		DESC_LINE_GAP = 18,
	};

	for (int i = 0; i < MAX_DESC ; i++)
	{
		m_Desc[i].PrintFullText( iXPos + X_OFFSET_DESC, iYPos + Y_OFFSET_DESC + (i*DESC_LINE_GAP), TAT_CENTER );
	}
}

void GrowthAllDownSelectWnd::UpdateData()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pClassBtn )
	{
		m_iClassType = pClassBtn->GetListMagicCode();
	}
	
	if( m_iClassType <= 0 )
	{
		vNewShopPullDownItem kItemList;
		GetClassList( kItemList );

		if( !kItemList.empty() )
			m_iClassType = kItemList[0].m_iMagicCode;
		else
			m_iClassType = 1;

		ioHashString sName = g_MyInfo.GetClassName( m_iClassType, g_MyInfo.GetCharPowerUpGradeByClassType( m_iClassType ) );
		SelectListBtn *pTarget = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
		if( pTarget )
		{
			pTarget->SetListBtnInfo( sName, m_iClassType );
			pTarget->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( m_iClassType ) ), 0 );
		}
	}

	// total point
	int iTotalPoint = 0;
	if( m_iClassType > 0 )
	{
		int iLevel  = g_MyInfo.GetClassLevel( m_iClassType, true );
		iTotalPoint = g_GrowthInfoMgr.GetTotalGrowthPoint( iLevel );

		const int iSexType = 0; // 0 : 남자 , 1 : 여자
		int iType = ( m_iClassType*100000 ) + ( iSexType * 1000 ) + UID_KINDRED; // 해당 종족에 여자 치장
		int iCode = RDT_HUMAN_WOMAN;
		if( g_MyInfo.IsSlotItem( iType, iCode ) )
			iTotalPoint += g_GrowthInfoMgr.GetWomanTotalGrowthPoint();
	}

	// level , point
	int iTotalUsePoint = 0;
	int iCharLevels[MAX_CHAR_GROWTH]={ 0, 0, 0, 0 };
	int iItemLevels[MAX_ITEM_GROWTH]={ 0, 0, 0, 0 };
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
		{
			iCharLevels[i] = ( pLevel->GetCharGrowthLevel( m_iClassType, i, true ) - pLevel->GetInstantLevel() );
			iTotalUsePoint += ( iCharLevels[i] * g_GrowthInfoMgr.GetGrowthUpNeedPoint( true ) );
		}

		for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
		{
			iItemLevels[i] = ( pLevel->GetItemGrowthLevel( m_iClassType, i, true ) - pLevel->GetInstantLevel() );
			iTotalUsePoint += ( iItemLevels[i] * g_GrowthInfoMgr.GetGrowthUpNeedPoint( false ) );
		}
	}

	// check
	if( iTotalUsePoint > iTotalPoint )
		iTotalUsePoint = iTotalPoint;

	// peso
	__int64 iReturnPeso = 0;
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		for (int j = 1; j < iCharLevels[i]+1 ; j++)
		{
			iReturnPeso += g_GrowthInfoMgr.GetGrowthReturnAllPeso( true, j );	
		}
	}
	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
	{
		for (int j = 1; j < iItemLevels[i]+1 ; j++)
		{
			iReturnPeso += g_GrowthInfoMgr.GetGrowthReturnAllPeso( false, j );	
		}
	}

	char szPoint[MAX_PATH]="";
	Help::ConvertNumToStrComma( iTotalUsePoint, szPoint, sizeof( szPoint ) );

	char szPeso[MAX_PATH]="";
	int iPeso = (int)iReturnPeso;
	if( iPeso < 0 )
		iPeso = 0;
	Help::ConvertNumToStrComma( iPeso, szPeso, sizeof( szPeso ) );

	// 육성복구 포인트 정보
	m_Desc[0].ClearList();
	m_Desc[1].ClearList();

	m_Desc[0].SetTextStyle( TS_NORMAL );
	m_Desc[0].SetBkColor( TCT_DEFAULT_WHITE );
	m_Desc[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[0].AddTextPiece( FONT_SIZE_13, "육성포인트 " );

	m_Desc[0].SetTextColor( TCT_DEFAULT_RED );
	m_Desc[0].AddTextPiece( FONT_SIZE_13, "%s 복구", szPoint );

	m_Desc[1].SetTextStyle( TS_NORMAL );
	m_Desc[1].SetBkColor( TCT_DEFAULT_WHITE );
	m_Desc[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
	m_Desc[1].AddTextPiece( FONT_SIZE_13, "페소 " );

	m_Desc[1].SetTextColor( TCT_DEFAULT_BLUE );
	m_Desc[1].AddTextPiece( FONT_SIZE_13, "%s 복구", szPeso );

	// titles
	for (int i = 0; i < MAX_TITLE ; i++)
	{
		m_sTitles[i].ClearList();
	}

	char szItemTitles[MAX_ITEM_GROWTH][MAX_PATH]={ "무기스킬", "갑옷스킬", "투구스킬", "망토스킬" };
	for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
	{
		m_sTitles[i].SetTextStyle( TS_NORMAL );
		m_sTitles[i].SetBkColor( TCT_DEFAULT_WHITE );

		if( iItemLevels[i] > 0 )
		{
			m_sTitles[i].SetTextColor( TCT_DEFAULT_DARKGRAY );
			m_sTitles[i].AddTextPiece( FONT_SIZE_12, szItemTitles[i] );

			m_sTitles[i].SetTextColor( TCT_DEFAULT_GREEN );
			m_sTitles[i].AddTextPiece( FONT_SIZE_12, "+%d", iItemLevels[i] );
		}
		else
		{
			m_sTitles[i].SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			m_sTitles[i].AddTextPiece( FONT_SIZE_12, szItemTitles[i] );
		}
	}
	
	char szCharTitles[MAX_CHAR_GROWTH][MAX_PATH]={ "공격력", "방어력", "이동속도", "드랍피해감소" };
	for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
	{
		int iArray = i+(MAX_TITLE/2);

		m_sTitles[iArray].SetTextStyle( TS_NORMAL );
		m_sTitles[iArray].SetBkColor( TCT_DEFAULT_WHITE );

		if( iCharLevels[i] > 0 )
		{
			m_sTitles[iArray].SetTextColor( TCT_DEFAULT_DARKGRAY );
			m_sTitles[iArray].AddTextPiece( FONT_SIZE_12, szCharTitles[i] );

			m_sTitles[iArray].SetTextColor( TCT_DEFAULT_GREEN );
			m_sTitles[iArray].AddTextPiece( FONT_SIZE_12, "+%d", iCharLevels[i] );
		}
		else
		{
			m_sTitles[iArray].SetTextColor( TCT_DEFAULT_LIGHTGRAY );
			m_sTitles[iArray].AddTextPiece( FONT_SIZE_12, szCharTitles[i] );
		}
	}

	// char info
	Help::GetDefaultCharInfo( m_CharInfo, m_iClassType );

	NewShopHelpWnd *pCharWnd = dynamic_cast<NewShopHelpWnd *>( FindChildWnd( ID_SOLDIER_WND ));
	if( pCharWnd )
	{
		pCharWnd->SetZPosition( 600.0f );
		pCharWnd->SetChar( m_CharInfo );
		pCharWnd->SetEquip( true, false, true );
		pCharWnd->ShowWnd();
	}
}

void GrowthAllDownSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	ioWnd::AddRenderImage( szType, pImage );		
}

void GrowthAllDownSelectWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_vExtraClassVec.clear();
	m_vExtraClassVec.reserve(10);
	int iMax = xElement.GetIntAttribute_e( "MaxClass" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "ClassType%d", i+1 );
		int iClassType = xElement.GetIntAttribute( szName );
		if( iClassType  <= 0 )
			continue;
		m_vExtraClassVec.push_back( iClassType );
	}
}

void GrowthAllDownSelectWnd::SendGrowthDown()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( !pClassBtn || pClassBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "용병이 선택되지 않았습니다." );
		return;
	}

	if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#육성 복구가 불가능합니다." );
		return;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return;
	}	

	int iTotalLevel = 0;
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
		{
			iTotalLevel += ( pLevel->GetCharGrowthLevel( m_iClassType, i, true ) - pLevel->GetInstantLevel() );
		}

		for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
		{
			iTotalLevel += ( pLevel->GetItemGrowthLevel( m_iClassType, i, true ) - pLevel->GetInstantLevel() );
		}
	}

	if( iTotalLevel <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "육성이 없는 용병입니다." );
		return;
	}

	TimeGrowthInfo *pInfo = pLevel->FindTimeGrowthInfo( m_iClassType );
	if( pInfo )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택하신 용병이 시간육성중이라 복구가 불가능합니다." );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "시간육성을 취소 또는 완료후 다시 시도하여 주십시오." );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}

	m_iClassType = pClassBtn->GetListMagicCode();

	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_GROWTH_ALL_DOWN;
	kPacket << m_iClassType;
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );

	HideWnd();
}

void GrowthAllDownSelectWnd::CheckSelectBtn()
{
	SelectListBtn *pClassBtn = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( !pClassBtn || pClassBtn->GetListMagicCode() <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "용병이 선택되지 않았습니다." );
		return;
	}

	if( g_MyInfo.IsCharRentalToClassType( m_iClassType ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 설정이 되어있는 용병은#육성 복구가 불가능합니다." );
		return;
	}

	if( g_MyInfo.IsCharExerciseStyleToClassType( m_iClassType, EXERCISE_RENTAL ) )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "전투체험 용병으로는 불가능합니다." );
		return;
	}	

	int iTotalLevel = 0;
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
		{
			iTotalLevel += ( pLevel->GetCharGrowthLevel( m_iClassType, i, true ) - pLevel->GetInstantLevel() );
		}

		for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
		{
			iTotalLevel += ( pLevel->GetItemGrowthLevel( m_iClassType, i, true ) - pLevel->GetInstantLevel() );
		}
	}

	if( iTotalLevel <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "육성이 없는 용병입니다." );
		return;
	}

	TimeGrowthInfo *pInfo = pLevel->FindTimeGrowthInfo( m_iClassType );
	if( pInfo )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택하신 용병이 시간육성중이라 복구가 불가능합니다." );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "시간육성을 취소 또는 완료후 다시 시도하여 주십시오." );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "육성복구 아이템을" );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter[1].AddTextPiece( FONT_SIZE_13, "정말 사용하시겠습니까?" );

	g_GUIMgr.SetPrevMsgListBoxWithTitle( NULL, MB_YESNO, this, kPrinter, "육성복구", "", "사용", "취소" );
}

void GrowthAllDownSelectWnd::GetClassList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ClassTypeInfoList vClassList;
	vClassList.clear();

	int iExtraSize = (int) m_vExtraClassVec.size();
	vClassList.reserve( g_ClassPrice.MaxActiveClassPrice() + iExtraSize );
	/*
	int iMax = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i < iMax; i++)
	{
		if( !g_ClassPrice.IsActiveClass( i ) )
		{
			// inactive 용병이지만 표시해야 하는 경우가 있다 ( 해외 가챠 캡슐용병등 )
			bool bContinue = true;
			int iClassType = g_ClassPrice.GetClassType( i );
			int iSize = m_vExtraClassVec.size();
			for (int j = 0; j < iSize ; j++)
			{
				int &rClassType = m_vExtraClassVec[j];
				if( iClassType == rClassType )
				{
					bContinue = false;
					break;
				}
			}

			if( bContinue )
				continue;
		}

		ClassTypeInfo kInfo;
		kInfo.m_iClassType = g_ClassPrice.GetClassType( i );
		kInfo.m_iCurLevel  = g_MyInfo.GetClassLevel( kInfo.m_iClassType, true );

		vClassList.push_back( kInfo );
	}
	*/
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( !pLevel )
		return;

	for ( int i=0; i< g_MyInfo.GetCharCount(); i++ )
	{
		ClassTypeInfo kInfo;
		kInfo.m_iClassType = g_MyInfo.GetClassType( i );
		kInfo.m_iCurLevel  = g_MyInfo.GetClassLevel( kInfo.m_iClassType, true );

		if ( kInfo.m_iClassType <= 0 || kInfo.m_iCurLevel <= 0 )
			continue;		

		int nGrowthLevel = 0;
		for( int i=0; i < 8; ++i )
		{			
			int iSlot = i;
			int iItemCode = 0;

			bool bItem = true;
			if( iSlot >= 4 )
			{
				bItem = false;
				iSlot = iSlot-4;
			}

			int iLevel = 0;
			if( bItem )
				iLevel = pLevel->GetItemGrowthLevel( kInfo.m_iClassType, iSlot, true );
			else
				iLevel = pLevel->GetCharGrowthLevel( kInfo.m_iClassType, iSlot, true );

			nGrowthLevel += iLevel;
		}

		if ( nGrowthLevel <= 0 ) 
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
		StringCbPrintf( szName, sizeof( szName ), "%s", g_MyInfo.GetClassName( kItem.m_iMagicCode, g_MyInfo.GetCharPowerUpGradeByClassType(kItem.m_iMagicCode) ) );
		kItem.m_dwTextColor = g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( kItem.m_iMagicCode ) );
		kItem.m_szName      = szName;
		rkList.push_back( kItem );
	}
}


void GrowthAllDownSelectWnd::SetClassSelect( ioWnd *pOpenBtn, int iListArray )
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

	SelectListBtn *pTarget = dynamic_cast<SelectListBtn*>(FindChildWnd(ID_CHANGE_CLASS_BTN));
	if( pTarget && iMagicCode > 0 )
	{
		pTarget->SetListBtnInfo( rkItem.m_szName, iMagicCode );
		pTarget->SetTitleColor( g_UIImageRenderMgr.GetGradeTextColor( g_UIImageRenderMgr.GetPowerUpGradeType( iMagicCode ) ), 0 );
	}
}