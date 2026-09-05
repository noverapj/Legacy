#pragma once

class HomeEtcItemNeedPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_CREATE_ITEM_BUY	= 2,
		ID_CANCEL			= 3,
	};

protected:	
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pIcon;

public:
	void SetType();

public:
	virtual void iwm_show();	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	HomeEtcItemNeedPopup();
	virtual ~HomeEtcItemNeedPopup();
};