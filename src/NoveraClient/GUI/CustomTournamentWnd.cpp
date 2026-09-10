#include "StdAfx.h"

#include "../io3DEngine/ioINILoader.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioUIRenderer.h"
#include "../io3DEngine/ioMath.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/HelpFunc.h"

#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/safesprintf.h"

#include "../WndID.h"
#include "../ioTournamentManager.h"
#include "../ioTournamentTeamManager.h"
#include "../ioTournamentRewardManager.h"

#include "CustomTournamentWnd.h"
#include "TournamentDetailMainWnd.h"
#include "TournamentMainWnd.h"
#include "ioSpecificDragItem.h"

CustomTournamentNoticeWnd::CustomTournamentNoticeWnd()
{
	m_iCurPage     = 0;
	m_iMaxPage     = 0;
	m_dwOwnerIndex = 0;

	m_pPreEdit = NULL;
}

CustomTournamentNoticeWnd::~CustomTournamentNoticeWnd()
{
}

void CustomTournamentNoticeWnd::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;
	SetEditKeyFocus();
}

void CustomTournamentNoticeWnd::iwm_hide()
{
	ioEdit *pEdit = (ioEdit*)FindChildWnd( ID_EDIT );
	if( pEdit )
	{
		pEdit->KillKeyFocus();
	}

	if( m_pPreEdit )
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			m_pPreEdit->SetKeyFocus();
		}

		m_pPreEdit = NULL;
	}
}
void CustomTournamentNoticeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_PREV:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
				if( pEdit )
				{
					pEdit->PrevPage();
				}
			}
		}
		break;
	case ID_NEXT:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
				if( pEdit )
				{
					pEdit->NextPage();
				}
			}
		}
		break;
	case ID_CONFIRM:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
				if( pEdit )
				{
					std::string szText;
					const StringVector szVec = pEdit->GetMultiText();
					StringVector::const_iterator iter = szVec.begin();
					for( ; iter != szVec.end(); ++iter )
					{
						szText += (*iter);
					}

					char szSlangName[MAX_PATH] = "";
					sprintf( szSlangName, "%s", g_SlangUtil.ConvertString( szText.c_str() ) );
					if( szText != szSlangName )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "공지사항에는 비속어나 욕설을 사용할 수 없습니다." );
						return;
					}
					else
					{
						if( m_dwOwnerIndex == g_MyInfo.GetUserIndex() )
							SendNoticeData( szVec );
						else
							g_GUIMgr.SetMsgBox( MB_OK, NULL, "공지사항은 대회 주최자만 수정 할 수 있습니다." );
					}
				}
			}
		}
		break;
	}
}

void CustomTournamentNoticeWnd::SetEditKeyFocus()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
	if( pEdit )
		pEdit->SetKeyFocus();
}

void CustomTournamentNoticeWnd::KillEditKeyFocus()
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
	if( pEdit )
		pEdit->KillKeyFocus();
}

void CustomTournamentNoticeWnd::SendNoticeData( const StringVector& szSrcVec )
{
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
	if( pEdit )
	{
		StringVector::const_iterator VecIter = szSrcVec.begin();	

		std::string szAnnounce;
		std::string szBuff;
		for( ; VecIter != szSrcVec.end(); ++VecIter )
		{
			szBuff = (*VecIter).c_str();
			if( std::string::npos != szBuff.find('_')  )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "'_'문자는 예약된 시스템 문자입니다. # 사용 하실 수 없습니다" );
				return;
			}
			else
			{
				szAnnounce += szBuff;
				szAnnounce += "_";
			}
		}

		if( szAnnounce.empty() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "공지사항 내용을 입력해주세요" );
			return;
		}

		SP2Packet kPacket( CTPK_TOURNAMENT_ANNOUNCE_CHANGE );
		kPacket << m_dwTournamentIndex;
		kPacket << szAnnounce.c_str();
		TCPNetwork::SendToServer( kPacket );

		HideWnd();
	}
}

void CustomTournamentNoticeWnd::SetNoticeData( DWORD dwTourIndex, StringVector& szSrcAnnounce, DWORD dwOwnerIndex )
{
	m_dwOwnerIndex = dwOwnerIndex;
	m_dwTournamentIndex = dwTourIndex;
	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
	if( pEdit )
	{
		pEdit->SetKeyFocus();
		pEdit->SetMultiText( szSrcAnnounce );
	}	
	ShowWnd();
}

void CustomTournamentNoticeWnd::ConvertStringVector( IN const ioHashString& szSrcAnnounce, OUT StringVector& szDest )
{
	if( szSrcAnnounce.IsEmpty() )
		return;

	std::string szAnnounce = szSrcAnnounce.c_str();

	int iCutStartPos = 0;
	std::string::const_iterator iter = szAnnounce.begin();

	for( int i = 0; iter != szAnnounce.end(); ++iter, ++i )
	{
		if( *iter == '_' )
		{
			std::string szBuff;
			szBuff = szAnnounce.substr( iCutStartPos, i - iCutStartPos );
			szDest.push_back( szBuff );
			iCutStartPos = min( (int)szAnnounce.length(), i + 1 );
		}
	}
}

void CustomTournamentNoticeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioEdit* pEdit = dynamic_cast<ioEdit*>( FindChildWnd( ID_EDIT ) );
	if( pEdit )
	{
		int iMax = pEdit->GetMultiLineCnt();
		if( iMax == 0 )
		{
			m_iMaxPage = 0;
			m_iCurPage = 0;
		}
		else
		{
			m_iMaxPage = pEdit->GetMaxPage() / pEdit->GetMaxLine();
			m_iCurPage = pEdit->GetCurPage() / pEdit->GetMaxLine();
		}
	}
}

void CustomTournamentNoticeWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "%d", m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_13, "/%d", m_iMaxPage + 1 );

	kPrinter.PrintFullText( iXPos + 112, iYPos + 196, TAT_CENTER );
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentAllocateWnd::CustomTournamentAllocateWnd()
{
	m_dwTournamentStartTeamCount = 0;
}

CustomTournamentAllocateWnd::~CustomTournamentAllocateWnd()
{
}

void CustomTournamentAllocateWnd::iwm_show()
{
	UnAllocateWnd* pUnAllocatedWnd = dynamic_cast<UnAllocateWnd*>( FindChildWnd( UNALLOCATE_WND ) );
	if( pUnAllocatedWnd )
		pUnAllocatedWnd->ShowWnd();

	AllocatedWnd* pAllocatedWnd = dynamic_cast<AllocatedWnd*>( FindChildWnd( ALLOCATED_WND ) );
	if( pAllocatedWnd )	
		pAllocatedWnd->ShowWnd();

	ioCheckButton* pBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_RANDOM_CHECK ) );
	if( pBtn )
		pBtn->SetCheck( true );
}

void CustomTournamentAllocateWnd::iwm_hide()
{
	AllocatedWnd* pAllocatedWnd = dynamic_cast<AllocatedWnd*>( FindChildWnd( ALLOCATED_WND ) );
	if( pAllocatedWnd )
		pAllocatedWnd->HideWnd();

	UnAllocateWnd* pUnAllocatedWnd = dynamic_cast<UnAllocateWnd*>( FindChildWnd( UNALLOCATE_WND ) );
	if( pUnAllocatedWnd)
		pUnAllocatedWnd->HideWnd();
}

void CustomTournamentAllocateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CLOSE :
	case ID_EXIT :
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	case ID_CONFIRM :
		{
			if( cmd == IOBN_BTNUP )
			{
				SetAllocateMessage();
			}
		}
		break;
	case ID_RANDOM_CHECK:
		{
			if( cmd == IOBN_BTNUP )
			{
				ioCheckButton* pCheck = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_RANDOM_CHECK ) );
				if( pCheck )					
					m_ProgressTournament = (byte)pCheck->IsChecked();
			}
		}
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendCustomTournamentAllocatedData();
			}
		}
		break;
	}
}

IWDropEffect CustomTournamentAllocateWnd::iwm_dragover( ioDragItem *pItem, const ioMouse& mouse )
{
	if( pItem->GetTypeID() == SDI_TOUR_UNALLOCATE_ITEM )
	{
		ioWnd* pAllocatedWnd = FindChildWnd( ALLOCATED_WND );
		if( pAllocatedWnd )
			pAllocatedWnd->iwm_dragover( pItem, mouse );
	}
	else if(  pItem->GetTypeID() == SDI_TOUR_ALLOCATED_ITEM )
	{
		ioWnd* pUnAllocateWnd = FindChildWnd( UNALLOCATE_WND );
		if( pUnAllocateWnd )
			pUnAllocateWnd->iwm_dragover( pItem, mouse );

		ioWnd* pAllocatedWnd = FindChildWnd( ALLOCATED_WND );
		if( pAllocatedWnd )
			pAllocatedWnd->iwm_dragover( pItem, mouse );
	}

	return ioWnd::iwm_dragover( pItem, mouse );
}

bool CustomTournamentAllocateWnd::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{
	if( pItem->GetTypeID() == SDI_TOUR_UNALLOCATE_ITEM )
	{
		ioWnd* pAllocatedWnd = FindChildWnd( ALLOCATED_WND );
		if( pAllocatedWnd )
		{
			pAllocatedWnd->iwm_dropitem( pItem, mouse, pStartWnd );
		}

		UnAllocateWnd* pUnAllocateWnd = dynamic_cast<UnAllocateWnd*>( FindChildWnd( UNALLOCATE_WND ) );
		if( pUnAllocateWnd )		
			pUnAllocateWnd->SetCurPageParsingData();
	}
	else if(  pItem->GetTypeID() == SDI_TOUR_ALLOCATED_ITEM )
	{
		ioWnd* pUnAllocateWnd = FindChildWnd( UNALLOCATE_WND );
		if( pUnAllocateWnd )		
			pUnAllocateWnd->iwm_dropitem( pItem, mouse, pStartWnd );

		ioWnd* pAllocatedWnd = FindChildWnd( ALLOCATED_WND );
		if( pAllocatedWnd )
			pAllocatedWnd->iwm_dropitem( pItem, mouse, pStartWnd );
	}

	return ioWnd::iwm_dropitem( pItem, mouse, pStartWnd );
}

void CustomTournamentAllocateWnd::SetCustomTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;

	TournamentNode *pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pNode )
	{
		m_dwTournamentStartTeamCount = pNode->GetTournamentStartTeamCount();
		AllocatedWnd* pAllocatedWnd = dynamic_cast<AllocatedWnd*>( FindChildWnd( ALLOCATED_WND ) );
		if( pAllocatedWnd )	
			pAllocatedWnd->SetCustomTournamentData( dwTourIndex );

		UnAllocateWnd* pUnAllocatedWnd = dynamic_cast<UnAllocateWnd*>( FindChildWnd( UNALLOCATE_WND ) );
		if( pUnAllocatedWnd )	
			pUnAllocatedWnd->SetCustomTournamentData( dwTourIndex );

		ShowWnd();
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 배정정보를 불러올 수 없습니다." );
	}
}

