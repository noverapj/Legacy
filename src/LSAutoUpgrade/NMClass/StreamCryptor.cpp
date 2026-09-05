#include "PCH.h"
#include "StreamCryptor.h"

///////////////////////
// CStreamCryptor
///////////////////////
UINT16 CStreamCryptor::GetCheckValue( BYTE* pBuf, UINT32 uBufLen )
{
	BYTE	bCheckSum	= 0;
	BYTE	bCheckXor	= 0;

	for ( UINT32 i = 0; i < uBufLen; ++i )
	{
		bCheckSum	= bCheckSum + pBuf[ i ];
		bCheckXor	^= pBuf[ i ];
	}

	bCheckSum	= ~bCheckSum;

	return (bCheckSum << 1 | bCheckXor);
}

///////////////////////
// CStreamCryptor_v0
///////////////////////
CStreamCryptor_v0* CStreamCryptor_v0::CreateObject( void )
{
	static CStreamCryptor_v0 streamCryptor_v0;
	return &streamCryptor_v0;
}

INT32 CStreamCryptor_v0::Encrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen )
{
	UNREFERENCED_PARAMETER(pBuf);
	UNREFERENCED_PARAMETER(uBufMaxLen);
	UNREFERENCED_PARAMETER(uBufLen);

	return kError_None;
}

INT32 CStreamCryptor_v0::Decrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen )
{
	UNREFERENCED_PARAMETER(pBuf);
	UNREFERENCED_PARAMETER(uBufMaxLen);
	UNREFERENCED_PARAMETER(uBufLen);

	return kError_None;
}

///////////////////////
// CStreamCryptor_v1
///////////////////////
CStreamCryptor_v1* CStreamCryptor_v1::CreateObject( void )
{
	static CStreamCryptor_v1 streamCryptor_v1;
	return &streamCryptor_v1;
}

INT32 CStreamCryptor_v1::Encrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen )
{
	BYTE*	pEncBuf;
	UINT32	uSeed;
	UINT32	uBufferPtr	= 0;
	UINT16	uCheckValue	= 0;

	if ( uBufMaxLen < this->GetEncSize( *uBufLen ) )
		return kError_NotEnoughBuffer;

	::srand( ::GetTickCount() );

	pEncBuf		= (BYTE*) _NMHeapAlloc( *uBufLen + kCryptor_v1_ExtraSize );
	uSeed		= ::rand();

	if ( pEncBuf == NULL )
		return kError_MemAllocFailed;

	*(pEncBuf + uBufferPtr)									= static_cast<BYTE>(::rand());
	uBufferPtr += kCryptor_v1_GarbageSize;
	*(pEncBuf + uBufferPtr)									= this->GetVersion();
	uBufferPtr += kCryptor_v1_VersionSize;
	*(pEncBuf + uBufferPtr)									= static_cast<BYTE>(::rand());
	uBufferPtr += kCryptor_v1_GarbageSize;
	*( reinterpret_cast<UINT32*>(pEncBuf + uBufferPtr) )	= uSeed;
	uBufferPtr += kCryptor_v1_SeedSize;
	*(pEncBuf + uBufferPtr)									= static_cast<BYTE>(::rand());
	uBufferPtr += kCryptor_v1_GarbageSize;

	BYTE		bPrevious	= 0;
	BYTE		bScrambleValue;

	for ( UINT32 i = 0; i < *uBufLen; ++i )
	{
		bScrambleValue	= static_cast<BYTE>(uSeed ^ bPrevious);
		pEncBuf[ uBufferPtr + i ]	= pBuf[ i ] ^ bScrambleValue;
		bPrevious	= pEncBuf[ uBufferPtr + i ];
	}
	uBufferPtr	+= *uBufLen;

	for ( UINT32 i = 0; i < uBufferPtr; ++i )
	{
		pBuf[ i ]	= pEncBuf[ (uBufferPtr - 1) - i ];
	}

	uCheckValue	= this->GetCheckValue( pBuf, uBufferPtr );
	*( reinterpret_cast<UINT16*>(pBuf + uBufferPtr) )	= uCheckValue;
	uBufferPtr += kCryptor_v1_CheckValueSize;

	*uBufLen	= uBufferPtr;

	_NMHeapFree( pEncBuf );

	return kError_None;
}

