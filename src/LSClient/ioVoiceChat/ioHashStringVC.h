
#ifndef _ioHashStringVC_h_
#define _ioHashStringVC_h_

#define HASH_CONST	5381
#define DEFAULT_CAPACITY	32

// Original version is www.gamza.net

class __EX ioHashStringVC
{
private:
	char	*m_pString;
	DWORD	m_dwHashCode;
	int		m_iLength;
	int		m_iCapacity;

private:
	void ReAllocCapacity( int iNewCapacity );

public:
	void Clear();

	bool IsEmpty() const;
	int  Length() const;
	char At( int i ) const;

	DWORD GetHashCode() const;
	const char* c_str() const;

public:
	static DWORD CalcHashCode( const char *str );

public:
	bool operator < ( const ioHashStringVC &rhs )	const;

	ioHashStringVC& operator=( const ioHashStringVC &rhs );
	ioHashStringVC& operator=( const char *szText );

	bool operator==( const ioHashStringVC &rhs ) const;
	bool operator==( const char *szText ) const;

	bool operator!=( const ioHashStringVC &rhs ) const;
	bool operator!=( const char *szText ) const;

public:
	ioHashStringVC();
	ioHashStringVC( const char *str );
	ioHashStringVC( const ioHashStringVC &rhs );
	~ioHashStringVC();
};

#endif
