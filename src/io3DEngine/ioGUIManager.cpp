

#include "stdafx.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"
#include "ioGUIManager.h"
#include "ioFrameTimer.h"

#include "ioWnd.h"
#include "ioButton.h"
#include "ioEdit.h"
#include "ioProgressBar.h"
#include "ioScroll.h"

#include "ioDragItem.h"
#include "ioUIImageSetManager.h"
#include "ioUIFrameManager.h"

#include "ioUIRenderImage.h"
#include "ioUIRenderFrame.h"
#include "ioUITitle.h"

#include "ioStringConverter.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ErrorReport.h"
#include "iostringmanager.h"
#include "HelpFunc.h"

#include "ioUIImageSet.h"
#include "ioUIImage.h"

#include "ioWndEX.h"
#include "ioFrameWndEX.h"
#include "ioImageWndEX.h"
#include "ioLabelWndEX.h"
#include "ioCustomWnd.h"
#include "ioButtonWndEX.h"
#include "ioCheckBoxEX.h"
#include "ioFlashPlayer.h"
#include "ioTabWndEX.h"
#include "ioScrollBarEX.h"
#include "ioRichLabel.h"
#include "ioMovingWnd.h"
#include "ioUI3DEffectRender.h"


ioGUIManager::ioGUIManager() : m_StaticSwitch( SATICS_OFF )
{
	m_pUIImageSetMgr = NULL;
	m_pUIFrameMgr = NULL;

	m_pDeskTopWnd		= NULL;
	m_pPreOverWnd		= NULL;
	m_pPreClickWnd		= NULL;
	m_pPreRightClickWnd = NULL;

	m_pCurDragMoveWnd = NULL;

	m_DragCheckState = DCS_NONE;
	m_dwDragCheckStartTime = 0;
	m_dwDragCheckDuration  = 500;
	m_pDragItem = NULL;
	m_pPreDragOverWnd = NULL;
	m_pDragStartWnd = NULL;

	m_LastMousePos.x = 0;
	m_LastMousePos.y = 0;
	m_isLButtonDown = false;

	m_dwInnerUniqueIndexCounter = 0;
}

ioGUIManager::~ioGUIManager()
{
	ClearAllGUI();

	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		delete(m_vToolTipWnd[i]);
	}
	m_vToolTipWnd.clear();

	SAFEDELETE( m_pUIFrameMgr );
	SAFEDELETE( m_pUIImageSetMgr );
}


void ioGUIManager::AddMem( const ioHashString& KeyName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		m_MemoryMap.insert( MemoryMap::value_type( KeyName, dwMem ) );
	}
}

void ioGUIManager::RemoveMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{
			m_MemoryMap.erase( iter );
		}
	}
}

void ioGUIManager::GetGUIMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	dwCount	= 0;
	dwSize	= 0;

	for( MemoryMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwSize	+= iter->second;
		dwCount++;
	}
}

void ioGUIManager::GetFrameMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	m_pUIFrameMgr->GetMemStatics( dwCount, dwSize );
}

void ioGUIManager::GetImageSetMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	m_pUIImageSetMgr->GetMemStatics( dwCount, dwSize );
}

void ioGUIManager::SetSwitch( int eSwitch )
{
	m_StaticSwitch = static_cast<StaticSwitch>( eSwitch );
	m_pUIFrameMgr->SetSwitch( eSwitch );
	m_pUIImageSetMgr->SetSwitch( eSwitch );
}

void ioGUIManager::InitManager()
{
	SAFEDELETE( m_pUIImageSetMgr );
	m_pUIImageSetMgr = new ioUIImageSetManager;

	SAFEDELETE( m_pUIFrameMgr );
	m_pUIFrameMgr = new ioUIFrameManager;

	ioDragItem::m_pGUIMgr = this;
}

void ioGUIManager::ClearAllGUI()
{
	SAFEDELETE( m_pDeskTopWnd );

	ClearPreState();
	ClearDragState();
}

void ioGUIManager::InitDeskTop( int iWidth, int iHeight )
{
	ClearAllGUI();

	m_pDeskTopWnd = new ioWnd;
	m_pDeskTopWnd->SetID( ID_DESKTOP );
	m_pDeskTopWnd->SetWndStyle( IWS_DRAG_DROP );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, iWidth, iHeight );
	m_pDeskTopWnd->iwm_create();
	m_pDeskTopWnd->SetWndRect( rcRect );
	m_pDeskTopWnd->ShowWnd();

	m_dwInnerUniqueIndexCounter = 0;
}

void ioGUIManager::LoadImageSetFile( const char *szFileName )
{
	if( m_pUIImageSetMgr )
	{
		m_pUIImageSetMgr->LoadFile( szFileName );
	}
}

void ioGUIManager::LoadFrameFile( const char *szFileName )
{
	if( m_pUIFrameMgr )
	{
		m_pUIFrameMgr->LoadFile( szFileName );
	}
}

void ioGUIManager::DoDragDrop( ioWnd *pStartWnd,
							   ioDragItem *pDragItem,
							   const RECT &rcRect,
							   const POINT &ptOffset,
							   const RECT *pRectStartDrag )
{
	m_DragCheckState = DCS_CHECK;
	m_rcDragCheck = rcRect;

	if( pRectStartDrag )
	{
		m_rcStartDrag = *pRectStartDrag;
	}
	else
	{
		m_rcStartDrag.left   = rcRect.left + ptOffset.x;
		m_rcStartDrag.top    = rcRect.top + ptOffset.y;
		m_rcStartDrag.right  = m_rcStartDrag.left + 1;
		m_rcStartDrag.bottom = m_rcStartDrag.top + 1;
	}

	m_ptDragOffset = ptOffset;
	m_dwDragCheckStartTime = REALGETTIME();

	m_pDragItem = pDragItem;
	m_pDragStartWnd = pStartWnd;
}

void ioGUIManager::SetDragCheckDuration( DWORD dwDuration )
{
	m_dwDragCheckDuration = dwDuration;
}

POINT ioGUIManager::GetDragCheckPoint( ioMouse &mouse )
{
	POINT ptCheck, ptMouse;
	ptMouse = mouse.GetMousePos();

	ptCheck.x = ptMouse.x - m_ptDragOffset.x;
	ptCheck.x = max( 0, min( ptCheck.x, m_pDeskTopWnd->GetWidth() ) );

	ptCheck.y = ptMouse.y - m_ptDragOffset.y;
	ptCheck.y = max( 0, min( ptCheck.y, m_pDeskTopWnd->GetHeight() ) );

	return ptCheck;
}

void ioGUIManager::ClearDragState()
{
	m_DragCheckState = DCS_NONE;
	m_dwDragCheckStartTime = 0;
	m_pDragItem = NULL;
	m_pPreDragOverWnd = NULL;
	m_pDragStartWnd = NULL;
}

void ioGUIManager::DeleteWnd( DWORD dwID )
{
	if( m_pDeskTopWnd )
	{
		m_pDeskTopWnd->DeleteChild( dwID );
	}
}

ioWnd* ioGUIManager::GetDeskTop() const
{
	return m_pDeskTopWnd;
}

ioWnd* ioGUIManager::FindWnd( DWORD dwID )
{
	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		if( m_vToolTipWnd[i]->GetID() == dwID )
		{
			return m_vToolTipWnd[i];
		}
	}
	if( m_pDeskTopWnd )
		return m_pDeskTopWnd->FindChildWnd( dwID );

	return NULL;
}


ioWnd* ioGUIManager::FindWnd( const ioHashString& szWndName )
{
	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		if( m_vToolTipWnd[i]->GetWndName() == szWndName )
		{
			return m_vToolTipWnd[i];
		}
	}
	if( m_pDeskTopWnd )
		return m_pDeskTopWnd->FindChildWnd( szWndName );

	return NULL;
}

void ioGUIManager::ShowWnd( DWORD dwID, bool bMustShow )
{
	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		if( m_vToolTipWnd[i]->GetID() == dwID )
		{
			m_vToolTipWnd[i]->ShowWnd();
			return;
		}
	}
	if( m_pDeskTopWnd )
	{
		m_pDeskTopWnd->ShowChildWnd( dwID, bMustShow );
	}
}

bool ioGUIManager::IsShow( DWORD dwWndID )
{
	ioWnd *pWnd = FindWnd( dwWndID );
	if( pWnd )
		return pWnd->IsShow();

	return false;
}

void ioGUIManager::HideWnd( DWORD dwID, bool bMustHide )
{
	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		if( m_vToolTipWnd[i]->GetID() == dwID )
		{
			m_vToolTipWnd[i]->HideWnd();
			return;
		}
	}
	if( m_pDeskTopWnd )
	{
		m_pDeskTopWnd->HideChildWnd( dwID, bMustHide );
	}
}

void ioGUIManager::ClearPreState()
{
	m_pPreOverWnd		= NULL;
	m_pPreClickWnd		= NULL;
	m_pPreRightClickWnd = NULL;
	m_pCurDragMoveWnd	= NULL;

	ioWnd::m_pTopModalWnd	= NULL;	
	ioEdit::m_pKeyFocusEdit = NULL;	
}

