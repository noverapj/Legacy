#pragma once

#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "ioMessageBox.h"

class ioUIRenderImage;
class ioExtraItemReleaseWnd : public ioMovingWnd
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
	ioUIRenderImage *m_pExtraItemNum;
	ioUIRenderImage *m_pExtraItemPlus;

protected:
	ioHashString m_szClassName;
	ioHashString m_szItemName;
	ioHashString m_szIconName;
	int          m_iReinforce;
	int m_nGradeType;

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
	ioExtraItemReleaseWnd();
	virtual ~ioExtraItemReleaseWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
