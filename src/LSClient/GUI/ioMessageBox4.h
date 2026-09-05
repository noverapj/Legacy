#pragma once


#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioButton.h"

#define MAX_MSG_LIST_SIZE		12

class ioUIRenderImage;
class ioUIRenderFrame;
class ioXMLElement;

class ioMessageBox4Btn : public ioButton
{
protected:
	ioHashString     m_szTitleName;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetTitleName( const char *szTitleName );
	
public:
	void AttachTitle();

public:
	ioMessageBox4Btn();
	virtual ~ioMessageBox4Btn();
};

class ioMessageBox4 : public ioWnd
{
public:
	enum
	{
		ID_X		= 1,
		ID_YES		= 2,
		ID_NO		= 3,

		ID_ITEM1_RBTN    = 5,
		ID_ITEM2_RBTN    = 6,
		ID_ITEM3_RBTN    = 7,
		ID_ITEM4_RBTN    = 8,
	};

	enum
	{
		MIN_HEIGHT_NONE_BTN = 278,
		MIN_HEIGHT          = 293,
		BTN_BOTTOM_YPOS = 48,
		BTN_LEFT_XPOS   = 17,
		XBTN_RIGHT_XPOS = 34,
		SIDE_TEXT_GAP	= 34,
		START_TEXT_YPOS = 53,
		TEXT_LINE_GAP   = 20,
		BOTTOM_MULTIPLY_H = 65,
		TEXT_XPOS         = 151,
		TEXT_YPOS         = 144,
	};

	struct ExpandSlotItemInfo
	{
		DWORD m_dwID;
		ioHashString m_szItemName;
		DWORD m_dwLimitTime;
		ExpandSlotItemInfo()
		{
			m_dwID = 0;
			m_szItemName.Clear();
			m_dwLimitTime = 0;
		}
	};
	typedef std::vector<ExpandSlotItemInfo> ExpandSlotItemInfoVec;
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pIconBackEnable;
	ioUIRenderImage *m_pIconBackDisable;
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;

protected:
	ioWnd *m_pPrevWnd;
	ioWnd *m_pReturnWnd;
	ioComplexStringPrinter m_PrinterList[MAX_MSG_LIST_SIZE];
	TextAlignType          m_AlignType;
	bool	m_bHasItem;
	int		m_iSelectSlot;
	DWORD	m_dwSelectItemType;

	DWORD m_dwSelectRadioBtnID;
	DWORD m_dwLastRadioBtnID;

	int m_iYOffset;

	ExpandSlotItemInfoVec m_vExpandSlotItemInfoList;

protected:
	void ClearList();

public:
	void SetPrintMessage( ioWnd *pPrevWnd, ioWnd *pReturnWnd );
	void UpdateMessage();
	void SetBtnTitle( const char *szYesTitle, const char *szNoTitle );
	void SetAlignType( TextAlignType eAlignType ) { m_AlignType = eAlignType; }

protected:
	virtual void ReBatchButton();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void OnRender();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	void SetInfo( int iSlotNumber );

public:
	ioMessageBox4();
	virtual ~ioMessageBox4();
};