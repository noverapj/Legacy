

#ifndef _ioFTManagerImpl_h_
#define _ioFTManagerImpl_h_

class ioFTFace;

class ioFTManagerImpl : public ioFTManager
{
protected:
	FT_Library	m_FTLibrary;

	typedef std::map< std::string, ioFTFaceImpl* > FTFaceMap;
	FTFaceMap m_FaceMap;

public:
	virtual bool InitLibrary();

	virtual ioFTFace* OpenNewFace( const char *szFaceName,
								   const char *szFileName,
								   int iFaceIndex );

	virtual ioFTFace* OpenNewFaceFromMemory( const char *szFaceName,
		                                     const BYTE *pBuffer,
											 const int iBufferSize,  
											 int iFaceIndex );
	virtual void DestroyFace( ioFTFace *pFace );
	virtual void DestroyFace( const char *szFaceName );
	virtual void DestroyAllFaces();

	virtual ioFTFace* GetFace( const char *szFaceName );

public:
	virtual void SetCharSize( const char *szFace, int iSize );
	virtual void SetDeviceResolution( const char *szFace, int iHorz, int iVert );

protected:
	void ReleaseAll();

public:
	ioFTManagerImpl();
	virtual ~ioFTManagerImpl();
};

#endif
