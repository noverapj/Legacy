#ifndef _ioGUIManager_h_
#define _ioGUIManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioWnd.h"
#include "ioMouse.h"
#include "ioResourceManager.h"

class ioXMLElement;
class ioDragItem;
class ioUIImageSetManager;
class ioUIFrameManager;

class ioUIRenderImage;
class ioUIRenderFrame;

#define ID_DESKTOP	-1

class __EX ioGUIManager
{
protected:
	ioUIImageSetManager *m_pUIImageSetMgr;
	ioUIFrameManager *m_pUIFrameMgr;

	ioWnd	*m_pDeskTopWnd;
	ioWnd	*m_pPreOverWnd;
	ioWnd	*m_pPreClickWnd;
	ioWnd	*m_pPreRightClickWnd;

	ioWnd	*m_pCurDragMoveWnd;
	POINT	m_ptDragMovePos;

	enum DragCheckState
	{
		DCS_NONE,
		DCS_CHECK,
		DCS_DRAGGING
	};

	DragCheckState	m_DragCheckState;
	RECT	m_rcDragCheck;
	RECT	m_rcStartDrag;
	POINT	m_ptDragOffset;
	DWORD	m_dwDragCheckStartTime;
	DWORD	m_dwDragCheckDuration;
	ioDragItem *m_pDragItem;

	ioWnd	*m_pPreDragOverWnd;
	ioWnd	*m_pDragStartWnd;

	ioHashString m_szStringMgrKeyName;
	ioHashString m_szChangeStringMgrKeyName;

	POINT m_LastMousePos;
	bool m_isLButtonDown;
	DWORD	m_dwInnerUniqueIndexCounter;

	std::vector<int> m_vToolTipWndID;
	std::vector<ioWnd*> m_vToolTipWnd;

protected:
	typedef std::map< ioHashString, DWORD > MemoryMap;
	MemoryMap m_MemoryMap;
	StaticSwitch m_StaticSwitch;

public:
	void AddMem( const ioHashString& KeyName, DWORD dwMem );
	void RemoveMem( const ioHashString& KeyName );
	void GetGUIMemStatics( DWORD& dwCount, DWORD& dwSize );
	void GetFrameMemStatics( DWORD& dwCount, DWORD& dwSize );
	void GetImageSetMemStatics( DWORD& dwCount, DWORD& dwSize );
	void SetCurDragMoveWnd( ioWnd *pWnd, POINT ptMouse );
	void SetSwitch( int eSwitch );

public:
	virtual void InitManager();
	virtual void InitDeskTop( int iWidth, int iHeight );
	virtual void UpdateGUI( ioMouse &mouse );
	virtual void RenderGUI();
	virtual void ClearAllGUI();
	virtual void RenderToolTip();

	ioWnd* GetDeskTop() const;
	void ClearPreState();

	const POINT& GetLastMousePos() const { return m_LastMousePos; }
	bool IsLastLButtonDown() const { return m_isLButtonDown; }

	ioWnd* CheckMouse( ioWnd *pWnd, const POINT& pos );
	void CheckMouse( ioWnd *pWnd, const RECT& rt, ioWnd** wins, int& index );

protected:
	void LoadImageSetFile( const char *szFileName );
	void LoadFrameFile( const char *szFileName );

protected:
	virtual void ProcessGUI( ioMouse& mouse );

	bool CheckMouse( ioWnd *pWnd, ioMouse &mouse );
	bool CheckMouseModal( ioWnd *pWnd, ioMouse &mouse );	

	bool CheckDragStart( ioMouse &mouse );
	bool CheckDragMouse( ioWnd *pWnd, ioMouse &mouse );

	bool CheckEditFocus( ioMouse &mouse );

	void ProcessDragMoveWnd( ioMouse &mouse );
	void InTargetWndProcess( ioWnd *pWnd, ioMouse& mouse );

	virtual void ChangeMouseStateToOver( ioWnd *pWnd, ioMouse &mouse );

public:
	void DoDragDrop( ioWnd *pStartWnd,
					 ioDragItem *pDragItem,
					 const RECT &rcRect,
					 const POINT &ptOffset,
					 const RECT *pRectStartDrag );

	void SetDragCheckDuration( DWORD dwDuration );

protected:
	POINT GetDragCheckPoint( ioMouse &mouse );
	void ClearDragState();

public:
	void AddDeskTopChild( ioWnd *pWnd );

