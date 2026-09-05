#pragma once

class ioUIRenderImage;
class ioWnd;
class ioXMLElement;

#define IO_RESOLUTION_CHANGE (IOCM_USER_DEFINED + 10 )
#define IO_FILTER_CHANGE (IOCM_USER_DEFINED + 11 )
#define IO_OVERAY_CHANGE (IOCM_USER_DEFINED + 12 )
#define IO_RESOLUTION_CONFIRM ( IOCM_USER_DEFINED + 13 )
#define IO_SCREEN_MODE_CHANGE ( IOCM_USER_DEFINED + 14 )

class ResolutionList : public ioWnd
{
protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	ioWnd *m_pOpenBtn;
	int m_iOverListIndex;

	struct Resolution
	{
		int iWidth;
		int iHeight;
		bool bCheck;
	};
	typedef std::vector< Resolution > EnableResolutionList;
	EnableResolutionList m_ResolutionList;

public:
	void OpenList( ioWnd *pOpenBtn, int iCurWidth, int iCurHeight );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ResolutionList();
	virtual ~ResolutionList();
};
//////////////////////////////////////////////////////////////////////////
class MovieResolutionList : public ioWnd
{
protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	ioWnd *m_pOpenBtn;
	int    m_iOverListIndex;
	int    m_iOptionType;

	struct Resolution
	{
		int iValue;
		bool bCheck;
	};
	typedef std::vector< Resolution > EnableResolutionList;
	EnableResolutionList m_ResolutionList;

public:
	void OpenList( ioWnd *pOpenBtn, int iType, int iCurValue );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	MovieResolutionList();
	virtual ~MovieResolutionList();
};

class StringSelectList : public ioWnd
{
protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	ioWnd *m_pNotifier;
	int m_iOverListIndex;
	int m_iPreSelectIndex;
	DWORD m_dwNotifyCommand;

	StringVector m_vNameList;

public:
	void OpenList( ioWnd *pNotifier, StringVector &vNameList, int iCurSelectIndex, DWORD dwNotifyCommand );
	std::string GetName( int iIndex ) const;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	StringSelectList();
	virtual ~StringSelectList();
};

//////////////////////////////////////////////////////////////////////////
class ResolutionConfirm : public ioWnd
{
public:
	enum
	{
		ID_CONFIRM = 1,
		ID_RESTORE = 2,
		ID_EXIT	   = 3
	};

protected:
	DWORD m_dwWaitDuration;
	DWORD m_dwStartTime;

	ioWnd *m_pNotifier;

public:
	void OpenConfirm( ioWnd *pWnd, int iWidth, int iHeight );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();

protected:
	void NotifyConfirm();
	void NotifyRestore();

public:
	ResolutionConfirm();
	virtual ~ResolutionConfirm();
};

//////////////////////////////////////////////////////////////////////////
class RoomOptionTypeList : public ioWnd
{
protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;

	ioWnd *m_pOpenBtn;
	int    m_iOverListIndex;
	int    m_iOptionType;

	struct Resolution
	{
		int iValue;
		bool bCheck;
	};
	typedef std::vector< Resolution > EnableResolutionList;
	EnableResolutionList m_ResolutionList;

public:
	void OpenList( int iXPos, int iYPos, ioWnd *pOpenBtn, int iType, int iCurValue );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	RoomOptionTypeList();
	virtual ~RoomOptionTypeList();
};

