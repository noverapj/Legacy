

#include "stdafx.h"

#include "ioFTFace.h"
#include "ioFTFaceImpl.h"

#include "ioFTManager.h"
#include "ioFTManagerImpl.h"

static ioFTManagerImpl *pMgr = NULL;

ioFTManager* ioFTManager::CreateManager()
{
	if( !pMgr )
	{
		pMgr = new ioFTManagerImpl;
	}

	return pMgr;
}

ioFTManager* ioFTManager::GetManager()
{
	return pMgr;
}

void ioFTManager::DestroyManager()
{
	if( pMgr )
	{
		delete pMgr;
		pMgr = NULL;
	}
}

ioFTManagerImpl::ioFTManagerImpl()
{
	m_FTLibrary = NULL;
}

ioFTManagerImpl::~ioFTManagerImpl()
{
	ReleaseAll();
}

void ioFTManagerImpl::ReleaseAll()
{
	DestroyAllFaces();

	if( m_FTLibrary )
	{
		FT_Done_FreeType( m_FTLibrary );
		m_FTLibrary = NULL;
	}
}
	

bool ioFTManagerImpl::InitLibrary()
{
	ReleaseAll();

	FT_Error ftError = FT_Init_FreeType( &m_FTLibrary );
	if( ftError )
	{
		LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::InitLibrary - Library Init Failed(%d)", ftError );
		return false;
	}

	return true;
}


ioFTFace* ioFTManagerImpl::OpenNewFace( const char *szFaceName,
										const char *szFileName,
										int iFaceIndex )
{
	if( !m_FTLibrary )
	{
		LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - Init Library First" );
		return NULL;
	}

	ioFTFace *pFtFace = GetFace( szFaceName );
	if( pFtFace )
	{
		LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - %s Already Loaded", szFaceName );
		return pFtFace;
	}

	FT_Error ftError;
	FT_Face ftNewFace;

	ftError = FT_New_Face( m_FTLibrary, szFileName, iFaceIndex, &ftNewFace );
	if( ftError )
	{
		if( ftError == FT_Err_Unknown_File_Format )
		{
			LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - %s Unknown File Format", szFileName );
			return NULL;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - %s cannot Find", szFileName );
			return NULL;
		}
	}

	ioFTFaceImpl *pFtFaceImpl = new ioFTFaceImpl;
	pFtFaceImpl->InitFTFace( szFaceName, ftNewFace );
	m_FaceMap.insert( FTFaceMap::value_type( pFtFaceImpl->GetName(), pFtFaceImpl ) );

	return pFtFaceImpl;
}

ioFTFace* ioFTManagerImpl::OpenNewFaceFromMemory( const char *szFaceName, const BYTE *pBuffer,const int iBufferSize, int iFaceIndex )
{
	if( !m_FTLibrary )
	{
		LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - Init Library First" );
		return NULL;
	}
	
	ioFTFace *pFtFace = GetFace( szFaceName );
	if( pFtFace )
	{
		LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - %s Already Loaded", szFaceName );
		return pFtFace;
	}
	
	FT_Error ftError;
	FT_Face ftNewFace;
	
	ftError = FT_New_Memory_Face( m_FTLibrary, pBuffer, iBufferSize, iFaceIndex, &ftNewFace );
	if( ftError )
	{
		if( ftError == FT_Err_Unknown_File_Format )
		{
			LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace -  Unknown File Format");
			return NULL;
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioFTManagerImpl::OpenNewFace - cannot Find");
			return NULL;
		}
	}

	ioFTFaceImpl *pFtFaceImpl = new ioFTFaceImpl;
	pFtFaceImpl->InitFTFace( szFaceName, ftNewFace );
	m_FaceMap.insert( FTFaceMap::value_type( pFtFaceImpl->GetName(), pFtFaceImpl ) );
	
	return pFtFaceImpl;	
}

void ioFTManagerImpl::DestroyFace( ioFTFace *pFace )
{
	DestroyFace( pFace->GetName() );
}

void ioFTManagerImpl::DestroyFace( const char *szFaceName )
{
	FTFaceMap::iterator iter = m_FaceMap.find( std::string( szFaceName ) );
	if( iter != m_FaceMap.end() )
	{
		delete iter->second;
		m_FaceMap.erase( iter );
	}
}

void ioFTManagerImpl::DestroyAllFaces()
{
	FTFaceMap::iterator iter, iEnd;
	iEnd = m_FaceMap.end();
	for( iter=m_FaceMap.begin() ; iter!=iEnd ; ++iter )
	{
		delete iter->second;
	}
	m_FaceMap.clear();
}

ioFTFace* ioFTManagerImpl::GetFace( const char *szFaceName )
{
	FTFaceMap::iterator iter = m_FaceMap.find( std::string(szFaceName) );
	if( iter != m_FaceMap.end() )
		return iter->second;

	return NULL;
}

void ioFTManagerImpl::SetDeviceResolution( const char *szFace, int iHorz, int iVert )
{
	ioFTFace *pFace = GetFace( szFace );
	if( pFace )
	{
		pFace->SetDeviceResolution( iHorz, iVert );
	}
}

void ioFTManagerImpl::SetCharSize( const char *szFace, int iSize )
{
	ioFTFace *pFace = GetFace( szFace );
	if( pFace )
	{
		pFace->SetCharSize( iSize );
	}
}