void CustomTournamentAllocateWnd::SetAllocateMessage()
{
	DWORD dwAllocateTeamCount   = 0;
	DWORD dwUnAllocateTeamCount = 0;

	UnAllocateWnd* pUnAllocateWnd = dynamic_cast<UnAllocateWnd*>( FindChildWnd( UNALLOCATE_WND ) );
	if( pUnAllocateWnd )
	{
		dwUnAllocateTeamCount = pUnAllocateWnd->GetUnAllocateTeamCount();
	}

	AllocatedWnd* pAllocateWnd = dynamic_cast<AllocatedWnd*>( FindChildWnd( ALLOCATED_WND ) );
	if( pAllocateWnd )
	{
		dwAllocateTeamCount = pAllocateWnd->GetAllocateCount();
	}

	ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
	kPrinter[0].SetTextStyle( TS_NORMAL );
	kPrinter[0].SetBkColor( 0, 0, 0 );
	kPrinter[0].SetTextColor( TCT_DEFAULT_GREEN );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "직접 배정된 팀 : ");
	kPrinter[0].SetTextColor( TCT_DEFAULT_RED );
	kPrinter[0].AddTextPiece( FONT_SIZE_13, "%d", dwAllocateTeamCount );

	kPrinter[1].SetTextStyle( TS_NORMAL );
	kPrinter[1].SetBkColor( 0, 0, 0 );
	kPrinter[1].SetTextColor( TCT_DEFAULT_GREEN );

	ioCheckButton* pBtn = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_RANDOM_CHECK ) );
	if( pBtn && pBtn->IsChecked() )
	{
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "자동 배정될 팀 : " );
		kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "%d", min( dwUnAllocateTeamCount - dwAllocateTeamCount, m_dwTournamentStartTeamCount) );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );
		kPrinter[2].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, " " );

		kPrinter[3].SetTextStyle( TS_NORMAL );
		kPrinter[3].SetBkColor( 0, 0, 0 );
		kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[3].AddTextPiece( FONT_SIZE_13, "총 " );
		kPrinter[3].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[3].AddTextPiece( FONT_SIZE_13, "%d 팀이 대회에 출전", min( dwUnAllocateTeamCount, m_dwTournamentStartTeamCount) );
		kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[3].AddTextPiece( FONT_SIZE_13, "합니다." );

		kPrinter[4].SetTextStyle( TS_NORMAL );
		kPrinter[4].SetBkColor( 0, 0, 0 );
		kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, "대진 배정을 완료하시겠습니까?" );
	}
	else
	{
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "자동 배정될 팀 : " );
		kPrinter[1].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "%d", 0 );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );
		kPrinter[2].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, "배정되지 않은 팀 : ");
		kPrinter[2].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, "%d", dwUnAllocateTeamCount - dwAllocateTeamCount );

		kPrinter[3].SetTextStyle( TS_NORMAL );
		kPrinter[3].SetBkColor( 0, 0, 0 );
		kPrinter[3].SetTextColor( TCT_DEFAULT_GREEN );
		kPrinter[3].AddTextPiece( FONT_SIZE_13, " " );

		kPrinter[4].SetTextStyle( TS_NORMAL );
		kPrinter[4].SetBkColor( 0, 0, 0 );
		kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, "총 " );
		kPrinter[4].SetTextColor( TCT_DEFAULT_RED );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, "%d 팀이 대회에 출전", dwAllocateTeamCount );
		kPrinter[4].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[4].AddTextPiece( FONT_SIZE_13, "합니다." );

		kPrinter[5].SetTextStyle( TS_NORMAL );
		kPrinter[5].SetBkColor( 0, 0, 0 );
		kPrinter[5].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[5].AddTextPiece( FONT_SIZE_13, "대진 배정을 완료하시겠습니까?" );
	}

	g_GUIMgr.SetPrevMsgListBox( NULL, MB_YESNO, this, kPrinter );
}

void CustomTournamentAllocateWnd::SendCustomTournamentAllocatedData()
{	
	AllocatedWnd* pAllocateWnd = dynamic_cast<AllocatedWnd*>( FindChildWnd( ALLOCATED_WND ) );
	if( pAllocateWnd )
	{
		SP2Packet kPacket( CTPK_TOURNAMENT_TEAM_ALLOCATE_DATA );
		kPacket << m_dwTournamentIndex;
		kPacket << m_ProgressTournament;
		kPacket << pAllocateWnd->GetAllocateCount();

		const TournamentNode::AllocateTeamDataPtrMap& kDataMap = pAllocateWnd->GetCustomTeamDataPtrMap();
		TournamentNode::AllocateTeamDataPtrMap::const_iterator iter = kDataMap.begin();
		for( ; iter != kDataMap.end(); ++iter )
		{
			TournamentNode::AllocateTeamData* pData = iter->second;
			if( pData )
			{
				kPacket << pData->m_dwTeamIndex;
				kPacket << iter->first;
			}
		}
		TCPNetwork::MouseBusy( true );
		TCPNetwork::SendToServer( kPacket );
		HideWnd();
	}
}

void CustomTournamentAllocateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
}

void CustomTournamentAllocateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 229, iYPos + 191, FONT_SIZE_12, "배정" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
	g_FontMgr.PrintText( iXPos + 229, iYPos + 285, FONT_SIZE_12, "제외" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 229, iYPos + 450, FONT_SIZE_12, "왼쪽의 참가 신청팀을 오른쪽으로 드래그해서 대회에 출전시켜주세요." );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 37, iYPos + 496, FONT_SIZE_12, "남은 팀 자동 배정" );
}

//////////////////////////////////////////////////////////////////////////

UnAllocateButton::UnAllocateButton()
{
	m_pDragItem = new ioUnAllocateButtonDragItem( SDI_TOUR_UNALLOCATE_ITEM );
	m_pCustomTeamData = NULL;
}

UnAllocateButton::~UnAllocateButton()
{
	SAFEDELETE( m_pDragItem );
}

void UnAllocateButton::iwm_hide()
{
	m_pCustomTeamData = NULL;
}

void UnAllocateButton::iwm_mouseleave( const ioMouse& mouse )
{
	POINT ptOffset = { 0, 0 };
	if( m_bClicked && m_pDragItem )
	{
		if( m_pCustomTeamData )
		{
			m_pDragItem->SetRenderFrame( m_pNormalFrm );
			m_pDragItem->DoDragDrop( this, m_rcPos, ptOffset );
			m_pDragItem->SetCustomTeamData( m_pCustomTeamData );
		}
	}

	ioButton::iwm_mouseleave( mouse );
}

void UnAllocateButton::iwm_mouseover(const ioMouse& mouse )
{	
	ioButton::iwm_mouseover( mouse );

	if(!HasWndStyle( IWS_INACTIVE ) )
	{
		ioButton::GoToTop();
	}
}

IWDropEffect UnAllocateButton::iwm_dragover( ioDragItem *pItem, const ioMouse& mouse )
{
	return ioButton::iwm_dragover( pItem, mouse );
}

bool UnAllocateButton::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{	
	return ioButton::iwm_dropitem( pItem, mouse, pStartWnd );
}

void UnAllocateButton::iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped )
{
	if( m_pCustomTeamData && 0 < m_pCustomTeamData->m_AllocatedPos )
		SetInActive();
	else
		SetActive();
}

void UnAllocateButton::SetCustomTeamData( TournamentNode::AllocateTeamData* TeamData )
{
	m_pCustomTeamData = TeamData;
	if( m_pCustomTeamData )
	{
		if( 0 < m_pCustomTeamData->m_AllocatedPos )
			SetInActive();
		else
			SetActive();

		TeamGlobalData* pTeam = g_TournamentTeamMgr.GetTeamData( TeamData->m_dwTeamIndex, false );
		if( pTeam )
		{
			SetTitleText( pTeam->GetTeamName().c_str() );
		}
	}
	else
	{
		SetTitleText( "참가신청팀 없음" );
		SetInActive();
	}
}


//////////////////////////////////////////////////////////////////////////

UnAllocateWnd::UnAllocateWnd()
{
	m_dwTournamentIndex = 0;

	m_iCurPage = 0;
	m_iMaxPage = 0;

	m_dwUnAllcateTeamCount		= 0;
	m_dwCustomTeamDataSyncTimer = 0;
}

UnAllocateWnd::~UnAllocateWnd()
{
}

void UnAllocateWnd::iwm_hide()
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pNode )
		pNode->ResetUnAllocateTeamData();
}

void UnAllocateWnd::iwm_show()
{
	SetCurPageParsingData();
}

void UnAllocateWnd::SetCustomTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;	
}

void UnAllocateWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_TEAM_BTN01:
	case ID_TEAM_BTN02:
	case ID_TEAM_BTN03:
	case ID_TEAM_BTN04:
	case ID_TEAM_BTN05:
	case ID_TEAM_BTN06:
	case ID_TEAM_BTN07:
	case ID_TEAM_BTN08:
	case ID_TEAM_BTN09:
	case ID_TEAM_BTN10:
	case ID_TEAM_BTN11:
	case ID_TEAM_BTN12:
	case ID_TEAM_BTN13:
	case ID_TEAM_BTN14:
	case ID_TEAM_BTN15:
	case ID_TEAM_BTN16:
		if( cmd == IOBN_BTNUP )
		{
			UnAllocateButton *pBtn = dynamic_cast< UnAllocateButton*>( pWnd );
			if( pBtn )
			{
				TournamentNode::AllocateTeamData* pData = pBtn->GetCustomTeamData();
				if( pData && 0 < pData->m_dwTeamIndex )
				{
					TournamentTeamInfoWnd *pTournamentTeam = dynamic_cast< TournamentTeamInfoWnd * >( g_GUIMgr.FindWnd( TOURNAMENT_TEAM_INFO_WND ) );
					if( pTournamentTeam )
					{
						pTournamentTeam->SetTournamentTeam( m_dwTournamentIndex, pData->m_dwTeamIndex );
					}					
				}
			}
		}
	}
}

IWDropEffect UnAllocateWnd::iwm_dragover( ioDragItem *pItem, const ioMouse& mouse )
{
	for( int ID = ID_TEAM_BTN01; ID <= ID_TEAM_BTN16; ++ID )
	{
		ioWnd* pWnd = FindChildWnd( ID );
		if( pWnd )
		{
			if( pWnd->IsInWndRect( mouse.GetMousePos() ) )
				pWnd->iwm_dragover( pItem, mouse );
			else
				pWnd->iwm_mouseleave( mouse );
		}
	}
	return ioWnd::iwm_dragover( pItem, mouse );
}

bool UnAllocateWnd::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{
	if( pItem->GetTypeID() == SDI_TOUR_ALLOCATED_ITEM )
	{
		ioAllocatedButtonDragItem* pAllocateItem = ToAllocatedButtonDragItem( pItem );
		if( pAllocateItem )
		{
			if( IsInWndRect( mouse.GetMousePos() ) || pAllocateItem->GetClickRemove() )
			{
				pAllocateItem->SetClickRemove( false );
				TournamentNode::AllocateTeamData* pData = pAllocateItem->GetCustomTeamData();
				if( pData )
				{
					pData->m_AllocatedPos = 0;
					SetCurPageParsingData();
				}
			}
		}
	}

	return ioWnd::iwm_dropitem( pItem, mouse, pStartWnd );
}

void UnAllocateWnd::iwm_wheel( int zDelta )
{
	if( zDelta == WHEEL_DELTA )
		PrevPage();	
	else if( zDelta == -WHEEL_DELTA )	
		NextPage();	
}

void UnAllocateWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	if( dwID == ID_VERT_SCROLL )
	{
		if( m_iCurPage != curPos )
		{
			int iPrevPage = m_iCurPage;
			if( iPrevPage < curPos )
			{
				m_iCurPage = curPos;
				m_iCurPage = min( m_iCurPage, m_iMaxPage );
			}
			else if( iPrevPage > curPos )
			{
				m_iCurPage = curPos;
				m_iCurPage = max( 0, m_iCurPage );
			}

			if( iPrevPage != m_iCurPage )
			{
				SetDataSync();
				SetCurPageParsingData();
			}
		}
	}
}

void UnAllocateWnd::PrevPage()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iPrevPage = m_iCurPage;
		m_iCurPage--;
		m_iCurPage = max( 0, m_iCurPage );

		if( m_iCurPage != iPrevPage )
		{
			pScroll->SetScrollPos( m_iCurPage );
			SetDataSync();
			SetCurPageParsingData();
		}
	}
}

void UnAllocateWnd::NextPage()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		int iPrevPage = m_iCurPage;
		m_iCurPage++;
		m_iCurPage = min( m_iCurPage, m_iMaxPage );

		if( m_iCurPage != iPrevPage )
		{
			pScroll->SetScrollPos( m_iCurPage );
			SetDataSync();
			SetCurPageParsingData();
		}
	}
}

void UnAllocateWnd::SetMaxPage()
{
	DWORD dwTotalCount = 0;
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pNode )
	{
		dwTotalCount = pNode->GetUnAllocateTotalCount();		
	}
	m_iMaxPage = max( 0, dwTotalCount / TournamentHelp::PAGE_TEAM_MAX );

	//스크롤 설정
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, m_iMaxPage+1 );
	}
}

void UnAllocateWnd::SetCurPageParsingData()
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );

	short Pos = 0;
	for( int ID = ID_TEAM_BTN01; ID <= ID_TEAM_BTN16; ++ID, ++Pos )
	{
		UnAllocateButton* pWnd = dynamic_cast<UnAllocateButton*>( FindChildWnd( ID ) );
		if( pWnd )
		{			
			if( pNode )
			{
				TournamentNode::AllocateTeamData& kData = pNode->GetUnAllocateTeamData( m_iCurPage * TournamentHelp::PAGE_TEAM_MAX + Pos );
				if( kData.m_dwTeamIndex != 0 )
				{
					pWnd->SetCustomTeamData( &kData );
					continue;
				}
			}
			pWnd->SetCustomTeamData( NULL );
		}
	}

	SetMaxPage();
}

