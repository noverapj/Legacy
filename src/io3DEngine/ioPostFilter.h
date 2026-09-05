

#ifndef _ioPostFilter_h_
#define _ioPostFilter_h_

class ioRenderSystem;
class ioRenderTexture;
class ioShader;
class ioMeshData;

class ioPostFilter
{
protected:
	ioRenderSystem  *m_pRenderSystem;
	ioShader *m_pShader;

	ioMeshData *m_pRHWMeshData;

	int   m_iCurWindowWidth;
	int   m_iCurWindowHeight;

	D3DCOLORVALUE m_AlphaValue;

public:
	bool InitFilter( const ioHashString &szShader );
	void RenderFilter( ioRenderTexture *pTarget );
	void ClearResource();

	void SetAlphaValue( int iAlpha );

protected:
	bool CheckChangedResource( int iTexWidth, int iTexHeight );
	bool CreateRHWBuffer( int iTexWidth, int iTexHeight );

public:
	ioPostFilter( ioRenderSystem *pSystem );
	virtual ~ioPostFilter();
};

#endif
