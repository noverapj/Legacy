#pragma once


#include "../ioComplexStringPrinter.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../io3DEngine/ioButton.h"

/*
WINUSER.H 
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
*/
#define MB_OKBACK                   0x00000006L
#define MB_YESNOOTHER				0x00000007L

/*
WINUSER.H 
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#if(WINVER >= 0x0400)
#define IDCLOSE         8
#define IDHELP          9
#endif  WINVER >= 0x0400 
*/
#define IDBACK              10
#define IDOTHER				11

#define MAX_MSG_LIST_SIZE		12
#define MAX_MSG_LONG_LIST_SIZE	 8

class ioUIRenderImage;
class ioUIRenderFrame;
class ioXMLElement;
/*********************************************************************************
 아래의 시스템 팝업 클래스를 상속받으면 확장성이 떨어진다.           
 아래의 버튼과 윈도우는 시스템 팝업 이외의 UI에서 사용하면 안된다. 
 아래의 내용을 수정한다는건 곧 시스템 팝업을 개선하고 보완한다는 이야기인데  
 다른 UI까지 영향을 받게되기 때문이다. 절대 다른 UI에서 사용 불가!!  2010.0212.LJH
**********************************************************************************/
class ioMessageBoxBtn : public ioButton
{
protected:
	ioHashString     m_szTitleName;
	ioHashString     m_szHotKey;

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	void SetTitleName( const char *szTitleName );
	void SetHotKey( const char *szHotKey );
	
public:
	void AttachTitle();

public:
	ioMessageBoxBtn();
	virtual ~ioMessageBoxBtn();
};

class ioMessageBox : public ioWnd
{
public:
	enum
	{
		ID_X		= 1,
		ID_YES		= 2,
		ID_NO		= 3,
		ID_OK       = 4,
	};

	enum
	{
		MIN_WIDTH		= 250,
		MIN_HEIGHT		= 147,
		BTN_BOTTOM_YPOS = 48,
		BTN_LEFT_XPOS   = 17,
		XBTN_RIGHT_XPOS = 34,
		SIDE_TEXT_GAP	= 34,
		START_TEXT_YPOS = 53,
		TEXT_LINE_GAP   = 20,
		BOTTOM_MULTIPLY_H = 65,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;

protected:
	int    m_iBoxType;	
	ioWnd *m_pPrevWnd;
	ioWnd *m_pReturnWnd;
	StringVector m_MessageList;

	IoString*	 m_pioString;

public:
	void SetMessage( int iType, const char *szBuf, ioWnd *pPrevWnd, ioWnd *pReturnWnd );
	void SetBtnTitle( const char *szOKTitle, const char *szYesTitle, const char *szNoTitle );
	void SetIoStringMessage( int iType, IoString* pStr, ioWnd *pPrevWnd, ioWnd *pReturnWnd );

protected:
	virtual void ReBatchButton( int iType );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();

protected:
	virtual void OnRender();

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	ioMessageBox();
	virtual ~ioMessageBox();
};
//////////////////////////////////////////////////////////////////////////
class ioMessageBox3 : public ioMessageBox
{
protected:
	ioComplexStringPrinter m_PrinterList[MAX_MSG_LIST_SIZE];
	TextAlignType          m_AlignType;
	
	void ClearList();

public:
	void SetPrintMessage( int iType, ioComplexStringPrinter *MsgPrinter, ioWnd *pPrevWnd, ioWnd *pReturnWnd );
	void SetAlignType( TextAlignType eAlignType ) { m_AlignType = eAlignType; }
	
protected:
	virtual void OnRender();
	
public:
	ioMessageBox3();
	virtual ~ioMessageBox3();
};

////////////////////////////////////////////////////////////////////////////////
