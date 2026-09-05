

#include "stdafx.h"

#include "ioUIRenderImage.h"
#include "ioUIRenderFrame.h"
#include "ioUIFrameManager.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"

#include "ioUIImageSetManager.h"
#include "ioStringConverter.h"

#include "ioStream.h"
#include "ioResourceLoader.h"

ioUIFrameManager::ioUIFrameManager() : m_StaticSwitch( SATICS_OFF )
{
}

ioUIFrameManager::~ioUIFrameManager()
{
	DestroyAllFrames();
}

void ioUIFrameManager::AddMem( const ioHashString& KeyName, DWORD dwMem )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		m_MemoryMap.insert( MemoryMap::value_type( KeyName, dwMem ) );
	}
}

void ioUIFrameManager::AddCloneMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_MemoryMap.find( KeyName );
		if( iter != m_MemoryMap.end() )
		{
			m_CloneMemoryMap.insert( MemoryMap::value_type( iter->first, iter->second ) );
		}
	}
}

void ioUIFrameManager::RemoveMem( const ioHashString& KeyName )
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

void ioUIFrameManager::RemoveCloneMem( const ioHashString& KeyName )
{
	if( m_StaticSwitch == SATICS_ON )
	{
		MemoryMap::iterator iter = m_CloneMemoryMap.find( KeyName );
		if( iter != m_CloneMemoryMap.end() )
		{
			m_CloneMemoryMap.erase( iter );
		}
	}
}

void ioUIFrameManager::GetMemStatics( DWORD& dwCount, DWORD& dwSize )
{
	dwCount	= 0;
	dwSize	= 0;

	for( MemoryMap::iterator iter = m_MemoryMap.begin(); iter != m_MemoryMap.end(); ++iter )
	{
		dwSize	+= iter->second;
		dwCount++;
	}

	for( MemoryMap::iterator iter = m_CloneMemoryMap.begin(); iter != m_CloneMemoryMap.end(); ++iter )
	{
		dwSize	+= iter->second;
		dwCount++;
	}
}

bool ioUIFrameManager::LoadFile( const char *szFile )
{
	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFile, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioUIFrameManager::LoadFile - %s Load Failed", szFile );
		return false;
	}

	ioXMLDocument xmlDoc;
	if( !xmlDoc.LoadFromMemory( (char*)kStream.GetPtr() ) )
	{
		LOG.PrintTimeAndLog( 0, "ioUIFrameManager::LoadFile - %s LoadFromMemory Failed", szFile );
		return false;
	}

	ioXMLElement xRootElement = xmlDoc.GetRootElement();
	if( xRootElement.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioUIFrameManager::LoadFile - %s, Root Element is NULL", szFile );
		return false;
	}
	
	ParseFrameList( xRootElement );

	return true;
}

void ioUIFrameManager::ParseFrameList( ioXMLElement &xRootElement )
{
	ioXMLElement xCurElement = xRootElement.FirstChild();
	
	while( !xCurElement.IsEmpty() )
	{
		if( xCurElement.IsTagRight( "FrameComponent" ) )
		{
			ioUIRenderFrame *pFrame = CreateFrame( xCurElement );
			if( pFrame )
			{
				ioHashString szFrameName = xCurElement.GetStringAttribute( "Name" );
				if( !RegisterFrame( szFrameName, pFrame ) )
				{
					delete pFrame;
				}
			}
		}

		xCurElement = xCurElement.NextSibling();
	}
}

ioUIRenderFrame* ioUIFrameManager::CreateFrame( ioXMLElement &rkFrame )
{
	ioUIRenderFrame *pFrame = new ioUIRenderFrame;

	ioXMLElement xCurElement = rkFrame.FirstChild();
	while( !xCurElement.IsEmpty() )
	{
		if( xCurElement.IsTagRight( "Image" ) )
		{
			ParsingImage( xCurElement, pFrame );
		}

		xCurElement = xCurElement.NextSibling();
	}

	return pFrame;
}

void ioUIFrameManager::ParsingImage( ioXMLElement &rkImage, ioUIRenderFrame *pFrame )
{
	ioHashString szImageSet = rkImage.GetStringAttribute( "Imageset" );

	ioUIRenderImage *pImg = NULL;
	if( !szImageSet.IsEmpty() )
	{
		ioHashString szImage = rkImage.GetStringAttribute( "Image" );
		pImg = g_UIImageSetMgr.CreateImage( szImageSet, szImage );
	}
	else
	{
		ioHashString szFileName = rkImage.GetStringAttribute( "File" );
		pImg = g_UIImageSetMgr.CreateImageByFile( szFileName );
	}

	if( pImg )
	{
		ioHashString szColor = rkImage.GetStringAttribute( "Color" );
		if( !szColor.IsEmpty() )
		{
			DWORD dwColor = ioStringConverter::ParseColor( szColor );
			pImg->SetColor( dwColor );
		}

		ioHashString szType = rkImage.GetStringAttribute( "Type" );
		if( !pFrame->SetImageForLocation( szType, pImg ) )
		{
			delete pImg;
		}
		else
		{
			ioHashString szSize = rkImage.GetStringAttribute( "Size" );
			pFrame->SetImageForSize( szType, szSize );
		}
	}
}

ioUIRenderFrame* ioUIFrameManager::CloneFrame( const ioHashString &rkName )
{
	const ioUIRenderFrame *pTemplete = GetFrame( rkName );
	if( pTemplete )
	{
		AddCloneMem( rkName );
		return (ioUIRenderFrame*)pTemplete->Clone();
	}

	LOG.PrintTimeAndLog( 0, "ioUIFrameManager::CloneFrame - %s Not Exist Frame", rkName.c_str() );
	return NULL;
}

const ioUIRenderFrame* ioUIFrameManager::GetFrame( const ioHashString &rkName ) const
{
	FrameTempleteMap::const_iterator iter = m_FrameTempleteMap.find( rkName );
	if( iter!=m_FrameTempleteMap.end() )
		return iter->second;

	return NULL;
}

const char* ioUIFrameManager::GetFrameName( int index ) const
{
	if ( index < 0 || index >= GetNumFrame() ) {
		return NULL;
	}

	FrameTempleteMap::const_iterator iter = m_FrameTempleteMap.begin();
	FrameTempleteMap::const_iterator eiter = m_FrameTempleteMap.end();
	for ( int i = 0; iter != eiter; ++iter, ++i ) {
		if ( index == i ) {
			return iter->first.c_str();
		}
	}
	return NULL;
}

bool ioUIFrameManager::RegisterFrame( const ioHashString &rkName,
									  ioUIRenderFrame *pFrame )
{
	if( !GetFrame( rkName ) )
	{
		pFrame->SetName( rkName.c_str() );
		m_FrameTempleteMap.insert( FrameTempleteMap::value_type( rkName, pFrame ) );
		return true;
	}

	LOG.PrintTimeAndLog( 0, "ioUIFrameManager::RegisterFrame - %s Already Exist",
							rkName.c_str() );

	return false;
}

void ioUIFrameManager::DestroyAllFrames()
{
	FrameTempleteMap::iterator iter;
	for( iter=m_FrameTempleteMap.begin() ; iter!=m_FrameTempleteMap.end() ; ++iter )
	{
		delete iter->second;
	}
	m_FrameTempleteMap.clear();
}
