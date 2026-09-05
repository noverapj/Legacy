

#include "stdafx.h"

#include "ioUIImage.h"
#include "ioUIImageSet.h"
#include "ioUIImageSetManager.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"
#include "ioUIRenderImage.h"
#include "ioTextureManager.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ErrorReport.h"

template<> ioUIImageSetManager* Singleton< ioUIImageSetManager >::ms_Singleton = 0;

ioUIImageSetManager::ioUIImageSetManager() : m_StaticSwitch( SATICS_OFF )
{
}

ioUIImageSetManager::~ioUIImageSetManager()
{
	DestroyAllImageSets();
}


void ioUIImageSetManager::AddMem( const ioHashString& KeyName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		m_MemoryMap.insert( MemoryMap::value_type( KeyName, dwMem ) );
	}
}

void ioUIImageSetManager::RemoveMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{
			m_MemoryMap.erase( iter );
		}
	}	
}

void ioUIImageSetManager::GetMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	dwCount	= 0;
	dwSize	= 0;

	for( MemoryMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwSize	+= iter->second;
		dwCount++;
	}
}

bool ioUIImageSetManager::LoadFile( const char *szFile )
{
	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFile, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::LoadFile - %s,Load Failed", szFile );
		return false;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( (char*)kStream.GetPtr() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::LoadFromMemory - %s,Load Failed", szFile );
		return false;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::LoadFile - %s, Root Element is NULL", szFile );
		return false;
	}

	AddMem( szFile, kStream.GetSize() );

	ParseImageSet( xRootElement );
	return true;
}

void ioUIImageSetManager::ParseImageSet( ioXMLElement &rkElement )
{
	ioHashString szName, szFile, szErase;

	ioXMLElement xImageSet = rkElement.FirstChild();
	while( !xImageSet.IsEmpty() )
	{
		if( xImageSet.IsTagRight( "Imageset" ) )
		{
			szName = xImageSet.GetStringAttribute( "Name" );
			szFile = xImageSet.GetStringAttribute( "File" );

			if( !szName.IsEmpty() && !szFile.IsEmpty() )
			{
				szErase = xImageSet.GetStringAttribute( "Erase" );

				ErasePolicy eErasePolicy = EP_AUTOMATIC;
				if( !szErase.IsEmpty() )
				{
					eErasePolicy = ioStringConverter::ParseErasePolicy( szErase );
				}

				bool bNowLoad = xImageSet.GetBoolAttribute( "NowLoad" );

				ioUIImageSet *pSet = CreateImageSet( szName, szFile, eErasePolicy, bNowLoad );
				if( pSet )
				{
					ioXMLElement xCurImage = xImageSet.FirstChild();
					while( !xCurImage.IsEmpty() )
					{
						ParseImage( xCurImage, pSet );
						xCurImage = xCurImage.NextSibling();
					}
				}
			}
		}

		xImageSet = xImageSet.NextSibling();
	}
}

void ioUIImageSetManager::ParseImage( ioXMLElement &rkElement, ioUIImageSet *pSet )
{
	ioHashString szName = rkElement.GetStringAttribute( "Name" );

	POINT ptPos, ptSize;
	ptPos.x = rkElement.GetIntAttribute( "X" );
	ptPos.y = rkElement.GetIntAttribute( "Y" );
	ptSize.x = rkElement.GetIntAttribute( "Width" );
	ptSize.y = rkElement.GetIntAttribute( "Height" );

	int iOffX = rkElement.GetIntAttribute( "OffsetX" );
	int iOffY = rkElement.GetIntAttribute( "OffsetY" );

	pSet->DefineImage( szName, ptPos, ptSize, iOffX, iOffY );
}

void ioUIImageSetManager::SaveFile( const char *szFile )
{
	ioXMLDocument kNewDoc;
	kNewDoc.CreateDocument( "ImagesetLayout", "1.0", "euc-kr", "" );

	ioXMLElement kRoot = kNewDoc.GetRootElement();

	UIImageSetMap::iterator iter;
	for( iter=m_UIImageSetMap.begin() ; iter!=m_UIImageSetMap.end() ; ++iter )
	{
		ioXMLElement kChild = kRoot.CreateChild( "Imageset" );

		SaveImageSet( kChild, iter->second );
	}

	kNewDoc.SaveXML( szFile );
}

void ioUIImageSetManager::SaveImageSet( ioXMLElement &rkSet, ioUIImageSet *pSet )
{
	rkSet.SetStringAttribute( "Name", pSet->GetName().c_str() );
	rkSet.SetStringAttribute( "File", pSet->GetTextureName().c_str() );

	if( pSet->GetErasePolicy() != EP_AUTOMATIC )
	{
		rkSet.SetStringAttribute( "Erase", ioStringConverter::toString( pSet->GetErasePolicy() ) );
	}

	if( pSet->IsLoadNow() )
	{
		rkSet.SetBoolAttribute( "NowLoad", true );
	}

	int iImageCnt = pSet->GetImageCount();
	for( int i=0 ; i<iImageCnt ; i++ )
	{
		ioUIImage *pImg = pSet->GetImage( i );

		ioXMLElement kChild = rkSet.CreateChild( "Image" );
		SaveImage( kChild, pImg );
	}
}

