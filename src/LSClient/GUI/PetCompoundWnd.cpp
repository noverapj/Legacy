
#include "StdAfx.h"

#include "PetCompoundWnd.h"

#include "SelectPetTargetBtn.h"
#include "ItemMaterialCompoundWnd.h"

#include "PetWnd.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PetCompoundWnd::PetCompoundWnd(void)
{
	m_pManualMark = NULL;
	m_nSelectPetSlot = 0;

	InitData();
}

PetCompoundWnd::~PetCompoundWnd(void)
{
	SAFEDELETE( m_pManualMark );
}

void PetCompoundWnd::iwm_create()
{
}

void PetCompoundWnd::iwm_show()
{
	if ( m_pManualMark )
		m_pManualMark->SetColor( 128, 128, 128 );

	SetChildActive( ID_MATERIALPET_BTN );
	ResetWnd();
	AutoShowMaterialList();	
}

void PetCompoundWnd::iwm_hide()
{
	m_nSelectPetSlot = 0;
	InitData();
	HideMainInfo();
	HideChildWnd( ID_RESULT_BTN );
}

void PetCompoundWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_COMPOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			SendCompound();
		}
		break;
	case ID_BACK_BTN:
	case ID_RESULT_BTN:
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetCompoundWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ManualMark" )
	{
		SAFEDELETE( m_pManualMark );
		m_pManualMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void PetCompoundWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_nHelpTipIdx = xElement.GetIntAttribute_e( "help_tip_idx" );	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void PetCompoundWnd::OnRender()
{
	ioWnd::OnRender();
	
	int nXPos = GetDerivedPosX();
	int nYPos = GetDerivedPosY();
	
	RenderEmpty( nXPos, nYPos );
	RenderResult( nXPos, nYPos );
}

void PetCompoundWnd::RenderEmpty( int nXPos, int nYPos )
{
	if ( !m_bEmpty || !m_pManualMark )
		return;

	enum
	{
		MANUAL_X = 132,
		MANUAL_Y = 429,

		MARK_X = 199,
		MARK_Y = 428,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );	
	kPrinter.PrintFullText( nXPos + MANUAL_X, nYPos + MANUAL_Y, TAT_CENTER );
	kPrinter.ClearList();

	m_pManualMark->Render( nXPos + MARK_X, nYPos + MARK_Y, UI_RENDER_NORMAL_ADD_COLOR );
}

void PetCompoundWnd::RenderResult( int nXPos, int nYPos )
{
	if ( !m_bResult )
		return;

	enum
	{
		MANUAL_X = 132,
		MANUAL_Y = 429,
	};

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );	
	kPrinter.PrintFullText( nXPos + MANUAL_X, nYPos + MANUAL_Y, TAT_CENTER );
	kPrinter.ClearList();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//펫코드 설정
void PetCompoundWnd::SelectPetSlot( int nSelectSlot )
{	
	m_nSelectPetSlot = nSelectSlot;
	ShowWnd();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//초기화
void PetCompoundWnd::InitData()
{
	m_vTotalPetList.clear();
	m_bEmpty = false;
	m_bResult = false;	
}

//펫리스트 얻기
void PetCompoundWnd::LoadTotalPetList()
{
	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( !pUserPet )
	{
		HideWnd();
		return;
	}

	int nSize = pUserPet->GetPetSlotCnt();
	for( int i=0; i<nSize; ++i )
	{
		PetSlot sPetSlot;
		if( !pUserPet->GetPetSlotArray( i, sPetSlot ) )
			continue;

		if( sPetSlot.m_nPetCode <= 0 || sPetSlot.m_PetRankType <=0 )
			continue;

		PetPullDownInfo sTargetInfo;
		sTargetInfo.m_nPetCode = sPetSlot.m_nPetCode;
		sTargetInfo.m_nPetRank = sPetSlot.m_PetRankType;
		sTargetInfo.m_nSlotIndex = sPetSlot.m_nIndex;

		m_vTotalPetList.push_back( sTargetInfo );
	}

	std::sort( m_vTotalPetList.begin(), m_vTotalPetList.end(), PetPullDownInfoSort() );
}

//자동 재료 아이템 설정
void PetCompoundWnd::AutoShowMaterialList()
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
		}
		else
		{
			SetChildInActive( ID_MATERIALPET_BTN );
			HideMainInfo();
			ShowChildWnd( ID_EMPTY_BTN );
			ShowChildWnd( ID_HELP_MARK );
			m_bEmpty = true;
		}
	}
}

