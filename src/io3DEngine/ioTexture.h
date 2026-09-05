

#ifndef _ioTexture_h_
#define _ioTexture_h_

#include "io3DCommon.h"
#include "ioResource.h"

class ioTextureManager;

class __EX ioTexture : public ioResource
{
protected:
	IDirect3DTexture9	*m_pTexture;
	mutable IDirect3DTexture9   *m_pGrayTexture;
	mutable IDirect3DTexture9   *m_pGrayLightTexture;

	int			m_iSrcWidth;
	int			m_iSrcHeight;
	D3DFORMAT	m_Format;
	DWORD		m_dwUsage;
	int			m_iMipMapLevelCnt;

	TextureQuality m_MinimumQuality;
	bool	    m_bDefaultPath;
	bool		m_bLoadedFromFile;
	bool	    m_bLoadPiece;

public:
	virtual void SetTexture( IDirect3DTexture9 *pTex );
	void SetGrayTexture( IDirect3DTexture9 *pGrayTex ) const;
	void SetGrayLightTexture( IDirect3DTexture9 *pGrayLightTex ) const;

	void SetSize( int iWidth, int iHeight );
	void SetFormat( D3DFORMAT eFormat ) { m_Format = eFormat; }
	void SetUsage( DWORD dwUsage ) { m_dwUsage = dwUsage; }
	void SetMipMapLevel( int iMipLevel ) { m_iMipMapLevelCnt = iMipLevel; }
	void SetDefaultPath( bool bDefault ) { m_bDefaultPath = bDefault; }
	void SetLoadPiece( bool bLoadPiece ) { m_bLoadPiece = bLoadPiece; }
	void SetLoadedFromFile( bool bFromFile ) { m_bLoadedFromFile = bFromFile; }
	void SetMinimumQuality( TextureQuality eQuality ) { m_MinimumQuality = eQuality; }

public:
	// 현재 텍스쳐와 동일한 포멧이라는 가정이 깔려있음
	bool FillImage( const RECT &rcArea, BYTE *pSrcBuf );

public:
	inline IDirect3DTexture9* GetTexture() const { return m_pTexture; }
	IDirect3DTexture9* GetGrayTexture() const;
	IDirect3DTexture9* GetGrayLightTexture() const;

	inline int GetWidth() const { return m_iSrcWidth; }
	inline int GetHeight() const { return m_iSrcHeight; }
	inline D3DFORMAT GetFormat() const { return m_Format; }
	inline DWORD GetUsage() const { return m_dwUsage; }
	inline int GetMipMapLevelCnt() const { return m_iMipMapLevelCnt; }

	inline bool IsDefaultPath() const { return m_bDefaultPath; }
	inline bool IsLoadPiece() const { return m_bLoadPiece; }
	inline bool IsLoadedFromFile() const { return m_bLoadedFromFile; }
	inline TextureQuality GetMinimumQuality() const { return m_MinimumQuality; }

	inline bool HasGrayTexture() const
	{
		if( m_pGrayTexture )
			return true;

		return false;
	}

	inline bool HasGrayLightTexture() const
	{
		if( m_pGrayLightTexture )
			return true;

		return false;
	}

public:
	void* Lock( int *pPitch, int iLevel, DWORD dwLockFlags, const RECT *pRect );
	void UnLock( int iLevel = 0 );

public:
	virtual void ReleaseIfDefaultPool();

private:
	friend class ioTextureManager;

protected:	// 텍스쳐의 생성/소멸은 매니저만이 할수 있다.
	ioTexture( const ioHashString &name );
	virtual ~ioTexture();
};

#endif