void UnAllocateWnd::SetDataSync()
{
	m_dwCustomTeamDataSyncTimer = FRAMEGETTIME();
}

void UnAllocateWnd::OnTeamDataSyncProcess()
{
	TournamentNode* pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( !pTournament )
		return;

	if( m_dwCustomTeamDataSyncTimer + 1000 < FRAMEGETTIME() )
	{		
		if( !pTournament->IsUnAllocatePageSync( m_iCurPage ) )
			pTournament->SendAllocatePageSync( m_iCurPage );
		
		if( pTournament->IsUnAllocatePageUpdate( m_iCurPage ) )
		{
			SetCurPageParsingData();
		}
	}
}

void UnAllocateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;	

	OnTeamDataSyncProcess();
}

//////////////////////////////////////////////////////////////////////////

AllocatedButton::AllocatedButton()
{
	m_pDragItem         = new ioAllocatedButtonDragItem( SDI_TOUR_ALLOCATED_ITEM );
	m_pCustomTeamData   = NULL;
	m_CurTourPosition = 0;
}

AllocatedButton::~AllocatedButton()
{
	SAFEDELETE( m_pDragItem );
}

void AllocatedButton::iwm_hide()
{
	m_pCustomTeamData = NULL;
	m_pDragItem->SetCustomTeamData( NULL );
}

void AllocatedButton::SetDragItem()
{
	if( !m_pDragItem )
		return; 

		POINT ptOffset = { 0, 0 };
	if( m_pCustomTeamData && 0 < m_pCustomTeamData->m_dwTeamIndex )
	{
		m_pDragItem->SetRenderFrame( m_pNormalFrm );
		m_pDragItem->DoDragDrop( GetParent(), m_rcPos, ptOffset );
		m_pDragItem->SetCustomTeamData( m_pCustomTeamData );
	}
}

void AllocatedButton::iwm_mouseleave( const ioMouse& mouse )
{
	if( m_bClicked )
		SetDragItem();

	ioButton::iwm_mouseleave( mouse );
}

void AllocatedButton::iwm_mouseover( const ioMouse& mouse )
{	
	ioButton::iwm_mouseover( mouse );

	if( !HasWndStyle( IWS_INACTIVE ) )
	{
		ioButton::GoToTop();
	}
}

IWDropEffect AllocatedButton::iwm_dragover( ioDragItem *pItem, const ioMouse& mouse )
{
	iwm_mouseover( mouse );
	return ioButton::iwm_dragover( pItem, mouse );
}

bool AllocatedButton::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{
	return ioButton::iwm_dropitem( pItem, mouse, pStartWnd );
}

void AllocatedButton::SetClickRemove()
{
	if( m_pDragItem )				
	{
		m_pDragItem->SetClickRemove( true );
		m_pDragItem->SetCustomTeamData( m_pCustomTeamData );
	}
}

void AllocatedButton::SetCustomTeamData( TournamentNode::AllocateTeamData* TeamData )
{
	m_pCustomTeamData = TeamData;
	if( m_pCustomTeamData && 0 < m_pCustomTeamData->m_dwTeamIndex )
	{
		SetActive();
		m_pUITitle->SetColor( "Normal", 0xFF117222, 0 );
		m_pUITitle->SetColor( "Over", 0xFF117222, 0 );
		m_pUITitle->SetColor( "Push", 0xFF117222, 0 );

		TeamGlobalData* pTeam = g_TournamentTeamMgr.GetTeamData( TeamData->m_dwTeamIndex, false );		
		if( pTeam )
		{
			SetTitleText( pTeam->GetTeamName().c_str() );
		}
	}
	else
	{
		char szTtitle[MAX_PATH];
		sprintf( szTtitle, "%d번 대회팀", m_CurTourPosition );
		m_pUITitle->SetColor( "Normal", TCT_DEFAULT_LIGHTGRAY, 0 );
		m_pUITitle->SetColor( "Over", TCT_DEFAULT_LIGHTGRAY, 0 );
		m_pUITitle->SetColor( "Push", TCT_DEFAULT_LIGHTGRAY, 0 );
		SetTitleText( szTtitle );
	}
}


//////////////////////////////////////////////////////////////////////////

AllocatedWnd::AllocatedWnd()
{
	m_iCurPage = 0;
	m_iMaxPage = 0;

	m_dwStartTeamCount = 0;	

	m_pTourLine = NULL;
}

AllocatedWnd::~AllocatedWnd()
{
	SAFEDELETE( m_pTourLine );
}

void AllocatedWnd::iwm_show()
{	
}

void AllocatedWnd::iwm_hide()
{
}

void AllocatedWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "TourLine" )
	{
		SAFEDELETE( m_pTourLine );
		m_pTourLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void AllocatedWnd::SetCustomTournamentData( DWORD dwTourIndex )
{
	TournamentNode* pNode = g_TournamentMgr.GetTournament( dwTourIndex, false );
	if( pNode )
	{
		m_dwStartTeamCount = pNode->GetTournamentStartTeamCount();
		m_CustomTeamDataPtrMap.clear();
		for( DWORD i = 1; i <= m_dwStartTeamCount; ++i )
		{
			//메모리 할당
			InsertCustomTeamData( i, NULL );
		}

		SetCurPageRefreshData();

		for( DWORD i = 0; i < TournamentHelp::PAGE_TEAM_MAX; ++i )
		{
			if( i < m_dwStartTeamCount )
				ShowChildWnd( ID_TEAM_BTN01 + i );
			else
				HideChildWnd( ID_TEAM_BTN01 + i );
		}

		m_iMaxPage = m_dwStartTeamCount / TournamentHelp::PAGE_TEAM_MAX;
		m_iMaxPage = max( 0, m_iMaxPage );

		//스크롤 설정
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll )
		{
			pScroll->SetScrollRange( 0, m_iMaxPage );
		}		
	}
}

DWORD AllocatedWnd::GetAllocateCount()
{
	DWORD dwCount = 0;
	TournamentNode::AllocateTeamDataPtrMap::iterator iter = m_CustomTeamDataPtrMap.begin();
	for( ; iter != m_CustomTeamDataPtrMap.end(); ++iter )
	{
		TournamentNode::AllocateTeamData* pTeamData = iter->second;
		if( pTeamData )
			dwCount++;
	}
	return dwCount;
}

void AllocatedWnd::SetCurPageRefreshData()
{
	int iPos = 1;
	for( int ID = ID_TEAM_BTN01; ID <= ID_TEAM_BTN16; ++ID, ++iPos )
	{
		AllocatedButton* pWnd = dynamic_cast<AllocatedButton*>( FindChildWnd( ID ) );
		if( pWnd )
		{
			short CurPos = m_iCurPage * TournamentHelp::PAGE_TEAM_MAX + iPos;
			pWnd->SetCurTourPosition( CurPos );

			TournamentNode::AllocateTeamDataPtrMap::iterator iter = m_CustomTeamDataPtrMap.find( CurPos );
			if( iter != m_CustomTeamDataPtrMap.end() )
			{
				pWnd->SetCustomTeamData( iter->second );
			}
		}
	}
}

void AllocatedWnd::RemoveEmptyData()
{
	TournamentNode::AllocateTeamDataPtrMap::iterator iter = m_CustomTeamDataPtrMap.begin();
	for( ; iter != m_CustomTeamDataPtrMap.end(); ++iter )
	{
		TournamentNode::AllocateTeamData* pTeamData = iter->second;
		if( pTeamData && pTeamData->m_AllocatedPos == 0 )
			iter->second = NULL;
	}
}

void AllocatedWnd::InsertCustomTeamData( short Pos, TournamentNode::AllocateTeamData* pData )
{
	TournamentNode::AllocateTeamDataPtrMap::iterator iter = m_CustomTeamDataPtrMap.find( Pos );
	if( iter == m_CustomTeamDataPtrMap.end() )
		m_CustomTeamDataPtrMap.insert( TournamentNode::AllocateTeamDataPtrMap::value_type( Pos, pData ) );
}

void AllocatedWnd::SetAllocated( ioUnAllocateButtonDragItem *pUnAllocateItem, short CurPos )
{
	if( !pUnAllocateItem )
		return;

	TournamentNode::AllocateTeamDataPtrMap::iterator iter = m_CustomTeamDataPtrMap.find( CurPos );
	if( iter != m_CustomTeamDataPtrMap.end() )
	{
		TournamentNode::AllocateTeamData* UnAllocateData = pUnAllocateItem->GetCustomTeamData();

		if( iter->second == NULL )
		{
			UnAllocateData->m_AllocatedPos = CurPos;
			iter->second = UnAllocateData;
		}
		else
		{
			UnAllocateData->m_AllocatedPos = CurPos;
			iter->second->m_AllocatedPos = 0;
			iter->second = UnAllocateData;
		}

		SetCurPageRefreshData();
	}
}

void AllocatedWnd::SwapAllocated( ioAllocatedButtonDragItem *pAllocatedItem, short CurPos )
{
	if( !pAllocatedItem )
		return;

	TournamentNode::AllocateTeamData* pData = pAllocatedItem->GetCustomTeamData();
	if( pData )
	{
		short DragAllocatedPos = pData->m_AllocatedPos;
		TournamentNode::AllocateTeamDataPtrMap::iterator dragged_iter = m_CustomTeamDataPtrMap.find( DragAllocatedPos );
		TournamentNode::AllocateTeamDataPtrMap::iterator dropped_iter = m_CustomTeamDataPtrMap.find( CurPos );
		if( dragged_iter != m_CustomTeamDataPtrMap.end() && dropped_iter != m_CustomTeamDataPtrMap.end() )
		{
			TournamentNode::AllocateTeamData* pSwap;
			pSwap = dropped_iter->second;
			dropped_iter->second = dragged_iter->second;
			dragged_iter->second = pSwap;

			if( dropped_iter->second )
				dropped_iter->second->m_AllocatedPos = CurPos;

			if( dragged_iter->second )
				dragged_iter->second->m_AllocatedPos = DragAllocatedPos;

			SetCurPageRefreshData();
		}
	}	
}

void AllocatedWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_TEAM_BTN01:
	case ID_TEAM_BTN02:
	case ID_TEAM_BTN03:
	case ID_TEAM_BTN04:
	case ID_TEAM_BTN05:
	case ID_TEAM_BTN06:
	case ID_TEAM_BTN07:
	case ID_TEAM_BTN08:
	case ID_TEAM_BTN09:
	case ID_TEAM_BTN10:
	case ID_TEAM_BTN11:
	case ID_TEAM_BTN12:
	case ID_TEAM_BTN13:
	case ID_TEAM_BTN14:
	case ID_TEAM_BTN15:
	case ID_TEAM_BTN16:
		if( cmd == IOBN_BTNUP )
		{
			AllocatedButton* pButton = dynamic_cast<AllocatedButton*>( FindChildWnd( dwID ) );
			if( pButton )
			{
				TournamentNode::AllocateTeamData* pData = pButton->GetCustomTeamData();
				if( pData && GetParent() )
				{
					pButton->SetClickRemove();
					GetParent()->iwm_dropitem( pButton->GetDragItem(), g_App.GetMouseRef(), GetParent() );
					iwm_dropdone( pButton->GetDragItem(), pButton, false );
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "왼쪽의 참가 신청팀을#드래그해서 대회에 출전시켜주세요." );
				}
			}
		}
	}
}

IWDropEffect AllocatedWnd::iwm_dragover( ioDragItem *pItem, const ioMouse& mouse )
{
	for( int ID = ID_TEAM_BTN01; ID <= ID_TEAM_BTN16; ++ID )
	{
		ioWnd* pWnd = FindChildWnd( ID );
		if( pWnd )
		{
			if( pWnd->IsInWndRect( mouse.GetMousePos() ) )	
				pWnd->iwm_dragover( pItem, mouse );
			else
				pWnd->iwm_mouseleave( mouse );
		}
	}
	return ioWnd::iwm_dragover( pItem, mouse );
}

