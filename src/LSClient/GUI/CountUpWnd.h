#pragma once

#include "FSMStateMgr.h"
//#include "../ioSkullHeroManager.h"

class ioUIRenderImage;

class CountUpWnd : public ioWnd
{
protected:
	ioUIRenderImage *m_pTextLine;
	ioUIRenderImage *m_pNum_Blue;
	ioUIRenderImage *m_pNum_Gray;
	ioUIRenderImage *m_pColon_Blue;
	ioUIRenderImage *m_pColon_Gray;

protected:
	int m_iX;
	int m_iY;

	DWORD m_dwCurrentTime;
	IoString m_ProgressText;

	// ÇØ°ñ¿µ¿õ

	ioHashString m_strRank[4];
	int m_iRank;

	bool m_bCountDown;

public:
	virtual void iwm_create();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	//virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

	void UpdateTime( DWORD dwTime );
	void SetCountDown( bool bDown ){ m_bCountDown = bDown; }

	//¼ö·ÃÀå
	void UpdateProgress_Practice( int iProgress, int iMaxProgress );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

	void OnRenderRank( int iXPos, int iYPos );

public:
	CountUpWnd();
	virtual ~CountUpWnd();
};

