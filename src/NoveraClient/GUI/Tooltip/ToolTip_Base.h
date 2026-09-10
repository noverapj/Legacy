
#ifndef __TOOLTIP_COMMON_H__
#define __TOOLTIP_COMMON_H__

class ioWnd;

enum TOOLTIP_CONTENTS_TYPE
{
    TTCT_TITLE = 0,
    TTCT_SUBTITLE,
    TTCT_COMMENT,

    TTCT_COUNT
};

typedef std::map<TOOLTIP_CONTENTS_TYPE, IoString*> TOOLTIP_STRING_MAP;

class ToolTip_Base : public ioWnd
{
public:
	ToolTip_Base();
	virtual ~ToolTip_Base();

	virtual void		iwm_create();
	virtual void		iwm_show();
	virtual void		iwm_hide();
	virtual void		OnOpen(bool bIsDirectOpen, int _x, int _y, const char* szIconName, const char* szTitle, const char* szSubTitle, const char* szComment);
	virtual void		OnShow();

	virtual void		SetIcon(const char* szIconName);
	virtual void		SetTitle(const char* szTitle);
	virtual void		SetSubTitle(const char* szSubTitle);
	virtual void		SetComment(const char* szComment);

	virtual void		SetTitleColor( DWORD dwTextColor, DWORD dwBkColor );
	virtual void		SetSubTitleColor( DWORD dwTextColor, DWORD dwBkColor );
	virtual void		SetCommentColor( DWORD dwTextColor, DWORD dwBkColor );

	virtual	void		CalculatorPos();
	virtual void		MoveLeftWnd();
	virtual void		MoveUpWnd();
	virtual void		MoveUpWnd( int iGapY );

	virtual void		AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void		ParseExtraInfo( ioXMLElement &xElement );

	virtual void		OnRender();

	virtual void		SetFixedPos( int iPosX, int iPosY );
	virtual void		SetFixed(bool bFixed) { m_bFixedPos = bFixed; }
	virtual void		SetReverse(bool bReverse) { m_bReverse = bReverse; }

    virtual IoString*   GetString(TOOLTIP_CONTENTS_TYPE eType);
    virtual void        SetStringSize(TOOLTIP_CONTENTS_TYPE eType, float fSize);

    virtual void        SetReinforce(int iReinforce) {}

protected:
	ioUIRenderFrame*	m_pBackGround;
	ioUIRenderImage*	m_pIcon;

	IoString*			m_pTitle;
	IoString*			m_pSubTitle;
	IoString*			m_pComment;

	int					m_nFrameGap;
	int					m_nItemGap;
	int					m_nTitleWidth;
	int					m_nSubTitleWidth;
	int					m_nCommentWidth;

	int					m_eToolTipType;

	int					m_nBase_X;
	int					m_nBase_Y;

	int					m_nIconRenderPos_x;
	int					m_nIconRenderPos_y;

	bool				m_bFixedPos;
	int					m_iFixedPosX;
	int					m_iFixedPosY;
	bool				m_bReverse;

    TOOLTIP_STRING_MAP  m_kToolTipStringMap;
};

#endif
