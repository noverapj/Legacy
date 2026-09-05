#pragma once

#include "ioPullDownEventListener.h"
#include "ioPullDownItem.h"

class ioPullDownList;

class NamedTitlePremiumSelectWnd : public ioWnd, public PullDownEventListener
{
public: 
	enum 
	{
		ID_RECV_BTN		= 1,
		ID_SELECT_BTN	= 2,
		ID_LIST_WND		= 100,

		MAXLINE = 10,
	};

protected:
	DWORD m_dwEtcItemCode;

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;
	ManualMgr* m_pManual;
	ioComplexStringPrinter m_ItemTitle;

	DWORD m_dwCode;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_pulldown_event( const PullDownEvent& Event );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();
	
public:
	void ShowSelectWnd( DWORD dwEtcItemCode );

protected:
	void PrintManual( int nXPos, int nYPos , float fScale );
	void GetNamedTitleList( ioPullDownList* pList );
	void ShowNamedTitleList();
	void SendNamedTitleChange();
	void CheckRecvBtn();

public:
	NamedTitlePremiumSelectWnd(void);
	virtual ~NamedTitlePremiumSelectWnd(void);
};

class NamedTitlePullDown : public SinglePullDownItem
{
public:
	DWORD m_dwCode;

public:
	NamedTitlePullDown()
	{
		m_dwCode = 0;
	}
	virtual ~NamedTitlePullDown(){}
};