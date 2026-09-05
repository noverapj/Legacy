

#ifndef _ioPostOveray_h_
#define _ioPostOveray_h_

class ioPostOveray
{
protected:
	ioRenderSystem  *m_pRenderSystem;

	ioTexture *m_pOverayTex;
	ioMeshData *m_pRHWMeshData;

	DWORD m_dwColor;
	int   m_iCurWindowWidth;
	int   m_iCurWindowHeight;

public:
	bool InitOveray( const ioHashString &szOverayTex );
	void RenderOveray();
	void ClearResource();

public:
	void SetColor( int iRed, int iGreen, int iBlue );

protected:
	bool CheckChangedResource();
	bool CreateRHWBuffer();

public:
	ioPostOveray( ioRenderSystem *pSystem );
	virtual ~ioPostOveray();
};

#endif