void ioGUIManager::UpdateGUI( ioMouse &mouse )
{
	ioEdit::CheckCaretTwinkle( REALGETTIME() );

	if( !mouse.IsBusy() )
	{
		m_LastMousePos  = mouse.GetMousePos();
		m_isLButtonDown = mouse.IsLBtnDown();

		ProcessDragMoveWnd( mouse );

		if( ioWnd::m_pTopModalWnd )
		{
			CheckMouseModal( ioWnd::m_pTopModalWnd, mouse );
		}
		else
		{
			switch( m_DragCheckState )
			{
			case DCS_NONE:
				CheckMouse( m_pDeskTopWnd, mouse );
				break;
			case DCS_CHECK:
				if( !CheckDragStart( mouse ) )
				{
					CheckMouse( m_pDeskTopWnd, mouse );
				}
				else
				{
					if( m_pPreOverWnd )
					{						
						m_pPreOverWnd->iwm_mouseleave( mouse );
						m_pPreOverWnd = NULL;
					}
				}
				break;
			case DCS_DRAGGING:
				CheckDragMouse( m_pDeskTopWnd, mouse );
				break;
			}

			CheckEditFocus( mouse );
		}
	}

	ProcessGUI( mouse );
}

void ioGUIManager::ProcessDragMoveWnd( ioMouse &mouse )
{
	if( !m_pCurDragMoveWnd || !mouse.IsMoved() )
		return;

	if( !m_pCurDragMoveWnd->HasWndStyle( IWS_CANMOVE ) )
		return;

	POINT ptWndPos;
	ptWndPos.x = m_pCurDragMoveWnd->GetDerivedPosX();
	ptWndPos.y = m_pCurDragMoveWnd->GetDerivedPosY();

	POINT ptMousePos = mouse.GetMousePos();
	ptWndPos.x += ptMousePos.x - m_ptDragMovePos.x;
	ptWndPos.y += ptMousePos.y - m_ptDragMovePos.y;

	m_pCurDragMoveWnd->SetScreenWndPos( ptWndPos.x, ptWndPos.y );
	m_ptDragMovePos = ptMousePos;

	ptWndPos.x = m_pCurDragMoveWnd->GetDerivedPosX();
	ptWndPos.y = m_pCurDragMoveWnd->GetDerivedPosY();

	if( m_pCurDragMoveWnd->HasExWndStyle( IWS_EX_NOTIFY_MOVED ) )
	{
		ioWnd *pParent = m_pCurDragMoveWnd->GetParent();
		if( pParent )
		{
			pParent->iwm_command( m_pCurDragMoveWnd,
								  IOWN_MOVED,
								  (DWORD)&ptWndPos );
		}
	}
}

void ioGUIManager::ChangeMouseStateToOver( ioWnd *pWnd, ioMouse &mouse )
{
	switch( pWnd->GetWndType() )
	{
	case IWT_BUTTON:
	case IWT_RADIO_BTN:
	case IWT_SLIDE:
	case IWT_CHECK_BOX:
	case IWT_TAB_BTN:
	case IWT_CUSTOM_BTN:
		mouse.SetMouseState( ioMouse::MS_OVER );
		break;
	default:
		mouse.SetMouseState( ioMouse::MS_NORMAL );
		break;
	}
}

bool ioGUIManager::CheckMouse( ioWnd *pWnd, ioMouse &mouse )
{
	if( !pWnd || !pWnd->IsCanMouseReaction() )
		return false;

	bool bInRect = pWnd->IsInWndRect( mouse.GetMousePos() );
	if( bInRect || pWnd->IsCheckParentOutSide() )
	{
		if( pWnd->HasChild() )
		{
			// Check과정에서 m_ChildList의 push, remove가 이루어지기때문에 참조가 아닌 복사를 쓴다.
			std::vector< ioWnd* > vTempList;
			vTempList.reserve( pWnd->m_ChildList.size() );
			std::copy( pWnd->m_ChildList.rbegin(), pWnd->m_ChildList.rend(), std::back_inserter( vTempList ) );

			std::vector< ioWnd* >::iterator iter=vTempList.begin();
			for( ; iter!=vTempList.end() ; ++iter )
			{
				if( CheckMouse( *iter, mouse ) )
					return true;
			}
		}
	}

	if( bInRect && !pWnd->HasWndStyle( IWS_CHILD_ONLY_RESPONSE ) )
	{
		InTargetWndProcess( pWnd, mouse );
		return true;
	}

	return false;
}

ioWnd* ioGUIManager::CheckMouse( ioWnd *pWnd, const POINT& pos )
{
	if ( !pWnd ) {
		return 0;
	}

	if ( pWnd->HasChild() )
	{
		std::vector< ioWnd* > tmplist;
		tmplist.reserve( pWnd->m_ChildList.size() );
		std::copy( pWnd->m_ChildList.rbegin(), pWnd->m_ChildList.rend(), std::back_inserter( tmplist ) );

		std::vector< ioWnd* >::iterator iter = tmplist.begin();
		for ( ; iter != tmplist.end(); ++iter )
		{
			ioWnd* wnd = CheckMouse( *iter, pos );
			if ( wnd ) {
				return wnd;
			}
		}
	}

	if ( pWnd->IsShow() && pWnd->IsInWndRect( pos ) && pWnd != m_pDeskTopWnd ) {
		return pWnd;
	}

	return 0;
}

void ioGUIManager::CheckMouse( ioWnd *pWnd, const RECT& rt, ioWnd** wins, int& index )
{
	if ( !pWnd || !wins ) {
		return;
	}

	if ( pWnd->HasChild() )
	{
		std::vector< ioWnd* > tmplist;
		tmplist.reserve( pWnd->m_ChildList.size() );
		std::copy( pWnd->m_ChildList.rbegin(), pWnd->m_ChildList.rend(), std::back_inserter( tmplist ) );

		std::vector< ioWnd* >::iterator iter = tmplist.begin();
		for ( ; iter != tmplist.end(); ++iter ) {
			CheckMouse( *iter, rt, wins, index );
		}
	}

	if ( pWnd->IsShow() && pWnd->IsInWndRect( rt ) && pWnd != m_pDeskTopWnd ) {
		wins[ index++ ] = pWnd;
	}
}

void ioGUIManager::InTargetWndProcess( ioWnd *pWnd, ioMouse& mouse )
{
	POINT ptMouse = mouse.GetMousePos();

	if( m_pPreOverWnd != pWnd )
	{
		if( m_pPreOverWnd )
		{
			m_pPreOverWnd->iwm_mouseleave( mouse );
		}

		pWnd->iwm_mouseover( mouse );
		m_pPreOverWnd = pWnd;

		ChangeMouseStateToOver( pWnd, mouse );
	}
	else if ( pWnd )
	{
		pWnd->iwm_mousemove( mouse );
	}

	if( mouse.IsLBtnDBLCLK() )
	{
		pWnd->iwm_lbuttonDBLCLK( mouse );
	}

	if( mouse.IsLBtnDown() )
	{
		//윤태철 ...  채팅창 포커스 때문에 막..막 추가..ㅈㅅ
		if( !pWnd->m_bClicked && !m_pPreClickWnd )
		{
			pWnd->iwm_lbuttondown( mouse );
			
			if( !pWnd->IsFocusRectTypeIsParent() )
			{
				if( pWnd->IsInHitRect( ptMouse ) )
				{
					m_pCurDragMoveWnd = pWnd;
					m_ptDragMovePos = ptMouse;
				}
			}
			else
			{
				ioWnd *pParentWnd = pWnd->GetParent();
				ioWnd *pGrandParent = pParentWnd->GetParent();
				if( pParentWnd && pParentWnd->IsInHitRect( ptMouse ) )
				{
					m_pCurDragMoveWnd = pParentWnd;
					m_ptDragMovePos = ptMouse;
				}
				else if( pGrandParent && pGrandParent->IsInHitRect( ptMouse ) )
				{
					m_pCurDragMoveWnd = pGrandParent;
					m_ptDragMovePos = ptMouse;
				}
			}

			m_pPreClickWnd = pWnd;
			pWnd->GoToTop();
		}
	}
	else
	{
		if( pWnd->m_bClicked && m_pPreClickWnd == pWnd )
		{
			pWnd->iwm_lbuttonup( mouse );
		}

		m_pPreClickWnd = NULL;
		m_pCurDragMoveWnd  = NULL;
	}

	if( mouse.IsRBtnDown() )
	{
		if( !pWnd->m_bRightClicked && !m_pPreRightClickWnd )
		{
			pWnd->iwm_rbuttondown( mouse );

			m_pPreRightClickWnd = pWnd;
			pWnd->GoToTop();
		}
	}
	else
	{
		if( pWnd->m_bRightClicked && m_pPreRightClickWnd == pWnd )
		{
			pWnd->iwm_rbuttonup( mouse );
		}

		m_pPreRightClickWnd = NULL;
	}

	if( mouse.GetWheelDelta() != 0 )
	{
		pWnd->iwm_wheel( mouse.GetWheelDelta() );
	}
}


