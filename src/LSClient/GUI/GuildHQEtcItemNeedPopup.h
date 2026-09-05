#pragma once

class GuildHQEtcItemNeedPopup : public ioWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_CREATE_ITEM_BUY	= 2,
		ID_USE_ITEM			= 3,
		ID_CANCEL			= 4,
	};

	enum PopUpType
	{
		PUT_NEED_HQ,
		PUT_USE_HQ,
	};

protected:
	PopUpType			m_PopUpType;
	ioUIRenderImage*	m_pIconBack;
	ioUIRenderImage*	m_pIcon;

public:
	void SetType( GuildHQEtcItemNeedPopup::PopUpType eType );

public:
	virtual void iwm_show();	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnRender();

public:
	GuildHQEtcItemNeedPopup();
	virtual ~GuildHQEtcItemNeedPopup();
};