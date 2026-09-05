

#ifndef _ioRenderTexture_h_
#define _ioRenderTexture_h_

#include "io3DCommon.h"
#include "ioTexture.h"

class __EX ioRenderTexture : public ioTexture
{
protected:
	IDirect3DSurface9 *m_pTextureSurf;

	IDirect3DTexture9 *m_pDepthTexture;
	IDirect3DSurface9 *m_pDepthSurf;
	
	D3DFORMAT	m_DepthFormat;
	bool		m_bUseDepthTexture;

public:
	virtual void SetTexture( IDirect3DTexture9 *pTex );
	virtual void ReleaseIfDefaultPool();

public:
	void SetDepthTexture( IDirect3DTexture9 *pDepthTex );
	void SetDepthSurface( IDirect3DSurface9 *pSurface );

	void SetDepthFormat( D3DFORMAT eFormat ) { m_DepthFormat = eFormat; }
	void SetUseDepthTexture( bool bUse ) { m_bUseDepthTexture = bUse; }

public:
	inline IDirect3DSurface9* GetTextureSurface() const { return m_pTextureSurf; }

	inline IDirect3DTexture9* GetDepthTexture() const { return m_pDepthTexture; }
	inline IDirect3DSurface9* GetDepthSurface() const { return m_pDepthSurf; }

	inline D3DFORMAT GetDepthFormat() const { return m_DepthFormat; }
	inline bool IsUseDepthTexture() const { return m_bUseDepthTexture; }

private:
	friend class ioTextureManager;

protected:
	ioRenderTexture( const ioHashString &name );
	virtual ~ioRenderTexture();
};

#endif