void ioGUIManager::SetCurDragMoveWnd( ioWnd *pWnd, POINT ptMouse )
{
	if( pWnd )
	{
		m_pCurDragMoveWnd = pWnd;
		m_pPreClickWnd = pWnd;

		m_ptDragMovePos = ptMouse;
	}
}

bool ioGUIManager::CheckMouseModal( ioWnd *pWnd, ioMouse &mouse )
{
	if( !pWnd || !pWnd->IsCanMouseReaction() )
		return false;

	bool bInRect = pWnd->IsInWndRect( mouse.GetMousePos() );
	if( bInRect || pWnd->IsCheckParentOutSide() )
	{
		if( pWnd->HasChild() )
		{
			// Check과정에서 m_ChildList의 push, remove가 이루어지기때문에 참조가 아닌 복사를 쓴다.
			std::vector< ioWnd* > vTempList;
			vTempList.reserve( pWnd->m_ChildList.size() );
			std::copy( pWnd->m_ChildList.rbegin(), pWnd->m_ChildList.rend(), std::back_inserter( vTempList ) );

			std::vector< ioWnd* >::iterator iter=vTempList.begin();
			for( ; iter!=vTempList.end() ; ++iter )
			{
				if( CheckMouseModal( *iter, mouse ) )
					return true;
			}
		}
	}

	if( bInRect && !pWnd->HasWndStyle( IWS_CHILD_ONLY_RESPONSE ) )
	{
		InTargetWndProcess( pWnd, mouse );
		return true;
	}

	// 모달윈도우 바깥쪽이 클릭된 경우
	if( pWnd->HasWndStyle( IWS_MODAL ) )
	{
		if( !mouse.IsLBtnDown() )
		{
			m_pPreClickWnd = NULL;
			m_pCurDragMoveWnd = NULL;
		}
//		else
//		{
//			// Modal Error...
//		}

		if( m_pPreOverWnd )
		{
			POINT ptMouse = mouse.GetMousePos();
			m_pPreOverWnd->iwm_mouseleave( mouse );
			m_pPreOverWnd = NULL;
		}
	}

	return false;
}

bool ioGUIManager::CheckDragStart( ioMouse &mouse )
{
	if( !mouse.IsLBtnDown() )
	{
		ClearDragState();
		return false;
	}

	bool bDrag = false;

	POINT vPos = mouse.GetMousePos();
	if( !PtInRect( &m_rcStartDrag, vPos ) )
		bDrag = true;

	if( !PtInRect( &m_rcDragCheck, vPos ) )
		bDrag = true;

	if( m_dwDragCheckStartTime + m_dwDragCheckDuration < REALGETTIME() )
		bDrag = true;

	if( bDrag )
	{
		mouse.SetDragDropStart( m_pDragItem, m_ptDragOffset );
		m_DragCheckState = DCS_DRAGGING;
	}

	return bDrag;
}

bool ioGUIManager::CheckDragMouse( ioWnd *pWnd, ioMouse &mouse )
{
	if( !pWnd->IsCanMouseReaction() )
		return false;

	if( !pWnd->HasWndStyle( IWS_DRAG_DROP ) )
		return false;

	POINT ptCheck = GetDragCheckPoint( mouse );
	if( !pWnd->IsInWndRect( ptCheck ) )
		return false;

	if( pWnd->HasChild() )
	{
		// Check과정에서 m_ChildList의 push, remove가 이루어지기때문에 참조가 아닌 복사를 쓴다.
		std::vector< ioWnd* > vTempList;
		vTempList.reserve( pWnd->m_ChildList.size() );
		std::copy( pWnd->m_ChildList.rbegin(), pWnd->m_ChildList.rend(), std::back_inserter( vTempList ) );

		std::vector< ioWnd* >::iterator iter=vTempList.begin();
		for( ; iter!=vTempList.end() ; ++iter )
		{
			if( CheckDragMouse( *iter, mouse ) )
				return true;
		}
	}

	if( mouse.IsLBtnDown() )
	{
		if( m_pPreDragOverWnd != pWnd )
		{
			if( m_pPreDragOverWnd )
			{
				m_pPreDragOverWnd->iwm_dragleave();
			}

			IWDropEffect eEffect = pWnd->iwm_dragenter( m_pDragItem, mouse );
			m_pPreDragOverWnd = pWnd;
			
			if( eEffect == IW_DROP_NONE )
			{
				mouse.SetMouseState( ioMouse::MS_DROP_DISABLE );
			}
			else if( eEffect == IW_DROP_ENABLE )
			{
				mouse.SetMouseState( ioMouse::MS_DROP_ENABLE );
			}
		}
		else
		{
			if( mouse.IsMoved() )
			{
				IWDropEffect eEffect = pWnd->iwm_dragover( m_pDragItem, mouse );
				if( eEffect == IW_DROP_NONE )
				{
					mouse.SetMouseState( ioMouse::MS_DROP_DISABLE );
				}
				else if( eEffect == IW_DROP_ENABLE )
				{
					mouse.SetMouseState( ioMouse::MS_DROP_ENABLE );
				}
			}
		}
	}
	else
	{
		bool bDropped = pWnd->iwm_dropitem( m_pDragItem, mouse, m_pDragStartWnd );
		if( m_pDragStartWnd )
		{
			m_pDragStartWnd->iwm_dropdone( m_pDragItem, pWnd, bDropped );
		}
		//HARDCODE : 이 이후로는 m_pDragItem의 메모리를 보장할수 없다.
		//			 iwm_dropdone에서 지워졌을 가능성이 크다.

		mouse.ClearDragDropState();
		ClearDragState();
	}

	if( mouse.GetWheelDelta() != 0 )
	{
		pWnd->iwm_wheel( mouse.GetWheelDelta() );
	}

	return true;
}

bool ioGUIManager::CheckEditFocus(ioMouse &mouse )
{
	if( !mouse.IsLBtnDown() ) return false;
	if( !ioEdit::m_pKeyFocusEdit ) return false;
	if( !ioEdit::m_pKeyFocusEdit->HasExWndStyle( IWS_EX_EDIT_CHECK_FOCUS ) ) return false;
	
	if( m_pPreClickWnd )
	{
		if( m_pPreClickWnd->GetHighParentID() == ioEdit::m_pKeyFocusEdit->GetHighParentID() )
			return false;
	}
	ioEdit::m_pKeyFocusEdit->KillKeyFocus();
	return true;	
}

void ioGUIManager::ProcessGUI( ioMouse& mouse )
{
	if( m_pDeskTopWnd )
	{
		m_pDeskTopWnd->ProcessWnd( g_FrameTimer.GetSecPerFrame(), mouse );
	}
	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		if(m_vToolTipWnd[i])
		{
			m_vToolTipWnd[i]->ProcessWnd( g_FrameTimer.GetSecPerFrame(), mouse );
		}
	}
}

void ioGUIManager::RenderGUI()
{
	if( m_pDeskTopWnd )
	{
		m_pDeskTopWnd->RenderWnd();
	}
}

void ioGUIManager::RenderToolTip()
{
	for(int i = 0; i < (int)m_vToolTipWnd.size(); ++i)
	{
		if(m_vToolTipWnd[i])
		{
			m_vToolTipWnd[i]->RenderWnd();
		}
	}
}

void ioGUIManager::AddDeskTopChild( ioWnd *pWnd )
{
	if( m_pDeskTopWnd && pWnd )
	{		
		if(	!m_pDeskTopWnd->FindChildWnd( pWnd->GetID() ) )
			m_pDeskTopWnd->AddChild( pWnd );
	}
}

ioWnd* ioGUIManager::AddWnd( const char *szXMLFileName )
{
	return AddWnd( szXMLFileName, m_pDeskTopWnd );
}

