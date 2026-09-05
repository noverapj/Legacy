

#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"

#include "FireBulletWnd.h"

FireBulletWnd::FireBulletWnd()
{
	m_iCurBullet = 0;
	m_iMaxBullet = 0;
	m_bLaserItem = false;
}

FireBulletWnd::~FireBulletWnd()
{
}

void FireBulletWnd::UpdateBullet( int iCur, int iMax, bool bLaserItem )
{
	m_iCurBullet = iCur;
	m_iMaxBullet = iMax;
	m_bLaserItem = bLaserItem;
}

void FireBulletWnd::OnRender()
{
	ioWnd::OnRender();

	int xPos, yPos;
	xPos = GetDerivedPosX();
	yPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_SHADOW );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	g_FontMgr.SetTextColor( 0, 255, 0 );

	if( !m_bLaserItem )
		g_FontMgr.PrintText( xPos + 2, yPos + 10, FLOAT1, STR(1), m_iCurBullet, m_iMaxBullet );
	else
		g_FontMgr.PrintText( xPos + 2, yPos + 10, FLOAT1, STR(2), m_iCurBullet, m_iMaxBullet );
}
