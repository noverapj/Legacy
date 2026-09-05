
#include "stdafx.h"

#include "ioWnd.h"
#include "ioMouse.h"
#include "ioFrameTimer.h"

int ioMouse::m_iMaxWidth  = 2048;
int ioMouse::m_iMaxHeight = 1280;

ioMouse::ioMouse()
{
	m_MouseState  = MS_NORMAL;

	m_ptMousePos.x      = 0;
	m_ptMousePos.y      = 0;
	m_iCurrScreenWidth  = 0;
	m_iCurrScreenHeight = 0;

	m_pDragItem = NULL;

	m_dwMouseBusyStartTime = 0;
	m_dwPacketID = 0;

	m_bCompulsionUnMouseBusy = false;

	ClearAllBtnStatus();
}

ioMouse::~ioMouse()
{
}

void ioMouse::SetMaxWindowSize( int iWidth, int iHeight )
{
	m_iMaxWidth  = iWidth;
	m_iMaxHeight = iHeight;
}

void ioMouse::ClearAllBtnStatus()
{
	m_bLBtnPress  = false;
	m_bLBtnDown	  = false;
	m_bLBtnUp	  = false;
	m_bLBtnDBLCLK = false;

	m_bMBtnPress  = false;
	m_bMBtnDown	  = false;
	m_bMBtnUp	  = false;
	m_bMBtnDBLCLK = false;

	m_bRBtnPress  = false;
	m_bRBtnDown	  = false;
	m_bRBtnUp	  = false;
	m_bRBtnDBLCLK = false;

	m_bMouseMoved = false;
	m_iWheelDelta = 0;
}

void ioMouse::SetCurrScreenSize( int iWidth, int iHeight )
{
	m_iCurrScreenWidth  = iWidth;
	m_iCurrScreenHeight = iHeight;
}

void ioMouse::SetMousePos( int xPos, int yPos )
{
	m_ptMousePos.x = max( 0, min( xPos, m_iMaxWidth ) );
	m_ptMousePos.y = max( 0, min( yPos, m_iMaxHeight ) );
}

void ioMouse::SetLBtnDown( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bLBtnPress  = true;
		m_bLBtnDown   = true;
		m_bLBtnUp     = false;
		m_bLBtnDBLCLK = false;

		SetMouseState( MS_CLICK );
	}
}

void ioMouse::SetLBtnUp( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bLBtnPress  = false;
		m_bLBtnDown   = false;
		m_bLBtnUp     = true;
		m_bLBtnDBLCLK = false;

		SetMouseState( MS_NORMAL );
	}
}

void ioMouse::SetLBtnDBLCLK( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bLBtnPress  = false;
		m_bLBtnDown   = false;
		m_bLBtnUp     = false;
		m_bLBtnDBLCLK = true;

		SetMouseState( MS_NORMAL );
	}
}

void ioMouse::SetMBtnDown( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bMBtnPress  = true;
		m_bMBtnDown   = true;
		m_bMBtnUp     = false;
		m_bMBtnDBLCLK = false;

		SetMouseState( MS_CLICK );
	}
}

void ioMouse::SetMBtnUp( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bMBtnPress  = false;
		m_bMBtnDown   = false;
		m_bMBtnUp     = true;
		m_bMBtnDBLCLK = false;

		SetMouseState( MS_NORMAL );
	}
}

void ioMouse::SetMBtnDBLCLK( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bMBtnPress  = false;
		m_bMBtnDown   = false;
		m_bMBtnUp     = false;
		m_bMBtnDBLCLK = true;

		SetMouseState( MS_NORMAL );
	}
}

void ioMouse::SetRBtnDown( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bRBtnPress  = true;
		m_bRBtnDown   = true;
		m_bRBtnUp	  = false;
		m_bRBtnDBLCLK = false;

		SetMouseState( MS_CLICK );
	}
}

void ioMouse::SetRBtnUp( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bRBtnPress  = false;
		m_bRBtnDown   = false;
		m_bRBtnUp	  = true;
		m_bRBtnDBLCLK = false;

		SetMouseState( MS_NORMAL );
	}
}

void ioMouse::SetRBtnDBLCLK( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bRBtnPress  = false;
		m_bRBtnDown   = false;
		m_bRBtnUp	  = false;
		m_bRBtnDBLCLK = true;

		SetMouseState( MS_NORMAL );
	}
}

void ioMouse::SetMouseMove( int xPos, int yPos )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_bMouseMoved = true;
	}
}

void ioMouse::SetMouseWheel( int xPos, int yPos, int zDelta )
{
	SetMousePos( xPos, yPos );

	if( !IsBusy() )
	{
		m_iWheelDelta = zDelta;
	}
}

void ioMouse::SetMouseState( MouseState eState )
{
	if( !IsBusy() )
	{
		m_MouseState = eState;
	}
}

void ioMouse::SetBusy()
{
	m_MouseState = MS_BUSY;
	m_dwMouseBusyStartTime = FRAMEGETTIME();
}

void ioMouse::SetUnBusy()
{
	m_MouseState = MS_NORMAL;
	m_dwMouseBusyStartTime = 0;
	m_dwPacketID = 0;
}

bool ioMouse::IsBusy() const
{
	if( m_MouseState == MS_BUSY )
		return true;

	return false;
}

void ioMouse::SetDragDropStart( ioDragItem *pDragItem, const POINT &ptOffset )
{
	if( !IsBusy() )
	{
		m_pDragItem    = pDragItem;
		m_ptDragOffset = ptOffset;

		m_MouseState = MS_DROP_ENABLE;
	}
}

void ioMouse::ClearDragDropState()
{
	m_pDragItem = NULL;

	if( !IsBusy() )
	{
		m_MouseState = MS_NORMAL;
	}
}

void ioMouse::ClearCheckOnlyOnceStatus()
{
	m_bLBtnPress  = false;
	m_bLBtnUp	  = false;
	m_bLBtnDBLCLK = false;

	m_bMBtnPress  = false;
	m_bMBtnUp	  = false;
	m_bMBtnDBLCLK = false;

	m_bRBtnPress  = false;
	m_bRBtnUp	  = false;
	m_bRBtnDBLCLK = false;	

	m_bMouseMoved = false;
	m_iWheelDelta = 0;
}

void ioMouse::ProcessMouse()
{
	if ( IsBusy() )
	{
		enum { BusyMouseChekTime = 10000,	};
		if ( m_dwMouseBusyStartTime && m_dwMouseBusyStartTime + BusyMouseChekTime < FRAMEGETTIME() )
		{
			LOG.PrintTimeAndLog( 0, "%s - Compulsory call SetUnBusy Function, Last PacketID : %x", __FUNCTION__, m_dwPacketID );
			SetUnBusy();
			SetCompulsionUnMouseBusy( true );
		}
	}
}

void ioMouse::SetPacketID( DWORD dwPacketID )
{
	m_dwPacketID = dwPacketID;
}
