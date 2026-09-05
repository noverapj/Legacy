#include "StdAfx.h"

#include "ioWnd.h"
#include "ioMath.h"
#include "ioMouse.h"

#include "ioMovingWnd.h"
#include "ioFrameTimer.h"

WndMove::WndMove() : m_fSpeed(0.0f), m_fWeight(0.0f), m_dwState(WMH_NONE), m_fAngle(0.0f), m_dwCurTime(0), m_pHelpWnd(NULL),m_fXPos(0.0f),m_fYPos(0.0f)
{
	m_OldMouse.x = m_OldMouse.y = 0;
}

WndMove::~WndMove()
{
}

void WndMove::SetWnd( ioWnd *pWnd )
{
	if( pWnd == NULL )
		return;

	m_pHelpWnd = pWnd;

	// 윈도우 무게
	int iWidth  = m_pHelpWnd->GetWidth();
	int iHeight = m_pHelpWnd->GetHeight();

	SetWeight( 1400 + ( ( iWidth * iHeight ) * 0.01f ) );
}

void WndMove::SetState( DWORD dwState, CONST POINT& MousePos )
{
	if( m_pHelpWnd == NULL ) 
		return;

	m_dwState = dwState;
	switch( m_dwState )
	{
	case WMH_NONE:
		{
			m_dwCurTime = FRAMEGETTIME();
			m_fAngle    = 0.0f;
			m_fSpeed    = 0.0f;
			m_fXPos     = m_pHelpWnd->GetXPos();
			m_fYPos     = m_pHelpWnd->GetYPos();
		}
		break;
	case WMH_CHECK:
		{
			m_dwCurTime     = FRAMEGETTIME();
			m_dwMouseUpTime = FRAMEGETTIME();
			m_OldMouse      = MousePos;
		}
		break;
	case WMH_MOVE:
		{
			m_fXPos = m_pHelpWnd->GetDerivedPosX();
			m_fYPos = m_pHelpWnd->GetDerivedPosY();
			m_dwCurTime = FRAMEGETTIME();

			if( m_fSpeed == 0.0f )
				SetState( WMH_NONE, MousePos );
		}
		break;
	}
}

void WndMove::MouseLDown( CONST POINT& MousePos )
{
	SetState( WMH_CHECK, MousePos );

	if( m_pHelpWnd )
	{
		m_fXPos = m_pHelpWnd->GetXPos();
		m_fYPos = m_pHelpWnd->GetYPos();
	}
}

bool WndMove::MouseLUp( CONST POINT& MousePos )
{
	if( m_pHelpWnd )
	{
		DWORD dwDownTime = FRAMEGETTIME() - m_dwMouseUpTime;
		if( dwDownTime < 1000 )
		{
			int iXPos = m_pHelpWnd->GetXPos();
			int iYPos = m_pHelpWnd->GetYPos();
			int tempDis = sqrt( ( ( m_fXPos - iXPos  ) *  ( m_fXPos - iXPos ) ) + ( ( m_fYPos - iYPos ) *  ( m_fYPos - iYPos ) ) );
			if( dwDownTime < 100 || tempDis < 10 )
			{
				SetState( WMH_MOVE, MousePos );
				return false; 
			}
		}
		SetState( WMH_MOVE, MousePos );
	}	
	return true;
}