bool AllocatedWnd::iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd )
{
	int iPos = 1;
	for( int ID = ID_TEAM_BTN01; ID <= ID_TEAM_BTN16; ++ID, ++iPos )
	{
		AllocatedButton* pDropWnd = dynamic_cast<AllocatedButton*>( FindChildWnd( ID ) );
		if( pDropWnd )
		{
			if( pDropWnd->IsInWndRect( mouse.GetMousePos() ) && !pDropWnd->IsDisabled() )
			{
				DWORD dwCurPos = m_iCurPage * TournamentHelp::PAGE_TEAM_MAX + iPos;

				//왼쪽 버튼에서 오른쪽버튼으로 드래그(조 배정)
				if( pItem->GetTypeID() == SDI_TOUR_UNALLOCATE_ITEM )
				{
					ioUnAllocateButtonDragItem *pUnAllocateItem = ToUnAllocateButtonDragItem( pItem );
					SetAllocated( pUnAllocateItem, dwCurPos );

					return ioWnd::iwm_dropitem( pItem, mouse, pStartWnd );
				}
				//오른쪽 버튼에서 오른쪽버튼으로 드래그(팀간 조 변경)
				else if( pItem->GetTypeID() == SDI_TOUR_ALLOCATED_ITEM )
				{
					ioAllocatedButtonDragItem *pAllocatedItem = ToAllocatedButtonDragItem( pItem );					
					SwapAllocated( pAllocatedItem, dwCurPos );

					return ioWnd::iwm_dropitem( pItem, mouse, pStartWnd );
				}
			}
		}
	}

	return ioWnd::iwm_dropitem( pItem, mouse, pStartWnd );
}

void AllocatedWnd::iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped )
{
	RemoveEmptyData();
	SetCurPageRefreshData();
}

void AllocatedWnd::iwm_wheel( int zDelta )
{
	if( zDelta == WHEEL_DELTA )		
		PrevPage();		
	else if( zDelta == -WHEEL_DELTA )
		NextPage();
}

void AllocatedWnd::iwm_vscroll(DWORD dwID, int curPos )
{
	if( dwID == ID_VERT_SCROLL )
	{
		if( m_iCurPage != curPos )
		{
			m_iCurPage = curPos;
			SetCurPageRefreshData();
		}
	}
}

void AllocatedWnd::PrevPage()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		m_iCurPage--;
		m_iCurPage = max( 0, m_iCurPage );

		pScroll->SetScrollPos( m_iCurPage );
		SetCurPageRefreshData();
	}
}

void AllocatedWnd::NextPage()
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{				
		m_iCurPage++;
		m_iCurPage = min( m_iCurPage, m_iMaxPage );

		pScroll->SetScrollPos( m_iCurPage );
		SetCurPageRefreshData();
	}
}

//void AllocatedWnd::OnProcess( float fTimePerSec )
//{
//	if( !IsShow() ) return;
//}

void AllocatedWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( !m_pTourLine ) return;

	int iOffset = 48;
	for( DWORD i = 0; i < m_dwStartTeamCount/2; ++i )
	{
		m_pTourLine->SetSize( 6, 2 );
		m_pTourLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pTourLine->Render( iXPos + 146, iYPos + 10 + i * iOffset );

		m_pTourLine->SetSize( 2, 22 );
		m_pTourLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pTourLine->Render( iXPos + 150, iYPos + 12 + i * iOffset );

		m_pTourLine->SetSize( 6, 2 );
		m_pTourLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pTourLine->Render( iXPos + 146, iYPos + 34 + i * iOffset );
	}
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentAllocateResultWnd::CustomTournamentAllocateResultWnd()
{
	m_pMarkBack = NULL;
	m_pMark     = NULL;

	m_dwTournamentIndex = 0;
}

CustomTournamentAllocateResultWnd::~CustomTournamentAllocateResultWnd()
{
	SAFEDELETE( m_pMarkBack );
	SAFEDELETE( m_pMark );
}

void CustomTournamentAllocateResultWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MarkBack" )
	{
		SAFEDELETE( m_pMarkBack );
		m_pMarkBack = pImage;
	}
	else if( szType == "Mark" )
	{
		SAFEDELETE( m_pMark );
		m_pMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentAllocateResultWnd::SetCustomTournamentData( DWORD dwTournamentIndex )
{
	m_dwTournamentIndex = dwTournamentIndex;
	ShowWnd();
}

void CustomTournamentAllocateResultWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CLOSE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			TournamentDetailMainWnd* pWnd = dynamic_cast<TournamentDetailMainWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_INFO ) );
			if( pWnd )
			{				
				pWnd->SetTournamentData( m_dwTournamentIndex );
				pWnd->SetChildActvieWnd( TournamentDetailMainWnd::ID_TOURNAMENT_SCHEDULE_BTN );
			}
		}
		break;
	}
}

void CustomTournamentAllocateResultWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pMarkBack )
		m_pMarkBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY, TFO_BILINEAR );

	if( m_pMark )
		m_pMark->Render( iXPos + 151, iYPos + 98, UI_RENDER_NORMAL, TFO_BILINEAR );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, "대진배정이 완료 되었습니다." );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "지금 바로 대진표가 공개됩니다." );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "대진배정이 완료된 이후에는 대진배정 수정이" );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 204, FONT_SIZE_13, " 불가능합니다." );
}

//////////////////////////////////////////////////////////////////////////

DWORD CustomTournamentRewardButton::m_dwTwinkeTime = 0;
bool CustomTournamentRewardButton::m_bTwinke       = false;

CustomTournamentRewardButton::CustomTournamentRewardButton()
{
	m_bChecked				= false;
	m_dwTournamentCoinSize	= 0;
	m_dwEtcItemType			= 0;
	m_dwRewardItemPirce		= 0;

	m_pIcon					= NULL;
	m_pSubIcon = NULL;
	m_pIconBack				= NULL;
	m_pNoneIcon				= NULL;
	m_pTournamentCoinIcon	= NULL;
	m_pSelectCheck			= NULL;
	m_pExDisable			= NULL;
}

CustomTournamentRewardButton::~CustomTournamentRewardButton()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pNoneIcon );
	SAFEDELETE( m_pTournamentCoinIcon );
	SAFEDELETE( m_pSelectCheck );
	SAFEDELETE( m_pExDisable );
}

void CustomTournamentRewardButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconkBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconNone" )
	{
		SAFEDELETE( m_pNoneIcon );
		m_pNoneIcon = pImage;
	}
	else if( szType == "TournamentCoinIcon" )
	{
		SAFEDELETE( m_pTournamentCoinIcon );
		m_pTournamentCoinIcon = pImage;
		if( m_pTournamentCoinIcon )
			m_dwTournamentCoinSize = m_pTournamentCoinIcon->GetWidth();
	}
	else if( szType == "SelectCheck" )
	{
		SAFEDELETE( m_pSelectCheck );
		m_pSelectCheck = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentRewardButton::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "ExDisable" )
	{		
		SAFEDELETE( m_pExDisable );
		m_pExDisable = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void CustomTournamentRewardButton::SetRewardItem( DWORD dwEtcItemType )
{
	m_szTitleNameVec.clear();
	const TournamentRewardItem* RewardItem = g_TourRewardItemManager.FindRewardItem( dwEtcItemType );
	if( 0 < dwEtcItemType && RewardItem )
	{
		SetActive();

		Help::SplitTextWidth( RewardItem->m_szRewardItemName.c_str(), m_szTitleNameVec, 75, TS_NORMAL, FONT_SIZE_12 );

		m_dwEtcItemType = dwEtcItemType;
		m_dwRewardItemPirce = RewardItem->m_dwRewardItemPirce;
		char szBuff[MAX_PATH];
		Help::ConvertNumToStrComma( m_dwRewardItemPirce, szBuff, sizeof( szBuff ) );
		m_szRewardItemPirce = szBuff;
		m_pIcon = RewardItem->m_pIcon;
		m_pSubIcon = RewardItem->m_pSubIcon;
	}
	else
	{
		m_pIcon	= NULL;
		m_pSubIcon = NULL;
		m_dwEtcItemType     = 0;
		m_dwRewardItemPirce = 0;
		m_szRewardItemPirce = "0";

		SetInActive();
	}
}

void CustomTournamentRewardButton::iwm_mouseover( const ioMouse& mouse )
{
	ioButton::iwm_mouseover( mouse );

	if( !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( GetID() );
	}
}

void CustomTournamentRewardButton::iwm_mouseleave( const ioMouse& mouse )
{
	ioButton::iwm_mouseleave( mouse );

	if( !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( 0 );
	}
}

void CustomTournamentRewardButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( m_bChecked )
	{
		if( m_dwTwinkeTime < FRAMEGETTIME() )
		{
			m_dwTwinkeTime = FRAMEGETTIME() + TWINKLE_TIME;
			m_bTwinke = m_bTwinke ^ true;
		}
	}
}

void CustomTournamentRewardButton::OnRender()
{
	ioButton::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnIconRender( iXPos, iYPos );
	OnTextRender( iXPos, iYPos );
	OnCheckRender( iXPos, iYPos );
}

void CustomTournamentRewardButton::OnDrawDisabled( int iXPos, int iYPos )
{
	if( m_pExDisable )
		m_pExDisable->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );
}

