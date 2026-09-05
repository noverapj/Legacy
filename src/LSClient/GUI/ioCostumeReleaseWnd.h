#pragma once

class ioUIRenderImage;

class ioCostumeReleaseWnd : public ioMovingWnd
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
	ioUIRenderImage *m_pCostumeIcon;
	ioUIRenderImage *m_pCostumeMark;

protected:
	ioHashString m_szClassName;
	ioHashString m_szName;
	ioHashString m_szIconName;
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
	ioCostumeReleaseWnd();
	virtual ~ioCostumeReleaseWnd();
};