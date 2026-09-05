

#ifndef _ioUIImageSetManager_h_
#define _ioUIImageSetManager_h_

#include "io3DCommon.h"
#include "ioResourceManager.h"

class ioUIImage;
class ioUIImageSet;
class ioTexture;
class ioXMLElement;
class ioUIRenderImage;

class __EX ioUIImageSetManager : public Singleton< ioUIImageSetManager >
{
protected:
	typedef std::map< ioHashString, ioUIImageSet* > UIImageSetMap;
	UIImageSetMap m_UIImageSetMap;

protected:
	typedef std::map< ioHashString, DWORD > MemoryMap;
	MemoryMap m_MemoryMap;

	StaticSwitch m_StaticSwitch;

public:
	void AddMem( const ioHashString& KeyName, DWORD dwMem );
	void RemoveMem( const ioHashString& KeyName );
	void GetMemStatics( DWORD& dwCount, DWORD& dwSize );
	void SetSwitch( int eSwitch ){ m_StaticSwitch = static_cast<StaticSwitch>( eSwitch ); }

public:
	bool LoadFile( const char *szFile );
	void SaveFile( const char *szFile );

protected:
	void ParseImageSet( ioXMLElement &rkElement );
	void ParseImage( ioXMLElement &rkElement, ioUIImageSet *pSet );

	void SaveImageSet( ioXMLElement &rkSet, ioUIImageSet *pSet );
	void SaveImage( ioXMLElement &rkImage, ioUIImage *pImg );

public:
	ioUIImageSet* CreateImageSet( const ioHashString &szName,
								  const ioHashString &szTexFile,
								  ErasePolicy ePolicy = EP_AUTOMATIC,
								  bool bNowLoad = false );

	void DestroyImageSet( const ioHashString &szName );
	void DestroyImageSet( ioUIImageSet *pImageSet );
	void DestroyAllImageSets();

public:
	ioUIRenderImage* CreateImage( const ioHashString &szImageSet,
								  const ioHashString &szImage,
								  bool bForceTouch = false );

	// ImageSet이름#이미지이름
	ioUIRenderImage* CreateImageByFullName( const ioHashString &szSetImage, bool bForceTouch = false );
	ioUIRenderImage* CreateImageByFile( const ioHashString &szFileName,
									    int iOffsetX = 0,
									    int iOffsetY = 0,
									    ErasePolicy ePolicy = EP_AUTOMATIC );

	ioUIImage* GetImageByFullName( const ioHashString &szSetImage );

	// 외부 JPG를 이미지Set로 만든다
	ioUIRenderImage* CreateImageByJPG( const ioHashString &szSetName, const ioHashString &szFileName, bool bLoadPiece, ErasePolicy ePolicy = EP_AUTOMATIC );

public:
	ioUIImageSet* GetImageSet( const ioHashString &szName );
	ioUIImageSet* GetImageSet( int iIndex );

	int GetNumImageSet() const;

public:
	ioUIImage* GetImage( const ioHashString &szImageSet,
					     const ioHashString &szImage );

public:
	static ioUIImageSetManager& GetSingleton();

public:
	ioUIImageSetManager();
	virtual ~ioUIImageSetManager();
};

#define g_UIImageSetMgr ioUIImageSetManager::GetSingleton()

#endif