void CustomTournamentRewardButton::OnIconRender( int iXPos, int iYPos )
{
	if( !m_pIconBack || !m_pNoneIcon )
		return;

	if( m_dwEtcItemType == 0 )
	{
		m_pNoneIcon->SetScale(0.9f, FLOAT09);
		m_pNoneIcon->Render( iXPos+ 42, iYPos + 38, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	else
	{
		if( m_pIcon )		
		{
			m_pIconBack->SetScale( FLOAT09, FLOAT09 );
			m_pIconBack->Render( iXPos + 42, iYPos + 38, UI_RENDER_MULTIPLY, TFO_BILINEAR );
			m_pIcon->SetScale( FLOAT09, FLOAT09 );
			m_pIcon->Render( iXPos+ 42, iYPos + 38, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( m_pSubIcon )		
			{
				m_pSubIcon->SetScale( FLOAT09, FLOAT09 );
				m_pSubIcon->Render( iXPos+ 42, iYPos + 38, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
}

void CustomTournamentRewardButton::OnTextRender( int iXPos, int iYPos )
{
	int iOffsetY = (int)m_szTitleNameVec.size() * -7;
	StringVector::const_iterator iter = m_szTitleNameVec.begin();
	for( ; iter < m_szTitleNameVec.end(); ++iter )
	{
		std::string szTtitle = *iter;
		if( !szTtitle.empty() )
		{
			g_FontMgr.SetTextStyle( TS_NORMAL );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
			g_FontMgr.PrintText( iXPos + 42, iYPos + 98 + iOffsetY, FONT_SIZE_12, szTtitle.c_str() );
			iOffsetY += 14;
		}
	}

	if( !IsDisabled() )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_BROWN );
		g_FontMgr.PrintText( iXPos + 41 - m_dwTournamentCoinSize/2, iYPos + 98 + iOffsetY, FONT_SIZE_12, m_szRewardItemPirce.c_str() );

		if( m_pTournamentCoinIcon )
		{
			int Width = g_FontMgr.GetTextWidth( m_szRewardItemPirce.c_str(), TS_NORMAL, FONT_SIZE_12 )/2;
			m_pTournamentCoinIcon->Render( iXPos + 43 + Width, iYPos + 98 + iOffsetY, UI_RENDER_NORMAL );
		}
	}
}

void CustomTournamentRewardButton::OnCheckRender( int iXPos, int iYPos )
{
	if( !m_pSelectCheck )
		return;

	if( m_bChecked )
	{
		m_pSelectCheck->SetScale( FLOAT08, FLOAT08 );
		m_pSelectCheck->Render( iXPos + 5, iYPos + 8, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_bTwinke )
			m_pSelectCheck->Render( iXPos + 5, iYPos + 8, UI_RENDER_ADD, TFO_BILINEAR );
	}
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentRewardLabel::CustomTournamentRewardLabel()
{
	m_bPlus = false;
	m_iInfoArrayPos = 0;
}

CustomTournamentRewardLabel::~CustomTournamentRewardLabel()
{

}

void CustomTournamentRewardLabel::SetLabel( const ioHashString& szLabel, int iXPos, int iYPos, int iWidth, int iInfoArrayPos, bool bPlus )
{
	m_szLabel		= szLabel;
	m_bPlus			= bPlus;
	m_iInfoArrayPos = iInfoArrayPos;

	SetWndPos( iXPos, iYPos );
	SetSize( iWidth, 12 );	
}

void CustomTournamentRewardLabel::iwm_mouseover( const ioMouse& mouse )
{
	ioButton::iwm_mouseover( mouse );

	if( !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( GetID() );
	}
}

void CustomTournamentRewardLabel::iwm_mouseleave( const ioMouse& mouse )
{
	ioButton::iwm_mouseleave( mouse );

	if( !m_bClicked )
	{
		ioLSMouse *pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetToolTipWndID( 0 );
	}
}

void CustomTournamentRewardLabel::OnRender()
{
	ioButton::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioComplexStringPrinter kPrinter;
	if( m_bPlus )
	{
		kPrinter.SetTextStyle( TS_NORMAL );
		kPrinter.SetBkColor( 0, 0, 0 );
		kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter.AddTextPiece( FONT_SIZE_12, " + " );
	}

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );

	if( IsOvered() )
		kPrinter.SetTextColor( TCT_DEFAULT_GREEN );
	else
		kPrinter.SetTextColor( TCT_DEFAULT_ORANGE );

	kPrinter.AddTextPiece( FONT_SIZE_12, "%s", m_szLabel.c_str() );
	kPrinter.PrintFullText( iXPos, iYPos, TAT_LEFT );
}
//////////////////////////////////////////////////////////////////////////

CustomTournamentRewardToolTip::CustomTournamentRewardToolTip()
{
	m_dwCurWndID = 0;
}

CustomTournamentRewardToolTip::~CustomTournamentRewardToolTip()
{
}

void CustomTournamentRewardToolTip::iwm_show()
{
	m_dwCurWndID = 0;
}

void CustomTournamentRewardToolTip::OnProcess( float fTimePerSec )
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse ) return;

	DWORD dwWndID = pMouse->GetToolTipWndID();
	NewShopItemInfo kInfo;
	if( g_GUIMgr.IsShow( CUSTOM_TOURNAMENT_REWARD_SELECT_WND ) )
	{
		CustomTournamentRewardSelectWnd* pRewardWnd = dynamic_cast<CustomTournamentRewardSelectWnd*>(  g_GUIMgr.FindWnd( CUSTOM_TOURNAMENT_REWARD_SELECT_WND ) );
		if( pRewardWnd )
			pRewardWnd->GetCurItemInfo( kInfo, dwWndID );
	}
	else
	{
		HideWnd();
		return;
	}

	if( SetInfo( kInfo ) )
	{
		pMouse->SetToolTipCursor();
		UpdateWndSize();
		int iXPos = 0;
		int iYPos = 0;

		int iMouseXPos = pMouse->GetMousePos().x - GetParent()->GetXPos();
		int iMouseYPos = pMouse->GetMousePos().y - GetParent()->GetYPos();
		Help::GetNotCutWndPos( iMouseXPos, iMouseYPos, GetWidth(), GetHeight(), iXPos, iYPos );
		SetWndPos( iXPos, iYPos );		
		ShowWnd();
		return;
	}
	HideWnd();
}

//////////////////////////////////////////////////////////////////////////

CustomTournamentRewardSelectWnd::CustomTournamentRewardSelectWnd()
{
	m_dwTournaemntIndex		  = 0;
	m_dwStartTeamCnt		  = 0;
	m_dwTournamentRound		  = 0;	
	m_dwMaxPlayer			  = 0;
	m_dwBuyMax				  = 0;
	m_dwSelectRewardPrice	  = 0;
	m_dwTotalRewardPrice	  = 0;
	m_TournamentPos		      = 0;

	m_dwBuyMax = 0;
	m_iMaxPage = 0;
	m_iCurPage = 0;

	m_pLine				  = NULL;
	m_pTournamentCoinIcon = NULL;
}

CustomTournamentRewardSelectWnd::~CustomTournamentRewardSelectWnd()
{
	SAFEDELETE( m_pLine );
	SAFEDELETE( m_pTournamentCoinIcon );
}

void CustomTournamentRewardSelectWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "Line" )
	{
		SAFEDELETE( m_pLine );
		m_pLine = pImage;
	}
	else if( szType == "TournamentCoinIcon" )
	{
		SAFEDELETE( m_pTournamentCoinIcon );
		m_pTournamentCoinIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void CustomTournamentRewardSelectWnd::iwm_show()
{
	const TournamentRewardItemMap RewardMap = g_TourRewardItemManager.GetTournamentRewardItemMap();
	m_iMaxPage = ((int)RewardMap.size()-1) / LIST_MAX;
	m_iCurPage = 0;
	m_SelectRewardPosMap.clear();

	UpdatePage();
	UpdateLabel();
	UpdateTournamentRewardPrice();
	UpdateEtcItemInfoVec();	
}

void CustomTournamentRewardSelectWnd::iwm_create()
{	
	g_GUIMgr.AddWnd( "XML/customtournamentrewardselecttooltip.xml", this );
}

void CustomTournamentRewardSelectWnd::SetCustomTournamentData( DWORD dwTournamentIdx, DWORD dwTournamentPos, DWORD dwBuyMax )
{	
	TournamentNode* pNode = g_TournamentMgr.GetTournament( dwTournamentIdx, false );
	if( !pNode )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "대회 정보를 불러올 수 없습니다." );
		return;
	}

	m_dwTournaemntIndex = dwTournamentIdx;
	m_TournamentPos     = (BYTE)dwTournamentPos;
	m_dwBuyMax			= dwBuyMax;

	m_dwStartTeamCnt	= pNode->GetTournamentStartTeamCount();
	m_dwMaxPlayer		= pNode->GetMaxPlayer();
	m_dwTournamentRound = Help::TournamentCurrentRoundWithTeam( m_dwStartTeamCnt, m_TournamentPos-1 );


	char szTtitle[MAX_PATH];
	ioHashString szRound;
	Help::ConvertTournamentRoundResultText( szRound, m_dwStartTeamCnt, m_TournamentPos-1 );
	sprintf( szTtitle, "%s 보상 추가", szRound.c_str() );
	SetTitleText( szTtitle );
	ShowWnd();
}

void CustomTournamentRewardSelectWnd::UpdatePage()
{	
	const TournamentRewardItemMap RewardMap = g_TourRewardItemManager.GetTournamentRewardItemMap();

	int iID = ID_REWARD01_CHECK;

	for( int iPos = m_iCurPage * LIST_MAX; iPos < m_iCurPage * LIST_MAX + LIST_MAX; ++iPos, ++iID )
	{
		TournamentRewardItemMap::const_iterator iter = RewardMap.begin();
		std::advance( iter, iPos );

		CustomTournamentRewardButton* pWnd = dynamic_cast<CustomTournamentRewardButton*>( FindChildWnd( iID ) );
		if( pWnd )
		{
			if( iter != RewardMap.end() )
			{
				TournamentRewardItem* pItem = iter->second;
				if( pItem )
				{
					pWnd->SetRewardItem( iter->first );
					DWORD dwEtcItemType = FindSelectReward( iPos );
					if( dwEtcItemType == iter->first )
						pWnd->SetCheck( true );
					else
						pWnd->SetCheck( false );

					continue;
				}
			}

			pWnd->SetRewardItem( 0 );
			pWnd->SetCheck( false );
		}
	}
}

void CustomTournamentRewardSelectWnd::UpdateLabel()
{
	//하단 라벨 버튼
	SelectRewardPosMap::const_iterator iter = m_SelectRewardPosMap.begin();
	int iPosX = 441;
	int iPosY = 381;
	DWORD dwEtcItemType = 0;

	for( DWORD WndID = ID_LABEL1; WndID <= ID_LABEL4; ++WndID )
	{
		if( iter != m_SelectRewardPosMap.end() )
			dwEtcItemType = iter->second;

		const TournamentRewardItem* pItem = g_TourRewardItemManager.FindRewardItem( dwEtcItemType );
		if( pItem )
		{
			int iArrayPos = WndID - LABEL_ID_SUB;
			if( iArrayPos < (int)m_SelectRewardPosMap.size() )
			{
				int iWidth = min( g_FontMgr.GetTextWidth( pItem->m_szRewardItemName.c_str(), TS_NORMAL, FONT_SIZE_12 ), LABEL_WIDTH_CUT );
				if( iWidth == LABEL_WIDTH_CUT )
					UpdateLabelWidthCut( WndID, iArrayPos, pItem->m_szRewardItemName, iPosX, iPosY, dwEtcItemType );
				else
					UpdateLabelNormal( WndID, iArrayPos, pItem->m_szRewardItemName, iPosX, iPosY, dwEtcItemType );
				iter++;
				continue;
			}
		}
		UpdateLabelNone( WndID );
	}
}

void CustomTournamentRewardSelectWnd::UpdateLabelWidthCut( DWORD WndID, int iArrayPos, ioHashString szItemName, int& iPosX, int& iPosY, DWORD dwEtcItemType )
{
	CustomTournamentRewardLabel* pLabel = dynamic_cast<CustomTournamentRewardLabel*>( FindChildWnd( WndID ) );
	if( !pLabel ) return;

	char szTtitle[MAX_PATH];
	if( iArrayPos == (int)m_SelectRewardPosMap.size()-1 )
	{
		// WidthCut 스타일 적용시 ...에 의해 Width 계산이 정확하지 않기 때문에 직접 자르고 ...을 넣어서 Width를 다시계산
		Help::StringCutFun( FONT_SIZE_12, LABEL_WIDTH_CUT, TS_NORMAL, szTtitle, sizeof(szTtitle), szItemName.c_str() );
		sprintf_e( szTtitle, "%s...", szTtitle );

		int iWidth = g_FontMgr.GetTextWidth( szTtitle, TS_NORMAL, FONT_SIZE_12 );
		iPosX -= iWidth;
		pLabel->SetLabel( szTtitle, iPosX, iPosY, iWidth, dwEtcItemType );
	}
	else
	{
		// WidthCut 스타일 적용시 ...에 의해 Width 계산이 정확하지 않기 때문에 직접 자르고 ...을 넣어서 Width를 다시계산
		Help::StringCutFun( FONT_SIZE_12, LABEL_WIDTH_CUT, TS_NORMAL, szTtitle, sizeof(szTtitle), szItemName.c_str() );
		sprintf_e( szTtitle, "%s...", szTtitle );

		int iWidth = g_FontMgr.GetTextWidth( szTtitle, TS_NORMAL, FONT_SIZE_12 );
		iWidth += LABEL_GAP;
		iPosX -= iWidth;
		pLabel->SetLabel( szTtitle, iPosX, iPosY, iWidth, dwEtcItemType, true );
	}
	pLabel->ShowWnd();	
}
void CustomTournamentRewardSelectWnd::UpdateLabelNormal( DWORD WndID, int iArrayPos, ioHashString szItemName, int& iPosX, int& iPosY, DWORD dwEtcItemType )
{
	CustomTournamentRewardLabel* pLabel = dynamic_cast<CustomTournamentRewardLabel*>( FindChildWnd( WndID ) );
	if( !pLabel ) return;

	if( iArrayPos == (int)m_SelectRewardPosMap.size()-1 )
	{
		int iWidth = g_FontMgr.GetTextWidth( szItemName.c_str(), TS_NORMAL, FONT_SIZE_12 );
		iPosX -= iWidth;
		pLabel->SetLabel( szItemName.c_str(), iPosX, iPosY, iWidth, dwEtcItemType );
	}
	else
	{
		int iWidth = g_FontMgr.GetTextWidth( szItemName.c_str(), TS_NORMAL, FONT_SIZE_12 );
		iWidth += LABEL_GAP;
		iPosX -= iWidth;
		pLabel->SetLabel( szItemName.c_str(), iPosX, iPosY, iWidth, dwEtcItemType, true );
	}

	pLabel->ShowWnd();
}

void CustomTournamentRewardSelectWnd::UpdateLabelNone( DWORD WndID )
{
	CustomTournamentRewardLabel* pLabel = dynamic_cast<CustomTournamentRewardLabel*>( FindChildWnd( WndID ) );
	if( pLabel )
	{
		pLabel->SetLabel( "", 0, 0, 0, 0 );
		pLabel->HideWnd();
	}
}

void CustomTournamentRewardSelectWnd::UpdateTournamentRewardPrice()
{
	m_dwSelectRewardPrice = 0;
	m_dwTotalRewardPrice  = 0;
	SelectRewardPosMap::const_iterator iter = m_SelectRewardPosMap.begin();
	for( ; iter != m_SelectRewardPosMap.end(); ++iter )
	{
		const TournamentRewardItem* pItem = g_TourRewardItemManager.FindRewardItem( iter->second );
		if( pItem )
		{
			m_dwSelectRewardPrice += pItem->m_dwRewardItemPirce;
		}
	}

	m_dwTotalRewardPrice = m_dwSelectRewardPrice * max(1, m_dwTournamentRound/2 ) * m_dwMaxPlayer;

	ioButton* pButton = dynamic_cast<ioButton*>( FindChildWnd( ID_BUY ) );
	if( pButton )
	{
		
		char szBuff[MAX_PATH];
		if( 0 < m_dwTotalRewardPrice )
		{
			sprintf( szBuff, "%d 대회코인으로 구입", m_dwTotalRewardPrice );
			pButton->SetTitleText( szBuff );
		}
		else
		{
			sprintf( szBuff, "구입하기" );
			pButton->SetTitleText( szBuff );
		}
	}
}

void CustomTournamentRewardSelectWnd::UpdateEtcItemInfoVec()
{
	m_vNewShopItemInfo.clear();

	const TournamentRewardItemMap RewardMap = g_TourRewardItemManager.GetTournamentRewardItemMap();
	TournamentRewardItemMap::const_iterator iter = RewardMap.begin();
	for( ; iter != RewardMap.end(); ++iter )
	{
		DWORD dwEtcItemType = iter->first;
		const TournamentRewardItem* pItem = g_TourRewardItemManager.FindRewardItem( dwEtcItemType );
		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( dwEtcItemType );
		if( pItem && pEtcItem )
		{
			NewShopItemInfo kInfo;
			int iSize = pEtcItem->GetValueSize();
			for( int i = 0; i < iSize; ++i )
			{
				kInfo.m_iItemType      = PRESENT_ETC_ITEM;
				kInfo.m_iSortPoint     = pEtcItem->GetShopOrder( i );
				kInfo.m_iShopMarkType  = pEtcItem->GetShopMarkType( i );

				if( kInfo.m_iShopMarkType == SHOP_MARK_TYPE_NONE )
				{
					ioUserEtcItem::ETCITEMSLOT kSlot;
					ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
					if( pUserEtcItem )
					{
						if( pUserEtcItem->GetEtcItem( pEtcItem->GetType(), kSlot ) )
							kInfo.m_iShopMarkType = SHOP_MARK_TYPE_BOUGHT;
					}
				}

				int iArray             = i;
				int iManualIndex       = pEtcItem->GetMainManual( i ) - 1; // -1: number -> idx

				kInfo.m_sName          = pEtcItem->GetShopName( i );
				kInfo.m_sIcon          = pEtcItem->GetIconName();
				kInfo.m_sSubIcon = pEtcItem->GetSubIconName();
				kInfo.m_bShowPeso      = false;
				kInfo.m_iValue1        = pEtcItem->GetType();

				kInfo.m_vManualIndex.push_back( iManualIndex );				
				kInfo.m_vValue3.push_back( iArray );

				//대회코인 가격
				kInfo.m_vTournamentCoin.push_back( pItem->m_dwRewardItemPirce );				
			}

			if( !kInfo.m_sName.IsEmpty() )
				m_vNewShopItemInfo.push_back( kInfo );
		}
	}
}

bool CustomTournamentRewardSelectWnd::GetCurItemInfo( OUT NewShopItemInfo &rkInfo, IN DWORD dwWndID )
{
	if( !COMPARE( dwWndID, ID_REWARD01_CHECK, ID_REWARD16_CHECK+1) )
		return false;

	int iStartArray = m_iCurPage * LIST_MAX;
	int iCurArray   = iStartArray + (dwWndID - LIS_ID_SUB);
	int iSize       = m_vNewShopItemInfo.size();

	if( !COMPARE( iCurArray, 0, iSize ) )
		return false;

	rkInfo = m_vNewShopItemInfo[iCurArray];

	return true;
}

void CustomTournamentRewardSelectWnd::PrevPage()
{
	m_iCurPage--;
	m_iCurPage = max( 0, m_iCurPage );

	UpdatePage();

}

void CustomTournamentRewardSelectWnd::NextPage()
{
	m_iCurPage++;
	m_iCurPage = min( m_iCurPage, m_iMaxPage );

	UpdatePage();
}

DWORD CustomTournamentRewardSelectWnd::FindSelectReward( int iPos )
{
	SelectRewardPosMap::const_iterator iter = m_SelectRewardPosMap.find( iPos );
	if( iter != m_SelectRewardPosMap.end() )
		return iter->second;

	return NONE_ETC_ITEM;
}

void CustomTournamentRewardSelectWnd::InsertSelectReward( int iPos, DWORD dwEtcType )
{
	SelectRewardPosMap::const_iterator iter = m_SelectRewardPosMap.find( iPos );
	if( iter == m_SelectRewardPosMap.end() )
	{
		m_SelectRewardPosMap.insert( SelectRewardPosMap::value_type( iPos, dwEtcType ) );
	}
}

void CustomTournamentRewardSelectWnd::RemoveSelectReward( int iPos )
{
	SelectRewardPosMap::const_iterator iter = m_SelectRewardPosMap.find( iPos );
	if( iter != m_SelectRewardPosMap.end() )
		m_SelectRewardPosMap.erase( iter );
}

bool CustomTournamentRewardSelectWnd::CheckSelectReward( DWORD dwID )
{
	//상품 버튼
	int iPos = m_iCurPage * LIST_MAX + (dwID - LIS_ID_SUB);
	DWORD dwEtcType = FindSelectReward( iPos );

	if( dwEtcType == NONE_ETC_ITEM )
	{
		CustomTournamentRewardButton* pWnd = dynamic_cast<CustomTournamentRewardButton*>( FindChildWnd( dwID ) );
		if( pWnd && !pWnd->IsDisabled() )
		{
			if( m_dwBuyMax <= (DWORD)m_SelectRewardPosMap.size() )
			{
				return false;
			}
			else
			{
				InsertSelectReward( iPos, pWnd->GeEtcItemType() );
			}
		}
	}
	else
	{
		RemoveSelectReward( iPos );	
	}

	UpdatePage();
	UpdateLabel();
	UpdateTournamentRewardPrice();

	return true;
}

void CustomTournamentRewardSelectWnd::SendBuyRewardItem()
{
	if( m_dwBuyMax < (DWORD)m_SelectRewardPosMap.size() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택 가능한 보상 최대 개수를 초과하였습니다." );
		HideWnd();
		return;
	}

	SP2Packet kPacket( CTPK_TOURNAMENT_CUSTOM_REWARD_BUY );
	kPacket << m_dwTournaemntIndex;
	kPacket << m_TournamentPos;

	int iSize = m_SelectRewardPosMap.size();
	kPacket << iSize;
	SelectRewardPosMap::const_iterator iter = m_SelectRewardPosMap.begin();
	for( ; iter != m_SelectRewardPosMap.end(); ++iter )
	{
		DWORD dwEtcItemType =  iter->second;
		kPacket << dwEtcItemType;
	}

	TCPNetwork::MouseBusy( true );
	TCPNetwork::SendToServer( kPacket );
}

void CustomTournamentRewardSelectWnd::iwm_wheel( int zDelta )
{
	if( zDelta == WHEEL_DELTA )
		PrevPage();
	else if( zDelta == -WHEEL_DELTA )
		NextPage();
}

void CustomTournamentRewardSelectWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_HEART_CHARGE:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->ShowEtcItemTab();
			}
		}
		break;
	case ID_LEFT:
		if( cmd == IOBN_BTNUP )
		{
			PrevPage();
		}
		break;
	case ID_RIGHT:
		if( cmd == IOBN_BTNUP )
		{
			NextPage();
		}
		break;
	case ID_BUY:
		if( cmd == IOBN_BTNUP )
		{
			if( 0 < m_dwTotalRewardPrice )
				g_GUIMgr.SetMsgBox( MB_YESNO, this, "총 %d 대회코인을 사용하여# 보상아이템을 구매 하시겠습니까?", m_dwTotalRewardPrice );
			else
				g_GUIMgr.SetMsgBox( MB_OK, this, "포상아이템을 선택해주세요" );
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_REWARD01_CHECK:
	case ID_REWARD02_CHECK:
	case ID_REWARD03_CHECK:
	case ID_REWARD04_CHECK:
	case ID_REWARD05_CHECK:
	case ID_REWARD06_CHECK:
	case ID_REWARD07_CHECK:
	case ID_REWARD08_CHECK:
	case ID_REWARD09_CHECK:
	case ID_REWARD10_CHECK:
	case ID_REWARD11_CHECK:
	case ID_REWARD12_CHECK:
	case ID_REWARD13_CHECK:
	case ID_REWARD14_CHECK:
	case ID_REWARD15_CHECK:
	case ID_REWARD16_CHECK:
		if( cmd == IOBN_BTNUP )
		{
			if( !CheckSelectReward( dwID ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "선택 가능한 보상 최대 개수를 초과하였습니다." );
			}
		}
		break;
	case MESSAGE_BOX:
	case MESSAGE_BOX3:
		if( cmd == IOBN_BTNUP )
		{
			if( param == IDYES )
			{
				SendBuyRewardItem();
				HideWnd();
			}
		}
		break;
	}
}

void CustomTournamentRewardSelectWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	DWORD dwCoin = 0;
	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_TOURNAMENT_COIN );
	if( pItem )
		dwCoin = pItem->GetCash( 0 );

	g_FontMgr.SetTextStyle( TS_OUTLINE_2X );
	g_FontMgr.SetAlignType( TAT_RIGHT );
	g_FontMgr.SetBkColor( 0x0C426F );
	g_FontMgr.SetTextColor( 0xFF88CCFF );
	g_FontMgr.PrintText( iXPos + 578, iYPos + 13, FONT_SIZE_13, "내 대회코인 : %d ", g_MyInfo.GetEtcCoinMoney(ioEtcItem::EIT_ETC_TOURNAMENT_COIN) );

	if( m_pLine )
		m_pLine->Render( iXPos, iYPos );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 357, iYPos + 332, FONT_SIZE_12, "%d/%d", m_iCurPage + 1, m_iMaxPage + 1 );

	OnPirceRender( iXPos, iYPos );
}

void CustomTournamentRewardSelectWnd::OnPirceRender( int iXPos, int iYPos )
{
	if( !m_pTournamentCoinIcon )
		return;

	int iWidth = 0;
	if( 0 < m_dwTotalRewardPrice )
	{
		int iCoinWidth = m_pTournamentCoinIcon->GetWidth();		
		m_pTournamentCoinIcon->Render( iXPos + 441 - iCoinWidth/2, iYPos + 393 );
		iWidth += iCoinWidth + TOURCOIN_ICON_GAP;

		char szBuffer[MAX_PATH];
		sprintf( szBuffer, "= %d", m_dwTotalRewardPrice );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );
		g_FontMgr.PrintText( iXPos + 441 - iWidth, iYPos + 393, FONT_SIZE_12, "%s", szBuffer );
		iWidth += g_FontMgr.GetTextWidth( szBuffer, TS_NORMAL, FONT_SIZE_12 );

		ioHashString szRound;
		Help::ConvertTournamentRoundResultText( szRound, m_dwStartTeamCnt, m_TournamentPos-1 );
		sprintf( szBuffer, "(%s %d팀 × 팀 당 %d명) ", szRound.c_str(), max(1, m_dwTournamentRound/2), m_dwMaxPlayer );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );	
		g_FontMgr.PrintText( iXPos + 441 - iWidth, iYPos + 393, FONT_SIZE_12, "%s", szBuffer );
		iWidth += g_FontMgr.GetTextWidth( szBuffer, TS_NORMAL, FONT_SIZE_12 );

		sprintf( szBuffer, "× %d개 ", max(1, m_dwTournamentRound/2) * m_dwMaxPlayer );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );		
		g_FontMgr.PrintText( iXPos + 441 - iWidth, iYPos + 393, FONT_SIZE_12, "%s", szBuffer );
		iWidth += g_FontMgr.GetTextWidth( szBuffer, TS_NORMAL, FONT_SIZE_12 );

		m_pTournamentCoinIcon->Render( iXPos + 441 - iWidth - iCoinWidth/2, iYPos + 393 );
		iWidth += iCoinWidth + TOURCOIN_ICON_GAP;

		sprintf( szBuffer, "%d", m_dwSelectRewardPrice );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_RIGHT );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_ORANGE );		
		g_FontMgr.PrintText( iXPos + 441 - iWidth, iYPos + 393, FONT_SIZE_12, "%s", szBuffer );
		iWidth += g_FontMgr.GetTextWidth( szBuffer, TS_NORMAL, FONT_SIZE_12 );
	}
}

