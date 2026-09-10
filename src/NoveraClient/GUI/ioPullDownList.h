#pragma once
#include "ioWnd.h"
#include "ioPullDownItemFactory.h"
#include "ioPullDownItem.h"

class ioPullDownList : public ioWnd
{
public:
	friend class ioPullDownItemFactory;
	enum
	{
		ID_VERT_SCROLL = 1000,
	};
	
	typedef std::vector< ioPullDownItem* >	vPullDownItem;
	
protected:
	PullDownStyle		m_ePullDownStyle;

	ioUIRenderImage		*m_pDarkLine;
	ioUIRenderImage		*m_pLightLine;
	ioUIRenderImage		*m_pOverLine;

	ioWnd				*m_pOpenBtn;

	int					m_iBlankWidth;
	int					m_iBlankHeight;
	int					m_iListHeight;
	int					m_iMaxPageList;
	int					m_iPullDownImageCount;	//풀 다운 아이템에 등록될 이미지 갯수

	int					m_iCurPos;
	int					m_iOverSelectIndex;
	int					m_iImageOverIndex;
		
	vPullDownItem		m_vPullDownItem;
	vPullDownInnerImage	m_vImageList;

protected:	
	void DestroyPullDown();
	void DestroyPullDownImage();

public:
	ioPullDownItem* GetPullDown( int nIdx ) const;
	int GetPullDownCount() const;
	virtual void OpenList( ioWnd *pOpenBtn );
	virtual void AddList( ioPullDownItem* pItem );

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	void ParsePullDown( ioXMLElement &xElement );
	void ParsePullDownItem( ioXMLElement &xElement );
	void ParseImage( ioXMLElement &xElement );

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );

public:
	int GetScrollBarCutWidth();

protected:	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioWnd *GetOpenBtn(){ return m_pOpenBtn; }
	ioPullDownItem* GetOverPullDown();

public:
	bool CheckPullDownItem( ioLSMouse *pMouse, int iOverIdx, int iOverImageIdx );
	bool CheckPullDownImage( ioLSMouse *pMouse, int iOverIdx, int iOverImageIdx );

	void OnPullDownOver( int iOverIdx, int iOverImageIdx );
	void OnPullDownClick( int iClickIdx, int iClickImageIdx );
	void OnPullDownLeave( int iLeaveIdx, int iLeaveImageIdx );

public:
	bool GetIndexToXYPos( int iSelectIndex, float &rfXPos, float &rfYPos );

public:
	ioPullDownList();
	virtual ~ioPullDownList();
};