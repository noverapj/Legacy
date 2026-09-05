
#ifndef _ioWnd_h_
#define _ioWnd_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

#include "ioWndType.h"
#include "ioUITitle.h"

class ioMouse;
class ioRadioButton;
class ioGUIManager;
class ioDragItem;

class ioXMLElement;
class ioXMLDocument;

class ioUIRenderElement;
class ioUIRenderFrame;
class ioUIRenderImage;
class ioUI3DEffectRender;

#define countof(x) (sizeof(x)/sizeof(x[0]))

#define DECL_NAME_INTERFACE_FOR_TOOL( NickName, ClassName ) \
	virtual const char* GetHeaderFileName() { return __FILE__; }\
	static const char* GetNickName() { static char* ret = #NickName; return ret; }\
	virtual const char* GetTemplateName() { return ClassName::GetNickName(); }\
	virtual const char* GetDefClassName() const { static char* ret = #ClassName; return ret; }

class __EX ioWnd
{
private:
	friend class ioGUIManager;

protected:
	struct Tooltip
	{
		TextAlignType m_TextAlign;
		DWORD		  m_dwTextColor;
		ioHashString  m_szHelp;		
	};

	typedef std::list< ioWnd* > ioWndList;	
	typedef std::vector< ioUIRenderElement* > UIElementList;
	typedef std::map< ioHashString, ioUIRenderImage* > NamedRenderImageMap;
	typedef std::vector< Tooltip > TooltipList;
	typedef std::vector< ioUI3DEffectRender* > ioEffectVec;


	ioHashString m_stWndName;

	DWORD	m_dwID;
	DWORD	m_dwStyle;
    DWORD	m_dwExStyle;
	DWORD	m_dwExStyleParam;

	//프로그램 내부 식별용 고유 인덱스(class name 및 ID로는 식별 할 수 없기때문에 추가)
	DWORD	m_dwInnerUniqueIndex;

	RECT	m_rcPos;
	RECT	*m_pHitRect;
	ioUITitle *m_pUITitle;

	ioWnd	*m_pParent;

	// double linked list...
	ioWnd	*m_pPreModalWnd;
	ioWnd	*m_pNextModalWnd;

	ioWndList	m_ChildList;
	ioEffectVec m_pUI3DEffect;
	UIElementList m_ElementList;
	NamedRenderImageMap m_NamedImageList;
	TooltipList m_TooltipList;

	ioWndList   m_GoToTopReservedChild;
	bool	    m_bNowGoToTopReserving;
	
	bool	m_bShow;
	bool	m_bClicked;
	bool	m_bRightClicked;
	bool	m_bOver;

	bool	m_bCheckParentOutSide;

	ioUIRenderFrame *m_pSelectedFrm;
	ioUIRenderFrame *m_pMultiplyFrm;
	ioUIRenderFrame *m_pSelectedTitleFrm;
	
	std::list<IoUIText*> m_pTextList;

	DWORD m_dwLayer;

	// Template
	typedef std::list< ioXMLElement* > ioTemplateList;
	ioTemplateList	m_TemplateList;
	bool m_bTemplate;

public:
	enum UIPosAlign
	{
		UI_LEFT_ALIGN,
		UI_RIGHT_ALIGN,
		UI_CENTER_ALIGN,
		UI_RATE_ALIGN,
		UI_TOP_ALIGN,
		UI_BOTTOM_ALIGN,
		UI_NONE_ALIGN,
	};

protected:
	UIPosAlign m_XPosAlign;
	UIPosAlign m_YPosAlign;
	int m_iDefXPos;
	int m_iDefYPos;
	int m_iDefWidth;
	int m_iDefHeight;
	bool m_bWidthParent;
	bool m_bHeightParent;

	DWORD m_dwWndAniTime;

	static float m_fScreenRateX;
	static float m_fScreenRateY;

	static DWORD m_dwSelectedID;

	bool m_bFocusParentType;

public:
	static ioWnd *m_pTopModalWnd;

public:
	static void SetScreenRate( float fRateX, float fRateY );

public:
	ioUITitle* CreateUITitle();
	ioUITitle* GetTitle() const { return m_pUITitle; }
	IoUIText* CreateUIText();

	void SetTitleText( const char *szTitle, const char *szDelim = "", int iGap = 0 );
	const char* GetTitleText();

	// 모든 상태의 색을 바꾼다 편의용함수, 특정 상태만 바꾸려면 ioUITitle에 직접 접근
	void SetTitleColor( DWORD dwTextColor, DWORD dwBkColor );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	
	virtual void AddTooltipHelp( const ioHashString &szTipHelp, const ioHashString &szTextAlign, const ioHashString &szTextColor );

	virtual void ParseExtraInfo( ioXMLElement &xElement );

	virtual void AddNamedRenderImage( const ioHashString &szName, ioUIRenderImage *pImage );
	virtual void RemoveNamedRenderImage( const ioHashString &szName );
	virtual ioUIRenderImage* GetNamedRenderImage( const ioHashString &szName ) const;

	virtual void SetWindowAlpha( int iAlpha );

	int GetNumRenderElement() const { return static_cast<int>( m_ElementList.size() ); }
	ioUIRenderElement* GetRenderElement( int index ) { if ( index < 0 || index >= GetNumRenderElement() ) return 0; return m_ElementList[ index ]; }

public:	// Window 상태 변경 함수
	void ShowWnd();
    void HideWnd();
	void ShowChildWnd();
	void HideChildWnd();

	virtual void SetActive();
	virtual void SetInActive();

	void ShowChildWnd( DWORD dwID, bool bMustShow = false );
	void HideChildWnd( DWORD dwID, bool bMustHide = false );
	ioWnd* HideChildByEsc();
	ioWnd* CommandChildBySpaceBar();
	void HideChildKillFocus();

	void SetChildInActive( DWORD dwID );
	void SetChildActive( DWORD dwID );

	void SetChildWndStyleAdd( DWORD dwID, DWORD dwStyle );
	void SetChildWndStyleRemove( DWORD dwID, DWORD dwStyle );

	void SetCheckParentOutSide( bool bCheck );
	inline bool IsCheckParentOutSide() const { return m_bCheckParentOutSide; }

	void ClearRenderImageList();
	virtual void ClearElementImageList();

public:
	void SetID( DWORD dwID ) { m_dwID = dwID; }
	void SetInnerUniqueIndex( DWORD dwInnerUniqueIndex ){ m_dwInnerUniqueIndex = dwInnerUniqueIndex; }

	void SetParent( ioWnd *pWnd ) { m_pParent = pWnd; }
	void SetWndPos( int iXPos, int iYPos );
	void SetScreenWndPos( int iXPos, int iYPos );
	virtual void SetSize( int iWidth, int iHeight );
	void SetWndStyle( DWORD dwStyle ) { m_dwStyle = dwStyle; }
	void SetWndExStyle( DWORD dwExStyle ) { m_dwExStyle = dwExStyle; }
	void SetWndExStyleParam( DWORD dwExParam ) { m_dwExStyleParam = dwExParam; }
	void SetDragRect( const RECT &rcDrag );
	void SetWndRect( const RECT &rcRect );
	void SetFocusRectTypeIsParent( bool b){ m_bFocusParentType = b; }
	bool IsFocusRectTypeIsParent(){ return m_bFocusParentType; }

public:
	void AddWndStyle( DWORD dwStyle );
	void RemoveWndStyle( DWORD dwStyle );

public:
	void SetDefWindowPos( ioXMLElement &rkElement );

	virtual void SetPositionToDefault( int iParentWidth,
									   int iParentHeight,
									   bool bChildMove,
									   bool bCreate = false );

protected:
	UIPosAlign ParseUIPosAlign( const ioHashString &szAlign );

	void SetWndName( const ioHashString &szWndName );

public:
	const ioHashString& GetWndName();

public:	// Interface 함수
	inline DWORD GetID() const { return m_dwID; }
	inline DWORD GetInnerUniqueIndex() const { return m_dwInnerUniqueIndex; }
    inline ioWnd* GetParent(){ return m_pParent; }
	
	inline DWORD GetWndStyle() const { return m_dwStyle; }
	inline DWORD GetWndExStyle() const { return m_dwExStyle; }
	inline DWORD GetWndExStyleParam() const { return m_dwExStyleParam; }

	inline bool HasWndStyle( DWORD dwStyle ) const
	{
		if( m_dwStyle & dwStyle )
			return true;

		return false;
	}

	inline bool HasExWndStyle( DWORD dwExStyle ) const
	{
		if( m_dwExStyle & dwExStyle )
			return true;

		return false;
	}

	inline bool HasExWndStyleParam( DWORD dwExStyleParam ) const
	{
		if( m_dwExStyleParam & dwExStyleParam )
			return true;

		return false;
	}

	inline int GetXPos() const { return m_rcPos.left; }
	inline int GetYPos() const { return m_rcPos.top; }
	inline int GetWidth() const { return m_rcPos.right - m_rcPos.left; }
	inline int GetHeight() const { return m_rcPos.bottom - m_rcPos.top; }

	int GetDerivedPosX() const;
	int GetDerivedPosY() const;
	
	DWORD GetHighParentID() const;

	void GetTooltipHelp( ioWnd::TooltipList &rkTooltipList );

	inline bool IsClicked() const { return m_bClicked; }
	inline bool IsRClicked() const { return m_bRightClicked; }
	inline bool IsOver() const { return m_bOver; }

	inline bool IsHaveSelectedFrm() const 
	{ 
		if( m_pSelectedFrm )
			return true;
		
		return false;
	}
public:
	virtual bool IsNeedProcess() const;
	virtual bool IsCanMouseReaction() const;

	inline bool IsShow() const { return m_bShow; }
	inline bool HasChild() const { return !m_ChildList.empty(); }

	bool IsInWndRect( const RECT& rt );
	bool IsInWndRect( const POINT &vPos );
	bool IsInHitRect( const POINT &vPos );

	int GetZOrderLayer() const;

public:
	virtual void AddChild( ioWnd *pChild );
	virtual void DeleteChild( DWORD dwID );
    virtual void DeleteAllChild();

public:
	int GetNumChildren() const { return m_ChildList.size(); }
	ioWnd* GetChild( int iIndex );
	ioWnd* FindChildWnd( DWORD dwID );
	ioWnd* FindChildWnd( const ioHashString& szWndName );
	ioWnd* FindChildWndEX( DWORD dwID, bool all = false ); //! all 이 true면 m_exceptfromid 를 무시한다. 주의 : 실시간 사용 금지!! ( 하위 모든 자식을 검색해서 자식을 얻어온다. id가 중복되면 빨리찾는걸 리턴한다.) 
	void ExceptFromID( bool seg ) { m_exceptfromid = seg; } // FindChildWndEX할때 만일 m_exceptfromid 가 true이면 해당 윈도우 이하는 Skip한다. 
	bool IsExceptFromID() const { return m_exceptfromid; }
	bool m_exceptfromid;
	
	IoUIText*	GetText(ioHashString name);

public:
	void MoveZorderLayer( DWORD dwWndID, DWORD dwMoveID );       // dwWndID 뒤에 dwMoveID를 위치 시킨다.
	void MoveZorderLayer2( DWORD dwWndID, DWORD dwMoveID );		// 기존의 MoveZorderLayer와 약간 동작방식이 다르다.
	void MoveZorderTop( DWORD id );	// ZOrder를 제일 위로 올린다.

public:
	void ProcessWnd( float fTimePerSec, ioMouse& mouse );

	void CheckRadioButton( DWORD dwMinID, DWORD dwMaxID, DWORD dwCheckID );
	void GoToTop();
	void GoToTopWnd();

protected:
	void ReserveChildGoToTop( ioWnd *pChild );
	void ExecuteReservedChildGoToTop();
	void MoveToTop( ioWnd *pChild );

	void HeadInsertToModalList();
	void EraseInModalList();

protected:
	virtual bool OnRenderShowAndHide();
	virtual bool OnRenderShowAndHideByDefault( DWORD dwCheckTime );
	virtual bool OnRenderShowAndHideByTop( DWORD dwCheckTime );
	virtual bool OnRenderShowAndHideByDown( DWORD dwCheckTime );

	virtual void OnRender();
	virtual void OnRenderAfterChild();
	virtual void OnProcess( float fTimePerSec );
	virtual void OnMouseProcess( const ioMouse& mouse ){};

	virtual void CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight );
	virtual void RenderImageList( int iXPos, int iYPos, UIRenderType eRenderType );

public:	// 메세지 처리
	virtual void iwm_create();
	virtual void iwm_destroy();
	virtual void iwm_lbuttondown( const ioMouse& mouse );
	virtual void iwm_lbuttonup( const ioMouse& mouse );
	virtual void iwm_lbuttonDBLCLK( const ioMouse& mouse );
	virtual void iwm_rbuttondown( const ioMouse& mouse );
	virtual void iwm_rbuttonup( const ioMouse& mouse );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_mouseover( const ioMouse& mouse );
	virtual void iwm_mousemove( const ioMouse& mouse );
	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();
	virtual bool iwm_spacebar();
	virtual void iwm_gototop();           //UI가 상위로 이동되었다

	virtual IWDropEffect iwm_dragenter( ioDragItem *pItem, const ioMouse& mouse );
	virtual IWDropEffect iwm_dragover( ioDragItem *pItem, const ioMouse& mouse );
	virtual void iwm_dragleave();
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );
	virtual void iwm_dropdone( ioDragItem *pItem, ioWnd *pDroppedWnd, bool bDropped );

public:
	virtual bool IsEXType() const { return false; }
	virtual DWORD GetWndType() const;
	virtual void RenderWnd();

	static DWORD GetSelectedID() { return m_dwSelectedID; }
	static void SetSelectedID(DWORD dwSelectedID) { m_dwSelectedID = dwSelectedID; }
	
public:
	__forceinline int GetDefaultPosX() const { return m_iDefXPos; }	
	__forceinline int GetDefaultPosY() const { return m_iDefYPos; }
	__forceinline int GetDefaultWidth()  const { return m_iDefWidth; }
	__forceinline int GetDefaultHeight() const { return m_iDefHeight; }

	//************************************************************************************************************************
	// Tool을 위한 인터페이스들		
	virtual void SetIDEX( DWORD& dwID, bool onlychild ); // 확장 ID 셋팅 함수 : 자식까지 전부 unique하게 아이디를 지정해주고... 그 최대값을 저장해준다.
	virtual void ParseXML( ioXMLElement& xml ) {}
#define CLONEFRAME( wnd, frame ) { if ( wnd->frame ) SAFEDELETE( wnd->frame ); if (frame) { wnd->frame = dynamic_cast<ioUIRenderFrame*>( frame->Clone() ); } }
#define CLONEIMAGE( wnd, image ) { if ( wnd->image ) SAFEDELETE( wnd->image ); if (image) { wnd->image = dynamic_cast<ioUIRenderImage*>( image->Clone() ); } }
	DECL_NAME_INTERFACE_FOR_TOOL( Wnd, ioWnd )
	ioWnd* Clone( bool ischilds );
	virtual ioWnd* _Clone() { return new ioWnd(); }
	virtual void Copy( ioWnd* wnd );
	void CopyChilds( ioWnd* wnd );
	bool IsParent( DWORD id );
	std::string m_idname;
	void SetIDName( const char* name ) { if ( name ) m_idname = name; }	
	const char* GetIDName() const { return m_idname.c_str(); }
	std::string m_membername; // 멤버 변수명
	void SetMemberName( const char* name ) { if ( name ) m_membername = name; }
	const char* GetMemberName() const { return m_membername.c_str(); }
	std::string m_classname;	
	std::string m_subclassname;
	void SetClassName( const char* name );
	const char* GetClassNameEX() const;
	void SetSubClassName( const char* name );
	const char* GetSubClassName() const;
	int UpChild( DWORD id );
	int DownChild( DWORD id );
	// Save Util
	static std::string ConvertToAlignString( ioWnd::UIPosAlign align, const std::string& def );
	static std::string ConvertToStyleString( TextStyle textstyle );
	static std::string ConvertToHorzAlignString( TextAlignType type );
	static std::string ConvertToVertAlignString( TextVertAlign type );
	static std::string ConvertToColorString( DWORD color );
	static void SetFrame( ioXMLElement& xml, ioUIRenderFrame* frame, const char* type );
	static void SetImagery( ioXMLElement& xml, ioUIRenderImage* img, const char* type );
	// Save
	bool Save( const char* filename );
	virtual bool Save( ioXMLElement& xml );
	virtual void SetSaveData( ioXMLElement& xml );
	void SaveChild( ioXMLElement& xml );
	void SaveSize( ioXMLElement& xml );
	void SaveStyle( ioXMLElement& xml );
	void SaveExStyle( ioXMLElement& xml );
	void SaveTitle( ioXMLElement& xml );
	void SaveToolTip( ioXMLElement& xml );
	void SaveDragRect( ioXMLElement& xml );
	ioXMLElement SaveImagery( ioXMLElement& xml );	
	// Size ( Set )
	void RecalcSize() { SetSize( GetWidth(), GetHeight() ); }
	void Layout();
	virtual void SetPosX( int x ) { SetWndPos( x, GetYPos() ); }
	virtual void SetPosY( int y ) { SetWndPos( GetXPos(), y); }
	virtual void SetWidth( int w ) { SetSize( w, GetHeight() ); }
	virtual void SetHeight( int h ) { SetSize( GetWidth(), h ); }
	__forceinline void SetHorzAlign( long a ) { m_XPosAlign = static_cast<UIPosAlign>(a); Layout(); }
	__forceinline void SetVertAlign( long a ) { m_YPosAlign = static_cast<UIPosAlign>(a); Layout(); }
	// Size ( Get )
	__forceinline int GetPosX() const { return GetXPos(); }	
	__forceinline int GetPosY() const { return GetYPos(); }
	__forceinline int GetDefWidth() const { return GetWidth(); }
	__forceinline int GetDefHeight() const { return GetHeight(); }
	__forceinline long GetHorzAlign() const { return m_XPosAlign; }
	__forceinline long GetVertAlign() const { return m_YPosAlign; }	
	// Title ( Set )
	__forceinline void CreateUT() { if ( !m_pUITitle ) { CreateUITitle(); } }
	__forceinline void SetTitleTextEX( const char* t ) { CreateUT(); m_pUITitle->SetText( t, "#" ); }
	__forceinline void SetTitleOffsetX( int x ) { CreateUT(); m_pUITitle->SetOffsetX( x ); }
	__forceinline void SetTitleOffsetY( int y ) { CreateUT(); m_pUITitle->SetOffsetY( y ); }
	__forceinline void SetTitleTextSize( int s ) { CreateUT(); m_pUITitle->SetSize( s ); }
	__forceinline void SetTitleVertGap( int g ) { CreateUT(); m_pUITitle->SetVertGap( g ); }
	__forceinline void SetTitleTextHorzAlign( int a ) {
		CreateUT(); 
		m_pUITitle->SetHorzAlign( static_cast<TextAlignType>(a) ); 
	}
	__forceinline void SetTitleTextVertAlign( int a ) {
		CreateUT();
		m_pUITitle->SetVertAlign( static_cast<TextVertAlign>(a) ); 
	}
	__forceinline void SetTitleTextStyle( int s ) { CreateUT(); m_pUITitle->SetStyle( static_cast<TextStyle>(s) ); }
	__forceinline void SetTitleTextDisableStyle( int s ) { CreateUT(); m_pUITitle->SetDisableStyle( static_cast<TextStyle>(s) ); }
	__forceinline void SetTitleNormalTextColor( DWORD c ) { CreateUT(); m_pUITitle->SetNormalTitleTextColor( c ); }
	__forceinline void SetTitleNormalBKColor( DWORD c ) { CreateUT(); m_pUITitle->SetNormalTitleBKColor( c ); }
	__forceinline void SetTitleOverTextColor( DWORD c ) { CreateUT(); m_pUITitle->SetOverTitleTextColor( c ); }
	__forceinline void SetTitleOverBKColor( DWORD c ) { CreateUT(); m_pUITitle->SetOverTitleBKColor( c ); }
	__forceinline void SetTitlePushTextColor( DWORD c ) { CreateUT(); m_pUITitle->SetPushTitleTextColor( c ); }
	__forceinline void SetTitlePushBKColor( DWORD c ) { CreateUT(); m_pUITitle->SetPushTitleBKColor( c ); }
	__forceinline void SetTitleDisableTextColor( DWORD c ) { CreateUT(); m_pUITitle->SetDisableTitleTextColor( c ); }
	__forceinline void SetTitleDisableBKColor( DWORD c ) { CreateUT(); m_pUITitle->SetDisableTitleBKColor( c ); }
	__forceinline void SetTitleNoneSelectedTextColor( DWORD c ) { CreateUT(); m_pUITitle->SetNoneSelectedTitleTextColor( c ); }
	__forceinline void SetTitleNoneSelectedBKColor( DWORD c ) { CreateUT(); m_pUITitle->SetNoneSelectedTitleBKColor( c ); }
	// Title ( Get )
#define CHECK_TITLE(r) if(!m_pUITitle)return r;
	__forceinline const char* GetTitleTextEX() const { CHECK_TITLE(0); return m_pUITitle->GetText(); }
	__forceinline int GetTitleOffsetX() const { CHECK_TITLE(0); return m_pUITitle->GetTextXOffSet(); }
	__forceinline int GetTitleOffsetY() const { CHECK_TITLE(0); return m_pUITitle->GetTextYOffSet(); }
	__forceinline int GetTitleTextSize() const { CHECK_TITLE(0); return m_pUITitle->GetSize(); }
	__forceinline int GetTitleVertGap() const { CHECK_TITLE(0); return m_pUITitle->GetVertGap(); }
	__forceinline TextAlignType GetTitleTextHorzAlign() const { CHECK_TITLE(TAT_LEFT); return m_pUITitle->GetHorzAlign(); }
	__forceinline TextVertAlign GetTitleTextVertAlign() const { CHECK_TITLE(TVA_TOP); return m_pUITitle->GetVertAlign(); }
	__forceinline TextStyle GetTitleTextStyle() const { CHECK_TITLE(TS_NORMAL); return m_pUITitle->GetTextStyle(); }
	__forceinline TextStyle GetTitleTextDisableStyle() const { CHECK_TITLE(TS_NORMAL); return m_pUITitle->GetDisableTextStyle(); }
	__forceinline DWORD GetTitleNormalTextColor() const { CHECK_TITLE(0); return m_pUITitle->GetNormalTitleColor().dwText; }
	__forceinline DWORD GetTitleNormalBKColor() const { CHECK_TITLE(0); return m_pUITitle->GetNormalTitleColor().dwBk; }
	__forceinline DWORD GetTitleOverTextColor() const { CHECK_TITLE(0); return m_pUITitle->GetOverTitleColor().dwText; }
	__forceinline DWORD GetTitleOverBKColor() const { CHECK_TITLE(0); return m_pUITitle->GetOverTitleColor().dwBk; }
	__forceinline DWORD GetTitlePushTextColor() const { CHECK_TITLE(0); return m_pUITitle->GetPushTitleColor().dwText; }
	__forceinline DWORD GetTitlePushBKColor() const { CHECK_TITLE(0); return m_pUITitle->GetPushTitleColor().dwBk; }
	__forceinline DWORD GetTitleDisableTextColor() const { CHECK_TITLE(0); return m_pUITitle->GetDisableTitleColor().dwText; }
	__forceinline DWORD GetTitleDisableBKColor() const { CHECK_TITLE(0); return m_pUITitle->GetDisableTitleColor().dwBk; }
	__forceinline DWORD GetTitleNoneSelectedTextColor() const { CHECK_TITLE(0); return m_pUITitle->GetNoneSelectedTitleColor().dwText; }
	__forceinline DWORD GetTitleNoneSelectedBKColor() const { CHECK_TITLE(0); return m_pUITitle->GetNoneSelectedTitleColor().dwBk; }
	// Style ( Set )
	__forceinline void AddStyle( DWORD style, bool add ) { if ( add ) { m_dwStyle |= style; } else { m_dwStyle &= ~style; } }
	__forceinline void AddExStyle( DWORD style, bool add ) { if ( add ) { m_dwExStyle |= style; } else { m_dwExStyle &= ~style; } }
	// Style ( Get )
	__forceinline DWORD GetStyle() const { return m_dwStyle; }
	__forceinline DWORD GetExStyle() const { return m_dwExStyle; }
	// Tooltip ( Set )	
	void AddTooltip( const char* text, int align, DWORD color );
	void SetTooltip( const char* text, int align, DWORD color );
	void SetTooltipText( const char* text );
	void SetTooltipAlign( int align );
	void SetTooltipColor( DWORD color );
	// Tooltip ( Get )
	__forceinline int GetNumToolTip() const { return static_cast<int>(m_TooltipList.size()); }
	__forceinline const char* GetTooltipText() const { static const char* ret = ""; if ( GetNumToolTip() <= 0 ) { return ret; } return m_TooltipList[ 0 ].m_szHelp.c_str(); }
	__forceinline TextAlignType GetTooltipAlign() const { if ( GetNumToolTip() <= 0 ) { return TAT_LEFT; } return m_TooltipList[ 0 ].m_TextAlign; }
	__forceinline DWORD GetTooltipColor() const { if ( GetNumToolTip() <= 0 ) { return 0; } return m_TooltipList[ 0 ].m_dwTextColor; }
	// Rect ( Set )
	__forceinline void CreateDragRect() { if ( !m_pHitRect ) m_pHitRect = new RECT; }
	__forceinline void SetDragRectLeft( int l ) { CreateDragRect(); m_pHitRect->left = l; }
	__forceinline void SetDragRectTop( int t ) { CreateDragRect(); m_pHitRect->top = t; }
	__forceinline void SetDragRectRight( int r ) { CreateDragRect(); m_pHitRect->right = r; }
	__forceinline void SetDragRecBottom( int b ) { CreateDragRect(); m_pHitRect->bottom = b; }
	// Rect ( Get )
#define CHECK_RECT(r) if(!m_pHitRect)return r;
	__forceinline int GetDragRectLeft() const { CHECK_RECT(0); return m_pHitRect->left; }
	__forceinline int GetDragRectTop() const { CHECK_RECT(0); return m_pHitRect->top; }
	__forceinline int GetDragRectRight() const { CHECK_RECT(0); return m_pHitRect->right; }
	__forceinline int GetDragRectBottom() const { CHECK_RECT(0); return m_pHitRect->bottom; }
public:
	// RenderElement ( common )
	void GetRenderElementIterator( int i, int type, UIElementList::iterator& iter );
	ioUIRenderElement** GetRenderElement( int i, int type );
	int GetNumRenderElement( int type );
	void RemoveRenderElementList( int i, int type );
public:
	// Renderframe ( virtual / common )
	virtual int GetNumFixedRenderFrame() const;
	virtual const char* GetFixedRenderFrameType( int index );
	virtual ioUIRenderElement** GetRenderFrame( const char* type );
	ioUIRenderElement** GetRenderFrame( const char* type, int i );
	void GenerateFrameList( int i );
	// Renderframe ( Set )
	void SetFrameRenderRectX( const char* type, int i, int x );
	void SetFrameRenderRectY( const char* type, int i, int y );
	void SetFrameRenderRectWidth( const char* type, int i, int w );
	void SetFrameRenderRectHeight( const char* type, int i, int h );
	void SetFrameRenderColor( const char* type, int i, DWORD color );
	void SetFrameRenderAutoResize( const char* type, int i, bool b );
	void SetFrameRenderTemplete( const char* type, int i, const char* templete, ioGUIManager* mgr );
	// Renderframe ( Get )	
	int GetFrameRenderRectX( const char* type, int i );
	int GetFrameRenderRectY( const char* type, int i );
	int GetFrameRenderRectWidth( const char* type, int i );
	int GetFrameRenderRectHeight( const char* type, int i );
	DWORD GetFrameRenderColor( const char* type, int i );
	bool GetFrameRenderAutoResize( const char* type, int i );
	const char* GetFrameRenderTemplete( const char* type, int i );	
public:
	// Renderimage ( virtual / common )
	virtual int GetNumFixedRenderImage() const;
	virtual const char* GetFixedRenderImageType( int index );
	virtual ioUIRenderElement** GetRenderImage( const char* type );
	ioUIRenderElement** GetRenderImage( const char* type, int i );
	void GenerateImageList( int i );
	// Renderimage ( Set )
	void SetRenderImageRectX( const char* type, int i, int x );
	void SetRenderImageRectY( const char* type, int i, int y );
	void SetRenderImageRectWidth( const char* type, int i, int w );
	void SetRenderImageRectHeight( const char* type, int i, int h );
	void SetRenderImageColor( const char* type, int i, DWORD color );		
	void SetRenderImageFilename( const char* type, int i, const char* filename, ioGUIManager* mgr );
	void SetRenderImageImageName( const char* type, int i, const char* setname, const char* name, ioGUIManager* mgr );
	// Renderimage ( Get )	
	int GetRenderImageRectX( const char* type, int i );
	int GetRenderImageRectY( const char* type, int i );
	int GetRenderImageRectWidth( const char* type, int i );
	int GetRenderImageRectHeight( const char* type, int i );
	DWORD GetRenderImageColor( const char* type, int i );
	bool GetRenderImageIsFromFile( const char* type, int i );
	const char* GetRenderImageFilename( const char* type, int i );
	const char* GetRenderImageImageSetName( const char* type, int i );
	const char* GetRenderImageImageName( const char* type, int i );
protected:
	// Renderimage ( Set )	
	void SetRenderImageRectX( ioUIRenderElement** img, int x );
	void SetRenderImageRectY( ioUIRenderElement** img, int y );
	void SetRenderImageRectWidth( ioUIRenderElement** img, int w );
	void SetRenderImageRectHeight( ioUIRenderElement** img, int h );
	void SetRenderImageColor( ioUIRenderElement** img, DWORD color );	
	void SetRenderImageFilename( ioUIRenderElement** img, int i, const char* filename, ioGUIManager* mgr );
	void SetRenderImageImageName( ioUIRenderElement** img, int i, const char* setname, const char* name, ioGUIManager* mgr );
	void _SetRenderImageImage( ioUIRenderElement** img, int i, const char* filename, const char* setname, const char* name, ioGUIManager* mgr );
	// Renderimage ( Get )		
	int GetRenderImageRectX( ioUIRenderElement** img );
	int GetRenderImageRectY( ioUIRenderElement** img );
	int GetRenderImageRectWidth( ioUIRenderElement** img );
	int GetRenderImageRectHeight( ioUIRenderElement** img );
	DWORD GetRenderImageColor( ioUIRenderElement** img );
	bool GetRenderImageIsFromFile( ioUIRenderElement** img );
	const char* GetRenderImageFilename( ioUIRenderElement** img );
	const char* GetRenderImageImageSetName( ioUIRenderElement** img );
	const char* GetRenderImageImageName( ioUIRenderElement** img );
	//************************************************************************************************************************

public:   // Child Wnd Show ID List Log
	void ChildWndShowIDList();

	//ui3dEffect 추가
	virtual void AddEffect( const ioHashString &szType, ioUI3DEffectRender *pUI3DRender );
	virtual ioUI3DEffectRender* GetEffect(const ioHashString szType);	
	void RenderEffect( int iRenderType );
	void RenderText( int iRenderPos );

	//바인드기능추가
	struct BINDSTRUCT;
	typedef std::tr1::function<int(BINDSTRUCT)> FUNC;
	struct BINDSTRUCT
	{
		int iValue;
		void* pVoid;
		FUNC func;
		BINDSTRUCT() : iValue(0),pVoid(NULL){};
	};
	void SetBindFunction( FUNC func, DWORD dwMessage );
	void SetBindFunction( FUNC func, DWORD dwMessage, BINDSTRUCT sData );
	void UpdateBindFunction( DWORD dwMessage );	

	bool m_bBind;
	bool IsBind(){ return m_bBind; }
	std::map<DWORD,BINDSTRUCT> m_BindMap;

	void SetLayer( DWORD dwLayer ) { if( dwLayer > 0 ) m_dwLayer = dwLayer; }
	DWORD GetLayer() const { return m_dwLayer; }

	// Template
public:
	void AddTemplate( ioXMLElement *pElement );
	void DeleteTemplateChild();
	
	ioXMLElement* FindTemplate( ioHashString &szClassName, DWORD dwFindID );

	bool IsTemplateChild()			{ return m_bTemplate; }
	void SetTemplateChild( bool b )	{ m_bTemplate = b; }

public:
	ioWnd();
	virtual ~ioWnd();
};

#endif
