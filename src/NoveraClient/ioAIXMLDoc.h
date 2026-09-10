#pragma once

#include "../io3DEngine\ioXMLDocument.h"
#include <string>

//ioXMLElement*	FindNthChild( ioXMLElement* root, const std::string& tagname = "", int n = 1 );

class	ioAIXMLDoc : public ioXMLDocument
{
public:
	ioAIXMLDoc();
	virtual ~ioAIXMLDoc();

	bool	Load( const char* filename );
	bool	Load( const char* filename, const std::string& ver, bool bStrict = false );

	std::string		GetVersion()	{	return m_szVersion;		}
	std::string		GetEncoding()	{	return m_szEncoding;	}

	bool	ExtractHeader(	const char* filename, 
							bool& isfile,
							std::string& version, 
							std::string& encoding );

private:
	std::string		m_szVersion;
	std::string		m_szEncoding;
};