INT32 CStreamCryptor_v1::Decrypt( IN BYTE* pBuf, IN UINT32 uBufMaxLen, IN OUT UINT32* uBufLen )
{
	UNREFERENCED_PARAMETER(uBufMaxLen);

	UINT32	uDataLen	= *uBufLen - kCryptor_v1_CheckValueSize;

	UINT16	uCheckValue	= *( reinterpret_cast<UINT16*>(pBuf + uDataLen) );

	if ( uCheckValue != this->GetCheckValue(pBuf, uDataLen ) )
		return kError_WrongCheckValue;

	BYTE*	pTempBuf;
	UINT32	uTempBufferPtr	= 0;
	UINT32	uSeed;

	pTempBuf	= (BYTE*) _NMHeapAlloc( uDataLen );

	if ( pTempBuf == NULL )
		return kError_MemAllocFailed;

	for ( UINT32 i = 0; i < uDataLen; ++i )
	{
		pTempBuf[ i ]	= pBuf[ (uDataLen - 1)  - i ];
	}

	uTempBufferPtr += kCryptor_v1_GarbageSize;
	if ( *(pTempBuf + uTempBufferPtr) != this->GetVersion() )
	{
		_NMHeapFree( pTempBuf );
		return kError_InvalidVersion;
	}
	uTempBufferPtr += kCryptor_v1_VersionSize;

	uTempBufferPtr += kCryptor_v1_GarbageSize;
	uSeed			= *( reinterpret_cast<UINT32*>(pTempBuf + uTempBufferPtr) );
	uTempBufferPtr += kCryptor_v1_SeedSize;
	uTempBufferPtr += kCryptor_v1_GarbageSize;

	BYTE		bPrevious	= 0;
	BYTE		bScrambleValue;

	*uBufLen	-= kCryptor_v1_ExtraSize;

	for ( UINT32 i = 0; i < *uBufLen; ++i )
	{
		bScrambleValue	= static_cast<BYTE>(uSeed ^ bPrevious);
		bPrevious		= pTempBuf[ uTempBufferPtr + i ];
		pBuf[ i ]		= pTempBuf[ uTempBufferPtr + i ] ^ bScrambleValue;
	}

	_NMHeapFree( pTempBuf );

	return kError_None;
}

///////////////////////
// CStreamCryptorFactory
///////////////////////
CStreamCryptorFactory::CStreamCryptorFactory()
{
	this->AddCryptor( CStreamCryptor_v0::CreateObject() );
	this->AddCryptor( CStreamCryptor_v1::CreateObject() );

	this->m_uLatestVersion	= CStreamCryptor_v1::CreateObject()->GetVersion();
}

void CStreamCryptorFactory::AddCryptor( CStreamCryptor* pStreamCryptor )
{
	if ( pStreamCryptor != NULL )
		this->m_mapCryptor.insert( std::make_pair( pStreamCryptor->GetVersion(), pStreamCryptor ) );
}

CStreamCryptor* CStreamCryptorFactory::FindStreamCryptor( UINT32 uStartMarker )
{
	CStreamCryptorMap::iterator	iterCur, iterEnd;

	iterCur	= this->m_mapCryptor.begin();
	iterEnd	= this->m_mapCryptor.end();

	for ( ; iterCur != iterEnd; ++iterCur )
	{
		if ( uStartMarker == iterCur->second->GetStartMarker() )
			return iterCur->second;
	}

	return static_cast<CStreamCryptor*>(NULL);
}

CStreamCryptor* CStreamCryptorFactory::FindStreamCryptor( UINT8 uCryptorVersion )
{
	CStreamCryptorMap::iterator	iterFind	= this->m_mapCryptor.find( uCryptorVersion );

	if ( iterFind != this->m_mapCryptor.end() )
	{
		return iterFind->second;
	}

	return static_cast<CStreamCryptor*>(NULL);
}
