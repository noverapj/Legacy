

#ifndef _io2DImageManager_h_
#define _io2DImageManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioResource;
class ioRc2DImage;
class ioResourceManager;
class ioBinaryStream;

class __EX io2DImageManager : public ioResourceManager, public Singleton< io2DImageManager >
{
protected:
	virtual ioResource* CreateImpl( const ioHashString &name, bool bThread );
	virtual void ReLoadImpl( const ioHashString &name );

public:
	ioRc2DImage* Create2DImage( const ioHashString &name );
	ioRc2DImage* Get2DImageByName( const ioHashString &name );

protected:
	bool LoadFile( const ioHashString &name, ioRc2DImage *p2DImage );

	bool Is2DImageFile( ioBinaryStream &rkStream );
	WORD* LoadPalette( ioBinaryStream &rkStream );

	void DeCompressPGF( int iWidth,
						int iHeight,
						const BYTE *pSrc,
						WORD *pDest,
						const WORD *pPalette );

	WORD* CompressImage( const WORD *pSrc,
						 int iWidth,
						 int iHeight );

public:
	static io2DImageManager& GetSingleton();

public:
	io2DImageManager();
	virtual ~io2DImageManager();
};

#endif
