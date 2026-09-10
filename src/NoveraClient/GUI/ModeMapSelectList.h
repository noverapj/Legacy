#pragma once

class ioWnd;
class ioXMLElement;
class ioUIRenderImage;

class ModeMapSelectList : public ioWnd
{
public:
	enum
	{	
		ID_MODE_HELP_TOOLTIP = 500,
		ID_VERT_SCROLL	     = 1000,	
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;
	ioUIRenderImage *m_pManualMark;

	ioWnd *m_pOpenBtn;
	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iStartXPos;
	int   m_iStartYPos;
	int   m_iCutWidth;
	int   m_iMaxPageList;
	int   m_iManualMarkX;
	int   m_iManualMarkWidth;

	int   m_iCurPos;	
	int   m_iSelectIndex;
	bool  m_bSafetyLevelRoom;
	int   m_iOverListIndex;
	int   m_iOverMarkListIndex;
	bool  m_bOverSafetyLevel;
	bool  m_bModeSelect;
	bool  m_bAI;
	int   m_iBackupWidth;

	struct SelectList
	{
		int			 m_iIndex;
		int          m_iHelpIndex;
		int          m_iManualIndex;
		bool		 m_bCheck;
		bool         m_bSafetyLevelRoom;
		ioHashString m_szTitle;
		ioUIRenderImage *m_pMarkRes;
		SelectList()
		{
			m_iIndex = -1;
			m_iHelpIndex = -1;
			m_iManualIndex = -1;
			m_bCheck = false;
			m_bSafetyLevelRoom = false;
			m_pMarkRes = NULL;
		}
	};
	typedef std::vector< SelectList > EnableSelectList;
	EnableSelectList m_vEnableSelectList;

protected:
	ioWnd        *m_pModeHelpTooltip;

public:
	virtual void OpenList( ioWnd *pOpenBtn, bool bModeSelect, int iSelectIndex, bool bSafetyLevelRoom, int iStartXPos, int iStartYPos, bool bAI = false );
	virtual void AddList( int iIndex, bool bSafetyLevelRoom, const ioHashString &rkName, int iHelpIndex, int iManualIndex, ioUIRenderImage *pMarkRes, int iAddWidth = 0 );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	void ProcessModeHelpTooltip();	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioWnd *GetOpenBtn(){ return m_pOpenBtn; }

public:
	bool GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos );

public:
	ModeMapSelectList();
	virtual ~ModeMapSelectList();
};
//////////////////////////////////////////////////////////////////////////
class CreateModeSelectList : public ioWnd
{
public:
	enum
	{	
		ID_MODE_HELP_TOOLTIP = 500,
		ID_VERT_SCROLL	     = 1000,	
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;
	ioUIRenderImage *m_pManualMark;

	ioWnd *m_pOpenBtn;
	int   m_iAddWidth;
	int   m_iAddHeight;
	int   m_iListXPos;
	int   m_iListYPos;
	int   m_iHeightGap;
	int   m_iCutWidth;
	int   m_iManualMarkX;
	int   m_iManualMarkWidth;
	int   m_iMaxPageList;

	int   m_iCurPos;	
	int   m_iOverListIndex;
	int   m_iOverMarkListIndex;
	struct SelectList
	{
		int			 m_iIndex;
		int          m_iHelpIndex;
		int          m_iManualIndex;
		ioHashString m_szTitle;
		ioUIRenderImage *m_pMarkRes;
		SelectList()
		{
			m_iIndex = -1;
			m_iHelpIndex = m_iManualIndex = -1;
			m_pMarkRes = NULL;
		}
	};
	typedef std::vector< SelectList > EnableSelectList;
	EnableSelectList m_vEnableSelectList;

protected:
	ioWnd        *m_pModeHelpTooltip;

public:
	virtual void OpenList( ioWnd *pOpenBtn );
	virtual void AddList( int iIndex, const ioHashString &rkName, int iHelpIndex, int iManualIndex, ioUIRenderImage *pMarkRes );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

protected:
	void ProcessModeHelpTooltip();	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioWnd *GetOpenBtn(){ return m_pOpenBtn; }

public:
	bool GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos );

public:
	CreateModeSelectList();
	virtual ~CreateModeSelectList();
};

