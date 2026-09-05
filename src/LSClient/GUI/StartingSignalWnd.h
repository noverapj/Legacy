#pragma once

class ioPlayMode;
class ioUIRenderImage;

class StartingSignalWnd : public ioWnd
{
protected:
	enum
	{
		TEXT_NUM_COUNT = 3
	};

protected:
	ioUIRenderImage *m_pText_Num[TEXT_NUM_COUNT];
	ioUIRenderImage *m_pStartText;
	ioUIRenderImage *m_pStartBlur;	
	ioUIRenderImage *m_pStartCount;

protected:
	DWORD m_dwFadeInTime;
	DWORD m_dwFadeOutTime;
	DWORD m_dwCountGap;

	DWORD m_dwStartTextTime;
	DWORD m_dwCurrentStartTextTime;

	ioHashString m_szCountSound[4];

	DWORD m_dwDuration;
	int m_iCount;

	ioPlayMode *m_pPlayMode;

	bool m_bSound;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

protected:
	void Render_Ready( int iXPos, int iYPos );
	void Render_Start( int iXPos, int iYPos );

public:
	void SetPlayMode( ioPlayMode *pPlayMode ){ m_pPlayMode = pPlayMode;	}
	void SetStart();

public:
	StartingSignalWnd();
	virtual ~StartingSignalWnd();
};

