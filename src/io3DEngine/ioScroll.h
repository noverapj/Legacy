

#ifndef _ioScroll_h_
#define _ioScroll_h_

class ioWnd;

class __EX ioSlide : public ioWnd
{
protected:
	ioUIRenderImage *m_pSlideIcon;

public:
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );

public:
	void ReSizeSlide( int iWidth, int iHeight );

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	virtual void SetSaveData( ioXMLElement& xml );
	DECL_NAME_INTERFACE_FOR_TOOL( Slide, ioSlide )
	virtual ioWnd* _Clone() { return new ioSlide(); }
	virtual void Copy( ioWnd* wnd );
	virtual int GetNumFixedRenderImage() const;
	virtual const char* GetFixedRenderImageType( int index );	
	virtual ioUIRenderElement** GetRenderImage( const char* type );
	//************************************************************************************************************************

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

public:
	ioSlide();
	virtual ~ioSlide();
};

class __EX ioScroll : public ioWnd
{
public:
	enum          // 스크롤 윈도우 예약 ID
	{
		ID_SCROLL_UP	= 1,
		ID_SCROLL_DOWN	= 2,
		ID_SCROLL_THUMB = 3,
	};
	
protected:
	int		m_iMinPos;
	int		m_iMaxPos;
	int		m_iCurPos;
	int     m_iPageRange;
	RECT    m_rcSlide;
	DWORD   m_dwStartTime;
	DWORD   m_dwCurTime;
	DWORD   m_dwCheckTime;
	float   m_dwScrollTime;

	ioWnd   *m_pScrollUpNDown;
	
public:
	virtual void SetScrollRange( int iMinPos, int iMaxPos );
	virtual void SetPageRange( int iPageRange ){ m_iPageRange = iPageRange; }
	virtual void SetScrollPos( int iPos );
	virtual void SetScrollSlide( ioWnd *pSlide );

	virtual void SetPrePage();
	virtual void SetNextPage();
	
	inline int GetCurPos() const { return m_iCurPos; }
	inline int GetMinPos() const { return m_iMinPos; }
	inline int GetMaxPos() const { return m_iMaxPos; }
	inline int GetPageRange() const { return m_iPageRange; }

public:
	float GetScrollRate() const;
	
public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_lbuttondown( const ioMouse& mouse );

public:
	virtual void SetActive();
	virtual void SetInActive();
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate );
	
public:
	virtual DWORD GetWndType() const;

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들
	virtual void SetSaveData( ioXMLElement& xml );
	DECL_NAME_INTERFACE_FOR_TOOL( Scroll, ioScroll )
	virtual ioWnd* _Clone() { return new ioScroll(); }
	virtual void Copy( ioWnd* wnd );
	void SetStartTime( int t ) { m_dwStartTime = t; }
	void SetCurTime( int t ) { m_dwCurTime = t; }
	int GetStartTime() const { return m_dwStartTime; }
	int GetCurTime() const { return m_dwCurTime; }
	//************************************************************************************************************************
	
public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	ioScroll();
	virtual ~ioScroll();
};

#endif

