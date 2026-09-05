

#ifndef _ioXMLElement_h_
#define _ioXMLElement_h_

class TiXmlElement;
class TiXmlAttribute;

class __EX ioXMLElement
{
protected:
	TiXmlElement *m_pElement;
	bool          m_bUseStringMgr;
	char          m_szStringMgrKeyName[MAX_PATH];
	
public:
	void SetElement( TiXmlElement *pElement ) { m_pElement = pElement; }

public:
	ioXMLElement CreateChild( const char *szTagName );

	ioXMLElement FirstChild();
	ioXMLElement FirstChild( const char *szName );

	ioXMLElement NextSibling();
	ioXMLElement NextSibling( const char *szName );

	ioXMLElement GetChildByIndex( int iIndex );
	int GetChildCount();

	void RemoveChild( int iIndex );

public:
	bool IsEmpty() const;
	bool IsTagRight( const char *szTag );

public:
	void SetText( const char *szText );
	
	void SetIntAttribute( const char *szName, int iValue );
	void SetBoolAttribute( const char *szName, bool bValue );
	void SetFloatAttribute( const char *szName, float fValue );
	void SetStringAttribute( const char *szName, const char *szValue );

	void RemoveAttribute( const char *szName );

public:
	const char* GetTagName();
	const char* GetText();

public:
	int   GetIntAttribute( const char *szName ) const;			// if not exist return 0
	bool  GetBoolAttribute( const char *szName ) const;			// if not exist return false
	float GetFloatAttribute( const char *szName ) const;		// if not exist return 0.0f
	const char* GetStringAttribute( const char *szName ) const;	// if not exist return ""

public:
	void SetUseStringMgr( bool bUseStringMgr );
	void SetStringMgrKeyName( const char *szStringMgrKeyName );

public:
	ioXMLElement();
	ioXMLElement( const ioXMLElement &rhs );
	ioXMLElement( TiXmlElement *pElement );
	virtual ~ioXMLElement();
};

#endif