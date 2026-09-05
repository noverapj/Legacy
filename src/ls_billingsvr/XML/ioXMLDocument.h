

#ifndef _ioXMLDocument_h_
#define _ioXMLDocument_h_

class TiXmlDocument;
class ioXMLElement;

class __EX ioXMLDocument
{
protected:
	TiXmlDocument *m_pDoc;

public:
	void CreateDocument( const char *szRootName,
						 const char *szVersion,
						 const char *szEncoding,
						 const char *szStandAlone );

public:
	bool LoadFromFile( const char *szFileName );
	bool LoadFromMemory( const char *pData );
	bool SaveXML( const char *szFileName );

public:
	ioXMLElement GetRootElement();

public:
	ioXMLDocument();
	virtual ~ioXMLDocument();
};

#endif