ioWnd* ioGUIManager::AddWnd( const char *szXMLFileName, ioWnd *pParent )
{
	if( !pParent )
	{
		LOG.PrintTimeAndLog( 0, "ioGUIManager::AddWnd - %s Parent is NULL", szXMLFileName );
		return NULL;
	}

	ioTextStream kStream;
	if( !g_ResourceLoader.LoadStream( szXMLFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioGUIManager::AddWnd - %s Load Failed", szXMLFileName );
		return NULL;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( (const char*)kStream.GetPtr() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioGUIManager::AddWnd - %s LoadFromMemory Failed", szXMLFileName );
		return NULL;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioGUIManager::AddWnd - %s, RootElement is NULL", szXMLFileName );
		return NULL;
	}

	AddMem( szXMLFileName, kStream.GetSize() );

#ifdef STRING_MANAGER 
	char szOnlyFileName[MAX_PATH]="";
	_splitpath( szXMLFileName, NULL, NULL, szOnlyFileName, NULL );
	m_szStringMgrKeyName = szOnlyFileName;
#endif 

	return LoadWnd( xRootElement, pParent );
}

ioWnd* ioGUIManager::AddWndByTemplate( ioHashString &szClassName, ioWnd *pParent, DWORD dwTPID, DWORD dwTemplateID /* = -1 */ )
{
	ioXMLElement* pElement = pParent->FindTemplate( szClassName, dwTemplateID );

	if( !pElement || !pElement->IsTagRight( "Template" ) )
		return NULL;

	ioWnd *pNewWnd = CreateNewWindow( szClassName );
	if( pNewWnd )
	{
		pNewWnd->SetID( dwTPID );
		pNewWnd->SetClassName( szClassName.c_str() );
		pNewWnd->SetInnerUniqueIndex( ++m_dwInnerUniqueIndexCounter );
		pNewWnd->SetTemplateChild( true );

		pParent->AddChild( pNewWnd );
		

		ioXMLElement xProperty = pElement->FirstChild();
		while( !xProperty.IsEmpty() )
		{
			LoadProperty( xProperty, pNewWnd );

			xProperty = xProperty.NextSibling();
		}

		pNewWnd->iwm_create();

		if( !pNewWnd->HasWndStyle( IWS_START_HIDE ) )
		{
			pNewWnd->ShowWnd();
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioGUIManager::LoadWnd - %s Unknown ClassName", szClassName.c_str() );
	}

	return pNewWnd;
}

ioWnd* ioGUIManager::LoadWnd( ioXMLElement &xElement, ioWnd *pParent )
{
	if( !xElement.IsTagRight( "Window" ) )
		return NULL;

	ioHashString szClassName = xElement.GetStringAttribute( "ClassName" );
	ioHashString szSubClassName = xElement.GetStringAttribute( "SubClassName" );
	DWORD dwID = (DWORD)xElement.GetIntAttribute( "Id" );

	ioWnd *pNewWnd = CreateNewWindow( szClassName );
	if ( !pNewWnd ) {
		pNewWnd = CreateNewWindowEX( szSubClassName );
	}

	if( pNewWnd )
	{
		pNewWnd->SetID( dwID );
		pNewWnd->SetClassName( szClassName.c_str() );
		pNewWnd->SetSubClassName( szSubClassName.c_str() );
		pNewWnd->SetInnerUniqueIndex( ++m_dwInnerUniqueIndexCounter );

		if(IsToolTipWnd(pNewWnd, pParent))
		{	
			m_vToolTipWnd.push_back(pNewWnd);
		}
		else
		{
			if( pParent )
		pParent->AddChild( pNewWnd );
		}

		//! tool에서 custom wnd를 위해서 추가된 처리
		pNewWnd->ParseXML( xElement );
		pNewWnd->SetIDName( xElement.GetStringAttribute( "IDName" ) );
		pNewWnd->SetMemberName( xElement.GetStringAttribute( "MemberName" ) );

		DWORD dwLayer = (DWORD)xElement.GetIntAttribute( "Layer" );
		pNewWnd->SetLayer( dwLayer );

		ioXMLElement xProperty = xElement.FirstChild();
		while( !xProperty.IsEmpty() )
		{
			LoadProperty( xProperty, pNewWnd );

			xProperty = xProperty.NextSibling();
		}

		pNewWnd->iwm_create();
		NotifyLoadWnd( pNewWnd );

		if( !pNewWnd->HasWndStyle( IWS_START_HIDE ) )
		{
			pNewWnd->ShowWnd();
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioGUIManager::LoadWnd - %s Unknown ClassName", szClassName.c_str() );
	}

	return pNewWnd;
}

void ioGUIManager::LoadTemplate( ioXMLElement &xElement, ioWnd *pParent )
{
	if( !xElement.IsTagRight( "Template" ) )
		return;

	ioXMLElement* pNewElement = new ioXMLElement;
	pNewElement->Copy( xElement );
	pParent->AddTemplate( pNewElement );
}

ioWnd* ioGUIManager::CreateNewWindowEX( const ioHashString &szClassName )
{
	ioWnd* wnd = CreateNewWindow( szClassName );
#ifndef _DEBUG
	if ( !wnd ) {
		return new ioCustomWnd;
	}
#endif
	return wnd;
}

ioWnd* ioGUIManager::CreateNewWindow( const ioHashString &szClassName )
{
	ioWnd *pNewWnd = NULL;

	if( szClassName == "ioWnd" )
		return new ioWnd;
	else if( szClassName == "ioButton" )
		return new ioButton;
	else if( szClassName == "ioScreenButton" )
		return new ioScreenButton;
	else if( szClassName == "ioConvexButton" )
		return new ioConvexButton;
	else if( szClassName == "ioScaleButton" )
		return new ioScaleButton;
	else if( szClassName == "ioCheckButton" )
		return new ioCheckButton;
	else if( szClassName == "ioRadioButton" )
		return new ioRadioButton;
	else if( szClassName == "ioStretchButton" )
		return new ioButton;
	else if( szClassName == "ioActiveScreenBtn" )
		return new ioActiveScreenBtn;
	else if( szClassName == "ioEdit" )
		return new ioEdit;
	else if( szClassName == "ioProgressBar" )
		return new ioProgressBar;
	else if( szClassName == "ioScroll" )
		return new ioScroll;
	else if( szClassName == "ioSlide" )
		return new ioSlide;
	else if ( szClassName == "ioToggleButton" )
		return new ioToggleBtn;
	else if ( szClassName == "ioWndEX" )
		return new ioWndEX;
	else if ( szClassName == "ioFrameWndEX" )
		return new ioFrameWndEX( this );
	else if ( szClassName == "ioImageWndEX" )
		return new ioImageWndEX( this );
	else if ( szClassName == "ioLabelWndEX" )
		return new ioLabelWndEX;
	else if ( szClassName == "ioFrameButtonWndEX" )
		return new ioFrameButtonWndEX( this );
	else if ( szClassName == "ioImageButtonWndEX" )
		return new ioImageButtonWndEX( this );
	else if ( szClassName == "ioCheckBoxEX" )
		return new ioCheckBoxEX( this );
	else if ( szClassName == "ioFlashPlayer" )
		return new ioFlashPlayer();
	else if ( szClassName == "ioTabFrameButtonEX" )
		return new ioTabFrameButtonEX( this );
	else if ( szClassName == "ioTabWndEX" )
		return new ioTabWndEX( this );
	else if ( szClassName == "ioScrollBarEX" )
		return new ioScrollBarEX( this );
	else if ( szClassName == "ioRichLabel" )
		return new ioRichLabel();
	else if ( szClassName == "ioRadioWndEX" )
		return new ioRadioWndEX();
	else if ( szClassName == "ioRadioFrameButtonEX" )
		return new ioRadioFrameButtonEX( this );
	else if ( szClassName == "ioMovingWnd" )
		return new ioMovingWnd;
	else if ( szClassName == "ioMovingBtn" )
		return new ioMovingBtn;
	else if ( szClassName == "ioMovingNormalBtn" )
		return new ioMovingNormalBtn;

	return NULL;
}

void ioGUIManager::LoadProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	if( !pWnd )	return;

	ioHashString szTagName = xElement.GetTagName();
	if( szTagName == "Title" )
	{
		OnTitleProperty( xElement, pWnd, m_szStringMgrKeyName );
	}
	else if( szTagName == "Tooltip" )
	{
		OnTooltipProperty( xElement, pWnd, m_szStringMgrKeyName );
	}
	else if( szTagName == "Size" )
	{
		OnSizeProperty( xElement, pWnd );
	}
	else if( szTagName == "Style" )
	{
		OnStyleProperty( xElement, pWnd );
	}
	else if( szTagName == "ExStyle" )
	{
		OnExStyleProperty( xElement, pWnd );
	}
	else if( szTagName == "DragRect" )
	{
		OnDragRectProperty( xElement, pWnd );
	}
	else if( szTagName == "ExtraInfo" )
	{
#ifdef STRING_MANAGER 
		xElement.SetUseStringMgr( true );
		xElement.SetStringMgrKeyName( m_szStringMgrKeyName.c_str() );
#endif 

		pWnd->ParseExtraInfo( xElement );

#ifdef STRING_MANAGER 
		xElement.SetUseStringMgr( false );
#endif 
	}
	else if( szTagName == "Imagery" )
	{
		OnImageryProperty( xElement, pWnd );
	}	
	else if( szTagName == "Window" )
	{
		LoadWnd( xElement, pWnd );
	}
	else if( szTagName == "Text")
	{
		OnTextProperty( xElement, pWnd );
	}
	else if( szTagName == "Template" )
	{
		LoadTemplate( xElement, pWnd );
	}
}

void ioGUIManager::OnTitleProperty( ioXMLElement &xElement, ioWnd *pWnd, const ioHashString& szStrMgrKeyName )
{
	ioUITitle *pTitle = pWnd->CreateUITitle();

	ioHashString szTagName;
	ioXMLElement xCurElement = xElement.FirstChild();
	while( !xCurElement.IsEmpty() )
	{
		szTagName = xCurElement.GetTagName();

		if( szTagName == "Format" )
		{
			int iXPos, iYPos, iSize;
			iXPos = xCurElement.GetIntAttribute( "X" );
			iYPos = xCurElement.GetIntAttribute( "Y" );
			pTitle->SetOffset( iXPos, iYPos );

			iSize = xCurElement.GetIntAttribute( "Size" );
			pTitle->SetSize( iSize );
			
			iSize = xCurElement.GetIntAttribute( "VertGap" );
			pTitle->SetVertGap( iSize );

			ioHashString szStyle, szAlign;
			szStyle = xCurElement.GetStringAttribute( "Style" );
			pTitle->SetStyle( szStyle );

			szStyle = xCurElement.GetStringAttribute( "DisableStyle" );
			pTitle->SetDisableStyle( szStyle );

			szAlign = xCurElement.GetStringAttribute( "HorzAlign" );
			pTitle->SetHorzAlign( szAlign );

			szAlign = xCurElement.GetStringAttribute( "VertAlign" );
			pTitle->SetVertAlign( szAlign );
		}
		else if( szTagName == "Color" )
		{
			ioHashString szType, szTextColor, szBkColor;
			szType = xCurElement.GetStringAttribute( "Type" );
			szTextColor = xCurElement.GetStringAttribute( "Text" );
			szBkColor = xCurElement.GetStringAttribute( "Back" );
			pTitle->SetColor( szType, szTextColor, szBkColor );
		}
		else if( szTagName == "Text" )
		{
#ifdef STRING_MANAGER 
			if( ioStringManager::GetSingletonPtr() )
				pTitle->SetText( g_StringMgr.GetStringXML( szStrMgrKeyName.c_str(), xCurElement.GetText() ) , "#" );
			else
				pTitle->SetText( xCurElement.GetText(), "#" );
#else 
			pTitle->SetText( xCurElement.GetText(), "#" );
#endif 
		}

		xCurElement = xCurElement.NextSibling();
	}
}

void ioGUIManager::OnTextProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	IoUIText *pText = pWnd->CreateUIText();

	ioHashString szTagName;
	ioXMLElement xCurElement = xElement.FirstChild();
	while( !xCurElement.IsEmpty() )
	{
		szTagName = xCurElement.GetTagName();

		if( szTagName == "Format" )
		{
			pText->SetName(xCurElement.GetStringAttribute( "Type" ));

			int iXPos, iYPos, iSize;
			iXPos = xCurElement.GetIntAttribute( "X" );
			iYPos = xCurElement.GetIntAttribute( "Y" );
			pText->SetOffset( iXPos, iYPos );

			iSize = xCurElement.GetIntAttribute( "Size" );
			pText->SetSize( iSize );

			iSize = xCurElement.GetIntAttribute( "VertGap" );
			pText->SetVertGap( iSize );

			ioHashString szStyle, szAlign;
			szStyle = xCurElement.GetStringAttribute( "Style" );
			pText->SetStyle( szStyle );

			szStyle = xCurElement.GetStringAttribute( "DisableStyle" );
			pText->SetDisableStyle( szStyle );

			szAlign = xCurElement.GetStringAttribute( "HorzAlign" );
			pText->SetHorzAlign( szAlign );

			szAlign = xCurElement.GetStringAttribute( "VertAlign" );
			pText->SetVertAlign( szAlign );

			szAlign = xCurElement.GetStringAttribute( "WidthCut" );
            if (!szAlign.IsEmpty())
            {
                pText->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
			    pText->SetWidthCutSize( szAlign );
            }

			ioHashString szRenderPos;
			szRenderPos = xCurElement.GetStringAttribute( "RenderPos" );
			pText->SetParserRenderPos( szRenderPos );
		}
		else if( szTagName == "Color" )
		{
			ioHashString szTextColor, szBkColor;
			szTextColor = xCurElement.GetStringAttribute( "Text" );
			szBkColor = xCurElement.GetStringAttribute( "Back" );
			pText->SetAllColor( ioStringConverter::ParseColor( szTextColor ), ioStringConverter::ParseColor( szBkColor ));
		}
		else if( szTagName == "Text" )
		{
#ifdef STRING_MANAGER 
			if( ioStringManager::GetSingletonPtr() )
				pText->SetText( g_StringMgr.GetStringXML( m_szStringMgrKeyName.c_str(), xCurElement.GetText() ) , "#" );
			else
				pText->SetText( xCurElement.GetText(), "#" );
#else 
			pText->SetText( xCurElement.GetText(), "#" );
#endif 
		}
		else if( szTagName == "Type")
		{
		}

		xCurElement = xCurElement.NextSibling();
	}
}

void ioGUIManager::OnTooltipProperty( ioXMLElement &xElement, ioWnd *pWnd, const ioHashString& szStrMgrKeyName )
{
	ioXMLElement xCurElement = xElement.FirstChild();
	while( !xCurElement.IsEmpty() )
	{
		ioHashString szTagName = xCurElement.GetTagName();
		if( szTagName == "Help" )
		{
			ioHashString szTipHelp, szTextAlign, szTextColor;
#ifdef STRING_MANAGER 
			if( ioStringManager::GetSingletonPtr() )
				szTipHelp   =  g_StringMgr.GetStringXML( szStrMgrKeyName.c_str(), xCurElement.GetStringAttribute( "Text" ) );
			else
				szTipHelp   =  xCurElement.GetStringAttribute( "Text" );
#else 
			szTipHelp   =  xCurElement.GetStringAttribute( "Text" );
#endif
			szTextAlign = xCurElement.GetStringAttribute( "Align" );
			szTextColor = xCurElement.GetStringAttribute( "TextColor" );
			pWnd->AddTooltipHelp( szTipHelp, szTextAlign, szTextColor );
		}
		xCurElement = xCurElement.NextSibling();
	}
}

void ioGUIManager::OnSizeProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	pWnd->SetDefWindowPos( xElement );

	ioWnd *pParent = pWnd->GetParent();
	if( pParent )
	{
		pWnd->SetPositionToDefault( pParent->GetWidth(), pParent->GetHeight(), false, true );
	}
	else
	{
		pWnd->SetPositionToDefault( 0, 0, false, true );
	}
}

void ioGUIManager::OnStyleProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	std::string szStyle( xElement.GetText() );
	const StringVector &vStyleList = ioStringConverter::Split( szStyle, "|" );

	DWORD dwStyle = 0;

	StringVector::const_iterator iter;
	for( iter=vStyleList.begin() ; iter!=vStyleList.end() ; ++iter )
	{
		const std::string &szSplitStyle = *iter;
		
		if( szSplitStyle == "IWS_START_HIDE" )
			dwStyle |= IWS_START_HIDE;
		else if( szSplitStyle == "IWS_INACTIVE" )
			dwStyle |= IWS_INACTIVE;
		else if( szSplitStyle == "IWS_BACKMOST" )
			dwStyle |= IWS_BACKMOST;
		else if( szSplitStyle == "IWS_CANMOVE" )
			dwStyle |= IWS_CANMOVE;
		else if( szSplitStyle == "IWS_MODAL" )
			dwStyle |= IWS_MODAL;
		else if( szSplitStyle == "IWS_EXIT_ESC" )
			dwStyle |= IWS_EXIT_ESC;
		else if( szSplitStyle == "IWS_PROCESS" )
			dwStyle |= IWS_PROCESS;
		else if( szSplitStyle == "IWS_DRAG_DROP" )
			dwStyle |= IWS_DRAG_DROP;
		else if( szSplitStyle == "IWS_NO_MOUSE_RESPONSE" )
			dwStyle |= IWS_NO_MOUSE_RESPONSE;
		else if( szSplitStyle == "IWS_CHILD_ONLY_RESPONSE" )
			dwStyle |= IWS_CHILD_ONLY_RESPONSE;
		else if( szSplitStyle == "IWS_TOPMOST" )
			dwStyle |= IWS_TOPMOST;
		else if( szSplitStyle == "IWS_EXACTIVE" )
			dwStyle |= IWS_EXACTIVE;
		else if( szSplitStyle == "IWS_MODAL_TOOLTIP" )
			dwStyle |= IWS_MODAL_TOOLTIP;
		else if( szSplitStyle == "IWS_MODAL_EXTEND" )
			dwStyle |= IWS_MODAL_EXTEND;
		else if( szSplitStyle == "IWS_ISINRECT_CENTER" )
			dwStyle |= IWS_ISINRECT_CENTER;
	}

	pWnd->SetWndStyle( dwStyle );
}