void WndMove::ProcessMove( const ioMouse& rkMouse )
{
	if( m_pHelpWnd == NULL )
		return;

	switch( m_dwState )
	{
	case WMH_NONE:
		{
			DWORD dwDis = ProcessCollision( rkMouse.GetCurrScreenWidth(), rkMouse.GetCurrScrrenHeight() );
			if( dwDis != 0 )
			{
				DWORD dwSpeed = 600;
				float fGapTime = min( 0.1f, (float)( FRAMEGETTIME() - m_dwCurTime ) /FLOAT1000 );

				m_dwCurTime = FRAMEGETTIME();

				m_fXPos += fGapTime * dwSpeed * cos( DEGtoRAD( m_fAngle ) );
				m_fYPos -= fGapTime * dwSpeed * sin( DEGtoRAD( m_fAngle ) );

				if( m_pHelpWnd->GetHeight() >= rkMouse.GetCurrScrrenHeight() )
					m_fYPos = 0.f;

				m_pHelpWnd->SetWndPos( m_fXPos, m_fYPos );
			}
		}
		break;
	case WMH_CHECK:
		{
			if( FRAMEGETTIME() - m_dwCurTime < 30 )
				return;

			m_dwCurTime = FRAMEGETTIME();

			// 이동 속도와 이동 방향 체크
			POINT kCurPoint = rkMouse.GetMousePos();

			// 이동 방향 
			SetAngle( 360 + 90 - (int)RADtoDEG( atan2( (double)kCurPoint.x - m_OldMouse.x, m_OldMouse.y - kCurPoint.y ) ) );

			// 초당 이동 속도
			int tempDis = ( ( m_OldMouse.x - kCurPoint.x  ) *  ( m_OldMouse.x - kCurPoint.x ) ) + ( ( m_OldMouse.y - kCurPoint.y ) *  ( m_OldMouse.y - kCurPoint.y ) );
			SetSpeed( sqrt((double) tempDis ) * 20 );

			m_OldMouse = kCurPoint;

			if( rkMouse.GetMouseState() == ioMouse::MS_NORMAL )
				SetState( WMH_NONE, rkMouse.GetMousePos() );
		}
		break;
	case WMH_MOVE:
		{
			float fGapTime = min( 0.1f, (float)(FRAMEGETTIME() - m_dwCurTime) /FLOAT1000 );
			m_dwCurTime = FRAMEGETTIME();

			m_fXPos += fGapTime * m_fSpeed * cos(DEGtoRAD(m_fAngle));
			m_fYPos -= fGapTime * m_fSpeed * sin(DEGtoRAD(m_fAngle));

			if( m_pHelpWnd->GetHeight() >= rkMouse.GetCurrScrrenHeight() )
				m_fYPos = 0.f;

			m_pHelpWnd->SetWndPos( m_fXPos, m_fYPos );

			if( m_fSpeed > 0 )
			{
				m_fSpeed -= fGapTime * m_fWeight;
				if( m_fSpeed < 0 )
				{
					m_fSpeed = 0;
					SetState( WMH_NONE, rkMouse.GetMousePos() );
				}
			}
			else if( m_fSpeed < 0 )
			{
				m_fSpeed += fGapTime * m_fWeight;
				if( m_fSpeed > 0 )
				{
					m_fSpeed = 0;
					SetState( WMH_NONE, rkMouse.GetMousePos() );
				}
			}

			ProcessMoveCollision( rkMouse.GetCurrScreenWidth(), rkMouse.GetCurrScrrenHeight() );			
		}
		break;
	}
}

int WndMove::ProcessCollision( int iCurrScreenWidth, int iCurrScreenHeight )
{
	int iXPos  = m_pHelpWnd->GetXPos();
	int iYPos  = m_pHelpWnd->GetYPos();
	int iWidth = m_pHelpWnd->GetWidth();
	int iHeight= m_pHelpWnd->GetHeight();

	int iParentX = 0;
	int iParentY = 0;
	int iParentW = iCurrScreenWidth;
	int iParentH = iCurrScreenHeight;

	if( m_pHelpWnd->GetParent() )
	{
		iParentX = m_pHelpWnd->GetParent()->GetXPos();
		iParentY = m_pHelpWnd->GetParent()->GetYPos();
		iParentW = m_pHelpWnd->GetParent()->GetWidth();
		iParentH = m_pHelpWnd->GetParent()->GetHeight();
	}

	int iCurXPos = iXPos;
	int iCurYPos = iYPos;

	if( iXPos < 0 )
	{
		iCurXPos = 0;		
		if( iYPos < 0 )
			iCurYPos = 0;

		else if( iYPos >= iParentH - iHeight )
			iCurYPos = iParentH - iHeight;

	}    
	else if( iXPos >= iParentW - iWidth )
	{
		iCurXPos = iParentW - iWidth;
		if( iYPos < 0 )
			iCurYPos = 0;

		else if( iYPos >= iParentH - iHeight )
			iCurYPos = iParentH - iHeight;
	}	
	else if( iYPos < 0 )
	{
		iCurYPos = 0;
	}	    
	else if( iYPos >= iParentH - iHeight )
	{
		iCurYPos = iParentH - iHeight;
	}
	else 
	{
		return 0;
	}

	// 이동 방향 
	SetAngle( 360 + 90 - (int)RADtoDEG( atan2( (double) iCurXPos - iXPos, iYPos - iCurYPos ) ) );

	// 초당 이동 속도
	int tempDis = ( ( iXPos - iCurXPos  ) *  ( iXPos - iCurXPos ) ) + ( ( iYPos - iCurYPos ) *  ( iYPos - iCurYPos ) );

	return sqrt( (double) tempDis );
}

