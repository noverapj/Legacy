#pragma once

class ioRenderTexture;

class ioRenderTarget
{
protected:
	ioRenderTexture*	m_pRenderTexture;
	int					m_Index;

protected:
	D3DVIEWPORT9		m_OldView;
	D3DRECT				m_D3DRect;
	
	int					m_iXPos;
	int					m_iYPos;
	int					m_iWidth;
	int					m_iHeight;

public:
	void Destroy();
	void Create( int iWidth, int iHeight );
	void FillBuffers( ioVtxRHWColorTex2 *pBuf );
	bool UpdateBuffers();

protected:
	bool BeginScene();
	void EndScene();

public:
	virtual void RenderTarget();
	virtual void OnRenderTarget();
	virtual void OnTextureRender( int iXPos, int iYPos, UIRenderType eRenderType, TextureFilterOption eFilter );
	virtual void OnTextureRender( int iXPos, int iYPos, const RECT &rcRect, UIRenderType eRenderType, TextureFilterOption eFilter );

public:
	ioRenderTarget();
	virtual ~ioRenderTarget();
};