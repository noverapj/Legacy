#pragma once

class ioXMLElement;

class ShuffleBonusResultWnd : public ioWnd
{
public:
	enum
	{
		ID_CONTINUE = 1,
		ID_LOBBY    = 2,
	};

protected:
	ioUIRenderImage *m_pStarIcon;
	ioUIRenderImage *m_pCross;
	ioUIRenderImage *m_pNumber;

	DWORD m_dwPartyJoinStartTime;

protected:
	int m_iStarCount;

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	void RenderStarCount( int iXPos, int iYPos );
	
public:
	ShuffleBonusResultWnd();
	virtual ~ShuffleBonusResultWnd();
};