void ioGUIManager::OnExStyleProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	std::string szExStyle( xElement.GetText() );
	const StringVector &vExStyleList = ioStringConverter::Split( szExStyle, "|" );

	DWORD dwExStyle = 0;

	StringVector::const_iterator iter;
	for( iter=vExStyleList.begin() ; iter!=vExStyleList.end() ; ++iter )
	{
		const std::string& szSplitStyle = *iter;
		std::string szExStyle;
		std::string szParam;

		OnExStyleParameter( szSplitStyle, szExStyle, szParam );
		OnExStyleParameterProperty( szParam, pWnd );
		
		if( szExStyle == "IWS_EX_EDIT_PW" )
			dwExStyle |= IWS_EX_EDIT_PW;
		else if( szExStyle == "IWS_EX_EDIT_RIGHT" )
			dwExStyle |= IWS_EX_EDIT_RIGHT;
		else if( szExStyle == "IWS_EX_EDIT_CENTER" )
			dwExStyle |= IWS_EX_EDIT_CENTER;
		else if( szExStyle == "IWS_EX_MULTI_EDIT" )
			dwExStyle |= IWS_EX_MULTI_EDIT;
		else if( szExStyle == "IWS_EX_NOTIFY_MOVED" )
			dwExStyle |= IWS_EX_NOTIFY_MOVED;		
		else if( szExStyle == "IWS_EX_OVER_SCREEN" )
			dwExStyle |= IWS_EX_OVER_SCREEN;		
		else if( szExStyle == "IWS_EX_PUSH_SCREEN" )
			dwExStyle |= IWS_EX_PUSH_SCREEN;
		else if( szExStyle == "IWS_EX_HSCROLL" )
			dwExStyle |= IWS_EX_HSCROLL;		
		else if( szExStyle == "IWS_EX_VSCROLL" )
			dwExStyle |= IWS_EX_VSCROLL;
		else if( szExStyle == "IWS_EX_PUSH_DOUBLE" )
			dwExStyle |= IWS_EX_PUSH_DOUBLE;
		else if( szExStyle == "IWS_EX_EDIT_NUM" )
			dwExStyle |= IWS_EX_EDIT_NUM;
		else if( szExStyle == "IWS_EX_EDIT_CHECK_FOCUS" )
			dwExStyle |= IWS_EX_EDIT_CHECK_FOCUS;
		else if( szExStyle == "IWS_EX_PUSHED_OVER_SCREEN")
			dwExStyle |= IWS_EX_PUSHED_OVER_SCREEN;
		else if( szExStyle == "IWS_EX_SHOW_NO_TOP" )
			dwExStyle |= IWS_EX_SHOW_NO_TOP;
		else if( szExStyle == "IWS_EX_SHOW_HIDE_ACTION" )
			dwExStyle |= IWS_EX_SHOW_HIDE_ACTION;
		else if( szExStyle == "IWS_EX_NO_REND_EDIT_NUM" )
			dwExStyle |= IWS_EX_NO_REND_EDIT_NUM;
	}

	pWnd->SetWndExStyle( dwExStyle );
}

