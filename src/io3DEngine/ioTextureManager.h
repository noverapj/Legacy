

#ifndef _ioTextureManager_h_
#define _ioTextureManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioResourceManager.h"

class ioStream;
class ioResource;
class ioTexture;
class ioRenderTexture;

#define NO_TEXTURE	"void"

class __EX ioTextureManager : public ioResourceManager, public Singleton< ioTextureManager >
{
protected:
	IDirect3DDevice9 *m_pD3DDevice;
	IDirect3DTexture9 *m_pTempTex;

	TextureQuality m_TextureQuality;
	int m_iGrayScaleRate;
	int m_iGrayLightScaleRate;

	typedef std::map< ioHashString, FILETIME > FileTimeMap;
	FileTimeMap m_FileTimeMap;
	bool m_bFileUpdateCheck;

protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void RegisterImpl( ioResource *pRes );

	virtual void ReLoadImpl( const ioHashString &name );
	void ReLoadImpl( ioTexture *pTexture );

protected:
	bool IsCustomTexture( const ioHashString &szFileName );

public:
	virtual void NotifyLoadDone( ioStream *pStream, void *pParam, bool bLoaded );

public:
	void SetGrayScaleRate( int iRate );
	void SetGrayLightScaleRate( int iRate );
	void ChangeTextureQuality( TextureQuality eQuality );
	void ChangeTextureQuality( ioTexture *pTexture, TextureQuality eQuality );

public:
	void SetCheckUpdateTime( bool bCheck ) { m_bFileUpdateCheck = bCheck; }
	void ReLoadUpdateFiles();

protected:
	void GetLastFileWriteTime( const char *szFileName, FILETIME &rkFileTime );

public:
	ioTexture* CreateTexture( const ioHashString &name,
							  DWORD dwUsage = 0,
							  int iMipMapLevelCnt = D3DX_DEFAULT,
							  TextureQuality eMinimum = TQ_DEFAULT,
							  bool bThread = false,
							  bool bDefaultPath = true,
							  bool bLoadPiece = false );

	ioTexture* CreateManual( const ioHashString &name,
							 int iWidth,
							 int iHeight,
							 D3DFORMAT fmt,
							 DWORD dwUsage,
							 int iMipMapLevelCnt );

	ioRenderTexture* CreateRTT( const ioHashString &name,
								int iWidth,
								int iHeight,
								D3DFORMAT eFormat,
								D3DFORMAT eDepthFormat,
								bool bUseDepthTexture );

	// 렌더텍스쳐의 경우는 비디오 메모리라서 생성시 실패할 우려가 있기때문에
	// 처음 생성된 이후로는 DestroyAll이 아니면 삭제 안되게 되어 있다.
	// 직접 Release()를 호출해서 Ref가 0이되면 실제 삭제한다. ( 물론 0이 아니면 삭제 안함 )
	bool DestroyRTTIfNoRef( ioRenderTexture *pRTT );

public:
	ioTexture* GetTextureByName( const ioHashString &name );
	void DestroyByName( const ioHashString &kName, bool bForceDestroy = false );

protected:
	bool LoadFile( const char *szFileName, ioTexture *pTexture, bool bThread, bool bLoadPiece );
	bool LoadFileInternal( ioTexture *pTexture, ioStream *pStream );

	IDirect3DTexture9* CreateD3DTexture( int iWidth,
										 int iHeight,
										 int iMipMap,
										 DWORD dwUsage,
										 D3DFORMAT fmt,
										 D3DPOOL ePool );

	D3DFORMAT CheckFileFormatByName( const ioHashString &kName );
	D3DFORMAT ChangeTo16BitForamt( D3DFORMAT eFormat );

public:
	void ReleaseDefaultPoolTextures();
	void ReCreateDefaultPoolTextures();

	void BuildGrayTexture( const ioTexture *pTex );
	void UpdateGrayTexture( const ioTexture *pTex, const RECT &rcArea, BYTE *pSrcBuf );

	void BuildGrayLightTexture( const ioTexture *pTex );
	void UpdateGrayLightTexture( const ioTexture *pTex, const RECT &rcArea, BYTE *pSrcBuf );

protected:
	void DownSizingToLowQuality( ioTexture *pTexObj, D3DPOOL ePool );

	void GrayA8R8G8B8Filter( const ioTexture *pTex, IDirect3DTexture9 *pGrayTex, int iScaleRate );
	void GrayR8G8B8Filter( const ioTexture *pTex, IDirect3DTexture9 *pGrayTex, int iScaleRate );

	void GrayA4R4G4B4Filter( const ioTexture *pTex, IDirect3DTexture9 *pGrayTex, int iScaleRate );
	void GrayR5G6B5Filter( const ioTexture *pTex, IDirect3DTexture9 *pGrayTex, int iScaleRate );
	void GrayA1R5G5B5Filter( const ioTexture *pTex, IDirect3DTexture9 *pGrayTex, int iScaleRate );

	void GrayDDSFilter( const ioTexture *pTex, IDirect3DTexture9 *pGrayTex, int iScaleRate );

public:
	int GetTotalTextureCount() const;
	int GetTextureAboutMemoryBytes() const;

	inline IDirect3DDevice9* GetDevice() const { return m_pD3DDevice; }

public:
	bool ExtractDDSColorValue( ioTexture *pDDSTex, BYTE *pRGBA );
	bool MixJPGAlphaValue( ioTexture *pJPGTex, BYTE *pRGBA, int iWidth, int iHeight );
	bool ColorCompressDDS( ioTexture *pDDSTex, BYTE *pRGBA );

public:
	static ioTextureManager& GetSingleton();

public:
	ioTextureManager( IDirect3DDevice9 *pDevice );
	virtual ~ioTextureManager();
};

#define g_TextureMgr ioTextureManager::GetSingleton()

#define DESTROY_TEXTURE(x) if(x) { g_TextureMgr.Destroy(x); x=NULL; }

#endif