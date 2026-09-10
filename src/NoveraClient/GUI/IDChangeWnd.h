#pragma once


class ioWnd;
class ioUIRenderImage;

class IDChangeWnd : public ioWnd
{
public:
	enum 
	{
		ID_SHOTDOWN_BTN = 1,
	};

protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pBackEffect;

	ioHashString m_szNewName;

	int m_iShutDownCnt;
	DWORD m_dwCntStartTime;

public:
	virtual void iwm_command( ioWnd *pWnd , int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	void MakeNewSaveFile();
	void SetExit();

public:
	void SetInfoAndShow( const ioHashString &szNewName );

public:
	IDChangeWnd(void);
	virtual ~IDChangeWnd(void);
};

