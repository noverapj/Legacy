#pragma once

class ioRenderSystem;
class ioWndBoundary;

namespace BoundaryUtil
{
	bool IsMouseOver( int XPos, int YPos, int Width, int Height, const POINT& Pos );
	bool IsDerivedShow( ioWnd* pWnd );
	bool IsDerivedWnd( ioWnd* pWnd, ioWnd* pTargetWnd );

	void OnBoundaryContextRender( ioRenderSystem *pSystem, ioHashStringVec& ContextList, int nContextMaxWidth );
	void ExportContextClipBoard( const ioHashStringVec& ContextList );
}

class WndBoundaryContainer
{
public:
	enum
	{
		TEXT_WIDTH_GAP	= 20,
		TEXT_HEIGHT_GAP	= 20,
		TEXT_OFFSET		= 5,
	};

protected:
	typedef std::vector<ioWndBoundary*> WndBoundaryList;
	WndBoundaryList m_WndBoundaryList;

public:
	void AddBoundary( int WndID, int eType, void* pTarget, ioWnd* pParentsWnd, const ioHashString& szType );
	ioWndBoundary* GetBoundary( int WndID );

	void CollectBoundaryContext( int& nContextMaxWidth, ioHashStringVec& ContextList );
	void RenderBoundary();

public:
	WndBoundaryContainer();
	virtual ~WndBoundaryContainer();
};

class ioWndBoundary
{	
public:
	enum
	{
		VTX_MAX = 4,
	};

	enum BindType
	{
		BT_NONE,
		BT_WND,
		BT_UIFRAME,
		BT_UIIMAGE,
	};

protected:	
	DWORD			m_dwInnerUniqueIndex;
	ioHashString	m_ImageType;

	BindType		m_eBindType;
	ioVtxRHWTex		m_Boundary[VTX_MAX];

	ioWnd*			m_pParentWnd;
	void*			m_pBindTarget;
	bool			m_bShow;
	DWORD			m_Color;

	ioHashStringVec	m_Context;

public:
	void SetBindType( BindType eType, void* pTarget, ioWnd* pParentsWnd );

public:
	void SetInnerUniqueIndex( DWORD InnerUniqueIndex );
	DWORD GetInnerUniqueIndex() const;

	void SetImageType( const ioHashString& szType );
	const ioHashString& GetImageType() const;

	const ioHashStringVec& GetContext() const;

public:
	bool IsShow();

public:
	void UpdateBind();
	void UIAlignToString( ioWnd::UIPosAlign Align, ioHashString& szAlign );

	void UpdateBindByWnd( ioWnd *pCurrWnd, const POINT& Pos );
	void UpdateBindByUIFrame( ioWnd *pCurrWnd, const POINT& Pos );
	void UpdateBindByUIImage( ioWnd *pCurrWnd, const POINT& Pos );
	void UpdateBoundary( int XPos, int YPos, int Width, int Height );

public:
	void OnBoundaryRender( ioRenderSystem *pSystem );	

public:
	ioWndBoundary();
	virtual ~ioWndBoundary();
};