void WndMove::ProcessMoveCollision( int iCurrScreenWidth, int iCurrScreenHeight )
{
	int iXPos  = m_pHelpWnd->GetXPos();
	int iYPos  = m_pHelpWnd->GetYPos();
	int iWidth = m_pHelpWnd->GetWidth();
	int iHeight= m_pHelpWnd->GetHeight();

	int iParentX = 0;
	int iParentY = 0;
	int iParentW = iCurrScreenWidth;
	int iParentH = iCurrScreenHeight;

	if( m_pHelpWnd->GetParent() )
	{
		iParentX = m_pHelpWnd->GetParent()->GetXPos();
		iParentY = m_pHelpWnd->GetParent()->GetYPos();
		iParentW = m_pHelpWnd->GetParent()->GetWidth();
		iParentH = m_pHelpWnd->GetParent()->GetHeight();
	}

	float angle = m_fAngle;
	if( angle < 0.0f ) angle += 360;
	else if( angle >= 360.0f ) angle -= 360;

	if( iXPos < 0 )
	{
		if( angle > 90.0f && angle < 270.0f )
		{
			iXPos  = 0;
			m_fAngle = 180.0f - angle;
			if( m_fAngle < 0 ) m_fAngle += 360;
		}
	}

	if( iXPos >= iParentW - iWidth )
	{
		if( angle < 90.0f || angle > 270.0f )
		{
			iXPos = iParentW - 1 - iWidth;
			m_fAngle = 180.0f - angle;
			if( m_fAngle < 0 ) m_fAngle += 360;
		}
	}

	if( iYPos < 0 )
	{
		if( angle < 180.0f && angle > 0.0f )
		{
			iYPos  = 0;
			m_fAngle = 0.0f - angle;
			if( m_fAngle < 0.0f ) m_fAngle += 360;
		}
	}

	if( iYPos >= iParentH - iHeight )
	{
		if( angle > 180.0f )
		{
			iYPos = iParentH - 1 - iHeight;
			m_fAngle = 0.0f - angle;
			if( m_fAngle < 0.0f ) m_fAngle += 360;
		}
	}   
}

//---------------------------------------------------------------------------------------

ioMovingWnd::ioMovingWnd()
{
}

ioMovingWnd::~ioMovingWnd()
{
}

void ioMovingWnd::iwm_lbuttondown( const ioMouse& mouse )
{
	if( GetWidth() < mouse.GetCurrScreenWidth() && GetHeight() < mouse.GetCurrScrrenHeight() )
	{
		if( m_pHitRect )
		{			
			if( IsInHitRect( mouse.GetMousePos() ) )
			{
				m_MoveWnd.SetWnd( (ioWnd*)this );
				m_MoveWnd.MouseLDown( mouse.GetMousePos() );
			}
		}
		else
		{
			m_MoveWnd.SetWnd( (ioWnd*)this );
			m_MoveWnd.MouseLDown( mouse.GetMousePos() );
		}
	}

	ioWnd::iwm_lbuttondown( mouse );
}

void ioMovingWnd::iwm_lbuttonup( const ioMouse& mouse )
{
	if( GetWidth() < mouse.GetCurrScreenWidth() && GetHeight() < mouse.GetCurrScrrenHeight() )
	{
		m_MoveWnd.MouseLUp( mouse.GetMousePos() );
	}
	ioWnd::iwm_lbuttonup( mouse );
}

void ioMovingWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	//WndMove::WMH_NONE 상태일때는 위치 값을 참조 하지 않음으로 0,0을 전달
	POINT NullPos = { 0, 0 };
	m_MoveWnd.SetState( WndMove::WMH_NONE, NullPos );
}

void ioMovingWnd::OnMouseProcess( const ioMouse& mouse )
{
	if( !IsShow() )
		return;

	m_MoveWnd.ProcessMove( mouse );
}

bool ioMovingWnd::IsMoving()
{
	if( m_MoveWnd.GetState() == WndMove::WMH_MOVE )
		return true;

	return false;
}

//---------------------------------------------------------------------------------------

ioMovingWndEX::ioMovingWndEX()
{
}

ioMovingWndEX::~ioMovingWndEX()
{
}

void ioMovingWndEX::iwm_lbuttondown( const ioMouse& mouse )
{
	if( GetWidth() < mouse.GetCurrScreenWidth() && GetHeight() < mouse.GetCurrScrrenHeight() )
	{
		if( m_pHitRect )
		{			
			if( IsInHitRect( mouse.GetMousePos() ) )
			{
				m_MoveWnd.SetWnd( this );
				m_MoveWnd.MouseLDown( mouse.GetMousePos() );
			}
		}
		else
		{
			m_MoveWnd.SetWnd( this );
			m_MoveWnd.MouseLDown( mouse.GetMousePos() );
		}
	}

	ioWndEX::iwm_lbuttondown( mouse );
}

void ioMovingWndEX::iwm_lbuttonup( const ioMouse& mouse )
{
	if( GetWidth() < mouse.GetCurrScreenWidth() && GetHeight() < mouse.GetCurrScrrenHeight() )
	{
		m_MoveWnd.MouseLUp( mouse.GetMousePos() );
	}
	ioWndEX::iwm_lbuttonup( mouse );
}

