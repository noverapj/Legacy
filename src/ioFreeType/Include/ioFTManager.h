

#ifndef _ioFTManager_h_
#define _ioFTManager_h_

class ioFTFace;

class FREETYPE_API ioFTManager
{
public:
	virtual bool InitLibrary() = 0;

	virtual ioFTFace* OpenNewFace( const char *szFaceName,
								   const char *szFileName,
								   int iFaceIndex ) = 0;

	virtual ioFTFace* OpenNewFaceFromMemory( const char *szFaceName,
											 const BYTE *pBuffer,
											 const int iBufferSize,  
											 int iFaceIndex ) = 0;

	virtual void DestroyFace( ioFTFace *pFace ) = 0;
	virtual void DestroyFace( const char *szFaceName ) = 0;
	virtual void DestroyAllFaces() = 0;

	virtual ioFTFace* GetFace( const char *szFaceName ) = 0;

public:
	virtual void SetCharSize( const char *szFace, int iSize ) = 0;
	virtual void SetDeviceResolution( const char *szFace, int iHorz, int iVert ) = 0;

public:
	static ioFTManager* CreateManager();
	static ioFTManager* GetManager();
	static void DestroyManager();

public:
	ioFTManager(){}
	virtual ~ioFTManager(){}
};

#endif