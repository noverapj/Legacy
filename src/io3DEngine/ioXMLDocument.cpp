

#include "stdafx.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"

#include "TinyXML/tinyxml.h"

ioXMLDocument::ioXMLDocument()
{
	m_pDoc = new TiXmlDocument;
}

ioXMLDocument::~ioXMLDocument()
{
	SAFEDELETE( m_pDoc );
}

void ioXMLDocument::CreateDocument( const char *szRootName,
								    const char *szVersion,
									const char *szEncoding,
									const char *szStandAlone )
{
	m_pDoc->Clear();

	m_pDoc->InsertEndChild( TiXmlDeclaration( szVersion, szEncoding, szStandAlone ) );
	m_pDoc->InsertEndChild( TiXmlElement( szRootName ) );
}

bool ioXMLDocument::LoadFromFile( const char *szFileName )
{
	m_pDoc->Clear();

	if( !m_pDoc->LoadFile( szFileName ) )
	{
		LOG.PrintTimeAndLog( 0, "ioXMLDocumemt::LoadFromFile - %s (%s)",
								m_pDoc->ErrorDesc(), szFileName );
		return false;
	}

	return true;
}

bool ioXMLDocument::LoadFromMemory( const char *pData )
{
	m_pDoc->Clear();

	if( !m_pDoc->Parse( pData ) )
	{
		if( m_pDoc->Error() )
		{
			LOG.PrintTimeAndLog( 0, "ioXMLDocument::LoadFromMemory - %s:%s(%d, %d)",
									m_pDoc->Value(), m_pDoc->ErrorDesc(),
									m_pDoc->ErrorRow(), m_pDoc->ErrorCol() );
			return false;
		}
	}

	return true;
}

bool ioXMLDocument::SaveXML( const char *szFileName )
{
	if( m_pDoc->SaveFile( szFileName ) )
		return true;

	return false;
}

ioXMLElement ioXMLDocument::GetRootElement()
{
	return ioXMLElement( m_pDoc->RootElement() );
}
