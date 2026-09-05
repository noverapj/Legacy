//----------------------------------------------------------------------------------
/*
Filename	: ioCustomWnd.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioCustomWnd.h"
#include "ioXMLElement.h"
#include "TinyXML/tinyxml.h"

//----------------------------------------------------------------------------------
/*
»ý¼º / ¼Ò¸ê
*/
ioCustomWnd::ioCustomWnd()
	: ioWnd()
	, m_xml( 0 )
{
}

ioCustomWnd::~ioCustomWnd()
{
	SAFEDELETE( m_xml );
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioCustomWnd::ParseXML( ioXMLElement& xml )
{
	SAFEDELETE( m_xml );

	m_xml = xml.Clone();
}

bool	ioCustomWnd::Save( ioXMLElement& xml )
{
	if ( m_xml )
	{		
		TiXmlElement* elem = m_xml->GetElement();
		if ( elem )
		{
			TiXmlNode* parent = xml.GetElement()->Parent();
			if ( parent ) 
			{
				parent->RemoveChild( xml.GetElement() );
				parent->LinkEndChild( elem->Clone() );				
			}
			else 
			{
				xml.SetElement( elem->Clone()->ToElement() );
			}
		}
	}
	return true;
}