//////////////////////////////////////////////////////////////////////////
TournamentCoinItemNeedPopup::TournamentCoinItemNeedPopup()
{
	m_pIconBack = NULL;
	m_pIcon		= NULL;
}

TournamentCoinItemNeedPopup::~TournamentCoinItemNeedPopup()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIcon );
}

void TournamentCoinItemNeedPopup::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_CREATE_ITEM_BUY:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();
				pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_TOURNAMENT_COIN, 0 );
			}
		}
		break;
	}
}

void TournamentCoinItemNeedPopup::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else 
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentCoinItemNeedPopup::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pIconBack )
		m_pIconBack->Render( iXPos + 151, iYPos + 98, UI_RENDER_MULTIPLY );

	if( m_pIcon )
		m_pIcon->Render( iXPos + 151, iYPos + 98 );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 144, FONT_SIZE_13, "[대회코인]이 부족합니다." );

	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 164, FONT_SIZE_13, "대회코인으로 보상을 구입할 수 있으며" );
	g_FontMgr.PrintText( iXPos + 151, iYPos + 184, FONT_SIZE_13, "상점 - 특별에서 구입하실 수 있습니다." );
}

//////////////////////////////////////////////////////////////////////////

TournamentAlarmCustomRewardWnd::TournamentAlarmCustomRewardWnd()
{
	m_pDevideDot     = NULL;
	m_pPresentBack   = NULL;
	m_pPresentNone   = NULL;

	m_pBackCircle	 = NULL;
	m_pBackHub		 = NULL;
	m_pCustomReward	 = NULL;
}

TournamentAlarmCustomRewardWnd::~TournamentAlarmCustomRewardWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );
	SAFEDELETE( m_pCustomReward );

	DestroyCsutomIconList();
}

void TournamentAlarmCustomRewardWnd::DestroyCsutomIconList()
{
	CustomRewardDataVec::iterator iter = m_CustomRewardDataVec.begin();
	for( ; iter != m_CustomRewardDataVec.end(); ++iter )
	{
		CustomRewardData rkData = *iter;
		DestroyCsutomIcon( rkData );
	}
}

void TournamentAlarmCustomRewardWnd::DestroyCsutomIcon( CustomRewardData& rkData )
{
	CustomRewardData::ioUIRenderImageVec::iterator element = rkData.m_vUIRenderImageVec.begin();
	for( ; element != rkData.m_vUIRenderImageVec.end(); ++element )
	{
		ioUIRenderImage* pIamge = *element;
		SAFEDELETE( pIamge );
	}
	rkData.m_vUIRenderImageVec.clear();

	element = rkData.m_vUIRenderSubImageVec.begin();
	for( ; element != rkData.m_vUIRenderSubImageVec.end(); ++element )
	{
		ioUIRenderImage* pIamge = *element;
		SAFEDELETE( pIamge );
	}
	rkData.m_vUIRenderSubImageVec.clear();
}

void TournamentAlarmCustomRewardWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CustomReward" )
	{
		SAFEDELETE( m_pCustomReward );
		m_pCustomReward = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentAlarmCustomRewardWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );
}

void TournamentAlarmCustomRewardWnd::AddCustomTournamentRewardData( DWORDVec& vEtcItemCodeVec, ioHashString szTourTitle, short MaxRound, int iTourPos )
{
	CustomRewardData rkRewardData;
	rkRewardData.m_TournamentPos = iTourPos;

	int StartTeamCnt = (int)pow( 2.0, (double)MaxRound );
	rkRewardData.m_TournamentRound	= Help::TournamentCurrentRoundWithTeam( StartTeamCnt, iTourPos - 1 );
	rkRewardData.m_dwCustomRewardVec = vEtcItemCodeVec;
	rkRewardData.szTtitle = szTourTitle;

	DWORDVec::const_iterator iter = vEtcItemCodeVec.begin();
	for( ; iter != vEtcItemCodeVec.end(); ++iter )
	{
		ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( *iter );
		if( pItem )
		{
			rkRewardData.m_vUIRenderImageVec.push_back( g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() ) );
			rkRewardData.m_vUIRenderSubImageVec.push_back( g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() ) );
		}
	}

	m_CustomRewardDataVec.push_back( rkRewardData );
}

void TournamentAlarmCustomRewardWnd::iwm_hide()
{
	CustomRewardDataVec::iterator iter = m_CustomRewardDataVec.begin();
	if( iter != m_CustomRewardDataVec.end() )
	{
		DestroyCsutomIcon( *iter );
		m_CustomRewardDataVec.erase( iter );
	}
}

void TournamentAlarmCustomRewardWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CLOSE :
	case ID_CONFIRM :
		{
			if( cmd == IOBN_BTNUP )
			{
				HideWnd();
			}
		}
		break;
	}
}

void TournamentAlarmCustomRewardWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
	{
		if( m_CustomRewardDataVec.empty() )
			return;
		else
		{
			ShowWnd();
		}
	}
}

void TournamentAlarmCustomRewardWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );

	//대회에 참가한 경우
	OnBackRender( iXPos, iYPos );
	OnCustomIconRender( iXPos, iYPos );
	OnCustomRewardResultRender( iXPos, iYPos );
	OnPresentRender( iXPos, iYPos );
}

void TournamentAlarmCustomRewardWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentAlarmCustomRewardWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	if( bReward )
	{		
		m_pBackHub->Render( iXPos, iYPos );
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}
}

void TournamentAlarmCustomRewardWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;

	for( DWORD i = 0; i < m_dwPresentBackCnt; ++i )
	{
		CustomRewardDataVec::iterator iter = m_CustomRewardDataVec.begin();
		if( iter == m_CustomRewardDataVec.end()  )
			continue;

		CustomRewardData &rData = *iter;
		ioUIRenderImage* pImage = NULL;
		ioUIRenderImage* pSubImage = NULL;
		if( bReward && i < (int)rData.m_vUIRenderImageVec.size() )
		{
			pImage = rData.m_vUIRenderImageVec[i];
		}
		if( bReward && i < (int)rData.m_vUIRenderSubImageVec.size() )
		{
			pSubImage = rData.m_vUIRenderSubImageVec[i];
		}
		if( pImage )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			pImage->SetPosition( 231, 374 );
			pImage->SetScale( 0.55f, 0.55f );
			pImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( pSubImage )
			{
				pSubImage->SetPosition( 231, 374 );
				pSubImage->SetScale( 0.55f, 0.55f );
				pSubImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentAlarmCustomRewardWnd::OnCustomIconRender( int iXPos, int iYPos )
{
	if( !m_pCustomReward )
		return;

	m_pCustomReward->Render( iXPos, iYPos );
}

void TournamentAlarmCustomRewardWnd::OnCustomRewardResultRender( int iXPos, int iYPos )
{	
	CustomRewardDataVec::iterator iter = m_CustomRewardDataVec.begin();
	if( iter == m_CustomRewardDataVec.end()  )
		return;

	CustomRewardData &rkData = *iter;
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	//줄
	if( rkData.m_dwCustomRewardVec.empty() )
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	else
		g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

	if( rkData.m_TournamentRound == 1 )
		g_FontMgr.PrintText( iXPos + 296, iYPos + 69, FONT_SIZE_24, "우승을 축하합니다." );
	else if( rkData.m_TournamentRound == 2 )
		g_FontMgr.PrintText( iXPos + 296, iYPos + 69, FONT_SIZE_24, "준우승을 축하합니다." );
	else
		g_FontMgr.PrintText( iXPos + 296, iYPos + 69, FONT_SIZE_24, "%d강 달성을 축하합니다.", rkData.m_TournamentRound );

	//2줄
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( rkData.m_dwCustomRewardVec.empty() )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 296, iYPos + 97, FONT_SIZE_24, "획득가능한 보상은 없습니다." );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 296, iYPos + 97, FONT_SIZE_24, "주최자님이 보상을 드립니다." );
	}

	//3줄
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 성적에 따라 팀 소속의 각 개인당" );
	kPrinter.PrintFullText( iXPos + 296, iYPos + 133, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 151, TAT_CENTER );
	kPrinter.ClearList();

	//하단 보상
	int iOffset = 0;
	DWORDVec::iterator element = rkData.m_dwCustomRewardVec.begin();
	for( ; element != rkData.m_dwCustomRewardVec.end(); ++element  )
	{
		ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( *element );
		if( pItem )
		{
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, "%s", pItem->GetName() );
			kPrinter.PrintFullText( iXPos + 296, iYPos + 275 + iOffset, TAT_CENTER );
			kPrinter.ClearList();
			iOffset += 18;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerCustomWnd::TournamentDetailInnerCustomWnd()
{
	m_pDevideDot     = NULL;
	m_pPresentBack   = NULL;
	m_pPresentNone   = NULL;

	m_pBackCircle	 = NULL;
	m_pBackHub		 = NULL;
	m_pCustomReward	 = NULL;

	m_TournamentPos   = 0;
	m_TournamentRound = 0;

	m_dwTournamentIndex = 0;
}

TournamentDetailInnerCustomWnd::~TournamentDetailInnerCustomWnd()
{
	SAFEDELETE( m_pDevideDot );

	SAFEDELETE( m_pPresentBack );
	SAFEDELETE( m_pPresentNone );

	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );
	SAFEDELETE( m_pCustomReward );

	DestroyCsutomIconList();
}

void TournamentDetailInnerCustomWnd::DestroyCsutomIconList()
{
	ioUIRenderImageVec::iterator iter = m_vUIRenderImageVec.begin();
	for( ; iter != m_vUIRenderImageVec.end(); ++iter )
	{
		ioUIRenderImage* pIamge = *iter;
		SAFEDELETE( pIamge );
	}
	m_vUIRenderImageVec.clear();

	iter = m_vUIRenderSubImageVec.begin();
	for( ; iter != m_vUIRenderSubImageVec.end(); ++iter )
	{
		ioUIRenderImage* pIamge = *iter;
		SAFEDELETE( pIamge );
	}
	m_vUIRenderSubImageVec.clear();
}

void TournamentDetailInnerCustomWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DevideStrip" )
	{
		SAFEDELETE( m_pDevideDot );
		m_pDevideDot = pImage;
	}
	else if( szType == "PresentBack" )
	{
		SAFEDELETE( m_pPresentBack );
		m_pPresentBack = pImage;
	}
	else if( szType == "PresentNone" )
	{
		SAFEDELETE( m_pPresentNone );
		m_pPresentNone = pImage;
	}
	else if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "CustomReward" )
	{
		SAFEDELETE( m_pCustomReward );
		m_pCustomReward = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerCustomWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fDevideOffsetX = xElement.GetFloatAttribute_e( "DevideOffsetX" );
	m_fDevideOffsetY = xElement.GetFloatAttribute_e( "DevideOffsetY" );
	m_dwDevideVertCnt = xElement.GetIntAttribute_e( "DevideVertCnt" );
	m_dwDevideHorzCnt = xElement.GetIntAttribute_e( "DevideHorzCnt" );

	m_dwPresentBackCnt = xElement.GetIntAttribute_e( "PresentBackCnt" );
	m_fPresentOffsetX = xElement.GetFloatAttribute_e( "PresentOffsetX" );
	m_fPresentScaleRate = xElement.GetFloatAttribute_e( "PresentScaleRate" );
}

void TournamentDetailInnerCustomWnd::SetTournamentData( DWORD dwTourIndex )
{
	m_dwTournamentIndex = dwTourIndex;
	m_dwCustomRewardVec.clear();
	DestroyCsutomIconList();
}

void TournamentDetailInnerCustomWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
}