void ioGUIManager::OnExStyleParameter( const std::string& szText, OUT std::string& szExStyle, OUT std::string& szParam )
{
	StringVector vExParamList;
	ioStringConverter::Split( vExParamList, szText, "=", 2 );
	if( static_cast<int>( vExParamList.size() ) == 2 )
	{
		szExStyle = vExParamList[0];
		szParam   = vExParamList[1];
	}
	else
	{
		szExStyle = szText;
	}
}

void ioGUIManager::OnExStyleParameterProperty( const std::string& szParam, ioWnd *pWnd )
{
	if( szParam.empty() )
		return;

	DWORD dwExParam = 0;
	if( szParam == "EXWP_SCALE_TOP" )
		dwExParam |= EXWP_SCALE_TOP;
	else if( szParam =="EXWP_SCALE_DOWN" )
		dwExParam |= EXWP_SCALE_DOWN;

	pWnd->SetWndExStyleParam( dwExParam );
}

void ioGUIManager::OnDragRectProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	RECT rcDrag;
	rcDrag.left   = xElement.GetIntAttribute( "Left" );
	rcDrag.top    = xElement.GetIntAttribute( "Top" );
	rcDrag.right  = xElement.GetIntAttribute( "Right" );
	rcDrag.bottom = xElement.GetIntAttribute( "Bottom" );

	pWnd->SetDragRect( rcDrag );
}

void ioGUIManager::OnImageryProperty( ioXMLElement &xElement, ioWnd *pWnd )
{
	ioXMLElement xCurElement = xElement.FirstChild();
	while( !xCurElement.IsEmpty() )
	{
		ioHashString szTagName = xCurElement.GetTagName();
		if( szTagName == "DefaultFrameImagery" )
		{
			ioHashString szType = xCurElement.GetStringAttribute( "Type" );
			ioHashString szFrameName = xCurElement.GetStringAttribute( "Templete" );

			ioUIRenderFrame *pFrame = m_pUIFrameMgr->CloneFrame( szFrameName );
			if( pFrame )
			{
				ioHashString szColor = xCurElement.GetStringAttribute( "Color" );
				if( !szColor.IsEmpty() )
				{
					DWORD dwColor = ioStringConverter::ParseColor( szColor );
					pFrame->SetColor( dwColor );
				}

				ioXMLElement xArea = xCurElement.FirstChild( "Area" );
				if( !xArea.IsEmpty() )
				{
					OnFrameArea( xArea, pFrame );
				}

				xCurElement.GetIntAttribute( "RenderType" );

				pFrame->SetRenderType( static_cast<UIRenderType>( xCurElement.GetIntAttribute( "RenderType" ) ) );
				pFrame->SetTextureFilterOption( static_cast<TextureFilterOption>( xCurElement.GetIntAttribute( "TextureFilterOption" ) ) );
				pFrame->SetGrayRenderType( static_cast<UIRenderType>( xCurElement.GetIntAttribute( "GrayRenderType" ) ) );

				pWnd->AddRenderFrame( szType, pFrame );
				NotifyLoadFrame( pFrame, szType, pWnd );
			}
		}
		else if( szTagName == "FrameImagery" )
		{
			ioHashString szType = xCurElement.GetStringAttribute( "Type" );

			ioUIRenderFrame *pFrame = m_pUIFrameMgr->CreateFrame( xCurElement );
			if( pFrame )
			{
				ioHashString szColor = xCurElement.GetStringAttribute( "Color" );
				if( !szColor.IsEmpty() )
				{
					DWORD dwColor = ioStringConverter::ParseColor( szColor );
					pFrame->SetColor( dwColor );
				}

				ioXMLElement xArea = xCurElement.FirstChild( "Area" );
				if( !xArea.IsEmpty() )
				{
					OnFrameArea( xArea, pFrame );
				}

				pFrame->SetRenderType( static_cast<UIRenderType>( xCurElement.GetIntAttribute( "RenderType" ) ) );
				pFrame->SetTextureFilterOption( static_cast<TextureFilterOption>( xCurElement.GetIntAttribute( "TextureFilterOption" ) ) );
				pFrame->SetGrayRenderType( static_cast<UIRenderType>( xCurElement.GetIntAttribute( "GrayRenderType" ) ) );

				pWnd->AddRenderFrame( szType, pFrame );
				NotifyLoadFrame( pFrame, szType, pWnd );
			}
		}
		else if( szTagName == "Image" )
		{
			ioHashString szType = xCurElement.GetStringAttribute( "Type" );
			ioHashString szImageSet = xCurElement.GetStringAttribute( "Imageset" );
			ioHashString szImage;

			ioUIRenderImage *pImg = NULL;
			if( !szImageSet.IsEmpty() )
			{
				szImage = xCurElement.GetStringAttribute( "Image" );
				pImg = g_UIImageSetMgr.CreateImage( szImageSet, szImage );
			}
			else
			{
				ioHashString szFileName = xCurElement.GetStringAttribute( "File" );
				pImg = g_UIImageSetMgr.CreateImageByFile( szFileName );
			}

			if( pImg )
			{
				float fScale = xCurElement.GetFloatAttribute( "Scale" );
				if( 0.0f < fScale )
				{
					pImg->SetScale( fScale );
				}

				ioHashString szColor = xCurElement.GetStringAttribute( "Color" );
				if( !szColor.IsEmpty() )
				{
					DWORD dwColor = ioStringConverter::ParseColor( szColor );
					pImg->SetColor( dwColor );
				}

				ioXMLElement xArea = xCurElement.FirstChild( "Area" );
				if( !xArea.IsEmpty() )
				{
					OnImageArea( xArea, pImg );
				}

				pImg->SetRenderType( static_cast<UIRenderType>( xCurElement.GetIntAttribute( "RenderType" ) ) );
				pImg->SetTextureFilterOption( static_cast<TextureFilterOption>( xCurElement.GetIntAttribute( "TextureFilterOption" ) ) );
				pImg->SetGrayRenderType( static_cast<UIRenderType>( xCurElement.GetIntAttribute( "GrayRenderType" ) ) );

				pImg->SetImageName( szImage.c_str() );
				pImg->SetImageSetName( szImageSet.c_str() );
				pImg->SetImageFileName( szImageSet.c_str(), szImage.c_str() );

				const StringVector &vParams = ioStringConverter::Split( szType.c_str(), "#", 1 );
				if( vParams.size() == 2 )
				{
					pWnd->AddNamedRenderImage( vParams[1].c_str(), pImg );
				}
				else
					pWnd->AddRenderImage( szType, pImg );

				NotifyUIRenderImage( pImg, szType, pWnd );
			}
		}
		else if( szTagName == "Effect" )
		{
			ioHashString szType = xCurElement.GetStringAttribute( "Type" );
			ioHashString szEffectFile = xCurElement.GetStringAttribute( "File" );
			ioHashString szRenderType = xCurElement.GetStringAttribute( "RenderType" );
			ioHashString szCamINI = xCurElement.GetStringAttribute( "CamINI" );
			ioHashString szCamName = xCurElement.GetStringAttribute( "CamName" );
			bool bStart = xCurElement.GetBoolAttribute( "Start" );

			int iX = 0, iY = 0, iWidth = 0, iHeight = 0;
			bool bEfectPosAbsolute = false;
			ioXMLElement xArea = xCurElement.FirstChild( "Area" );
			if (!xArea.IsEmpty())
			{
				iX = xArea.GetIntAttribute("X");
				iY = xArea.GetIntAttribute("Y");
				iWidth = xArea.GetIntAttribute("Width");
				iHeight = xArea.GetIntAttribute("Height");
				bEfectPosAbsolute = xArea.GetBoolAttribute("Absolute");
			}

			if (iWidth  == 0) iWidth  = ioUI3DEffectRender::UI_3D_EFFECT_BASE_WIDTH;
			if (iHeight == 0) iHeight = ioUI3DEffectRender::UI_3D_EFFECT_BASE_HEIGHT;

			D3DXVECTOR3 vEffectPos = ioMath::VEC3_ZERO;
			ioXMLElement xEffectPos = xCurElement.FirstChild( "EffectPos" );
			if (!xEffectPos.IsEmpty())
			{
				vEffectPos.x = xEffectPos.GetFloatAttribute("X");
				vEffectPos.y = xEffectPos.GetFloatAttribute("Y");
				vEffectPos.z = xEffectPos.GetFloatAttribute("Z");
			}

			D3DXVECTOR3 vEffectScale = ioMath::UNIT_ALL;
			ioXMLElement xEffectScale = xCurElement.FirstChild( "EffectScale" );
			if (!xEffectScale.IsEmpty())
			{
				vEffectScale.x = xEffectScale.GetFloatAttribute("X");
				vEffectScale.y = xEffectScale.GetFloatAttribute("Y");
				vEffectScale.z = xEffectScale.GetFloatAttribute("Z");
			}

			ioUI3DEffectRender* pUI3DRender = new ioUI3DEffectRender;
			pUI3DRender->Initialize(szType, szCamINI, szCamName);
			pUI3DRender->AddEffect(szEffectFile, szRenderType, vEffectPos, vEffectScale, ioMath::UNIT_Y, bStart);
			pUI3DRender->SetPosX(iX);
			pUI3DRender->SetPosY(iY);
			pUI3DRender->SetSize(iWidth, iHeight);
			pUI3DRender->SetPosAbsolute(bEfectPosAbsolute);

			pWnd->AddEffect( szType, pUI3DRender );
		}


		xCurElement = xCurElement.NextSibling();
	}
}