void ioUIImageSetManager::SaveImage( ioXMLElement &rkImage, ioUIImage *pImg )
{
	rkImage.SetStringAttribute( "Name", pImg->GetName().c_str() );

	RECT rcArea = pImg->GetAreaRect();
	rkImage.SetIntAttribute( "X", rcArea.left );
	rkImage.SetIntAttribute( "Y", rcArea.top );
	rkImage.SetIntAttribute( "Width", rcArea.right - rcArea.left );
	rkImage.SetIntAttribute( "Height", rcArea.bottom - rcArea.top );

	if( pImg->GetOffsetX() != 0 )
		rkImage.SetIntAttribute( "OffsetX", pImg->GetOffsetX() );

	if( pImg->GetOffsetY() != 0 )
		rkImage.SetIntAttribute( "OffsetY", pImg->GetOffsetY() );
}

ioUIImageSet* ioUIImageSetManager::CreateImageSet( const ioHashString &szName,
												   const ioHashString &szTexFile,
												   ErasePolicy ePolicy,
												   bool bNowLoad )
{
	UIImageSetMap::iterator iter = m_UIImageSetMap.find( szName );
	if( iter != m_UIImageSetMap.end() )
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::CreateImageSet - %s Set Already Exist",
								szName.c_str() );
		return iter->second;
	}

	ioUIImageSet *pSet = new ioUIImageSet( szName, szTexFile, ePolicy );
	if( pSet )
	{
		m_UIImageSetMap.insert( UIImageSetMap::value_type( szName, pSet ) );

		if( bNowLoad )
			pSet->Touch( false );

		pSet->SetLoadNow( bNowLoad );
	}

	return pSet;
}

void ioUIImageSetManager::DestroyImageSet( const ioHashString &szName )
{
	UIImageSetMap::iterator iter = m_UIImageSetMap.find( szName );
	if( iter != m_UIImageSetMap.end() )
	{
		ioUIImageSet *pSet = iter->second;
		if( pSet->GetRefCount() > 0 )
		{
			LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::DestroyImageSet - %s Ref(%d)",
									pSet->GetName().c_str(),
									pSet->GetRefCount() );
		}

		delete pSet;

		m_UIImageSetMap.erase( iter );
	}
}

void ioUIImageSetManager::DestroyImageSet( ioUIImageSet *pImageSet )
{
	if( pImageSet )
	{
		DestroyImageSet( pImageSet->GetName() );
	}
}

void ioUIImageSetManager::DestroyAllImageSets()
{
	UIImageSetMap::iterator iter;
	for( iter=m_UIImageSetMap.begin() ; iter!=m_UIImageSetMap.end() ; ++iter )
	{
		ioUIImageSet *pSet = iter->second;
		if( pSet->GetRefCount() > 0 )
		{
			LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::DestroyAllImageSets - %s Ref(%d)",
									pSet->GetName().c_str(),
									pSet->GetRefCount() );
		}

		RemoveMem( pSet->GetName() );
		delete pSet;		
	}

	m_UIImageSetMap.clear();
}

ioUIRenderImage* ioUIImageSetManager::CreateImage( const ioHashString &szImageSet,
												   const ioHashString &szImage,
												   bool bForceTouch )
{
	const ioUIImage *pImg = GetImage( szImageSet, szImage );
	if( pImg )
	{
		if( bForceTouch )
			pImg->ForceTouch();

		return new ioUIRenderImage( pImg );
	}

	return NULL;	
}

ioUIRenderImage* ioUIImageSetManager::CreateImageByFullName( const ioHashString &szSetImage,
															 bool bForceTouch )
{
	if( szSetImage.IsEmpty() )
		return NULL;

	const StringVector &vParams = ioStringConverter::Split( szSetImage.c_str(), "#", 1 );

	if( vParams.size() == 2 )
	{
		return CreateImage( vParams[0].c_str(), vParams[1].c_str(), bForceTouch );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::CreateImage - %s Not Vaild",
								szSetImage.c_str() );

		return CreateImageByFile( szSetImage );	// 사이즈가 필요하기때문에 ForceTouch를 호출한다.
	}

	return NULL;
}

