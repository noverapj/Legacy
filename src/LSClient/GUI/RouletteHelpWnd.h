#pragma once

class RouletteHelpWnd : public ioWnd
{
protected:
	enum
	{
		ID_EXIT  = 1,
		ID_CLOSE = 2,
	};

	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_show();	
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	RouletteHelpWnd();
	virtual ~RouletteHelpWnd();
};

//////////////////////////////////////////////////////////////////////////
class RouletteCenterHelpWnd : public ioWnd
{
protected:
	enum UIState
	{
		US_NONE,
		US_START,
		US_STAY,
		US_END,
	};

protected:
	UIState m_State;
	int m_iAlphaRate;
	ioUIRenderImage *m_pTextBack;

public:
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetInfo();

public:
	RouletteCenterHelpWnd();
	virtual ~RouletteCenterHelpWnd();
};