void ioGUIManager::OnFrameArea( ioXMLElement &xElement, ioUIRenderFrame *pUIFrame )
{
	RECT rcArea;
	rcArea.left	  = xElement.GetIntAttribute( "X" );
	rcArea.top	  = xElement.GetIntAttribute( "Y" );
	rcArea.right  = rcArea.left + xElement.GetIntAttribute( "Width" );
	rcArea.bottom = rcArea.top + xElement.GetIntAttribute( "Height" );

	pUIFrame->SetRenderRect( rcArea );
	
	ioHashString szReSize = xElement.GetStringAttribute( "AutoResize" );
	if( szReSize == "False" )
	{
		pUIFrame->SetAutoReSize( false );
	}
}

void ioGUIManager::OnImageArea( ioXMLElement &xElement, ioUIRenderImage *pUIImage )
{
	RECT rcArea;
	rcArea.left	  = xElement.GetIntAttribute( "X" );
	rcArea.top	  = xElement.GetIntAttribute( "Y" );
	rcArea.right  = xElement.GetIntAttribute( "Width" );
	rcArea.bottom = xElement.GetIntAttribute( "Height" );

	if( rcArea.right != 0 && rcArea.bottom != 0 )
	{
		rcArea.right += rcArea.left;
		rcArea.bottom += rcArea.top;
		pUIImage->SetRenderRect( rcArea );
	}
	else
	{
		pUIImage->SetPosition( rcArea.left, rcArea.top );
	}

	ioHashString szAlign;
	szAlign = xElement.GetStringAttribute( "HorzAlign" );
	pUIImage->SetHorzFormatting( szAlign );

	szAlign = xElement.GetStringAttribute( "VertAlign" );
	pUIImage->SetVertFormatting( szAlign );

	ioHashString szReverse;
	szReverse = xElement.GetStringAttribute( "Reverse" );
	pUIImage->SetReverseFormatting( szReverse );

	ioHashString szRenderType;
	szRenderType = xElement.GetStringAttribute( "RenderType" );
	pUIImage->SetFixedRenderType( szRenderType );

	ioHashString szFilterOpt = xElement.GetStringAttribute( "TexFilter" );
	pUIImage->SetTexFilterOpt( szFilterOpt );
}

ioUIRenderFrame* ioGUIManager::CloneFrame( const char* templete )
{
	if ( m_pUIFrameMgr ) {
		return m_pUIFrameMgr->CloneFrame( templete );
	}
	return 0;
}

ioUIRenderImage* ioGUIManager::CreateImageByFile( const char* filename )
{
	if ( m_pUIImageSetMgr ) {
		return m_pUIImageSetMgr->CreateImageByFile( filename );
	}
	return 0;
}

ioUIRenderImage* ioGUIManager::CreateImage( const char* imageset, const char* image )
{
	if ( m_pUIImageSetMgr ) {
		return m_pUIImageSetMgr->CreateImage( imageset, image );
	}
	return 0;
}

int ioGUIManager::GetNumImageSet() const
{
	if ( m_pUIImageSetMgr ) {
		return m_pUIImageSetMgr->GetNumImageSet();
	}
	return 0;
}

const char* ioGUIManager::GetImageSetName( int index )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( index );
		if ( imageset ) {
			return imageset->GetName().c_str();
		}
	}
	return NULL;
}

const char* ioGUIManager::GetImageSetFileName( const char* imagesetname )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) {
			return imageset->GetTextureName().c_str();
		}
	}
	return NULL;
}

IDirect3DTexture9* ioGUIManager::GetImageSetTexture( const char* imagesetname )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) {
			return imageset->GetD3DTexture();
		}
	}
	return NULL;
}

int ioGUIManager::GetNumImage( const char* imagesetname ) const
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) {
			return imageset->GetImageCount();
		}
	}
	return 0;
}

const char* ioGUIManager::GetImageTagName( const char* imagesetname, int index )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) 
		{
			return imageset->GetImageName( index );
		}
	}
	return NULL;
}

int ioGUIManager::GetImageRectLeft( const char* imagesetname, const char* imagename )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) 
		{
			ioUIImage* image = imageset->GetImage( imagename );
			if ( image ) {
				return image->GetX();
			}
		}
	}
	return 0;
}

int ioGUIManager::GetImageRectTop( const char* imagesetname, const char* imagename )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) 
		{
			ioUIImage* image = imageset->GetImage( imagename );
			if ( image ) {
				return image->GetY();
			}
		}
	}
	return 0;
}

int ioGUIManager::GetImageRectRight( const char* imagesetname, const char* imagename )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) 
		{
			ioUIImage* image = imageset->GetImage( imagename );
			if ( image ) {
				return image->GetWidth() + image->GetX();
			}
		}
	}
	return 0;
}

int ioGUIManager::GetImageRectBottom( const char* imagesetname, const char* imagename )
{
	if ( m_pUIImageSetMgr ) 
	{
		ioUIImageSet* imageset = m_pUIImageSetMgr->GetImageSet( imagesetname );
		if ( imageset ) 
		{
			ioUIImage* image = imageset->GetImage( imagename );
			if ( image ) {
				return image->GetHeight() + image->GetY();
			}
		}
	}
	return 0;
}

int ioGUIManager::GetNumFrame() const
{
	if ( m_pUIFrameMgr ) {
		return m_pUIFrameMgr->GetNumFrame();
	}
	return 0;
}

const char* ioGUIManager::GetFrameName( int index )
{
	if ( m_pUIFrameMgr ) {
		return m_pUIFrameMgr->GetFrameName( index );
	}
	return 0;
}

void	ioGUIManager::RenderTestFrame( const char* name, int offsetx, int offsety, int width, int height )
{
	if ( !name || !m_pUIFrameMgr ) {
		return;
	}

	const ioUIRenderFrame* pFrame = m_pUIFrameMgr->GetFrame( name );
	if ( pFrame )
	{
		static ioWnd* wnd = new ioWnd();
		ioUIRenderFrame* renderframe = dynamic_cast<ioUIRenderFrame*>(pFrame->Clone());
		renderframe->SetSize( width, height );
		wnd->ShowWnd();
		wnd->SetPosX( offsetx );
		wnd->SetPosY( offsety );
		wnd->AddRenderFrame( "Selected", renderframe );
		wnd->RenderWnd();
	}
}


