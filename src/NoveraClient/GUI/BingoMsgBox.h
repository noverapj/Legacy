#pragma once

#include "../ioComplexStringPrinter.h"

class BingoMsgBox : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_OK     = 2,
		ID_CANCEL = 3,
		ID_INIT   = 4,
		ID_CLOSE  = 5,
	};

	enum UseType
	{
		UT_NONE,
		UT_GET_NUMBER,
		UT_SHUFFLE_NUMBER,
		UT_SHUFFLE_REWARD,
		UT_SPECIAL_NUMBER,
		UT_INIT,
	};

	enum
	{
		WINDOW_WIDTH  = 302,
		WINDOW_HEIGHT = 278,
		BUTTON_Y = 230,
	};

protected:
	ioUIRenderImage *m_pGetNumber;
	ioUIRenderImage *m_pShuffleNumber;
	ioUIRenderImage *m_pShuffleItem;
	ioUIRenderImage *m_pSpecialNumber;
	ioUIRenderImage *m_pInit;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBg;
	ioUIRenderFrame *m_pBottomFrm;

	UseType m_UseType;
	int m_iBingoType;

	ioComplexStringPrinter m_Printer;
	ioHashString m_szItemName;
	int m_iItemCount;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_spacebar();

protected:
	//virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void PrintText( int iXPos, int iYPos );
	void UpdateWndSize( int iWidth, int iHeight );

public:
	void SetInfo( UseType eType, int iBingoType );

public:
	BingoMsgBox();
	virtual ~BingoMsgBox();
};
