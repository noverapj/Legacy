
#include "StdAfx.h"
#include "ioRichLabel.h"

#include "ioBasicToolTip.h"
#include "../ioShuffleRoomMgr.h"

ioBasicToolTip::ioBasicToolTip()
{
}

ioBasicToolTip::~ioBasicToolTip()
{

}

void ioBasicToolTip::iwm_show()
{
}

void ioBasicToolTip::iwm_hide()
{
}

void ioBasicToolTip::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
		
	ioLSMouse *pMouse = g_App.GetMouse();
	if ( !pMouse )
		return;

	int iHeight = GetHeight();
	for( ioWndList::iterator iter = m_ChildList.begin(); iter != m_ChildList.end(); ++iter )
	{
		ioRichLabel* pLabel = dynamic_cast<ioRichLabel*>( *iter );
		if( pLabel )
		{
			iHeight = pLabel->GetTextHeight() + 8;
		}
	}
	SetSize( GetWidth(), iHeight );
	
	//툴팁의 크기가 전체 화면의 크기를 벗어나면 마우스 위치가 툴팁 내부에 들어와서 오버한 윈도우의 포커스를 잃어버려 문제가 발생됨.
	//툴팁이 보였다가 안보였다가 상태가 변경되어 마우스 위치에 따라 위치 재조정

	//기존 코드
	/*	
	POINT pt = pMouse->GetMousePos();
	int iXPos = pt.x + 8;
	int iYPos = pt.y + 8;

	int iXOffset = pt.x + GetWidth() + 8;
	if( Setting::Width() < iXOffset )
	{
		iXPos -= ( iXOffset - Setting::Width() ) + 8;
	}

	int iYOffset = pt.y + GetHeight() + 8;
	if( Setting::Height() < iYOffset )
	{
		iYPos -= ( iYOffset - Setting::Height() ) + 8;
	}
	*/

	int iOffSetXY = 8;
	POINT pt = pMouse->GetMousePos();
	int iXPos = pt.x;
	int iYPos = pt.y;

	if( iXPos + iOffSetXY + GetWidth() > Setting::Width() )
		iXPos -= ( GetWidth() + iOffSetXY );
	else 
		iXPos += iOffSetXY;

	if( iYPos + iOffSetXY + GetHeight() > Setting::Height() - 45 )
		iYPos -= ( GetHeight() + iOffSetXY );
	else 
		iYPos += iOffSetXY;
	
	SetScreenWndPos( iXPos, iYPos );
	pMouse->SetToolTipCursor();
}