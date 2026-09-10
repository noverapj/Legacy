#pragma once
#include "ioPullDownItemFactory.h"

class ioPullDownItem
{
protected:
	PullDownItemType m_eType;

public:
	friend class ioPullDownItemFactory;

	int					m_iIndex;
	int					m_iHeight;
	int					m_iBlankWidth;
	int					m_iBlankHeight;	

	RECT				m_SelectRect;	

	ioUIRenderImage*	m_pOverLine;
	ioUIRenderImage*	m_pDarkLine;
	ioUIRenderImage*	m_pLightLine;

	vPullDownInnerImage	m_vImageList;
	bool				m_bImageRender;

public:
	void SetBlank( int iBlankWidth, int iBlankHeight );
	void ReSize();

	void AddImage( vPullDownInnerImage& rkList );
	void AddImage( ioUIRenderImage* pImage, DWORD dwOverColor );
	void AddImage( const std::string& szImage, DWORD dwOverColor );

	int OnSelectOver( const POINT& mousepos );
	int OnImageOver( const POINT& mousepos );

	RECT MakeImageRect( PullDownInnerImage& rkImage );
	__forceinline PullDownItemType GetState(){ return m_eType; }
	__forceinline int GetWidth(){ return m_SelectRect.right - m_SelectRect.left; }
	__forceinline int GetHeight(){ return m_SelectRect.bottom - m_SelectRect.top; }

	__forceinline void SetEnableImageRender( bool bRender ){ m_bImageRender =bRender; }
	__forceinline bool IsEnableIamgeRender(){ return m_bImageRender; }

public:
	virtual void OnRender( int iOverIdx, int iOverImageIdx );
	virtual void OnImageListRender( int iOverIdx, int iOverImageIdx );

public:
	ioPullDownItem();
	virtual ~ioPullDownItem();
};

//-----------------------------------------------------------------------------------------------------------------------------

class SinglePullDownItem : public ioPullDownItem
{
public:
	std::string	m_szTitle;
	DWORD		m_dwTitleColor;
	DWORD		m_dwTitleOverColor;

	SinglePullDownItem()
	{
		m_eType				= PDIT_SINGLE;
		m_dwTitleColor		= TCT_DEFAULT_DARKGRAY;
		m_dwTitleOverColor	= TCT_DEFAULT_WHITE;
	}
	virtual ~SinglePullDownItem(){}

	virtual void OnRender( int iOverIdx, int iOverImageIdx );
};

//-----------------------------------------------------------------------------------------------------------------------------

class DoublePullDownItem : public ioPullDownItem
{
public:
	std::string	m_szTitle;
	std::string	m_szSubTitle;

	DWORD		m_dwTitleColor;
	DWORD		m_dwTitleOverColor;
	DWORD		m_dwTitleSubColor;
	DWORD		m_dwTitleSubOverColor;
	
	DoublePullDownItem()
	{
		m_eType					= PDIT_DOUBLE;

		m_dwTitleColor			= TCT_DEFAULT_DARKGRAY;
		m_dwTitleOverColor		= TCT_DEFAULT_WHITE;
		m_dwTitleSubColor		= TCT_DEFAULT_DARKGRAY;
		m_dwTitleSubOverColor	= TCT_DEFAULT_WHITE;		
	}
	virtual ~DoublePullDownItem(){}

	virtual void OnRender( int iOverIdx, int iOverImageIdx );
};