	ioWnd* AddWnd( const char *szXMLFileName );
	ioWnd* AddWnd( const char *szXMLFileName, ioWnd *pParent );
	ioWnd* AddWndByTemplate( ioHashString &szClassName, ioWnd *pParent, DWORD dwTPID, DWORD dwTemplateID = -1 );
	ioWnd* FindWnd( DWORD dwID );	
	ioWnd* FindWnd( const ioHashString& szWndName );

	void DeleteWnd( DWORD dwID );

	bool IsShow( DWORD dwID );
	void ShowWnd( DWORD dwID, bool bMustShow = false );
	void HideWnd( DWORD dwID, bool bMustHide = false );

	ioUIRenderFrame* CloneFrame( const char* templete );
	ioUIRenderImage* CreateImageByFile( const char* filename );
	ioUIRenderImage* CreateImage( const char* imageset, const char* image );

	// image
	int GetNumImageSet() const;	
	const char* GetImageSetName( int index );
	const char* GetImageSetFileName( const char* imagesetname );
	IDirect3DTexture9* GetImageSetTexture( const char* imagesetname );
	int GetNumImage( const char* imagesetname ) const;
	const char* GetImageTagName( const char* imagesetname, int index );
	int GetImageRectLeft( const char* imagesetname, const char* imagename );
	int GetImageRectTop( const char* imagesetname, const char* imagename );
	int GetImageRectRight( const char* imagesetname, const char* imagename );
	int GetImageRectBottom( const char* imagesetname, const char* imagename );
	// frame
	int GetNumFrame() const;
	const char* GetFrameName( int index );
	void RenderTestFrame( const char* name, int offsetx, int offsety, int width, int height );

protected:
	ioWnd* LoadWnd( ioXMLElement &xElement, ioWnd *pParent );	
	void LoadTemplate( ioXMLElement &xElement, ioWnd *pParent );
	void LoadProperty( ioXMLElement &xElement, ioWnd *pWnd );

	virtual ioWnd* CreateNewWindow( const ioHashString &szClassName );
	ioWnd* CreateNewWindowEX( const ioHashString &szClassName );

	virtual void OnTitleProperty( ioXMLElement &xElement, ioWnd *pWnd, const ioHashString& szStrMgrKeyName );
	virtual void OnTextProperty( ioXMLElement &xElement, ioWnd *pWnd );
	virtual void OnTooltipProperty( ioXMLElement &xElement, ioWnd *pWnd, const ioHashString& szStrMgrKeyName );
	virtual void OnSizeProperty( ioXMLElement &xElement, ioWnd *pWnd );
	virtual void OnStyleProperty( ioXMLElement &xElement, ioWnd *pWnd );
	virtual void OnExStyleProperty( ioXMLElement &xElement, ioWnd *pWnd );
	virtual void OnExStyleParameter( const std::string& szText, OUT std::string& szExStyle, OUT std::string& szParam );
	virtual void OnExStyleParameterProperty( const std::string& szParam, ioWnd *pWnd );
	virtual void OnDragRectProperty( ioXMLElement &xElement, ioWnd *pWnd );
	virtual void OnImageryProperty( ioXMLElement &xElement, ioWnd *pWnd );

	void OnFrameArea( ioXMLElement &xElement, ioUIRenderFrame *pUIFrame );
	void OnImageArea( ioXMLElement &xElement, ioUIRenderImage *pUIImage );

	virtual void NotifyLoadWnd( ioWnd* pWnd );
	virtual void NotifyLoadFrame( ioUIRenderFrame* pFrame, const ioHashString& szType, ioWnd* pWnd );
	virtual void NotifyUIRenderImage( ioUIRenderImage* pImage, const ioHashString& szType, ioWnd* pWnd );

public:
	bool ChangeWndElementValue( const char *szOrgXMLFileName, const char *szChangeXMLFileName, const char* szChangeValueKey, ioWnd* pPresent );

	virtual void RegistToolTipID(int nID);
	virtual bool IsToolTipWnd(ioWnd* pWnd, ioWnd* pParent);

protected:
	bool ChangeWndElementValue( ioXMLElement &xOrgElement, ioXMLElement &xChangeElement, ioWnd* pPresent );
	void ChangeProperty( ioXMLElement &xOrgElement, ioXMLElement &xChangeElement, ioWnd* pChangeWnd );

public:
	bool IsElsewhereClick( IN ioWnd* pWnd, IN const std::list< ioWnd* >& ExceptionList, IN bool bRoot=true );

private:
	bool IsElsewhereException( IN ioWnd* pWnd );

public:
	ioGUIManager();
	virtual ~ioGUIManager();
};

#endif