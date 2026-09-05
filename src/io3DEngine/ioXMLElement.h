

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
	TiXmlElement* GetElement() { return m_pElement; }
	ioXMLElement* Clone();
	void Copy( ioXMLElement& rhs );

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

	//UJ 130812, put this at the end of class definition
	// VTable 순서에 영향이 있을 수 있으므로 무조건 맨 아래에 아래 내용을 유지할것
	//
	// 주의1: 최대 63글자(64bytes)까지만 지원
	// 주의2: debug 빌드에서는 string 보호가 되지 않는다
	// 주의3: 절대 문자열 상수 말고 char* szKey 등을 넘기지마라!! crash 가능!!
	//
public:
#ifndef SHIPPING //UJ
#define GetIntAttribute_e		GetIntAttribute
#define GetBoolAttribute_e		GetBoolAttribute
#define GetFloatAttribute_e		GetFloatAttribute
#define GetStringAttribute_e	GetStringAttribute
#else
	template <int N>
	__forceinline int GetIntAttribute_e( const char (&k)[N] ) const
	{
		_ENCSTR(k, _k);
		return GetIntAttribute(_k);
	}

	template <int N>
	__forceinline bool GetBoolAttribute_e( const char (&k)[N] ) const
	{
		_ENCSTR(k, _k);
		return GetBoolAttribute(_k);
	}

	template <int N>
	__forceinline float GetFloatAttribute_e( const char (&k)[N] ) const
	{
		_ENCSTR(k, _k);
		return GetFloatAttribute(_k);
	}

	template <int N>
	__forceinline const char* GetStringAttribute_e( const char (&k)[N] ) const
	{
		_ENCSTR(k, _k);
		return GetStringAttribute(_k);
	}
#endif // _DEBUB
};

#endif