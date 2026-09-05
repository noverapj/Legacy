
#include "StdAfx.h"

#include "PetEatWnd.h"

#include "SelectPetTargetBtn.h"
#include "ItemMaterialCompoundWnd.h"
#include "PetWnd.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PetEatWnd::PetEatWnd(void)
{
	m_pManualMark = NULL;
	m_nSelectPetSlot = 0;

	InitData();
}

PetEatWnd::~PetEatWnd(void)
{
	SAFEDELETE( m_pManualMark );
}

void PetEatWnd::iwm_show()
{
	if ( m_pManualMark )
		m_pManualMark->SetColor( 128, 128, 128 );

	ResetWnd();
	AutoShowMaterialList();	
}

void PetEatWnd::iwm_hide()
{
	m_nSelectPetSlot = 0;
	InitData();
	HideMainInfo();
}

void PetEatWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EAT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendExpUp();
		}
		break;
	case ID_BACK_BTN:
	case ID_EMPTY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.FindWnd( PET_WND ) );
			if ( pPetWnd && pPetWnd->IsShow() )
				pPetWnd->ResetMainMenu();

			HideWnd();
		}
		break;
	case ID_MATERIALPET_BTN:
		if( cmd == IOBN_BTNUP )
		{
			//리스트 설정
			NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_MATERIALPET_WND));
			if ( !pListWnd )
				return;

			if( pListWnd->IsShow() )
				pListWnd->HideWnd();
			else
				AutoShowMaterialList();
		}
		break;
	case ID_MATERIALPET_WND:
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
	case ID_HELP_MARK:
		if( cmd == IOBN_BTNUP )
		{
			HideChildWnd( ID_HELP_TOOL_TIP );
			ManualListWnd *pManualWnd = dynamic_cast<ManualListWnd*> (g_GUIMgr.FindWnd( MANUAL_LIST_WND ) );
			if( pManualWnd )
				pManualWnd->ShowWndByCurManual( m_nHelpTipIdx );
		}
		else if( cmd == IOWN_OVERED )
		{
			if ( m_pManualMark )
				m_pManualMark->SetColor( 206, 78, 0 );

			ShowChildWnd( ID_HELP_TOOL_TIP );
		}
		else if( cmd == IOWN_LEAVED )
		{
			if ( m_pManualMark )
				m_pManualMark->SetColor( 128, 128, 128 );

			HideChildWnd( ID_HELP_TOOL_TIP );
		}
		break;
	}
}

void PetEatWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PetEatWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_nHelpTipIdx = xElement.GetIntAttribute_e( "help_tip_idx" );	
}

void PetEatWnd::OnRender()
{
	ioWnd::OnRender();

	if ( !m_bEmpty || !m_pManualMark )
		return;
	
	enum
	{
		MANUAL_X = 129,
		MANUAL_Y = 429,

		MARK_X = 183,
		MARK_Y = 428,
	};

	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );	
	kPrinter.PrintFullText( nXPos + MANUAL_X, nYPos + MANUAL_Y, TAT_CENTER );
	kPrinter.ClearList();	

	m_pManualMark->Render( nXPos + MARK_X, nYPos + MARK_Y, UI_RENDER_NORMAL_ADD_COLOR );
}

//펫코드 설정
void PetEatWnd::SelectPetSlot( int nSelectSlot )
{	
	m_nSelectPetSlot = nSelectSlot;
	ShowWnd();
}

//재료 코드 설정
void PetEatWnd::SetMaterialCode( int nMaterialCode )
{
	if ( m_nSelectPetSlot <= 0 || m_nSelectMaterialCode == nMaterialCode )
		return;

	m_nSelectMaterialCode = nMaterialCode;
	m_bIsSelectMaterial = true;
}

//초기화
void PetEatWnd::InitData()
{
	m_nSelectMaterialCode = 0;
	m_bIsSelectMaterial = false;
	m_bEmpty = false;
	m_bMaxLv = false;
}

//자동 재료 아이템 설정
void PetEatWnd::AutoShowMaterialList()
{
	ioWnd *pWnd = FindChildWnd( ID_MATERIALPET_BTN );
	if ( !pWnd )
		return;

	//리스트 설정
	NewShopPullDownWnd *pListWnd = dynamic_cast<NewShopPullDownWnd*>(FindChildWnd(ID_MATERIALPET_WND));
	if( pListWnd )
	{
		vNewShopPullDownItem kItemList;
		GetMaterialList( kItemList );

		if( !kItemList.empty() )
		{
			int nSize = kItemList.size();
			int nYPos = GetMaterialListPosY( nSize );
			pListWnd->CreatePullDown( pWnd, pWnd->GetXPos(), pWnd->GetYPos() - nYPos );
			for(int i = 0;i < nSize; i++)
				pListWnd->InsertItem( kItemList[i] );

			pListWnd->ShowWnd();
			SetChildActive( ID_MATERIALPET_BTN );
		}
		else
		{
			HideMainInfo();
			ShowChildWnd( ID_EMPTY_BTN );
			ShowChildWnd( ID_HELP_MARK );
			SetChildInActive( ID_MATERIALPET_BTN );
			m_bEmpty = true;
		}
	}
}