void ioMovingWndEX::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	//WndMove::WMH_NONE 상태일때는 위치 값을 참조 하지 않음으로 0,0을 전달
	POINT NullPos = { 0, 0 };
	m_MoveWnd.SetState( WndMove::WMH_NONE, NullPos );
}

void ioMovingWndEX::OnMouseProcess( const ioMouse& mouse )
{
	if( !IsShow() )
		return;

	m_MoveWnd.ProcessMove( mouse );
}

bool ioMovingWndEX::IsMoving()
{
	if( m_MoveWnd.GetState() == WndMove::WMH_MOVE )
		return true;

	return false;
}

//---------------------------------------------------------------------------------------

ioMovingBtn::ioMovingBtn()
{
}

ioMovingBtn::~ioMovingBtn()
{
}

bool ioMovingBtn::IsCanMouseReaction() const
{
	if( !IsShow() )
		return false;

	if( m_dwStyle & IWS_NO_MOUSE_RESPONSE )
		return false;

	return true;
}

void ioMovingBtn::iwm_lbuttondown( const ioMouse& mouse )
{
	m_MoveWnd.SetWnd( (ioWnd*)this );
	m_MoveWnd.MouseLDown( mouse.GetMousePos() );

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		ioWnd::iwm_lbuttondown( mouse );
		return;
	}

	ioScaleButton::iwm_lbuttondown( mouse );
}

void ioMovingBtn::iwm_lbuttonup( const ioMouse& mouse )
{
	if( m_MoveWnd.MouseLUp( mouse.GetMousePos() ) ) 
	{
		ioWnd::iwm_lbuttonup( mouse );
		return;   
	}
	if( HasWndStyle( IWS_INACTIVE ) )
	{
		ioWnd::iwm_lbuttonup( mouse );
		return;
	}

	ioScaleButton::iwm_lbuttonup( mouse );
}

void ioMovingBtn::iwm_mouseover( const ioMouse& mouse )
{
	if( HasWndStyle( IWS_INACTIVE ) )
	{
		return;
	}

	ioScaleButton::iwm_mouseover( mouse );
}

void ioMovingBtn::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioScaleButton::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	//WndMove::WMH_NONE 상태일때는 위치 값을 참조 하지 않음으로 0,0을 전달
	POINT NullPos = { 0, 0 };
	m_MoveWnd.SetState( WndMove::WMH_NONE, NullPos );
}

void ioMovingBtn::OnMouseProcess( const ioMouse& mouse )
{
	if( !IsShow() )	return;	

	m_MoveWnd.ProcessMove( mouse );
}

//---------------------------------------------------------------------------------------

ioMovingNormalBtn::ioMovingNormalBtn()
{
}

ioMovingNormalBtn::~ioMovingNormalBtn()
{
}

bool ioMovingNormalBtn::IsCanMouseReaction() const
{
	if( !IsShow() )
		return false;

	if( m_dwStyle & IWS_NO_MOUSE_RESPONSE )
		return false;

	return true;
}

void ioMovingNormalBtn::iwm_lbuttondown( const ioMouse& mouse )
{
	m_MoveWnd.SetWnd( (ioWnd*)this );
	m_MoveWnd.MouseLDown(  mouse.GetMousePos() );

	if( HasWndStyle( IWS_INACTIVE ) )
	{
		ioWnd::iwm_lbuttondown( mouse );
		return;
	}

	ioButton::iwm_lbuttondown( mouse );
}

void ioMovingNormalBtn::iwm_lbuttonup( const ioMouse& mouse )
{
	if( m_MoveWnd.MouseLUp( mouse.GetMousePos() ) ) 
	{
		ioWnd::iwm_lbuttonup( mouse );
		return;   
	}
	if( HasWndStyle( IWS_INACTIVE ) )
	{
		ioWnd::iwm_lbuttonup( mouse );
		return;
	}

	ioButton::iwm_lbuttonup( mouse );
}

void ioMovingNormalBtn::iwm_mouseover( const ioMouse& mouse )
{
	if( HasWndStyle( IWS_INACTIVE ) )
	{
		return;
	}

	ioButton::iwm_mouseover( mouse );
}

void ioMovingNormalBtn::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioButton::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	//WndMove::WMH_NONE 상태일때는 위치 값을 참조 하지 않음으로 0,0을 전달
	POINT NullPos = { 0, 0 };
	m_MoveWnd.SetState( WndMove::WMH_NONE, NullPos );
}

void ioMovingNormalBtn::OnMouseProcess( const ioMouse& mouse )
{
	if( !IsShow() )	return;	

	m_MoveWnd.ProcessMove( mouse );
}