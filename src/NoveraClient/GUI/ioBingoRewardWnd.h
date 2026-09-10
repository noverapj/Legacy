#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "NewShopWnd.h"

///////////////////////////////////////////////////////////////////
class ioBingoIconButton : public ioButton
{
protected:	
	int m_Index;
	ioUIRenderImage* m_pIcon;
	ioUIRenderImage* m_pIconBack;
	ioUIRenderImage* m_pIconEmpty;

	ioHashString m_szName;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	void SetBingoInfo(int Index);
	void OnIconRender( int iXPos, int iYPos );


public:
	ioBingoIconButton();
	virtual ~ioBingoIconButton();
};

///////////////////////////////////////////////////////////////////
class ioUIRenderImage;
class ioBingoRewardWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_ICON_BTN1	= 100,
		ID_ICON_BTN2	= 101,
		ID_ICON_BTN3	= 102,
		ID_ICON_BTN4	= 103,
		ID_ICON_BTN5	= 104,
		ID_ICON_BTN6	= 105,
		ID_ICON_BTN7	= 106,
		ID_ICON_BTN8	= 107,
		ID_ICON_BTN9	= 108,
		ID_ICON_BTN10	= 109,
		ID_ICON_BTN11	= 110,
		ID_ICON_BTN12	= 111,

		ICON_BTN_MAX	= 12,
	};

protected:
	typedef std::vector<ioUIRenderImage*> ioUIRenderImageList;

protected:
	int m_iMaxPage;
	int m_iCurPage;

	ioHashString m_szPageText;	
	ioUIRenderImageList m_BingoIconList;

public:
	void ShowItemList();

protected:
	void UpdatePage( int iPage );	

	void UpdateIconBtn();
	void UpdateIconBingoBtn( int iPos, int index );
	void UpdateIconAllBingoBtn( int iPos, int index );

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ioBingoRewardWnd();
	virtual ~ioBingoRewardWnd();
};
////////////////////////////////////////////////////////////////////////////////////////////////
