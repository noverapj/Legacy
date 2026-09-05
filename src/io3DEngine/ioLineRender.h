


#ifndef _ioLineRender_h_
#define _ioLineRender_h_

class ioMeshData;
class ioRenderSystem;

class __EX ioLineRender
{
protected:
	DWORD m_dwLineColor;
	int m_iRenderVtx;
	int m_iLockStartVertex;

	ioMeshData *m_pVtxData;
	bool m_bUpdateBufferOK;

public:
	void UpdateBuffers( const D3DXVECTOR2 *pSrcBuf, int iVtxCnt );
	void Render();

public:
	void RenderNow( ioRenderSystem *pSystem );

public:
	void SetAlpha( int iAlpha );
	void SetLineColor( int iRed, int iGreen, int iBlue );
	void SetLineColor( DWORD dwLineColor );

protected:
	bool CheckBufferSize( int iNeedVtx );
	void DestroyResource();

public:
	ioLineRender();
	virtual ~ioLineRender();
};

#endif