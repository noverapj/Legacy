#pragma once
#include "../io3DEngine/ioWnd.h"
#include "../ioRenderTarget.h"

class ADTextPanelWnd : public ioWnd, public ioRenderTarget
{
protected:	
	int					m_iCurrIndex;
	float				m_fCurrXPos;
		
	float				m_fADSpeed;
	DWORD				m_dwColor;
	DWORD				m_dwTextWidth;

	std::string			m_ADText;

protected:
	ioUIRenderImage*	m_pBackGround;
	ioUIRenderImage*	m_pOutBorder;

public:
	virtual void iwm_hide();
	virtual void iwm_show();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	void Clear();
	void NextAD();	

protected:
	virtual void RenderTarget();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();	

public:
	ADTextPanelWnd();
	virtual ~ADTextPanelWnd();
};