#pragma once

#include "../ioComplexStringPrinter.h"

class BingoToolTip : public ioWnd
{
public:
	enum ToolTipType
	{
		TTT_REWARD,
		TTT_ALL_BINGO_REWARD,
		TTT_DUMMY,
	};

protected:
	enum
	{
		ID_ICON = 1002,
	};

protected:
	ToolTipType m_ToolTipType;

	ioUIRenderImage *m_pIconBg;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pHiddenIcon;

	int m_iManualIndex;
	ioHashString m_szTtitle;
	bool m_bHidden;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();	
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void PrintManual( int iXPos, int iYPos, float fScale );
	void UpdateWndSize();

public:
	void SetInfo( int iIndex, int iManualIndex, ToolTipType eType );

public:
	BingoToolTip();
	virtual ~BingoToolTip();
};
