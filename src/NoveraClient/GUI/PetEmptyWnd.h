#pragma once

class ioUIRenderImage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PetEmptyWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT			= 1,
		ID_SHOP			= 2,
		ID_CANCEL		= 3,
	};
		
protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderImage *m_pBackEffect;
	ioUIRenderImage *m_pPetIcon;
		
public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnRender();

protected:
	void RenderDesc( int nXPos, int nYPos );

public:
	PetEmptyWnd();
	virtual ~PetEmptyWnd();
};