//리셋 윈도우
void PetEatWnd::ResetWnd()
{
	ShowMainInfo();

	HideChildWnd( ID_EMPTY_BTN );
	HideChildWnd( ID_HELP_MARK );
	HideChildWnd( ID_MATERIALPET_WND );

	InitData();
		
	SelectPetTargetBtn *pMaterialItemBtn = dynamic_cast<SelectPetTargetBtn*>(FindChildWnd(ID_MATERIALPET_BTN));
	if( pMaterialItemBtn )
	{
		pMaterialItemBtn->SetSelectMaterial( 0, "" );
		pMaterialItemBtn->SetCurTitleColor( TCT_DEFAULT_DARKGRAY );
	}
}

//경험치 패킷 전송
void PetEatWnd::SendExpUp()
{
	if ( m_nSelectPetSlot <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if ( m_nSelectMaterialCode <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	if ( m_bMaxLv )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	bool bTarget = false;
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( pUserPet )
	{
		PetSlot sPetSlot;
		if( pUserPet->GetPetSlot( m_nSelectPetSlot, sPetSlot ) )
			bTarget = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return;

	ioUserEtcItem::ETCITEMSLOT rkEtcItem;
	if( !pUserEtc->GetEtcItem( m_nSelectMaterialCode, rkEtcItem ) )
	{
		ResetWnd();
		AutoShowMaterialList();
		return;
	}

	SP2Packet kPacket( CTPK_PET_NURTURE );
	kPacket << m_nSelectPetSlot;
	kPacket << m_nSelectMaterialCode;
	TCPNetwork::SendToServer( kPacket );
	TCPNetwork::MouseBusy( true );
}

//재료 리스트 위치 설정
int PetEatWnd::GetMaterialListPosY( int nSize )
{
	if ( nSize > 6)
		return 144;

	switch( nSize )
	{
	case 1:
		return 39;
	case 2:
		return 60;
	case 3:
		return 81;
	case 4:
		return 102;
	case 5:
		return 123;
	case 6:
		return 144;
	}
	return 0;
}

//재료 리스트 얻기
void PetEatWnd::GetMaterialList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return;

	for( int iEtcCode = ioEtcItem::EIT_ETC_PET_FEED_01; iEtcCode < ioEtcItem::EIT_ETC_ADDICTIVE_PIECE + 1; iEtcCode++ )
	{
		ioUserEtcItem::ETCITEMSLOT rkEtcItem;
		if( pUserEtc->GetEtcItem( iEtcCode, rkEtcItem ) )
		{
			NewShopPullDownItem kItem;	

			int nMaterialCode = rkEtcItem.m_iType;
			int nMaterialCount = rkEtcItem.m_iValue1;

			if( iEtcCode == ioEtcItem::EIT_ETC_ADDICTIVE_PIECE &&
				nMaterialCount < g_PetInfoMgr.GetPetEatAdditiveCount() )
				continue;

			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( rkEtcItem.m_iType );
			if( !pEtcItem )
				continue;

			char szCnt[MAX_PATH]="";	
			wsprintf_e( szCnt, "%d개", nMaterialCount );

			kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
			kItem.m_dwCntTextColor = TCT_DEFAULT_DARKGRAY;
			kItem.m_szName = pEtcItem->GetName();
			kItem.m_iMagicCode = nMaterialCode;
			kItem.m_bCount = true;
			kItem.m_szCnt = szCnt;
			rkList.push_back( kItem );
		}
	}
}

//재료 리스트 버튼 클릭 시
void PetEatWnd::MaterialListBtnUp( ioWnd *pOpenBtn, int nListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetMaterialList( kItemList );	

	if( !COMPARE( nListArray, 0, (int)kItemList.size() ) ) 
		return;			

	ioUITitle *pTitle = pOpenBtn->GetTitle();
	if( !pTitle )
		return;

	ioHashString szTitle;
	pTitle->GetFirstText( szTitle );

	NewShopPullDownItem &rkItem = kItemList[nListArray];
	if( szTitle == rkItem.m_szName ) 
		return;

	int nSelectMaterialCode = rkItem.m_iMagicCode;
	SetMaterialCode( nSelectMaterialCode );

	SelectPetTargetBtn *pTarget = dynamic_cast<SelectPetTargetBtn*>(FindChildWnd(ID_MATERIALPET_BTN));
	if( pTarget )
	{
		pTarget->SetSelectMaterial( nSelectMaterialCode, rkItem.m_szName.c_str() );
		pTarget->SetCurTitleColor( rkItem.m_dwTextColor );
	}
}

//최대 레벨시 UI 적용
void PetEatWnd::ApplyMaxLv()
{
	SetChildInActive( ID_MATERIALPET_BTN );
	m_bMaxLv = true;
}

//UI 설정
void PetEatWnd::HideMainInfo()
{
	HideChildWnd( ID_MATERIALPET_BTN );
	HideChildWnd( ID_MATERIALPET_WND );
	HideChildWnd( ID_EAT_BTN );
	HideChildWnd( ID_BACK_BTN );
}

void PetEatWnd::ShowMainInfo()
{
	ShowChildWnd( ID_MATERIALPET_BTN );
	ShowChildWnd( ID_EAT_BTN );
	ShowChildWnd( ID_BACK_BTN );
}