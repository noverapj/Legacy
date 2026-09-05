#pragma once


#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioMovingWnd.h"

class ioUIRenderImage;
class ioExMedalSlotReleaseWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_X		= 1,
		ID_ALL_OK	= 2,
		ID_OK		= 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pExtraItemIcon;

protected:
	ioHashString m_szIconName;
	ioHashString m_szSubIconName;
	ioHashString m_szClassName;
	int          m_iSlotLevel;

protected:
	DWORD   m_dwHideDelayTime;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioExMedalSlotReleaseWnd();
	virtual ~ioExMedalSlotReleaseWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