void TournamentDetailInnerCustomWnd::ProcessUpdateTournament()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) return;
	if( m_TournamentState != pTournament->GetState() )

	m_TournamentState		= pTournament->GetState();
	m_dwTournamentMaxRound	= pTournament->GetTournamentMaxRound();

	m_TournamentPos = 0;
	ioUserTournament *pUserTournament = g_MyInfo.GetTournament();
	if( pUserTournament )
	{
		DWORD dwTeamIndex = pUserTournament->GetTeamIndex( m_dwTournamentIndex );
		TeamGlobalData *pGlobalTeamData = g_TournamentTeamMgr.GetTeamData( dwTeamIndex );
		if( pGlobalTeamData )
		{
			m_TournamentPos = pGlobalTeamData->GetTourPos();
		}
	}

	m_TournamentRound = Help::TournamentCurrentRoundWithTeam( pTournament->GetTournamentStartTeamCount(), m_TournamentPos - 1 );
}

void TournamentDetailInnerCustomWnd::ProcessSyncCheckRewardData()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex, false );
	if( pTournament == NULL ) return;
	
	if( pTournament->IsCustomRewardDataSync() )
	{
		if( m_dwCustomRewardVec.empty() )
		{
			pTournament->GetCustomRewardData( m_dwTournamentIndex, m_TournamentPos, m_dwCustomRewardVec );
			DestroyCsutomIconList();
			DWORDVec::const_iterator iter = m_dwCustomRewardVec.begin();
			for( ; iter != m_dwCustomRewardVec.end(); ++iter )
			{
				ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( *iter );
				if( pItem )
				{
					m_vUIRenderImageVec.push_back( g_UIImageSetMgr.CreateImageByFullName( pItem->GetIconName() ) );
					m_vUIRenderSubImageVec.push_back( g_UIImageSetMgr.CreateImageByFullName( pItem->GetSubIconName() ) );
				}
			}
		}
	}
	else
	{
		pTournament->SendCustomRewardDataSync();
	}
}

void TournamentDetailInnerCustomWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );

	ProcessUpdateTournament();
	ProcessSyncCheckRewardData();
}

void TournamentDetailInnerCustomWnd::OnRender()
{
	if( !IsShow() ) return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnDevideDotRender( iXPos, iYPos );
	OnBackRender( iXPos, iYPos );
	OnCustomIconRender( iXPos, iYPos );

	//포상기간일때
	switch(	m_TournamentState )
	{
	case TournamentNode::STATE_WAITING:
		{
			if( 0 < m_TournamentPos )
				OnCustomRewardResultRender( iXPos, iYPos ); //대회에 참가한 경우
			else			
				OnCustomRewardNoneRender( iXPos, iYPos );	//참가하지 않은 경우
		}	
		break;
	default:
		OnCustomNoneRender( iXPos, iYPos );
		break;
	}
	OnPresentRender( iXPos, iYPos );
}

void TournamentDetailInnerCustomWnd::OnDevideDotRender( int iXPos, int iYPos )
{
	if( !m_pDevideDot )
		return;

	for( DWORD i = 0; i < m_dwDevideHorzCnt; ++i )
		for( DWORD j = 0; j < m_dwDevideVertCnt; ++j )
			m_pDevideDot->Render( iXPos + i * m_fDevideOffsetX, iYPos + m_fDevideOffsetY * j );
}

void TournamentDetailInnerCustomWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	if( bReward )
	{		
		m_pBackHub->Render( iXPos, iYPos );
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}
}

void TournamentDetailInnerCustomWnd::OnPresentRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pPresentBack || !m_pPresentNone )
		return;

	for( DWORD i =0; i < m_dwPresentBackCnt; ++i )
	{
		ioUIRenderImage* pImage = NULL;
		ioUIRenderImage* pSubImage = NULL;
		if( bReward && i < m_vUIRenderImageVec.size() )
		{
			pImage = m_vUIRenderImageVec[i];
		}
		if( bReward && i < m_vUIRenderSubImageVec.size() )
		{
			pSubImage = m_vUIRenderSubImageVec[i];
		}
		if( pImage )
		{
			m_pPresentBack->SetScale( 0.55f, 0.55f );
			m_pPresentBack->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );

			pImage->SetPosition( 204, 320 );
			pImage->SetScale( 0.55f, 0.55f );
			pImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );

			if( pSubImage )
			{
				pSubImage->SetPosition( 204, 320 );
				pSubImage->SetScale( 0.55f, 0.55f );
				pSubImage->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
		else
		{
			m_pPresentNone->SetScale( 0.55f, 0.55f );
			m_pPresentNone->Render( iXPos + i * m_fPresentOffsetX, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
	}
}

void TournamentDetailInnerCustomWnd::OnCustomIconRender( int iXPos, int iYPos )
{
	if( !m_pCustomReward )
		return;

	m_pCustomReward->Render( iXPos, iYPos );
}

void TournamentDetailInnerCustomWnd::OnCustomRewardResultRender( int iXPos, int iYPos )
{
	if( m_TournamentRound == 1 )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		if( m_dwCustomRewardVec.empty() )
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

		g_FontMgr.PrintText( iXPos + 279, iYPos + 15, FONT_SIZE_24, "우승을 축하합니다." );
	}
	else if( m_TournamentRound <= 2 )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		if( m_dwCustomRewardVec.empty() )
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

		g_FontMgr.PrintText( iXPos + 279, iYPos + 15, FONT_SIZE_24, "준우승을 축하합니다." );
	}
	else
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetBkColor( 0, 0, 0 );

		if( m_dwCustomRewardVec.empty() )
			g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		else
			g_FontMgr.SetTextColor( TCT_DEFAULT_GREEN );

		g_FontMgr.PrintText( iXPos + 279, iYPos + 15, FONT_SIZE_24, "%d강 달성을 축하합니다.", m_TournamentRound );
	}

	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	if( m_dwCustomRewardVec.empty() )
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 279, iYPos + 43, FONT_SIZE_24, "획득가능한 보상은 없습니다." );
	}
	else
	{
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 279, iYPos + 43, FONT_SIZE_24, "주최자님이 보상을 드립니다." );
	}

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 참가시 대회성적에 따라 획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "포상기간에 접속하시면 선물로 획득하실 수 있습니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();

	int iOffset = 0;
	DWORDVec::iterator iter = m_dwCustomRewardVec.begin();
	for( ; iter != m_dwCustomRewardVec.end(); ++iter  )
	{
		ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( *iter );
		if( pItem )
		{
			kPrinter.SetTextStyle( TS_NORMAL );
			kPrinter.SetBkColor( 0, 0, 0 );
			kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kPrinter.AddTextPiece( FONT_SIZE_12, "%s", pItem->GetName() );
			kPrinter.PrintFullText( iXPos + 279, iYPos + 221 + iOffset, TAT_CENTER );
			kPrinter.ClearList();
			iOffset += 18;
		}
	}
}

void TournamentDetailInnerCustomWnd::OnCustomRewardNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 15, FONT_SIZE_24, "대회에 참가 하지 않았습니다" );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 43, FONT_SIZE_24, "획득 가능한 보상은 없습니다" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 참가시 대회성적에 따라 획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "포상기간에 접속하시면 선물로 획득하실 수 있습니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerCustomWnd::OnCustomNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 15, FONT_SIZE_24, "????" );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 43, FONT_SIZE_24, "대회성적 보상" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "대회 참가시 대회성적에 따라 획득가능한 보상입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 79, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "포상기간에 접속하시면 선물로 획득하실 수 있습니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 97, TAT_CENTER );
	kPrinter.ClearList();
}


//////////////////////////////////////////////////////////////////////////

TournamentDetailInnerAllocateWnd::TournamentDetailInnerAllocateWnd()
{
	m_pBackCircle = NULL;
	m_pBackHub	  = NULL;
	m_pIcon		  = NULL;

	m_TournamentState  = 0;
	m_TournamentPos    = 0;
	m_TournamentRound  = 0;
	m_dwRoundTeamCount = 0;

	m_bAllocate = false;
}

TournamentDetailInnerAllocateWnd::~TournamentDetailInnerAllocateWnd()
{
	SAFEDELETE( m_pBackCircle );
	SAFEDELETE( m_pBackHub );
	SAFEDELETE( m_pIcon );
}

void TournamentDetailInnerAllocateWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackCircle" )
	{
		SAFEDELETE( m_pBackCircle );
		m_pBackCircle = pImage;
	}
	else if( szType == "BackHub" )
	{
		SAFEDELETE( m_pBackHub );
		m_pBackHub = pImage;
	}
	else if( szType == "Icon" )
	{
		SAFEDELETE( m_pIcon );
		m_pIcon = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void TournamentDetailInnerAllocateWnd::SetTournamentData( DWORD dwTourIndex )
{	
	m_dwRoundTeamCount		= 0;
	m_dwTournamentIndex		= dwTourIndex;	

	ShowWnd();
}

void TournamentDetailInnerAllocateWnd::RoundCheckProcess()
{
	TournamentNode *pTournament = g_TournamentMgr.GetTournament( m_dwTournamentIndex );
	if( pTournament == NULL ) return;
	if( pTournament->GetType() == TournamentNode::TYPE_REGULAR ) return;

	m_bAllocate				= pTournament->IsEntryAllocated();
	m_dwRoundTeamCount		= pTournament->GetCustomRoundTeamCount( 1 );
	m_TournamentState		= pTournament->GetState();
	m_dwTournamentMaxRound	= pTournament->GetTournamentMaxRound();
		
	if( !pTournament->IsRoundTeamDataSyncCheck( 1, 1, pTournament->GetTournamentStartTeamCount(), 0 ) )
		pTournament->SendRoundTeamDataSync( 1, 1, pTournament->GetTournamentStartTeamCount(), 0  );
}

void TournamentDetailInnerAllocateWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	ioWnd::OnProcess( fTimePerSec );
	RoundCheckProcess();
}

void TournamentDetailInnerAllocateWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	OnBackRender( iXPos, iYPos );
	OnCustomIconRender( iXPos, iYPos );

	if( TournamentNode::STATE_TEAM_DELAY == m_TournamentState )
	{
		if( m_bAllocate )
			OAllocateDone( iXPos, iYPos );
		else
			OAllocateRender( iXPos, iYPos );
	}
	else if( TournamentNode::STATE_TOURNAMENT <= m_TournamentState || TournamentNode::STATE_WAITING == m_TournamentState )
	{
		OAllocateDone( iXPos, iYPos );			
	}
	else
	{
		OnAllocateNoneRender( iXPos, iYPos );
	}
}

void TournamentDetailInnerAllocateWnd::OnBackRender( int iXPos, int iYPos, bool bReward )
{
	if( !m_pBackCircle || !m_pBackHub )
		return;

	if( bReward )
	{		
		m_pBackHub->Render( iXPos, iYPos );
	}	
	else
	{
		m_pBackCircle->Render( iXPos, iYPos );
	}
}

void TournamentDetailInnerAllocateWnd::OnCustomIconRender( int iXPos, int iYPos )
{
	if( !m_pIcon )
		return;

	m_pIcon->Render( iXPos, iYPos );
}

void TournamentDetailInnerAllocateWnd::OnAllocateNoneRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 58, FONT_SIZE_24, "지금은" );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 86, FONT_SIZE_24, "대진배정 기간이 아닙니다" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "모집기간 동안 참가 신청한 팀들로 주최자가 대진표를 작성하는 기간입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 122, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "주최자가 대진표를 작성하면 대회일정은 빨리 공개될 수도 있습니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 140, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerAllocateWnd::OAllocateRender( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 58, FONT_SIZE_24, "대진배정이" );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 86, FONT_SIZE_24, "진행되고 있습니다" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "모집기간 동안 참가 신청한 팀들로 주최자가 대진표를 작성하는 기간입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 122, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "주최자가 대진표를 작성하면 대회일정은 빨리 공개될 수도 있습니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 140, TAT_CENTER );
	kPrinter.ClearList();
}

void TournamentDetailInnerAllocateWnd::OAllocateDone( int iXPos, int iYPos )
{
	//
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 58, FONT_SIZE_24, "총 %d개 팀이", m_dwRoundTeamCount );
	g_FontMgr.PrintText( iXPos + 279, iYPos + 86, FONT_SIZE_24, "대회일정에 배치 되었습니다" );

	//
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "모집기간 동안 참가 신청한 팀들로 주최자가 대진표를 작성하는 기간입니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 122, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, "주최자가 대진표를 작성하면 대회일정은 빨리 공개될 수도 있습니다." );
	kPrinter.PrintFullText( iXPos + 279, iYPos + 140, TAT_CENTER );
	kPrinter.ClearList();
}