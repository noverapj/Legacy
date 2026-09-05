#pragma once

class ioUIRenderImage;
class ioAwakeReleaseWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_X		= 1,
		ID_ALL_OK	= 2,
		ID_OK		= 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pSoliderIcon;
	ioUIRenderImage *m_pSoliderSubIcon;

protected:
	std::string m_szIconName;
	std::string m_szSubIconName;
	std::string m_szClassName;
	std::string m_szAwakeName;
	DWORD		m_dwAwakeColor;
	int m_nGradeType;

protected:
	DWORD   m_dwHideDelayTime;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual bool iwm_esc();

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	ioAwakeReleaseWnd();
	virtual ~ioAwakeReleaseWnd();
};