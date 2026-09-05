

#include "stdafx.h"

#include "ioWndType.h"
#include "ioWnd.h"

#include "ioButton.h"
#include "ioTabControl.h"

ioTabButton::ioTabButton()
{
}

ioTabButton::~ioTabButton()
{
}

void ioTabButton::iwm_lbuttondown( const ioMouse& mouse )
{
	ReSize( true );

	ioButton::iwm_lbuttondown( mouse );
}

void ioTabButton::ReSize( bool bClick )
{
/*
	int iWidth, iHeight;
	
	if( bClick )
	{
		iWidth  = m_WndImage.pClicked->GetWidth();
		iHeight = m_WndImage.pClicked->GetHeight();
	}
	else
	{
		iWidth  = m_WndImage.pNormal->GetWidth();
		iHeight = m_WndImage.pNormal->GetHeight();
	}

	RECT rcTemp = m_rcPos;
	rcTemp.right = rcTemp.left + iWidth;
	rcTemp.top	 = rcTemp.bottom - iHeight;
	m_rcPos = rcTemp;
*/
}

DWORD ioTabButton::GetWndType() const
{
	return IWT_TAB_BTN;
}

ioTabControl::ioTabControl()
{	
}

ioTabControl::~ioTabControl()
{
}

void ioTabControl::AddTabButton( ioTabButton *pTabBtn, ioWnd* pWnd )
{
	pWnd->SetWndPos( GetDerivedPosX(), GetDerivedPosY() + GetHeight() );
	m_TabWndList.push_back( std::make_pair(pTabBtn, pWnd) );

	ReArrangeButton();
}

void ioTabControl::CheckTabButton( DWORD dwCheckID )
{
	ioTabButton* pButton = NULL;
	ioWnd* pWnd = NULL;

	ioTabButtonVec::iterator iter;
	for( iter = m_TabWndList.begin() ; iter!=m_TabWndList.end() ; ++iter )
	{
		pButton = (*iter).first;
		pWnd = (*iter).second;

		if( pButton->GetID() == dwCheckID )
		{
			pButton->SetClick( true );
			pButton->ReSize( true );
			pWnd->ShowWnd();
		}
		else
		{
			pButton->SetClick( false );
			pButton->ReSize( false );
			pWnd->HideWnd();
		}
	}

	ReArrangeButton();
}

void ioTabControl::ReArrangeButton()
{
	int iCtrlWidth = 0;
	int iBtnWidth = 0;
	int xPos;

	iCtrlWidth = GetWidth();
	xPos = GetXPos();

	ioTabButton* pButton = NULL;
	ioWnd*	pWnd = NULL;
	ioTabButtonVec::iterator iter;

	for( iter=m_TabWndList.begin() ; iter!=m_TabWndList.end() ; ++iter )
	{
		pButton = (*iter).first;
		iBtnWidth += pButton->GetWidth();
	}

	if( iCtrlWidth > iBtnWidth )
	{
		xPos += ( iCtrlWidth- iBtnWidth ) / 2;
	}

	for( iter=m_TabWndList.begin() ; iter!=m_TabWndList.end() ; ++iter )
	{
		pButton = (*iter).first;
		pButton->SetWndPos( xPos, pButton->GetYPos() );
		xPos += pButton->GetWidth();
	}
}

void ioTabControl::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	if( pWnd->GetWndType() == IWT_TAB_BTN && cmd == IOBN_BTNDOWN )
	{
		CheckTabButton( pWnd->GetID() );
	}
}

DWORD ioTabControl::GetWndType() const
{
	return IWT_TAB_CTRL;
}