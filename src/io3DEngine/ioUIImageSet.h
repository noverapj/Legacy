

#ifndef _ioUIImageSet_h_
#define _ioUIImageSet_h_

#include "io3DCommon.h"

class ioUIImage;
class ioTexture;

class __EX ioUIImageSet
{
private:
	friend class ioUIImageSetManager;

private:
	typedef std::map< ioHashString, ioUIImage* > UIImageMap;
	UIImageMap m_UIImageMap;

	ioHashString m_Name;
	ioHashString m_TextureName;
	ErasePolicy  m_ErasePolicy;
	bool	     m_bLoadNow;
	bool         m_bDefaultPath;
	bool         m_bLoadPiece;

	mutable ioTexture *m_pUITexture;
	mutable int m_iImageRef;

public:
	void ImageLoadOption( bool bDefaultPath, bool bLoadPiece );
	ioUIImage* DefineImage( const ioHashString &szName,
							const RECT &rcTexRect,
							int iOffX = 0,
							int iOffY = 0 );

	ioUIImage* DefineImage( const ioHashString &szName,
							const POINT &ptPos,
							const POINT &ptSize,
							int iOffX = 0,
							int iOffY = 0 );

	void UnDefineImage( const ioHashString &szName );
	void UnDefineAllImages();

public:
	ioUIImage* GetImage( const ioHashString &szName );
	ioUIImage* GetImage( int iIndex );
	const char* GetImageName( int index );

	bool IsImageDefined( const ioHashString &szName );
	float GetImageWidth( const ioHashString &szName );
	float GetImageHeight( const ioHashString &szName );

public:
	bool IsLoaded() const;
	
	void Touch( bool bThread ) const;
	void ResetTextureFile( const char *szFileName );

	inline void SetLoadNow( bool bNow ) { m_bLoadNow = bNow; }
	inline bool IsLoadNow() const { return m_bLoadNow; }

	inline ErasePolicy GetErasePolicy() const { return m_ErasePolicy; }

public:
	inline const ioHashString& GetName() const { return m_Name; }
	inline ioTexture* GetTexture() const { return m_pUITexture; }
	inline int GetImageCount() const { return m_UIImageMap.size(); }

	inline const ioHashString& GetTextureName() const { return m_TextureName; }

	IDirect3DTexture9* GetD3DTexture() const;
	IDirect3DTexture9* GetGrayTexture() const;
	IDirect3DTexture9* GetGrayLightTexture() const;
	int GetTextureWidth() const;
	int GetTextureHeight() const;

public:
	bool FillFileImage( const char *szFileName,
						const int iFileWidth,
						const int iFileHeight,
						const ioUIImage *pDestImg, 
						OUT D3DXIMAGE_INFO *pSrcInfo = NULL );

public:
	void AddRef() const;
	void ReleaseRef() const;
	inline int  GetRefCount() const { return m_iImageRef; }

private:
	ioUIImageSet( const ioHashString &szName,
				  const ioHashString &rkTexName,
				  ErasePolicy ePolicy );

public:
	~ioUIImageSet();
};

#endif