bool ioGUIManager::ChangeWndElementValue( const char *szOrgXMLFileName, const char *szChangeXMLFileName, const char* szChangeValueKey, ioWnd* pPresent )
{
	//original xml file
	ioTextStream kOrgStream;
	if( !g_ResourceLoader.LoadStream( szOrgXMLFileName, &kOrgStream ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Load Failed", __FUNCTION__, szOrgXMLFileName );
		return false;
	}

	ioXMLDocument kOrgXmlDoc;
	if( !kOrgXmlDoc.LoadFromMemory( (const char*)kOrgStream.GetPtr() ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s LoadFromMemory Failed", __FUNCTION__, szOrgXMLFileName );
		return false;
	}

	ioXMLElement xOrgElement = kOrgXmlDoc.GetRootElement();
	if( xOrgElement.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s, RootElement is NULL", __FUNCTION__, szOrgXMLFileName );
		return false;
	}

	//change xml file
	ioTextStream kChangeStream;
	if( !g_ResourceLoader.LoadStream( szChangeXMLFileName, &kChangeStream ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Load Failed", __FUNCTION__, szChangeXMLFileName );
		return false;
	}

	ioXMLDocument kChangeXmlDoc;
	if( !kChangeXmlDoc.LoadFromMemory( (const char*)kChangeStream.GetPtr() ) )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s LoadFromMemory Failed", __FUNCTION__, szChangeXMLFileName );
		return false;
	}

	ioXMLElement xChangeElement = kChangeXmlDoc.GetRootElement();
	if( xChangeElement.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s, RootElement is NULL", __FUNCTION__, szChangeXMLFileName );
		return false;
	}
	
#ifdef STRING_MANAGER 
	char szOnlyFileName[MAX_PATH]="";
	_splitpath( szChangeXMLFileName, NULL, NULL, szOnlyFileName, NULL );
	m_szChangeStringMgrKeyName = szOnlyFileName;
#endif 

	while( !xChangeElement.IsEmpty() )
	{
		ioHashString szTag = xChangeElement.GetTagName();

		if( szTag == szChangeValueKey )
		{
			ioXMLElement xChild = xChangeElement.FirstChild();
			if( !xChild.IsEmpty() )
			{				
				if( pPresent )
					return ChangeWndElementValue( xOrgElement, xChild, pPresent );
				else
					return ChangeWndElementValue( xOrgElement, xChild, m_pDeskTopWnd );
			}
		}
		
		xChangeElement = xChangeElement.NextSibling();
	}

	return false;
}

bool ioGUIManager::ChangeWndElementValue( ioXMLElement &xOrgElement, ioXMLElement &xChangeElement, ioWnd* pPresent )
{
	if( !pPresent )
		return false;

	if( !xOrgElement.IsTagRight( "Window" ) || !xChangeElement.IsTagRight( "Window" ) )
		return false;

	ioHashString szOrgClassName    = xOrgElement.GetStringAttribute( "ClassName" );
	ioHashString szChangeClassName = xChangeElement.GetStringAttribute( "ClassName" );

	if( szOrgClassName != szChangeClassName )
		return false;
	
	DWORD dwOrgID    = xOrgElement.GetIntAttribute( "Id" );
	DWORD dwChagneID = xChangeElement.GetIntAttribute( "Id" );

	if( dwOrgID != dwChagneID )
		return false;

	ioWnd* pChangeWnd = pPresent->FindChildWnd( dwOrgID );
	if( !pChangeWnd )
		return false;
	
	ioXMLElement xOrgProperty = xOrgElement.FirstChild();

	while( !xOrgProperty.IsEmpty() )
	{
		ioHashString szOrgTag = xOrgProperty.GetTagName();
		ioXMLElement xChangeProperty = xChangeElement.FirstChild();

		while( !xChangeProperty.IsEmpty() )
		{
			ioHashString szChangeTag = xChangeProperty.GetTagName();
			if( szOrgTag == szChangeTag )
			{
				ChangeProperty( xOrgProperty, xChangeProperty, pChangeWnd );
			}

			xChangeProperty = xChangeProperty.NextSibling();
		}

		xOrgProperty = xOrgProperty.NextSibling();
	}

	return true;
}

void ioGUIManager::ChangeProperty( ioXMLElement &xOrgElement, ioXMLElement &xChangeElement, ioWnd* pChangeWnd )
{
	ioHashString szTagName = xOrgElement.GetTagName();
	if( szTagName == "Title" )
	{
		OnTitleProperty( xChangeElement, pChangeWnd, m_szChangeStringMgrKeyName );
	}	
	else if( szTagName == "Tooltip" )
	{
		OnTooltipProperty( xChangeElement, pChangeWnd, m_szChangeStringMgrKeyName );
	}
	else if( szTagName == "Size" )
	{
		OnSizeProperty( xChangeElement, pChangeWnd );
	}
	else if( szTagName == "Style" )
	{
		OnStyleProperty( xChangeElement, pChangeWnd );
	}
	else if( szTagName == "ExStyle" )
	{
		OnExStyleProperty( xChangeElement, pChangeWnd );
	}
	else if( szTagName == "DragRect" )
	{
		OnDragRectProperty( xChangeElement, pChangeWnd );
	}
	else if( szTagName == "ExtraInfo" )
	{
#ifdef STRING_MANAGER 
		xChangeElement.SetUseStringMgr( true );
		xChangeElement.SetStringMgrKeyName( m_szStringMgrKeyName.c_str() );
#endif 

		pChangeWnd->ParseExtraInfo( xChangeElement );

#ifdef STRING_MANAGER 
		xChangeElement.SetUseStringMgr( false );
#endif 
	}
	else if( szTagName == "Imagery" )
	{
		pChangeWnd->ClearElementImageList();
		OnImageryProperty( xChangeElement, pChangeWnd );
	}
	else if( szTagName == "Window" )
	{
		ChangeWndElementValue( xOrgElement, xChangeElement, pChangeWnd );
	}	
}


bool ioGUIManager::IsElsewhereClick( IN ioWnd* pWnd, IN const std::list< ioWnd* >& ExceptionList, IN bool bRoot )
{
	if( m_pPreClickWnd == NULL || pWnd == NULL )
		return false;

	//m_pDeskTopWnd부터 검사하면 모든 윈도우를 검사하기 떄문에 강제 return 처리
	//(해당 함수는 특정 윈도우 및 예외 리스트만을 검사하여 다른곳에 클릭이 발생했는지를 판단)
	if( pWnd == m_pDeskTopWnd )
		return false;

	//최상위 루트 윈도우에서만 검사
	if( bRoot )
	{
		//ExceptionList에 포함된 윈도우에서 클릭이 일어났는지 체크, ExceptionList 클릭이 발생 했다면
		for( std::list< ioWnd* >::const_iterator iter = ExceptionList.begin(); iter != ExceptionList.end(); ++iter )
		{
			if( IsElsewhereException( *iter ) )
				return false;
		}
	}

	// 다른 곳에서 클릭이 일어났는지 확인
	if( m_pPreClickWnd == pWnd )
		return false;

	if( pWnd->HasChild() )
	{
		ioWnd::ioWndList::iterator iter = pWnd->m_ChildList.begin();
		for( ; iter != pWnd->m_ChildList.end(); ++iter )
		{
			if( !IsElsewhereClick( *iter, ExceptionList, false ) )
				return false;
		}
	}

	return true;
}

bool ioGUIManager::IsElsewhereException( IN ioWnd* pWnd )
{
	if( m_pPreClickWnd == NULL )
		return false;

	if( pWnd && pWnd == m_pDeskTopWnd )
		return false;

	if( m_pPreClickWnd == pWnd )
		return true;

	if( pWnd && pWnd->HasChild() )
	{
		ioWnd::ioWndList::iterator iter = pWnd->m_ChildList.begin();
		for( ; iter != pWnd->m_ChildList.end(); ++iter )
		{
			if( IsElsewhereException( *iter ) )
				return true;
		}
	}

	return false;
}

void ioGUIManager::NotifyLoadWnd( ioWnd* pWnd )
{
}

void ioGUIManager::NotifyLoadFrame( ioUIRenderFrame* pFrame, const ioHashString& szType, ioWnd* pWnd )
{
}

void ioGUIManager::NotifyUIRenderImage( ioUIRenderImage* pImage, const ioHashString& szType, ioWnd* pWnd )
{
}

bool ioGUIManager::IsToolTipWnd(ioWnd* pWnd, ioWnd* pParent)
{
	if(!pWnd || !pParent) return false;

	int nSize = m_vToolTipWndID.size();
	for(int i = 0; i < nSize; ++i)
	{
		if(pWnd->GetID() == m_vToolTipWndID[i] && pParent->GetID() == -1)
		{
			return true;
		}
	}

	return false;
}

void ioGUIManager::RegistToolTipID( int nID )
{
	m_vToolTipWndID.push_back(nID);;
}