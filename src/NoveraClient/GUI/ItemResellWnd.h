#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

class ioUIRenderImage;

enum
{
	SELL_EDIT_NONE		= 0,
	SELL_EDIT_EXTRAITEM	= 1,
};

enum
{
	ACST_SELL			= 1,
	ACST_DISASSEMBLE	= 2,
	ACST_SELL_MEDAL		= 3,
	ACST_SELL_COSTUME	= 4,
	ACST_SELL_ACCESSORY	= 5,
	ACST_SELL_DECO		= 6,
};

//////////////////////////////////////////////////////////////////////////
class ItemResellWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_RESELL	= 2,
		ID_CANCEL	= 3,
		ID_SELL_EDIT = 4,

		ID_ITEM_ICON	= 11,
	};

	enum
	{
		MAX_INFO_LINE	= 5,
	};

	enum
	{
		BOTTOM_FRAME_GAP	= 65,
		BUTTON_GAP			= 48,
		EDIT_GAP			= 98,

		NORMAL_HEIGHT		= 333,
		EDIT_HEIGHT			= 390,

		DIS_NORMAL_HEIGHT	= 313,
		DIS_EDIT_HEIGHT		= 370,
	};

protected:
	DWORD m_dwSellEditType;
	DWORD m_dwSellActionType;

	ioComplexStringPrinter m_Desc[MAX_INFO_LINE];
	ioUIRenderImage* m_pSellMark;
	ioWnd* m_pReturnWnd;

protected:
	ioWnd *m_pPreEdit;
	ioWnd *m_pSellEdit;

	ioUIRenderFrame *m_pEditFrm;
	ioUIRenderFrame *m_pBottomFrm;
	
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void SetInfoAndShow( const ioHashString &szTitle, ioUIRenderImage *pIcon, ioUIRenderImage *pSubIcon, 
						 int iIconNum, ioComplexStringPrinter *pPrinter, ioWnd *pReturnWnd, DWORD dwEditType,
						 DWORD dwActionType, bool bIsCostume, int nGradeType = 0 );
	
protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	ItemResellWnd();
	virtual ~ItemResellWnd();
};
//////////////////////////////////////////////////////////////////////////

