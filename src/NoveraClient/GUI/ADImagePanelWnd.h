#pragma once
#include "../io3DEngine/ioMovingWnd.h"
#include "../ioShopManager.h"

class ADImagePanelWnd : public ioButton
{
public:
	enum ADState
	{		
		ADS_DELAY,
		ADS_PAGING,
	};

	enum SlideMove
	{
		SM_PREV,
		SM_NEXT,
	};

	enum
	{
		ID_PREV_BTN				= 1,
		ID_NEXT_BTN				= 2,

		ID_AD_DIRECT_BTN01		= 10,
		ID_AD_DIRECT_BTN02		= 11,
		ID_AD_DIRECT_BTN03		= 12,
		ID_AD_DIRECT_BTN04		= 13,
		ID_AD_DIRECT_BTN05		= 14,
		ID_AD_DIRECT_BTN06		= 15,
		ID_AD_DIRECT_BTN07		= 16,
		ID_AD_DIRECT_BTN08		= 17,
		ID_AD_DIRECT_BTN09		= 18,
		ID_AD_DIRECT_BTN10		= 19,
		ID_AD_DIRECT_BTN_MAX	= 20,
	};

protected:
	ioUIRenderFrame*	m_pBorder;

protected:
	ADState				m_ADState;
	SlideMove			m_SlideMove;

	IntVec				m_ADIndexList;

	int					m_iCurrIndex;

	ioUIRenderImage*	m_pCurrBanner;
	ioUIRenderImage*	m_pPrevBanner;

	DWORD				m_dwDelayTime;
	DWORD				m_dwDelayStartTime;
	DWORD				m_dPagingTime;
	DWORD				m_dPagingStartTime;

	float				m_fCurrRate;

protected:
	ioUIRenderImage* GetImage( int index );

protected:
	void ParseAD();	
	void ChangeAD();

	void CheckPrevMoveADIndex();
	void CheckNextMoveADIndex();
	void CheckDirectMoveADIndex( int iTargetIndex );

public:
	bool IsSliding();
	void MoveDirectLink();

protected:
	void ParseDirectADButton();

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_mouseover( const ioMouse& mouse );	
	virtual void iwm_mouseleave( const ioMouse& mouse );
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void OnPrevPaigingRender( int iXPos, int iYPos );
	void OnNextPaigingRender( int iXPos, int iYPos );

public:
	ADImagePanelWnd();
	virtual ~ADImagePanelWnd();
};
