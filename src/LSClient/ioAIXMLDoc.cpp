#include "stdafx.h"
#include "ioAIXMLDoc.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
/*
ioXMLElement*	FindNthChild( ioXMLElement* root, const std::string& tagname, int n )
{
	if( root )
	{
		if( !root->IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "Investigating %s", root->GetTagName() );
			ioXMLElement*	child = &(root->FirstChild( tagname.c_str() ));
			while( child && !child->IsEmpty() )
			{
				LOG.PrintTimeAndLog( 0, "Verifying %s %d", child->GetTagName(), n );
				if( n <= 1 )
				{
					LOG.PrintTimeAndLog( 0, "Found!!" );

					return child;
				}

				child = &(child->NextSibling());
			}
		}
		else LOG.PrintTimeAndLog( 0, "Given node is empty" );
	}
	else LOG.PrintTimeAndLog( 0, "Given node is null pointer" );

	LOG.PrintTimeAndLog( 0, "Failed to find %d th child with %s", n, tagname.c_str() );
	return (ioXMLElement*)0;
}
*/

ioAIXMLDoc::ioAIXMLDoc()
{
}

ioAIXMLDoc::~ioAIXMLDoc()
{
}

bool	ioAIXMLDoc::ExtractHeader(	const char* filename, 
									bool& isfile,
									std::string& version, std::string& encoding )
{
	std::string	line;
	std::ifstream	ifile( filename );

	if( ifile )
	{
		isfile = true;
		std::getline( ifile, line );
	}
	else
	{
		isfile = false;
		ioTextStream	kStream;
		if( g_ResourceLoader.LoadStream( filename, &kStream ) )
		{
			kStream.GetLine( line );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioAIXMLDoc::ExtractHeader - XML file [%s] Fail", filename );

			return false;
		}
	}

	if( !line.empty() )
	{
		std::regex	reg( "<\\?xml[\\s]*version[\\s]*=[\\s]*\"([0-9\\.]*)\"[\\s]*encoding[\\s]*=[\\s]*\"([A-Za-z0-9\\-_]*)\"[\\s]*\\?>" );
		std::smatch sm;

		if( std::regex_match( line, sm, reg ) )
		{
			version		= sm[1];
			encoding	= sm[2];

			return true;
		}
	}

	LOG.PrintTimeAndLog( 0, "encoding and version could not be found. : %s", line.c_str() );

	return false;
}

bool	ioAIXMLDoc::Load( const char* filename )
{
	bool	isfile = true, isvalid = false;

	isvalid = ExtractHeader( filename, isfile, m_szVersion, m_szEncoding );

	if( isvalid )
	{
		if( isfile )
		{
			return ioXMLDocument::LoadFromFile( filename );
		}
		else
		{
			ioTextStream	kStream;
			if( g_ResourceLoader.LoadStream( filename, &kStream ) )
			{
				return ioXMLDocument::LoadFromMemory( (const char*)kStream.GetPtr() );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioAIXMLDoc::Load - XML file [%s] Fail", filename );
				return false;
			}
		}
	}

	return false;
}

bool	ioAIXMLDoc::Load( const char* filename, const std::string& ver, bool bStrict )
{
	if( Load( filename ) )
	{
		if( ver.empty() )
		{
			LOG.PrintTimeAndLog( 0, 
				"### WARNING : %s : config version missing : assuming current version",	
					filename );

			return !bStrict;
		}

		if( m_szVersion != ver )
		{
			LOG.PrintTimeAndLog( 0, 
				"### WARNING : %s : version mismatch : we need %s, but are having %s", 
					filename, ver.c_str(), m_szVersion.c_str() );
			
			return !bStrict;
		}

		return true;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "[AI] ERROR : %s : failed to load", filename );

		return false;
	}
}