ioUIRenderImage* ioUIImageSetManager::CreateImageByFile( const ioHashString &szFileName,
													     int iOffsetX,
														 int iOffsetY,
														 ErasePolicy ePolicy )
{
	if( szFileName.IsEmpty() )
		return NULL;

	ioUIImageSet *pExistSet = GetImageSet( szFileName );
	if( pExistSet )
	{
		const ioUIImage *pImg = pExistSet->GetImage( szFileName );
		if( pImg )
			return new ioUIRenderImage( pImg );

		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::CreateImageByFile - %s UIImage Not Exist",
								szFileName.c_str() );
		return NULL;
	}

	ioUIImageSet *pNewSet = CreateImageSet( szFileName, szFileName, ePolicy );
	if( pNewSet )
	{
		pNewSet->Touch( false );

		RECT rcImage;
		rcImage.left = 0;
		rcImage.top	 = 0;
		rcImage.right = pNewSet->GetTextureWidth();
		rcImage.bottom= pNewSet->GetTextureHeight();

		const ioUIImage *pImg = pNewSet->DefineImage( szFileName, rcImage, iOffsetX, iOffsetY );
		if( pImg )
			return new ioUIRenderImage( pImg );
	}

	return NULL;
}

ioUIRenderImage* ioUIImageSetManager::CreateImageByJPG( const ioHashString &szSetName, const ioHashString &szFileName, bool bLoadPiece, ErasePolicy ePolicy /* = EP_AUTOMATIC  */ )
{
	if( szSetName.IsEmpty() || szFileName.IsEmpty() ) 
		return NULL;

	// 이미 존재하면 삭제하고 다시 로드한다.
	DestroyImageSet( szSetName );    

	// 파일 정보
	D3DXIMAGE_INFO kSrcInfo;
	{
		IDirect3DTexture9 *pSrcTex = NULL;
		HRESULT hr = D3DXCreateTextureFromFileEx( g_TextureMgr.GetDevice(),	szFileName.c_str(), 16, 16, 1, 0, D3DFMT_A4R4G4B4, 
												  D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, &kSrcInfo, NULL, &pSrcTex );

		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "CreateImageByJPG: %x", hr );
			LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::CreateImageByJPG - Load Failed(%x) : %s", hr, szFileName.c_str() );
			return NULL;
		}
		SAFERELEASE( pSrcTex );
	}

	// 이미지Set로 Insert
	ioUIImageSet *pSet = new ioUIImageSet( szSetName, szFileName, ePolicy );
	if( pSet )
	{
		m_UIImageSetMap.insert( UIImageSetMap::value_type( szSetName, pSet ) );
		
		pSet->ImageLoadOption( !bLoadPiece, bLoadPiece );
		pSet->Touch( false );
		pSet->SetLoadNow( true );

		// 텍스쳐가 2의승수로 생성되므로 이미지 사이즈는 따로 체크한다.
		POINT ptPos, ptSize;
		ptPos.x = ptPos.y = 0;
		ptSize.x = kSrcInfo.Width;
		ptSize.y = kSrcInfo.Height;
		pSet->DefineImage( szFileName, ptPos, ptSize, 0, 0 );

		return CreateImage( szSetName.c_str(), szFileName.c_str(), true );
	}

	return NULL;
}

ioUIImageSet* ioUIImageSetManager::GetImageSet( const ioHashString &szName )
{
	UIImageSetMap::iterator iter = m_UIImageSetMap.find( szName );
	if( iter != m_UIImageSetMap.end() )
		return iter->second;

	return NULL;
}

ioUIImageSet* ioUIImageSetManager::GetImageSet( int iIndex )
{
	if( COMPARE( iIndex, 0, GetNumImageSet() ) )
	{
		UIImageSetMap::iterator iter = m_UIImageSetMap.begin();
		std::advance( iter, iIndex );
		return iter->second;
	}

	return NULL;
}

int ioUIImageSetManager::GetNumImageSet() const
{
	return m_UIImageSetMap.size();
}

ioUIImage* ioUIImageSetManager::GetImage( const ioHashString &szImageSet,
										  const ioHashString &szImage )
{
	ioUIImageSet *pSet = GetImageSet( szImageSet );
	if( !pSet )
	{
		LOG.PrintTimeAndLog( 0, "ioUIImageSetManager::GetImage - %s Not Exist Set(%s)",
								szImageSet.c_str(), szImage.c_str() );

		return NULL;
	}

	return pSet->GetImage( szImage );
}

ioUIImageSetManager& ioUIImageSetManager::GetSingleton()
{
	return Singleton<ioUIImageSetManager>::GetSingleton();
}

ioUIImage* ioUIImageSetManager::GetImageByFullName( const ioHashString &szSetImage )
{
	if( szSetImage.IsEmpty() )
		return NULL;

	ioUIImage* pImg = NULL;

	const StringVector &vParams = ioStringConverter::Split( szSetImage.c_str(), "#", 1 );

	if( vParams.size() == 2 )
	{
		pImg = GetImage( vParams[0].c_str(), vParams[1].c_str() );
	}	

	return pImg;
}
