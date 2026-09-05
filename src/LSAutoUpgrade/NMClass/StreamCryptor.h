#pragma once

#ifdef	_NMSERVER
#	include "../LoginServer/NMHeapManager.h"
#endif

#ifdef	_NMSERVER
#	define	_NMHeapAlloc( size )	CNMHeapManager().GetInstance().Alloc( size )
#	define	_NMHeapFree( ptr )		CNMHeapManager().GetInstance().Free( ptr )
#else
#	define	_NMHeapAlloc( size )	::HeapAlloc( ::GetProcessHeap(), NULL, size )
#	define	_NMHeapFree( ptr )		::HeapFree( ::GetProcessHeap(), NULL, ptr )
#endif

class CStreamCryptor
{
public:
	enum CryptorError
	{
		kError_None				= 0,

		kError_MemAllocFailed	= -1,
		kError_WrongCheckValue	= -2,
		kError_InvalidVersion	= -3,
		kError_NotEnoughBuffer	= -4,
	};

protected:
	UINT16	GetCheckValue( BYTE* pBuf, UINT32 uBufLen );

public:
	explicit CStreamCryptor( void )	{}
	virtual ~CStreamCryptor( void ) {};

	virtual INT32	Encrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen ) = 0;
	virtual INT32	Decrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen ) = 0;

	virtual UINT8	GetVersion( void ) const = 0;
	virtual UINT32	GetStartMarker( void ) const = 0;
	virtual UINT32	GetEndMarker( void ) const = 0;
	virtual UINT32	GetEncSize( UINT32 uBufLen ) const = 0;
};

class CStreamCryptor_v0 : public CStreamCryptor
{
private:
	enum Cryptor_v0_Constant
	{
		kCryptor_v0_Version		= 0,

		kCryptor_v0_StartMarker	= 0x1fca34,
		kCryptor_v0_EndMarker	= 0x8a119e,

		kCryptor_v0_ExtraSize	= 0,
	};

public:
	CStreamCryptor_v0( void )			{}
	virtual ~CStreamCryptor_v0( void )	{}

	static CStreamCryptor_v0* CreateObject( void );

	virtual INT32	Encrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen );
	virtual INT32	Decrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen );

	virtual UINT8	GetVersion( void ) const			{ return kCryptor_v0_Version; }
	virtual UINT32	GetStartMarker( void ) const		{ return kCryptor_v0_StartMarker; }
	virtual UINT32	GetEndMarker( void ) const			{ return kCryptor_v0_EndMarker; }
	virtual UINT32	GetEncSize( UINT32 uBufLen ) const	{ return kCryptor_v0_ExtraSize + uBufLen; }
};


class CStreamCryptor_v1 : public CStreamCryptor
{
private:
	enum Cryptor_v1_Constant
	{
		kCryptor_v1_Version			= 1,

		kCryptor_v1_StartMarker		= 0x0a98c32a,
		kCryptor_v1_EndMarker		= 0x820a9e12,

		kCryptor_v1_GarbageSize		= 1,
		kCryptor_v1_VersionSize		= 1,
		kCryptor_v1_SeedSize		= 4,
		kCryptor_v1_CheckValueSize	= 2,

		kCryptor_v1_ExtraSize		= (kCryptor_v1_GarbageSize * 3) + kCryptor_v1_VersionSize + kCryptor_v1_SeedSize + kCryptor_v1_CheckValueSize,
	};

public:
	CStreamCryptor_v1( void )			{}
	virtual ~CStreamCryptor_v1( void )	{}

	static CStreamCryptor_v1* CreateObject( void );

	virtual INT32	Encrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen );
	virtual INT32	Decrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen );

	virtual UINT8	GetVersion( void ) const			{ return kCryptor_v1_Version; }
	virtual UINT32	GetStartMarker( void ) const		{ return kCryptor_v1_StartMarker; }
	virtual UINT32	GetEndMarker( void ) const			{ return static_cast<UINT32>(kCryptor_v1_EndMarker); }
	virtual UINT32	GetEncSize( UINT32 uBufLen ) const	{ return static_cast<UINT32>(kCryptor_v1_ExtraSize) + uBufLen; }
};

class CStreamCryptorFactory
{
public:
	static CStreamCryptorFactory& GetInstance( void )
	{
		static CStreamCryptorFactory* streamCryptorFactory = NULL;
		if ( streamCryptorFactory == NULL )
			streamCryptorFactory = new CStreamCryptorFactory;
		return ( *streamCryptorFactory );
	}

private:
	typedef std::map< UINT8, CStreamCryptor*>	CStreamCryptorMap;

private:
	CStreamCryptorMap	m_mapCryptor;
	UINT8				m_uLatestVersion;

public:
	CStreamCryptorFactory( void );
	~CStreamCryptorFactory( void ) {}

public:
	void	AddCryptor( CStreamCryptor* pStreamCryptor );

	UINT8	GetLatestVersion( void ) const	{ return this->m_uLatestVersion; }

	CStreamCryptor*	FindStreamCryptor( UINT32 uStartMarker );
	CStreamCryptor*	FindStreamCryptor( UINT8 uCryptorVersion );
};
