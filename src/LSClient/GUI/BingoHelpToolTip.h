#pragma once

#include "../ioComplexStringPrinter.h"

class BingoHelpToolTip : public ioWnd
{
public:
	enum
	{
		WND_HEIGHT		= 26,
		START_TEXT_XPOS = 7,
		START_TEXT_YPOS = 6,
		SIDE_TEXT_GAP	= 14,
		TEXT_LINE_GAP   = 16,
	};

protected:
	DWORD m_dwParentID;

public:
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetInfo( DWORD dwParentID );

public:
	BingoHelpToolTip();
	virtual ~BingoHelpToolTip();
};
