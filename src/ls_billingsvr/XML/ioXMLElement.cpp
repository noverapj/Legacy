

#include "../stdafx.h"

#include "../../../extra/tinyxml/tinyxml/tinyxml.h"

#include "ioXMLDocument.h"
#include "ioXMLElement.h"
#include "../Util/ioHashString.h"
#include <strsafe.h>

ioXMLElement::ioXMLElement()
{
	m_pElement = NULL;
#ifdef STRING_MANAGER 
	m_bUseStringMgr = false;
	ZeroMemory( m_szStringMgrKeyName, sizeof( m_szStringMgrKeyName ) );
#endif
}

ioXMLElement::ioXMLElement( const ioXMLElement &rhs )
{
	m_pElement = rhs.m_pElement;
#ifdef STRING_MANAGER 
	m_bUseStringMgr = false;
	ZeroMemory( m_szStringMgrKeyName, sizeof( m_szStringMgrKeyName ) );
#endif
}

ioXMLElement::ioXMLElement( TiXmlElement *pElement )
{
	m_pElement = pElement;
#ifdef STRING_MANAGER 
	m_bUseStringMgr = false;
	ZeroMemory( m_szStringMgrKeyName, sizeof( m_szStringMgrKeyName ) );
#endif
}

ioXMLElement::~ioXMLElement()
{
}

ioXMLElement ioXMLElement::CreateChild( const char *szTagName )
{
	TiXmlNode *pNew = m_pElement->InsertEndChild( TiXmlElement( szTagName ) );
	
	return ioXMLElement( pNew->ToElement() );
}

void ioXMLElement::RemoveChild( int iIndex )
{
	ioXMLElement kChild = GetChildByIndex( iIndex );
	if( !kChild.IsEmpty() )
	{
		m_pElement->RemoveChild( kChild.m_pElement );
	}
}

ioXMLElement ioXMLElement::FirstChild()
{
	return ioXMLElement( m_pElement->FirstChildElement() );
}

ioXMLElement ioXMLElement::FirstChild( const char *szName )
{
	return ioXMLElement( m_pElement->FirstChildElement( szName ) );
}

ioXMLElement ioXMLElement::NextSibling()
{
	return ioXMLElement( m_pElement->NextSiblingElement() );
}

ioXMLElement ioXMLElement::NextSibling( const char *szName )
{
	return ioXMLElement( m_pElement->NextSiblingElement( szName ) );
}

ioXMLElement ioXMLElement::GetChildByIndex( int iIndex )
{
	ioXMLElement kCursor = FirstChild();
	for( int i=0 ; i<iIndex && !kCursor.IsEmpty() ; i++ )
	{
		if( i==iIndex )
			return kCursor;

		kCursor = kCursor.NextSibling();
	}

	return ioXMLElement();
}

int ioXMLElement::GetChildCount()
{
	int iCount = 0;

	ioXMLElement kCursor = FirstChild();
	while( !kCursor.IsEmpty() )
	{
		iCount++;
		kCursor = kCursor.NextSibling();
	}

	return iCount;
}

bool ioXMLElement::IsEmpty() const
{
	if( m_pElement == NULL )
		return true;

	return false;
}

bool ioXMLElement::IsTagRight( const char *szTag )
{
	if( !strcmp( szTag, GetTagName() ) )
		return true;

	return false;
}

void ioXMLElement::SetText( const char *szText )
{
	TiXmlNode *pChild = m_pElement->FirstChild();
	if( pChild )
	{
		TiXmlText *pText = pChild->ToText();
		if( pText )
		{
			pText->SetValue( szText );
		}
		else
		{
			m_pElement->InsertBeforeChild( pChild, TiXmlText(szText) );
		}
	}
	else
	{
		m_pElement->InsertEndChild( TiXmlText(szText) );
	}
}

void ioXMLElement::SetIntAttribute( const char *szName, int iValue )
{
	m_pElement->SetAttribute( szName, iValue );
}

void ioXMLElement::SetBoolAttribute( const char *szName, bool bValue )
{
	if( bValue )
		m_pElement->SetAttribute( szName, "true" );
	else
		m_pElement->SetAttribute( szName, "false" );
}

void ioXMLElement::SetFloatAttribute( const char *szName, float fValue )
{
	m_pElement->SetAttribute( szName, (double)fValue );
}

void ioXMLElement::SetStringAttribute( const char *szName, const char *szValue )
{
	m_pElement->SetAttribute( szName, szValue );
}

void ioXMLElement::RemoveAttribute( const char *szName )
{
	m_pElement->RemoveAttribute( szName );
}

const char* ioXMLElement::GetTagName()
{
	return m_pElement->Value();
}

const char* ioXMLElement::GetText()
{
	return m_pElement->GetText();
}

int ioXMLElement::GetIntAttribute( const char *szName ) const
{
	int iValue = 0;
	if( m_pElement->Attribute( szName, &iValue ) )
		return iValue;

	return 0;
}

bool ioXMLElement::GetBoolAttribute( const char *szName ) const
{
	const char *pValue = m_pElement->Attribute( szName );
	if( pValue )
	{
		if( !strcmp( pValue, "true" ) )
			return true;
	}

	return false;
}

float ioXMLElement::GetFloatAttribute( const char *szName ) const
{
	double dValue = 0.0;
	if( m_pElement->Attribute( szName, &dValue ) )
		return (float)dValue;

	return 0.0f;
}

const char* ioXMLElement::GetStringAttribute( const char *szName ) const
{
	const char *pValue = m_pElement->Attribute( szName );
	if( pValue )
	{
#ifdef STRING_MANAGER 
		if( m_bUseStringMgr )
		{
			if( ioStringManager::GetSingletonPtr() )
				return g_StringMgr.GetStringXML( m_szStringMgrKeyName, pValue );
			else
				return pValue;
		}
		else
			return pValue;
#else 
		return pValue;
#endif 
	}

	return "";
}


void ioXMLElement::SetUseStringMgr( bool bUseStringMgr )
{
#ifdef STRING_MANAGER 
	m_bUseStringMgr = bUseStringMgr;
#endif 
}

void ioXMLElement::SetStringMgrKeyName( const char *szStringMgrKeyName )
{
#ifdef STRING_MANAGER 
	 StringCbCopy( m_szStringMgrKeyName, sizeof( m_szStringMgrKeyName ), szStringMgrKeyName );
#endif 
}