//리셋 윈도우
void PetCompoundWnd::ResetWnd()
{
	ShowMainInfo();

	HideChildWnd( ID_EMPTY_BTN );
	HideChildWnd( ID_HELP_MARK );
	HideChildWnd( ID_RESULT_BTN );
	HideChildWnd( ID_MATERIALPET_WND );

	InitData();

	LoadTotalPetList();	

	SelectPetTargetBtn *pTarget = dynamic_cast<SelectPetTargetBtn*>(FindChildWnd(ID_MATERIALPET_BTN));
	if( pTarget )
		pTarget->InitData();	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//합성 패킷 전송
void PetCompoundWnd::SendCompound()
{
	if ( m_nSelectPetSlot <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	SelectPetTargetBtn *pMaterial = dynamic_cast<SelectPetTargetBtn*>(FindChildWnd( ID_MATERIALPET_BTN ));
	if ( !pMaterial )
		return;

	int nMaterialSlot = pMaterial->GetSelectSlot();
	if ( nMaterialSlot <= 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	bool bTarget, bMaterial;
	bTarget = false;
	bMaterial = false;

	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( pUserPet )
	{
		PetSlot sPetSlot;
		if( pUserPet->GetPetSlot( m_nSelectPetSlot, sPetSlot ) )
			bTarget = true;
		if ( pUserPet->GetPetSlot( nMaterialSlot, sPetSlot ) )
			bMaterial = true;
	}

	if( !bTarget )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	if ( !bMaterial )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	SP2Packet kPacket( CTPK_PET_COMPOUND );
	kPacket << m_nSelectPetSlot;
	kPacket << nMaterialSlot;
	TCPNetwork::SendToServer( kPacket );

	TCPNetwork::MouseBusy( true );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//재료 리스트 위치 설정
int PetCompoundWnd::GetMaterialListPosY( int nSize )
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
void PetCompoundWnd::GetMaterialList( vNewShopPullDownItem &rkList )
{
	rkList.clear();

	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( !pUserPet )
		return;

	int nSize = m_vTotalPetList.size();
	rkList.reserve( nSize );

	std::sort( m_vTotalPetList.begin(), m_vTotalPetList.end(), PetPullDownInfoSort() );

	int nTargetRank = PRT_NONE;
	PetSlot sPetSlot;
	if ( pUserPet->GetPetSlot( m_nSelectPetSlot, sPetSlot ) )
		nTargetRank = sPetSlot.m_PetRankType;

	for( int i=0; i<nSize; ++i )
	{
		PetSlot sPetTargetSlot;
		if( !pUserPet->GetPetSlot( m_vTotalPetList[i].m_nSlotIndex, sPetTargetSlot ) )
			continue;

		if ( nTargetRank != sPetTargetSlot.m_PetRankType )
			continue;

		if ( sPetTargetSlot.m_bEquip )
			continue;

		if ( sPetTargetSlot.m_nIndex == m_nSelectPetSlot )
			continue;

		if ( sPetTargetSlot.m_nCurLevel < g_PetInfoMgr.GetMaxLevel( (PetRankType)sPetTargetSlot.m_PetRankType ) )
			continue;

		NewShopPullDownItem kItem;
		kItem.m_iMagicCode = sPetTargetSlot.m_nIndex;

		char szLevel[MAX_PATH]="", szName[MAX_PATH]="", szCnt[MAX_PATH]="";

		if ( sPetTargetSlot.m_nCurLevel >= g_PetInfoMgr.GetMaxLevel((PetRankType)sPetTargetSlot.m_PetRankType) )
		{
			StringCbPrintf( szLevel, sizeof( szLevel ), "MAX" );
			kItem.m_dwPetLevelTextColor = TCT_DEFAULT_RED;
		}
		else
		{
			StringCbPrintf( szLevel, sizeof( szLevel ), "%d", sPetTargetSlot.m_nCurLevel );
			kItem.m_dwPetLevelTextColor = TCT_DEFAULT_DARKGRAY;
		}

		kItem.m_bPetLevel = true;
		kItem.m_szPetLevel = szLevel;

		StringCbPrintf( szName, sizeof( szName ), "%s", sPetTargetSlot.m_Name.c_str() );

		kItem.m_dwTextColor = TCT_DEFAULT_DARKGRAY;
		kItem.m_szName = szName;

		ioHashString szRank = g_PetInfoMgr.GetRankName( sPetTargetSlot.m_PetRankType );
		StringCbPrintf( szCnt, sizeof( szCnt ), "랭크 %s", szRank.c_str() );		
		kItem.m_bCount = true;
		kItem.m_szCnt = szCnt;
		kItem.m_dwCntTextColor = TCT_DEFAULT_BLUE;

		rkList.push_back( kItem );
	}
}

//재료 리스트 버튼 클릭시
void PetCompoundWnd::MaterialListBtnUp( ioWnd *pOpenBtn, int nListArray )
{
	if( !pOpenBtn ) 
		return;	

	vNewShopPullDownItem kItemList;
	GetMaterialList( kItemList );	

	if( !COMPARE( nListArray, 0, (int)kItemList.size() ) ) 
		return;			

	NewShopPullDownItem &rkItem = kItemList[nListArray];

	int nSlot = rkItem.m_iMagicCode;
	SelectPetTargetBtn *pTarget = dynamic_cast<SelectPetTargetBtn*>( FindChildWnd( ID_MATERIALPET_BTN ) );
	if( pTarget && nSlot > 0 )
		pTarget->SetSelectPetSlot( nSlot );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//결과 UI 적용
void PetCompoundWnd::ApplyCompoundResult()
{
	HideMainInfo();
	ShowChildWnd( ID_RESULT_BTN );
	m_bResult = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//UI 설정
void PetCompoundWnd::HideMainInfo()
{
	HideChildWnd( ID_MATERIALPET_BTN );
	HideChildWnd( ID_MATERIALPET_WND );
	HideChildWnd( ID_COMPOUND_BTN );
	HideChildWnd( ID_BACK_BTN );
}

void PetCompoundWnd::ShowMainInfo()
{
	ShowChildWnd( ID_MATERIALPET_BTN );
	ShowChildWnd( ID_COMPOUND_BTN );
	ShowChildWnd( ID_BACK_BTN );
}